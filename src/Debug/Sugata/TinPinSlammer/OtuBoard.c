#include "OtuFieldAccessShared.h"

/*
 * ov039 region 0x02088698 - 0x02089950. One translation unit of the
 * overlay; dsd gives each file a single contiguous `.text` claim. The
 * shared types, externs and prototypes are in OtuFieldAccessShared.h.
 */
/** The round's per-slot score, a `u16` at data_02071cf0 + 0x3434.
 *
 *  The target reaches it as `add rX, rbase, rI, lsl #1 / add rX, rX, #0x3400 /
 *  strh r0, [rX, #0x14]` off a pool word of `data_02071cf0 + 0x20`, so the two
 *  constants 0x3400 and 0x14 are load-bearing and are written as one byte offset
 *  here. The score is what 020893fc compares to find the round's leader. */
#define OTU_BOARD_SLOT_SCORE(i) (*(u16*)((u8*)data_02071cf0 + 0x3434 + (i) * 2))

/** The scene's heap, formed the way four functions in this band form it.
 *
 *  `scene + 0x18C + 0x11400` = scene + 0x1158C = `&scene->base.heap`. The two
 *  adds are not an accident of folding: 0x1158C is not an encodable ARM `add`
 *  immediate, and the target consistently splits it at 0x11400, which is
 *  TIN_PIN_SLAMMER_HEAP_OFFSET. OTU_HEAP() would hand mwcc one 32-bit constant
 *  and a different two-instruction split. */
#define OTU_BOARD_HEAP(scene) ((Heap*)((u8*)(scene) + 0x18C + 0x11400))

/** The helper-handle array at stage + 0x18C, one word per slot.
 *
 *  Same spelling rule as the feature header's OTU_CHILD_ID, and for the same
 *  reason: `(base + i * 4) + 0x18C` keeps the scaled add and the fixed
 *  displacement separate, where `*(s32*)((u8*)stage + i * 4 + 0x18C)` folds into
 *  one computed displacement. 64 entries. */
#define OTU_HELPER_ID(stage, i) (((s32*)((u8*)(stage) + 0x18C))[i])

/** The effect-handle array at stage + 0x28C -- immediately after the helper
 *  array's 0x40 entries, not a separate region. */
#define OTU_EFFECT_ID(stage, i) (((s32*)((u8*)(stage) + 0x28C))[i])

/** The board's countdown block for the menu currently being shown.
 *
 *  Formed as `(scene + 0x84) + 0x44000 + menuIndex * 0x34`, in that order, so the
 *  menu stride stays a single `mla` against the target's `add r2, r0, #0x84 /
 *  add r2, r2, #0x44000 / mla r1, r3, r1, r2`. The header's OTU_COUNTDOWN()
 *  starts at 0x44000 and leaves the +0x84 to the caller, which costs an add. */
#define OTU_BOARD_COUNTDOWN(scene) ((OtuBoardCountdown*)((u8*)(scene) + 0x44000 + 0x84 + (scene)->menuIndex * 0x34))

// clear +0x114/+0x118 on two grandchildren

/* ============================================================================
 * 0x02088698 -- the wireless packet receiver.
 * ==========================================================================*/

/**
 * @brief Accepts one inbound wireless packet and files it in the stage block.
 *
 * Two parameters, not one: the packet arrives in r0 and the scene in r1, which is
 * why the stage block is fetched from the *second* argument and the packet's own
 * fields are read through the first. Five gate conditions run in the order the
 * target tests them, and each exits rather than nesting -- `popeq`/`popne` off
 * the comparison, so this is a chain of early returns, not one `if`.
 *
 * On success the packet's first 0xC0 bytes are copied twelve records at a time
 * into the stage's ring and the block is flagged, then ov040 is told to send the
 * acknowledgement. The copy is a struct assignment per record: the target's
 * `ldm r6!, {r0-r3} / stm r5!, {r0-r3}` is mwcc's 16-byte block move.
 *
 * (m2c reads this as one argument, so every access through its `arg0` is the
 * wrong object and the `pop`-based early exits become returns.)
 *
 * @param packet  the received record; read at +0x3C, +0x4A, +0x4B, +0x50
 * @param scene   the Otosu scene block
 */
void func_ov039_02088698(void* packet, TinPinSlammer_Scene* scene) {
    OtuPinStage* stage = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32          i;

    if (stage == NULL) {
        return;
    }

    if (*(u16*)((u8*)packet + 0x3C) == 0) {
        return;
    }

    if (*(u8*)((u8*)packet + 0x4A) != 0x70) {
        return;
    }

    if ((*(u8*)((u8*)packet + 0x4B) & 1) == 0) {
        return;
    }

    if (*(u16*)((u8*)packet + 0x50) != stage->packetKind) {
        return;
    }

    // Walks both pointers forward with a down-counter: the target's loop is a
    // post-increment `ldmia`/`stmia` pair and `subs lr, lr, #1 / bne`, with no
    // compare against the bound at the top.
    {
        OtuWireRecord* src = (OtuWireRecord*)packet;
        OtuWireRecord* dst = stage->rxRecord;

        i = 0xC;
        do {
            *dst++ = *src++;
        } while (--i);
    }

    stage->gotPacket = 1;
    func_ov040_0209cb98(packet, 1);
}

/* ============================================================================
 * 0x0208871c -- hand the pin positions out to everything that follows them.
 * ==========================================================================*/

/**
 * @brief Publishes the round's geometry: the local pin's position first, then
 *        every other pin's, then every helper, then every effect.
 *
 * Four passes, in this order and no others, and the order is the point: the
 * local pin's own point is left in the frame buffer at the end of the first pass
 * and *that* is the point handed to all 64 helpers and to every effect, so the
 * whole background is aimed at the player rather than at the last pin walked.
 *
 * Two frame locals, both OtuPoints, at sp+0x10/sp+0x08 and sp+0x00/sp+0x04 --
 * declared in that order so the reverse-declaration rule puts them there.
 * (m2c declares `s32 sp0; s32 sp8;` and then references an undeclared `spC`,
 * because it did not notice these are two OtuPoints.)
 */
void func_ov039_0208871c(TinPinSlammer_Scene* scene) {
    OtuPinStage* stage = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));
    void*        pin;
    s32          event;
    OtuPoint     at;
    OtuPoint     mine;
    OtuPoint     other;
    s32          me;
    s32          i;

    pin = EasyTask_GetTaskData(OTU_POOL2(scene), OTU_CHILD_ID(stage, func_ov039_02088418(scene->state.unk_AF0)));
    func_ov039_0208e6fc(pin, &at);
    event = func_ov039_0208eff8(pin);
    func_ov039_0208e848(pin, &at);
    func_ov039_0208e870(pin, at.x, at.y);

    func_ov039_02092348(EasyTask_GetTaskData(OTU_POOL2(scene), stage->bgTaskA), at.x, at.y);
    func_ov039_020923b4(EasyTask_GetTaskData(OTU_POOL2(scene), stage->bgTaskA), event);
    func_ov039_02092d0c(EasyTask_GetTaskData(OTU_POOL2(scene), stage->bgTaskB), at.x, at.y);
    func_ov039_02092e04(EasyTask_GetTaskData(OTU_POOL2(scene), stage->bgTaskB), event);

    // The local pin is skipped: its aim point is already in `at`. The mode
    // selector is re-read and re-resolved inside the loop, which the target does
    // on every iteration rather than hoisting.
    for (i = 0; i < stage->childCount; i++) {
        me = func_ov039_02088418(scene->state.unk_AF0);
        if (i == me) {
            continue;
        }

        pin = EasyTask_GetTaskData(OTU_POOL2(scene), OTU_CHILD_ID(stage, i));
        func_ov039_0208e848(pin, &at);
        func_ov039_0208e6fc(pin, &other);
        func_ov039_0208e870(pin, other.x, other.y);
    }

    for (i = 0; i < 0x40; i++) {
        func_ov039_02094e88(EasyTask_GetTaskData(OTU_POOL2(scene), OTU_HELPER_ID(stage, i)), &at);
    }

    for (i = 0; i < stage->effectCount; i++) {
        func_ov039_02092730(EasyTask_GetTaskData(OTU_POOL2(scene), OTU_EFFECT_ID(stage, i)), &at);
    }
}

/* ============================================================================
 * 0x020888e0 -- (re)create every helper, effect and stage task.
 * ==========================================================================*/

/**
 * @brief Tears down nothing and rebuilds everything: 64 helpers, the effect
 *        pool, seven stage tasks, then republishes the geometry.
 *
 * The value passed to every factory is `scene->base.spareDataType` (scene +
 * 0x11584) and it is *re-read from memory* at each call rather than kept in a
 * register -- the target holds `scene + 0x11000` in a callee-saved register and
 * loads `[rX, #0x584]` afresh, which is what stops a local from reproducing it.
 * (m2c hoists it, and also spells the heap as `arg0 + 0x1158C` in one folded
 * offset; the target splits it `+ 0x18C` then `+ 0x11400`, which is why
 * OTU_BOARD_HEAP exists.)
 *
 * Six of the seven factories take the block's parameter block by address; only
 * 020941d0 takes a word instead, and 02095468/02094ab4 take a pin handle.
 */
void func_ov039_020888e0(TinPinSlammer_Scene* scene) {
    OtuPinStage* stage = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32          i;

    for (i = 0; i < 0x40; i++) {
        OTU_HELPER_ID(stage, i) = func_ov039_02094e58(OTU_POOL2(scene), scene->base.spareDataType);
    }

    for (i = 0; i < stage->effectCount; i++) {
        // bufB is walked eight bytes at a time and handed on by address; nothing
        // here interprets the record.
        OTU_EFFECT_ID(stage, i) = func_ov039_020926f0(OTU_POOL2(scene), scene->base.spareDataType, stage->params.kind,
                                                      stage->params.variant, (OtuObstacle_Params*)(stage->bufB + i * 8));
    }

    stage->pinTask = func_ov039_02095468(OTU_POOL2(scene), scene->base.spareDataType,
                                         OTU_CHILD_ID(stage, func_ov039_02088418(scene->state.unk_AF0)));
    stage->bgTaskA = func_ov039_02092310(OTU_POOL2(scene), scene->base.spareDataType, OTU_BOARD_HEAP(scene), &stage->params);
    stage->bgTaskB = func_ov039_02092cd4(OTU_POOL2(scene), scene->base.spareDataType, OTU_BOARD_HEAP(scene), &stage->params);
    stage->bgTaskC = func_ov039_020934a8(OTU_POOL2(scene), scene->base.spareDataType, OTU_BOARD_HEAP(scene), &stage->params);
    stage->fxTask  = func_ov039_020941d0(OTU_POOL2(scene), scene->base.spareDataType, stage->countdownAux);
    stage->fxTask2 = func_ov039_02094ab4(OTU_POOL2(scene), scene->base.spareDataType,
                                         OTU_CHILD_ID(stage, func_ov039_02088418(scene->state.unk_AF0)));
    // The one task in this function that lives in pool 1.
    stage->soundTask = func_ov039_02096b18(OTU_POOL1(scene), scene->base.spareDataType);

    func_ov039_0208871c(scene);
}

/* ============================================================================
 * 0x02088a8c / 0x02088b80 -- the two pin-children spawners.
 * ==========================================================================*/

/**
 * @brief The first-game spawner: one child per populated countdown group.
 *
 * Called only from 02088df0, which has just filled the countdown from the pin
 * trays, so `childCount` here is one plus the number of non-empty groups. The
 * per-child sprite base is therefore *this* group's own start: for i > 0 it is
 * the previous group's cell count scaled by 0x22 and added to scene + 0x44974,
 * which is what spreads three children across three menu columns. For i == 0
 * there is no previous group and the base is NULL.
 *
 * The tenth argument is that base and the ninth is the "first child" flag; both
 * are computed from the same `i == 0` test the target makes three times, once
 * per predicate, which is why they are spelled as separate comparisons rather
 * than one hoisted flag. (m2c has the ninth and tenth arguments the wrong way
 * round, drops the +0x18C on the heap, and passes `arg0 + 0x44974` without the
 * `groupCount * 0x22`.)
 */
void func_ov039_02088a8c(TinPinSlammer_Scene* scene) {
    OtuPinStage* stage = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32          i;

    for (i = 0; i < stage->childCount; i++) {
        OTU_CHILD_ID(stage, i) =
            func_ov039_0208dcb0(OTU_POOL2(scene), scene->base.spareDataType, i, (i == 0) ? func_ov039_02088440(i) : (void*)0,
                                &stage->params, scene, (u8*)scene + 0x41EF0, OTU_PIN_TRAY(scene, i), (i == 0),
                                (i == 0) ? (u8*)0 : (u8*)scene + 0x44974 + stage->countdown->group[i - 1].groupCount * 0x22);
    }

    func_ov039_020888e0(scene);
}

/**
 * @brief The resume spawner: same children, but the pad descriptor is fetched
 *        for every slot and only the local player's child is flagged.
 *
 * The tenth argument is a literal zero here against 02088a8c's computed sprite
 * base, and the ninth argument is inverted in meaning: `i == 0` in one,
 * `i == localPlayer` in the other. The mode selector is re-read and re-resolved
 * inside the loop, before the pad descriptor is fetched -- the target calls
 * func_ov039_02088418 first and only then 02088440, so the pad pointer is live in
 * r0 across the first call.
 */
void func_ov039_02088b80(TinPinSlammer_Scene* scene) {
    OtuPinStage* stage = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32          i;

    for (i = 0; i < stage->childCount; i++) {
        OTU_CHILD_ID(stage, i) = func_ov039_0208dcb0(OTU_POOL2(scene), scene->base.spareDataType, i, func_ov039_02088440(i),
                                                     &stage->params, scene, (u8*)scene + 0x41EF0, OTU_PIN_TRAY(scene, i),
                                                     (i == func_ov039_02088418(scene->state.unk_AF0)), 0);
    }

    func_ov039_020888e0(scene);
}

/* ============================================================================
 * 0x02088c50 -- load the stage's data out of the overlay's bin.
 * ==========================================================================*/

/**
 * @brief Loads the round's four data blocks and hands out the three heap
 *        buffers.
 *
 * The bin is one file addressed by an offset per block, so every load is the
 * same five-argument call: (1, destination, size-in-file, &bin, byte-offset).
 * Note the argument roles: the *third* argument is the size and the *fifth* is
 * the offset, which is the reverse of what the names suggest -- the tables make
 * it unambiguous, with the all-0x10 table sizing the inline parameter block and
 * the growing table giving the offsets.
 *
 * The first load has no allocation: its destination is the block's own parameter
 * area. The other three are guarded on an all-but-one-zero size table, and the
 * guard is written as `== 0 -> store NULL` rather than `!= 0 -> allocate`,
 * because the target emits `cmp / moveq / streq / beq` -- a predicated store of
 * NULL, not a branch around the assignment.
 *
 * (m2c gets the guard the right way round but then types `effectCount` as a plain
 * field read; the target is `ldrb [r4, #0x154]`, so the source is a `u8`.)
 *
 * Ends by publishing the effect count, zeroing the helper cursor, reseeding the
 * RNG and turning three display layers back on.
 */
void func_ov039_02088c50(TinPinSlammer_Scene* scene) {
    OtuPinStage* stage = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32          size;

    DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, &stage->params, data_ov039_020990fc[stage->stageIndex],
                                                    &data_ov039_0209a114, data_ov039_02099124[stage->stageIndex]));

    size = data_ov039_0209914c[stage->stageIndex];
    if (size == 0) {
        stage->bufA = NULL;
    } else {
        stage->bufA = Mem_AllocHeapTail(OTU_BOARD_HEAP(scene), size);
        DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, stage->bufA, data_ov039_0209914c[stage->stageIndex],
                                                        &data_ov039_0209a114, data_ov039_02099174[stage->stageIndex]));
    }

    size = data_ov039_0209919c[stage->stageIndex];
    if (size == 0) {
        stage->bufB = NULL;
    } else {
        stage->bufB = Mem_AllocHeapTail(OTU_BOARD_HEAP(scene), size);
        DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, stage->bufB, data_ov039_0209919c[stage->stageIndex],
                                                        &data_ov039_0209a114, data_ov039_020991c4[stage->stageIndex]));
    }

    size = data_ov039_020991ec[stage->stageIndex];
    if (size == 0) {
        stage->bufC = NULL;
    } else {
        stage->bufC = Mem_AllocHeapTail(OTU_BOARD_HEAP(scene), size);
        DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, stage->bufC, data_ov039_020991ec[stage->stageIndex],
                                                        &data_ov039_0209a114, data_ov039_02099214[stage->stageIndex]));
    }

    // `ldrb` into a word field: the byte is zero-extended and the count is
    // replaced, not incremented.
    stage->effectCount  = stage->params.effectMax;
    stage->helperCursor = 0;
    RNG_SetSeed(0);
    Display_SetMainLayers(0x13);
}

/* ============================================================================
 * 0x02088df0 / 0x02088ec4 -- the two ways into a round.
 * ==========================================================================*/

/**
 * @brief Starts a round from the countdown: fills the score rows from the pin
 *        trays, spawns one child per populated group, and raises the "round
 *        running" flag.
 *
 * `scene->state.unk_698` is the flag 020894cc polls; it is raised *first*, before
 * the countdown is even touched, which the scheduler makes look like an
 * afterthought. The child count starts at one -- for the local player, whose tray
 * slot is slot 0 and is never copied -- and is incremented per group whose
 * `groupCount` is non-zero.
 *
 * The tray copy runs slot i+1 -> group i, for i in 0..2, and takes the feature
 * header's OTU_PIN_TRAY: it already describes exactly these bytes
 * (0x4404C + slot * 0xE). The destination, 0x4408A + menuIndex * 0x34 +
 * slot * 0x10, is the header's OTU_SCORE_ROW -- which is also
 * `countdown->group[i].row`, and modelled here so the +4 group count is
 * reachable. (m2c swaps MI_CpuCopyU8's source and destination, because it did
 * not know the prototype is (src, dest, len).)
 */
void func_ov039_02088df0(TinPinSlammer_Scene* scene) {
    OtuPinStage*       stage = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuBoardCountdown* countdown;
    s32                i;

    scene->state.unk_698 = 1;

    countdown           = OTU_BOARD_COUNTDOWN(scene);
    stage->countdown    = countdown;
    stage->childCount   = 1;
    stage->stageIndex   = countdown->digits[0];
    stage->countdownAux = countdown->digits[1];

    for (i = 0; i < 3; i++) {
        MI_CpuCopyU8(OTU_PIN_TRAY(scene, i + 1), countdown->group[i].row, 0xE);

        if (countdown->group[i].groupCount != 0) {
            stage->childCount++;
        }
    }

    func_ov039_02088c50(scene);
    func_ov039_02088a8c(scene);
}

/**
 * @brief Resumes a round: same block, but the stage index comes from the scene
 *        rather than the countdown, and the children are spawned by 02088b80.
 *
 * The stage index is read from scene + 0x44A68, a word the feature header leaves
 * as padding inside its 0x44A68 run -- deliberately, since nothing else in the
 * overlay names it. It indexes both data_ov039_0209a360 and the eight bin tables,
 * so it has to be 0..9 in both entry paths, which is the only evidence that
 * `digits[0]` in 02088df0 is a single digit and not a score.
 */
void func_ov039_02088ec4(TinPinSlammer_Scene* scene) {
    OtuPinStage* stage = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32          which;

    scene->state.unk_698 = 0;

    which               = *(s32*)((u8*)scene + 0x44A68);
    stage->stageIndex   = which;
    stage->countdownAux = data_ov039_0209a360[which];

    func_ov039_02088c50(scene);
    func_ov039_02088b80(scene);
}

/* ============================================================================
 * 0x02088f18 -- teardown.
 * ==========================================================================*/

/**
 * @brief Stops the round: stop the sound, delete its task, wipe pool 2, and give
 *        the three heap buffers back.
 *
 * Every one of the three frees is guarded on a NULL pointer, and the third is
 * guarded with `popeq` -- it returns straight out of the frame rather than
 * branching to a shared tail, which is why the last free is not a branch. Pool 1
 * is *not* cleaned up here; 02089918 deletes the pool-1 tasks individually.
 */
void func_ov039_02088f18(TinPinSlammer_Scene* scene) {
    OtuPinStage* stage = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));

    CriSndMgr_Stop(0x18);
    EasyTask_DeleteTask(OTU_POOL1(scene), stage->soundTask);
    EasyTask_CleanupAllTasks(OTU_POOL2(scene));

    if (stage->bufA != NULL) {
        Mem_Free(OTU_BOARD_HEAP(scene), stage->bufA);
    }

    if (stage->bufB != NULL) {
        Mem_Free(OTU_BOARD_HEAP(scene), stage->bufB);
    }

    if (stage->bufC != NULL) {
        Mem_Free(OTU_BOARD_HEAP(scene), stage->bufC);
    }
}

/* ============================================================================
 * 0x02088fac -- pairwise pass one: claim.
 * ==========================================================================*/

/**
 * @brief Every child that can be resolved claims the others.
 *
 * The inner loop deliberately includes i itself and skips it with a comparison
 * rather than starting at i+1, which is what the target does -- so this is not
 * the same shape as the three passes below even though all four are "for each
 * pair".
 */
void func_ov039_02088fac(TinPinSlammer_Scene* scene) {
    OtuPinStage* stage = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuPinTask*  pin;
    s32          claimed;
    s32          i;
    s32          j;

    for (i = 0; i < stage->childCount; i++) {
        pin = (OtuPinTask*)EasyTask_GetTaskData(OTU_POOL2(scene), OTU_CHILD_ID(stage, i));

        claimed = func_ov039_0208e9f8(OTU_POOL2(scene), pin);
        if (claimed != 0) {
            for (j = 0; j < stage->childCount; j++) {
                if (i != j) {
                    func_ov039_0208eaa0(pin, EasyTask_GetTaskData(OTU_POOL2(scene), OTU_CHILD_ID(stage, j)));
                }
            }
        }
    }
}

/* ============================================================================
 * 0x02089064 / 0x020890d8 -- the effect placement helpers.
 *
 * Placed here rather than beside their callers because 02088fac is the first of
 * four pairwise passes and these two are called from inside a later one; the file
 * follows address order.
 * ==========================================================================*/

/**
 * @brief Hands a point to the next 16 helpers in the ring, then advances it.
 *
 * The cursor is a base index, not a running count: the target adds it to the loop
 * counter and only then scales by four, which is why the array is reached with
 * OTU_HELPER_ID(stage, cursor + i) rather than a walked pointer. The wrap is a
 * post-store clamp (`cmp #0x40 / movge #0 / strge`) on a value that is written
 * unconditionally first -- the same idiom as func_ov039_0208f048 in band 1.
 */
void func_ov039_02089064(TinPinSlammer_Scene* scene, OtuPoint* at) {
    OtuPinStage* stage = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32          i;

    for (i = 0; i < 0x10; i++) {
        func_ov039_02094e9c(EasyTask_GetTaskData(OTU_POOL2(scene), OTU_HELPER_ID(stage, i + stage->helperCursor)), at);
    }

    stage->helperCursor = stage->helperCursor + 0x10;
    if (stage->helperCursor >= 0x40) {
        stage->helperCursor = 0;
    }
}

/**
 * @brief Places one contact effect: rotated by a fixed bearing, off the midpoint
 *        of the two pins, on the board's own surface.
 *
 * Two s32 locals and three OtuPoints. The declaration order sets the frame:
 * `pa` at sp+0x10, `pb` at sp+0x08, `board` at sp+0x00, because mwcc hands the
 * first-declared local the highest offset.
 *
 * `pb` is the midpoint: it starts as pin b's position, is rotated by 0x800 (half
 * a turn) about pin a's, and then restored by the third vector call. The two
 * short-circuit tests are separate `if`s setting a flag rather than one boolean
 * expression, which is what the target's two `bne`/`beq` pairs are.
 *
 * The effect id is 0x33D when either pin is alive and 0x32F when neither is --
 * the same pair of constants OtuMeters's hammer uses for its states.
 */
void func_ov039_020890d8(TinPinSlammer_Scene* scene, OtuPinTask* a, OtuPinTask* b) {
    OtuPinStage* stage = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32          bothEmpty;
    s32          effectId;
    s32          t;
    OtuPoint     pa;    // sp+0x10
    OtuPoint     pb;    // sp+0x08
    OtuPoint     board; // sp+0x00

    // Both of these are short-circuit `||` in the target -- the second predicate
    // is only reached when the first is false -- so they are nested rather than
    // hoisted into two unconditional calls. The duplicated `= 1` / `= id` bodies
    // are what merge back into the target's single join point.
    bothEmpty = 0;
    t         = func_ov039_0208f0b0(a);
    if (t != 0) {
        bothEmpty = 1;
    } else {
        t = func_ov039_0208f0b0(b);
        if (t != 0) {
            bothEmpty = 1;
        }
    }

    func_ov039_0208e6e0(a, &pa);
    func_ov039_0208e6e0(b, &pb);
    func_ov039_02098bb0(&pb, &pa, &pb);
    func_ov039_02098bd4(0x800, &pb, &pb);
    func_ov039_02098b8c(&pb, &pa, &pb);

    // An effect between two absent pins has nothing to sit on, so the helper ring
    // is only advanced when both are present.
    if (bothEmpty == 0) {
        func_ov039_02089064(scene, &pb);
    }

    t = func_ov039_0208ee84(a);
    if (t != 0) {
        effectId = 0x33D;
    } else {
        t = func_ov039_0208ee84(b);
        if (t != 0) {
            effectId = 0x33D;
        } else {
            effectId = 0x32F;
        }
    }

    func_ov039_0208e85c(EasyTask_GetTaskData(OTU_POOL2(scene), OTU_CHILD_ID(stage, func_ov039_02088418(scene->state.unk_AF0))),
                        &board);
    func_ov039_02087d04(effectId, &pb, &board);
}

/* ============================================================================
 * 0x020891fc - 0x02089360 -- pairwise passes two to four.
 * ==========================================================================*/

/**
 * @brief Pass two: contact resolution. Each touching pair is flagged and then
 *        given its own effect.
 *
 * The inner loop starts at i+1, so each unordered pair is visited once. The outer
 * bound is `childCount - 1` and the target pre-tests it with
 * `sub r0, r0, #1 / cmp r0, #0 / pople` before entering -- the guard mwcc emits
 * for a counted loop whose bound it cannot prove positive.
 */
void func_ov039_020891fc(TinPinSlammer_Scene* scene) {
    OtuPinStage* stage = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuPinTask*  a;
    OtuPinTask*  b;
    s32          touching;
    s32          i;
    s32          j;

    for (i = 0; i < stage->childCount - 1; i++) {
        a = (OtuPinTask*)EasyTask_GetTaskData(OTU_POOL2(scene), OTU_CHILD_ID(stage, i));

        for (j = i + 1; j < stage->childCount; j++) {
            b = (OtuPinTask*)EasyTask_GetTaskData(OTU_POOL2(scene), OTU_CHILD_ID(stage, j));

            touching = func_ov039_0208e28c(a, b);
            if (touching != 0) {
                func_ov039_0208f03c(a);
                func_ov039_0208f03c(b);
                func_ov039_020890d8(scene, a, b);
            }
        }
    }
}

/**
 * @brief Pass three: push-apart, applied unconditionally to every pair.
 *
 * Same pair walk as 020891fc with the test removed, so this is the one pass whose
 * shape could be mistaken for a de-duplicated version of the other. It is not:
 * 020891fc visits j from i+1 and only acts on contact, this visits every pair.
 */
void func_ov039_020892c4(TinPinSlammer_Scene* scene) {
    OtuPinStage* stage = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));
    void*        a;
    s32          i;
    s32          j;

    for (i = 0; i < stage->childCount - 1; i++) {
        a = EasyTask_GetTaskData(OTU_POOL2(scene), OTU_CHILD_ID(stage, i));

        for (j = i + 1; j < stage->childCount; j++) {
            func_ov039_0208e37c(a, EasyTask_GetTaskData(OTU_POOL2(scene), OTU_CHILD_ID(stage, j)));
        }
    }
}

/**
 * @brief Pass four: every child is pulled towards every effect.
 *
 * The only pass with two *different* arrays as its two loop bounds -- children on
 * the outside, effects on the inside -- and the only one whose inner loop starts
 * at zero rather than at i+1, because a child and an effect are never the same
 * object and there is nothing to skip.
 */
void func_ov039_02089360(TinPinSlammer_Scene* scene) {
    OtuPinStage* stage = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));
    void*        pin;
    s32          i;
    s32          j;

    for (i = 0; i < stage->childCount; i++) {
        pin = EasyTask_GetTaskData(OTU_POOL2(scene), OTU_CHILD_ID(stage, i));

        for (j = 0; j < stage->effectCount; j++) {
            func_ov039_0208e504(pin, EasyTask_GetTaskData(OTU_POOL2(scene), OTU_EFFECT_ID(stage, j)));
        }
    }
}

/* ============================================================================
 * 0x020893fc -- the round's verdict.
 * ==========================================================================*/

/**
 * @brief Works out who won: 1 for a local win, 2 for a loss, 3 for a shared win.
 *
 * Builds a 16-bit mask of every child tied for the top score. The mask really is
 * 16 bits wide and the target says so: every set goes through an
 * `lsl #0x10 / lsr #0x10` round trip, and both the maximum (`best`, compared as
 * a 32-bit unsigned against a zero-extended halfword) and the mask are separate
 * locals. Written as `u16 mask` rather than m2c's `s32`, because a 32-bit mask
 * would not need those round trips and would not match.
 *
 * The three outcomes: the local player is not among the winners -> 2; the local
 * player is alone at the top -> 1; the local player shares the top with more than
 * one other -> 3, where the "more than one" test is a population count and is
 * skipped entirely when the scene mode is 0 (which 02088418 maps to player 0).
 *
 * Written with three returns rather than one accumulated local because the target
 * materialises the result in r1 and jumps to a single exit; see the mwcc-emission
 * note on `if`-inversion before changing this to an accumulator.
 */
s32 func_ov039_020893fc(TinPinSlammer_Scene* scene) {
    OtuPinStage* stage = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32          best;
    u16          mask;
    s32          me;
    s32          i;

    best = 0;
    mask = 0;

    for (i = 0; i < stage->childCount; i++) {
        u16 score = OTU_BOARD_SLOT_SCORE(i);

        if (score > best) {
            best = score;
            mask = (u16)(1 << i);
        } else if (best == score) {
            mask |= (u16)(1 << i);
        }
    }

    me = func_ov039_02088418(scene->state.unk_AF0);

    if ((mask & (1 << me)) == 0) {
        return 2;
    }

    if (scene->state.unk_AF0 != 0) {
        return (func_02047e84(mask) > 1) ? 3 : 1;
    }

    return 1;
}

/* ============================================================================
 * 0x020894cc -- the round's tick.
 * ==========================================================================*/

/**
 * @brief Advances the round, and finishes it once the children stop moving.
 *
 * Three exits. The round-state task reports "not yet" -> score every child,
 * decide the outcome, tell the sound task, and drop the running flag. It reports
 * "yes" -> one child (or none) is left and the rest are still settling: retract
 * the settled ones, re-score, and finish. Anything else -> run the four pairwise
 * passes and return, leaving the round live.
 *
 * The "at most one child left" test is written `count <= 1 && count < childCount`
 * rather than `count == 1`, because that is the two-compare form the target uses
 * (`cmp #1 / bgt` then `cmp / bge`) and because the two cases really do differ:
 * zero children left must not finish the round either.
 *
 * The re-scoring loops are near-copies of each other. That is deliberate -- the
 * target has two separate copies, differing only in the retraction test, and the
 * first one is in the other half of the function, 250 bytes away.
 *
 * (m2c drops the task argument from six calls in here -- 0208f034, 0208f00c,
 * 02094204, 0209420c, 0208f024, 020934e0 -- all of which the target passes a
 * freshly fetched task in r0.)
 */
void func_ov039_020894cc(TinPinSlammer_Scene* scene) {
    OtuPinStage* stage = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32          settled;
    u16          settledMask;
    void*        pin;
    s32          notYet;
    s32          hasPin;
    s32          me;
    s32          i;

    if (scene->state.unk_698 == 0) {
        return;
    }

    notYet = func_ov039_02094204(EasyTask_GetTaskData(OTU_POOL2(scene), stage->fxTask));

    if (notYet == 0) {
        func_ov039_02096b48(EasyTask_GetTaskData(OTU_POOL1(scene), stage->soundTask), 0);

        for (i = 0; i < stage->childCount; i++) {
            pin                     = EasyTask_GetTaskData(OTU_POOL2(scene), OTU_CHILD_ID(stage, i));
            OTU_BOARD_SLOT_SCORE(i) = (u16)func_ov039_0208f034(pin);

            me = func_ov039_02088418(scene->state.unk_AF0);
            if (i == me) {
                func_ov039_0208f0c8(pin);
            }

            func_ov039_0208f104(pin);
        }

        stage->outcome = func_ov039_020893fc(scene);

        if (stage->params.kind == 1) {
            g_DisplaySettings.controls[DISPLAY_MAIN].layers &= ~0xC;
        }

        scene->state.unk_698 = 0;
        return;
    }

    settled     = 0;
    settledMask = 0;

    for (i = 0; i < stage->childCount; i++) {
        pin = EasyTask_GetTaskData(OTU_POOL2(scene), OTU_CHILD_ID(stage, i));

        hasPin = func_ov039_0208f00c(pin);
        if (hasPin != 0) {
            settled++;
            settledMask |= (u16)(1 << i);
        }
    }

    if ((settled <= 1) && (settled < stage->childCount)) {
        for (i = 0; i < stage->childCount; i++) {
            pin = EasyTask_GetTaskData(OTU_POOL2(scene), OTU_CHILD_ID(stage, i));

            if ((settledMask & (1 << i)) != 0) {
                func_ov039_0208f024(pin);
            }

            OTU_BOARD_SLOT_SCORE(i) = (u16)func_ov039_0208f034(pin);

            me = func_ov039_02088418(scene->state.unk_AF0);
            if (i == me) {
                func_ov039_0208f0c8(pin);
            }

            func_ov039_0208f104(pin);
        }

        stage->outcome = func_ov039_020893fc(scene);
        // The target's `lsl #0x10 / asr #0x10` pair is this call site's narrowing
        // cast, not the field's: `outcome` is a word and 02096b48 wants a half.
        func_ov039_02096b48(EasyTask_GetTaskData(OTU_POOL1(scene), stage->soundTask), (s16)stage->outcome);

        if (stage->params.kind == 1) {
            g_DisplaySettings.controls[DISPLAY_MAIN].layers &= ~0xC;
        }

        scene->state.unk_698 = 0;
        return;
    }

    // Still in play. Only re-point the animated palette once the palette task
    // says its first pass is done -- the note in OtuCounters records that this
    // routine refuses to re-point twice.
    notYet = func_ov039_0209420c(EasyTask_GetTaskData(OTU_POOL2(scene), stage->fxTask));

    if (notYet != 0) {
        func_ov039_020934e0(EasyTask_GetTaskData(OTU_POOL2(scene), stage->bgTaskC));
    }

    func_ov039_0208871c(scene);
    func_ov039_02088fac(scene);
    func_ov039_020891fc(scene);
    func_ov039_020892c4(scene);
    func_ov039_02089360(scene);
}

/* ============================================================================
 * 0x02089780 - 0x02089918 -- the four stage entry points.
 * ==========================================================================*/

/**
 * @brief Enters the board stage: raises the block's active flag, latches the
 *        touch state, starts a fresh round, and fades in.
 *
 * The fade is FADER_INSTANT (3) at brightness 0 over 30 steps, and it comes
 * *after* the round is already running -- the fade is a reveal, not a gate.
 *
 * (m2c reads this as a zero-argument function and reports every access through
 * r0 as an error; the scene arrives in r0 and is live across the first call.)
 */
void func_ov039_02089780(TinPinSlammer_Scene* scene) {
    OtuPinStage* stage = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));

    stage->active = 1;
    func_ov039_02088564(func_ov039_02088440(0));
    func_ov039_02088df0(scene);
    CriSndMgr_PlayFile(0x18);
    EasyFade_FadeBothDisplays(FADER_INSTANT, 0, 0x1E);
}

/**
 * @brief Leaves it. A twelve-byte tail-call thunk onto 02088f18 and nothing else
 *        -- `ldr ip, .L / bx ip`, with no frame at all.
 *
 * Written as a plain call, following the precedent of func_ov039_0208f024 in band
 * 1: mwcc turns a one-call body into the same two instructions.
 */
void func_ov039_020897d0(TinPinSlammer_Scene* scene) {
    func_ov039_02088f18(scene);
}

/**
 * @brief The stage's update: latches the touch state, and once the round has
 *        finished and the sound task has gone quiet, pushes the next stage.
 *
 * Both guards are on the same frame -- the round must be over (`state.unk_698`
 * clear, i.e. 020894cc has finished) *and* the sound task must have stopped
 * playing -- so the transition is not taken the instant the last pin settles but
 * a frame later, after the result jingle has had its first tick.
 *
 * The stage pointer at +0x17C is fetched and then discarded, which the target
 * really does: `EasyTask_GetTaskData` is called and its result is overwritten by
 * the next load. Reproduced rather than tidied.
 *
 * The result code written to `gSaveData.unk_24B4` is 6 for outcome 1 and 7 for
 * anything else -- two constants, chosen by a `cmp #1` with the bodies identical
 * apart from them, so this is an `if`/`else` and not a table.
 */
void func_ov039_020897dc(TinPinSlammer_Scene* scene) {
    OtuPinStage* stage = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32          quiet;

    func_ov039_02088564(func_ov039_02088440(0));

    if (scene->state.unk_698 == 0) {
        quiet = func_ov039_02096c44(EasyTask_GetTaskData(OTU_POOL1(scene), stage->soundTask));

        if (quiet == 0) {
            EasyTask_GetTaskData(OTU_POOL2(scene), OTU_CHILD_ID(stage, func_ov039_02088418(scene->state.unk_AF0)));

            if (stage->outcome == 1) {
                gSaveData.unk_24B4 = 6;
                func_ov039_02098a40(OTU_STAGE(scene), NULL);
            } else {
                gSaveData.unk_24B4 = 7;
                func_ov039_02098a40(OTU_STAGE(scene), NULL);
            }
        }
    }

    // Unconditional, and last: the tick runs even on the frame the transition
    // was taken.
    func_ov039_020894cc(scene);
}

/**
 * @brief Enters the board stage's *other* variant: the same active flag, but the
 *        child count comes from the scene and two pool-1 sound tasks are made.
 *
 * 0x258 at +0x174 is set here and read by nothing in this batch -- it is
 * presumably the round timer the update consumes, but nothing decompiled shows
 * it, so it stays named for the offset. This is the only place the child count is
 * not derived from the countdown, which is why 02088b80 (not 02088a8c) is the
 * spawner this entry point needs.
 */
void func_ov039_020898a8(TinPinSlammer_Scene* scene) {
    OtuPinStage* stage = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));

    stage->active     = 1;
    stage->timer      = 0x258;
    stage->childCount = scene->state.unk_EE0;
    stage->soundTask2 = func_ov039_02096e4c(OTU_POOL1(scene), scene->base.spareDataType);
    stage->soundTask3 = func_ov039_020989f0(OTU_POOL1(scene), scene->base.spareDataType);
}

/**
 * @brief Leaves that variant: delete the pool-1 sound task this entry point made,
 *        then the common teardown.
 *
 * Only one of the two pool-1 tasks is deleted. The other, at +0x2CC, has no delete
 * anywhere in this batch; it is presumably cleaned up with pool 1 itself by
 * whatever owns the pool.
 */
void func_ov039_02089918(TinPinSlammer_Scene* scene) {
    OtuPinStage* stage = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));

    EasyTask_DeleteTask(OTU_POOL1(scene), stage->soundTask2);
    func_ov039_02088f18(scene);
}
