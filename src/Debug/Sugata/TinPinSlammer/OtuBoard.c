/**
 * @file OtuBoard.c
 * @brief The single-player board stage: the round's setup, per-frame physics and
 *        outcome, the wireless scan callback, and the stages' enter and exit.
 */

#include "OtuFieldAccessShared.h"

/* The overlay's own bin identifier for this stage's data, and the eight 10-entry
 * tables that go with it. Still gap-filled from the ROM. */
extern const BinIdentifier data_ov039_0209a114;

/* Offsets and sizes of the inline parameter block at stage+0x150. */
extern const s32 data_ov039_020990fc[10];
extern const s32 data_ov039_02099124[10];

/* Size and bin offset of each of the three heap buffers. */
extern const s32 data_ov039_0209914c[10];
extern const s32 data_ov039_02099174[10];
extern const s32 data_ov039_0209919c[10];
extern const s32 data_ov039_020991c4[10];
extern const s32 data_ov039_020991ec[10];
extern const s32 data_ov039_02099214[10];

/* Score-row seeds, indexed by the same stage index as the tables above. */
extern const s32 data_ov039_0209a360[10]; // Size: 0x34

/** ov040's packet hand-off. */
void func_ov040_0209cb98(WMBssDesc* parent, s32 which);

/* ============================================================================
 * 0x02088698 -- the wireless scan callback.
 * ==========================================================================*/

/**
 * @brief Accepts a scanned parent's beacon if it is an Otosu game in entry mode
 *        advertising this stage's `packetKind`, and keeps it to connect to.
 */
// Nonmatching: 96.6%. mwcc merges the first two early returns into one
// conditional chain where the target tests them separately.
void func_ov039_02088698(WMBssDesc* bss, TinPinSlammer_Scene* scene) {
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));

    if (stage == NULL) {
        return;
    }

    if (bss->gameInfoLength == 0) {
        return;
    }

    if (bss->gameInfo.userGameInfoLength != 0x70) {
        return;
    }

    if ((bss->gameInfo.gameNameCount_attribute & 1) == 0) {
        return;
    }

    if (bss->gameInfo.userGameInfo[0] != stage->packetKind) {
        return;
    }

    stage->parent    = *bss;
    stage->gotPacket = 1;
    func_ov040_0209cb98(bss, 1);
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
 */
void func_ov039_0208871c(TinPinSlammer_Scene* scene) {
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));
    void*          pin;
    s32            event;
    OtuPoint       at;
    OtuPoint       mine;
    OtuPoint       other;
    s32            me;
    s32            i;

    pin = EasyTask_GetTaskData(OTU_POOL2(scene), stage->badgeIds[func_ov039_02088418(scene->multiplayer)]);
    func_ov039_0208e6fc(pin, &at);
    event = func_ov039_0208eff8(pin);
    func_ov039_0208e848(pin, &at);
    func_ov039_0208e870(pin, at.x, at.y);

    pin = EasyTask_GetTaskData(OTU_POOL2(scene), stage->floorId);
    func_ov039_02092348(pin, at.x, at.y);
    func_ov039_020923b4(pin, event);
    pin = EasyTask_GetTaskData(OTU_POOL2(scene), stage->bgId);
    func_ov039_02092d0c(pin, at.x, at.y);
    func_ov039_02092e04(pin, event);

    // The local pin is skipped: its aim point is already in `at`. The mode
    // selector is re-read and re-resolved inside the loop, which the target does
    // on every iteration rather than hoisting.
    for (i = 0; i < stage->badgeCount; i++) {
        me = func_ov039_02088418(scene->multiplayer);
        if (i == me) {
            continue;
        }

        pin = EasyTask_GetTaskData(OTU_POOL2(scene), stage->badgeIds[i]);
        func_ov039_0208e848(pin, &at);
        func_ov039_0208e6fc(pin, &other);
        func_ov039_0208e870(pin, other.x, other.y);
    }

    for (i = 0; i < 0x40; i++) {
        func_ov039_02094e88(EasyTask_GetTaskData(OTU_POOL2(scene), stage->sparkIds[i]), &at);
    }

    for (i = 0; i < stage->obstacleCount; i++) {
        func_ov039_02092730(EasyTask_GetTaskData(OTU_POOL2(scene), stage->obstacleIds[i]), &at);
    }
}

/* ============================================================================
 * 0x020888e0 -- (re)create every helper, effect and stage task.
 * ==========================================================================*/

/**
 * @brief Tears down nothing and rebuilds everything: 64 helpers, the effect
 *        pool, seven stage tasks, then republishes the geometry.
 *
 * The value passed to every factory is `scene->spareDataType` (scene +
 * 0x11584) and it is *re-read from memory* at each call rather than kept in a
 * register -- the target holds `scene + 0x11000` in a callee-saved register and
 * loads `[rX, #0x584]` afresh, which is what stops a local from reproducing it.
 * The target adds the heap's offset as `+ 0x18C` then `+ 0x11400`, which is why
 * OTU_BOARD_HEAP exists.
 *
 * Six of the seven factories take the block's parameter block by address; only
 * 020941d0 takes a word instead, and 02095468/02094ab4 take a pin handle.
 */
void func_ov039_020888e0(TinPinSlammer_Scene* scene) {
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32            i;

    for (i = 0; i < 0x40; i++) {
        stage->sparkIds[i] = OtosuGame_spark_CreateTask(OTU_POOL2(scene), scene->spareDataType);
    }

    for (i = 0; i < stage->obstacleCount; i++) {
        // bufB is walked eight bytes at a time and handed on by address; nothing
        // here interprets the record.
        stage->obstacleIds[i] =
            OtosuGame_obstacle_CreateTask(OTU_POOL2(scene), scene->spareDataType, stage->layout.kind, stage->layout.variant,
                                          (OtosuGame_obstacle_Params*)(stage->layout.obstacles + i * 8));
    }

    stage->slashId = OtosuGame_slash_CreateTask(OTU_POOL2(scene), scene->spareDataType,
                                                stage->badgeIds[func_ov039_02088418(scene->multiplayer)]);
    stage->floorId = OtosuGame_floor_CreateTask(OTU_POOL2(scene), scene->spareDataType, OTU_HEAP(scene), &stage->layout);
    stage->bgId    = OtosuGame_bg_CreateTask(OTU_POOL2(scene), scene->spareDataType, OTU_HEAP(scene), &stage->layout);
    stage->ovbgId  = OtosuGame_ovbg_CreateTask(OTU_POOL2(scene), scene->spareDataType, OTU_HEAP(scene), &stage->layout);
    stage->timerId = OtosuGame_timer_CreateTask(OTU_POOL2(scene), scene->spareDataType, stage->timerSeconds);
    stage->gaugeId = OtosuGame_specialgauge_CreateTask(OTU_POOL2(scene), scene->spareDataType,
                                                       stage->badgeIds[func_ov039_02088418(scene->multiplayer)]);
    // The one task in this function that lives in pool 1.
    stage->gameoverId = OtosuGame_gameover_CreateTask(OTU_POOL1(scene), scene->spareDataType);

    func_ov039_0208871c(scene);
}

/* ============================================================================
 * 0x02088a8c / 0x02088b80 -- the two pin-children spawners.
 * ==========================================================================*/

/**
 * @brief The first-game spawner: one child per populated match group.
 *
 * Called only from 02088df0, which has just filled the match from the pin
 * trays, so `childCount` here is one plus the number of non-empty groups. The
 * per-child sprite base is therefore *this* group's own start: for i > 0 it is
 * the previous group's cell count scaled by 0x22 and added to scene + 0x44974,
 * which is what spreads three children across three menu columns. For i == 0
 * there is no previous group and the base is NULL.
 *
 * The tenth argument is that base and the ninth is the "first child" flag; both
 * are computed from the same `i == 0` test the target makes three times, once
 * per predicate, which is why they are spelled as separate comparisons rather
 * than one hoisted flag.
 */
void func_ov039_02088a8c(TinPinSlammer_Scene* scene) {
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32            i;

    for (i = 0; i < stage->badgeCount; i++) {
        stage->badgeIds[i] = OtosuGame_badge_CreateTask(
            OTU_POOL2(scene), scene->spareDataType, i, (i == 0) ? func_ov039_02088440(i) : (void*)0, &stage->layout, scene,
            scene->badgeParams, scene->decks[i], (i == 0), (i == 0) ? (u8*)0 : scene->ai[stage->match->opponents[i - 1].ai]);
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
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32            i;

    for (i = 0; i < stage->badgeCount; i++) {
        BOOL local = (i == func_ov039_02088418(scene->multiplayer));

        stage->badgeIds[i] = OtosuGame_badge_CreateTask(OTU_POOL2(scene), scene->spareDataType, i, func_ov039_02088440(i),
                                                        &stage->layout, scene, scene->badgeParams, scene->decks[i], local, 0);
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
 * `effectCount` is a `u8`: the target reads it with `ldrb [r4, #0x154]`.
 *
 * Ends by publishing the effect count, zeroing the helper cursor, reseeding the
 * RNG and turning three display layers back on.
 */
void func_ov039_02088c50(TinPinSlammer_Scene* scene) {
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32            size;

    DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, &stage->layout, data_ov039_020990fc[stage->stageIndex],
                                                    &data_ov039_0209a114, data_ov039_02099124[stage->stageIndex]));

    size = data_ov039_0209914c[stage->stageIndex];
    if (size == 0) {
        stage->layout.cells = NULL;
    } else {
        stage->layout.cells = Mem_AllocHeapTail(OTU_HEAP(scene), size);
        DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, stage->layout.cells, data_ov039_0209914c[stage->stageIndex],
                                                        &data_ov039_0209a114, data_ov039_02099174[stage->stageIndex]));
    }

    size = data_ov039_0209919c[stage->stageIndex];
    if (size == 0) {
        stage->layout.obstacles = NULL;
    } else {
        stage->layout.obstacles = Mem_AllocHeapTail(OTU_HEAP(scene), size);
        DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, stage->layout.obstacles, data_ov039_0209919c[stage->stageIndex],
                                                        &data_ov039_0209a114, data_ov039_020991c4[stage->stageIndex]));
    }

    size = data_ov039_020991ec[stage->stageIndex];
    if (size == 0) {
        stage->layout.warps = NULL;
    } else {
        stage->layout.warps = Mem_AllocHeapTail(OTU_HEAP(scene), size);
        DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, stage->layout.warps, data_ov039_020991ec[stage->stageIndex],
                                                        &data_ov039_0209a114, data_ov039_02099214[stage->stageIndex]));
    }

    // `ldrb` into a word field: the byte is zero-extended and the count is
    // replaced, not incremented.
    stage->obstacleCount = stage->layout.obstacleCount;
    stage->sparkCursor   = 0;
    RNG_SetSeed(0);
    Display_SetMainLayers(0x13);
}

/* ============================================================================
 * 0x02088df0 / 0x02088ec4 -- the two ways into a round.
 * ==========================================================================*/

/**
 * @brief Starts a round from the match: fills the score rows from the pin
 *        trays, spawns one child per populated group, and raises the "round
 *        running" flag.
 *
 * `scene->playing` is the flag 020894cc polls; it is raised *first*, before
 * the match is even touched, which the scheduler makes look like an
 * afterthought. The child count starts at one -- for the local player, whose tray
 * slot is slot 0 and is never copied -- and is incremented per group whose
 * `groupCount` is non-zero.
 *
 * The tray copy runs slot i+1 -> group i, for i in 0..2, and takes the feature
 * header's OTU_PIN_TRAY: it already describes exactly these bytes
 * (0x4404C + slot * 0xE). The destination, 0x4408A + menuIndex * 0x34 +
 * slot * 0x10, is the header's OTU_SCORE_ROW -- which is also
 * `match->opponents[i].deck`, and modelled here so the +4 group count is
 * reachable.
 */
void func_ov039_02088df0(TinPinSlammer_Scene* scene) {
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32            i;

    scene->playing = 1;

    stage->match        = &scene->matches[scene->matchIndex];
    stage->badgeCount   = 1;
    stage->stageIndex   = stage->match->board;
    stage->timerSeconds = stage->match->timeLimit;

    for (i = 0; i < 3; i++) {
        MI_CpuCopyU8(stage->match->opponents[i].deck, scene->decks[i + 1], sizeof(scene->decks[0]));

        if (stage->match->opponents[i].ai != 0) {
            stage->badgeCount++;
        }
    }

    func_ov039_02088c50(scene);
    func_ov039_02088a8c(scene);
}

/**
 * @brief Resumes a round: same block, but the stage index comes from the scene
 *        rather than the match, and the children are spawned by 02088b80.
 *
 * The stage index is read from scene + 0x44A68, a word the feature header leaves
 * as padding inside its 0x44A68 run -- deliberately, since nothing else in the
 * overlay names it. It indexes both data_ov039_0209a360 and the eight bin tables,
 * so it has to be 0..9 in both entry paths, which is the only evidence that
 * `digits[0]` in 02088df0 is a single digit and not a score.
 */
void func_ov039_02088ec4(TinPinSlammer_Scene* scene) {
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32            which;

    scene->playing = 0;

    which               = scene->boardIndex;
    stage->stageIndex   = which;
    stage->timerSeconds = data_ov039_0209a360[which];

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
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));

    CriSndMgr_Stop(0x18);
    EasyTask_DeleteTask(OTU_POOL1(scene), stage->gameoverId);
    EasyTask_CleanupAllTasks(OTU_POOL2(scene));

    if (stage->layout.cells != NULL) {
        Mem_Free(OTU_HEAP(scene), stage->layout.cells);
    }

    if (stage->layout.obstacles != NULL) {
        Mem_Free(OTU_HEAP(scene), stage->layout.obstacles);
    }

    if (stage->layout.warps != NULL) {
        Mem_Free(OTU_HEAP(scene), stage->layout.warps);
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
    OtuBoardStage*   stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));
    OtosuGame_badge* pin;
    s32              claimed;
    s32              i;
    s32              j;

    for (i = 0; i < stage->badgeCount; i++) {
        pin = (OtosuGame_badge*)EasyTask_GetTaskData(OTU_POOL2(scene), stage->badgeIds[i]);

        claimed = func_ov039_0208e9f8(OTU_POOL2(scene), pin);
        if (claimed != 0) {
            for (j = 0; j < stage->badgeCount; j++) {
                if (i != j) {
                    func_ov039_0208eaa0(pin, EasyTask_GetTaskData(OTU_POOL2(scene), stage->badgeIds[j]));
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
 * stage->sparkIds[cursor + i] rather than a walked pointer. The wrap is a
 * post-store clamp (`cmp #0x40 / movge #0 / strge`) on a value that is written
 * unconditionally first -- the same idiom as func_ov039_0208f048.
 */
void func_ov039_02089064(TinPinSlammer_Scene* scene, OtuPoint* at) {
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32            i;

    for (i = 0; i < 0x10; i++) {
        func_ov039_02094e9c(EasyTask_GetTaskData(OTU_POOL2(scene), stage->sparkIds[i + stage->sparkCursor]), at);
    }

    stage->sparkCursor = stage->sparkCursor + 0x10;
    if (stage->sparkCursor >= 0x40) {
        stage->sparkCursor = 0;
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
 * the same pair of constants the hammer task uses for its states.
 */
void func_ov039_020890d8(TinPinSlammer_Scene* scene, OtosuGame_badge* a, OtosuGame_badge* b) {
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32            bothEmpty;
    s32            effectId;
    OtuPoint       pa;    // sp+0x10
    OtuPoint       pb;    // sp+0x08
    OtuPoint       board; // sp+0x00

    bothEmpty = 0;
    if (func_ov039_0208f0b0(a) != 0 || func_ov039_0208f0b0(b) != 0) {
        bothEmpty = 1;
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

    if (func_ov039_0208ee84(a) != 0 || func_ov039_0208ee84(b) != 0) {
        effectId = 0x33D;
    } else {
        effectId = 0x32F;
    }

    func_ov039_0208e85c(EasyTask_GetTaskData(OTU_POOL2(scene), stage->badgeIds[func_ov039_02088418(scene->multiplayer)]),
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
    OtuBoardStage*   stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));
    OtosuGame_badge* a;
    OtosuGame_badge* b;
    s32              touching;
    s32              i;
    s32              j;

    for (i = 0; i < stage->badgeCount - 1; i++) {
        a = (OtosuGame_badge*)EasyTask_GetTaskData(OTU_POOL2(scene), stage->badgeIds[i]);

        for (j = i + 1; j < stage->badgeCount; j++) {
            b = (OtosuGame_badge*)EasyTask_GetTaskData(OTU_POOL2(scene), stage->badgeIds[j]);

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
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));
    void*          a;
    s32            i;
    s32            j;

    for (i = 0; i < stage->badgeCount - 1; i++) {
        a = EasyTask_GetTaskData(OTU_POOL2(scene), stage->badgeIds[i]);

        for (j = i + 1; j < stage->badgeCount; j++) {
            func_ov039_0208e37c(a, EasyTask_GetTaskData(OTU_POOL2(scene), stage->badgeIds[j]));
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
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));
    void*          pin;
    s32            i;
    s32            j;

    for (i = 0; i < stage->badgeCount; i++) {
        pin = EasyTask_GetTaskData(OTU_POOL2(scene), stage->badgeIds[i]);

        for (j = 0; j < stage->obstacleCount; j++) {
            func_ov039_0208e504(pin, EasyTask_GetTaskData(OTU_POOL2(scene), stage->obstacleIds[j]));
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
 * locals. Written as `u16 mask` rather than `s32`, because a 32-bit mask
 * would not need those round trips and would not match.
 *
 * The three outcomes: the local player is not among the winners -> 2; the local
 * player is alone at the top -> 1; the local player shares the top with more than
 * one other -> 3, where the "more than one" test is a population count and is
 * skipped entirely when the scene mode is 0 (which 02088418 maps to player 0).
 *
 * The result is one accumulated local with a single exit, and the "is a winner"
 * test is the outer `if`, which is how the target lays out its r1 result.
 */
s32 func_ov039_020893fc(TinPinSlammer_Scene* scene) {
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));
    u16            mask;
    s32            me;
    s32            i;
    u32            best;
    s32            result;

    best = 0;
    mask = 0;

    for (i = 0; i < stage->badgeCount; i++) {
        u16 score = gSaveData.otosuScores[i];

        if (score > best) {
            best = score;
            mask = (u16)(1 << i);
        } else if (best == score) {
            mask |= 1 << i;
        }
    }

    me = func_ov039_02088418(scene->multiplayer);

    if (mask & (1 << me)) {
        if (scene->multiplayer != 0) {
            result = ((u32)func_02047e84(mask) > 1) ? 3 : 1;
        } else {
            result = 1;
        }
    } else {
        result = 2;
    }

    return result;
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
 */
void func_ov039_020894cc(TinPinSlammer_Scene* scene) {
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32            settled;
    u16            settledMask;
    void*          pin;
    s32            notYet;
    s32            hasPin;
    s32            me;
    s32            i;

    if (scene->playing == 0) {
        return;
    }

    notYet = func_ov039_02094204(EasyTask_GetTaskData(OTU_POOL2(scene), stage->timerId));

    if (notYet == 0) {
        func_ov039_02096b48(EasyTask_GetTaskData(OTU_POOL1(scene), stage->gameoverId), 0);

        for (i = 0; i < stage->badgeCount; i++) {
            pin                      = EasyTask_GetTaskData(OTU_POOL2(scene), stage->badgeIds[i]);
            gSaveData.otosuScores[i] = (u16)func_ov039_0208f034(pin);

            me = func_ov039_02088418(scene->multiplayer);
            if (i == me) {
                func_ov039_0208f0c8(pin);
            }

            func_ov039_0208f104(pin);
        }

        stage->outcome = func_ov039_020893fc(scene);

        if (stage->layout.kind == 1) {
            g_DisplaySettings.controls[DISPLAY_MAIN].layers &= ~0xC;
        }

        scene->playing = 0;
        return;
    }

    settled     = 0;
    settledMask = 0;

    for (i = 0; i < stage->badgeCount; i++) {
        pin = EasyTask_GetTaskData(OTU_POOL2(scene), stage->badgeIds[i]);

        hasPin = func_ov039_0208f00c(pin);
        if (hasPin != 0) {
            settled++;
            settledMask |= 1 << i;
        }
    }

    if ((settled <= 1) && (settled < stage->badgeCount)) {
        for (i = 0; i < stage->badgeCount; i++) {
            pin = EasyTask_GetTaskData(OTU_POOL2(scene), stage->badgeIds[i]);

            if ((settledMask & (1 << i)) != 0) {
                func_ov039_0208f024(pin);
            }

            gSaveData.otosuScores[i] = (u16)func_ov039_0208f034(pin);

            me = func_ov039_02088418(scene->multiplayer);
            if (i == me) {
                func_ov039_0208f0c8(pin);
            }

            func_ov039_0208f104(pin);
        }

        stage->outcome = func_ov039_020893fc(scene);
        // The target's `lsl #0x10 / asr #0x10` pair is this call site's narrowing
        // cast, not the field's: `outcome` is a word and 02096b48 wants a half.
        func_ov039_02096b48(EasyTask_GetTaskData(OTU_POOL1(scene), stage->gameoverId), (s16)stage->outcome);

        if (stage->layout.kind == 1) {
            g_DisplaySettings.controls[DISPLAY_MAIN].layers &= ~0xC;
        }

        scene->playing = 0;
        return;
    }

    // Still in play. On the frame the timer's last-twenty-seconds alarm fires,
    // switch the overlay background's palette too.
    notYet = func_ov039_0209420c(EasyTask_GetTaskData(OTU_POOL2(scene), stage->timerId));

    if (notYet != 0) {
        func_ov039_020934e0(EasyTask_GetTaskData(OTU_POOL2(scene), stage->ovbgId));
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
 */
void func_ov039_02089780(TinPinSlammer_Scene* scene) {
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));

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
 * Written as a plain call, like func_ov039_0208f024: mwcc turns a one-call body into the same two instructions.
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
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32            quiet;

    func_ov039_02088564(func_ov039_02088440(0));

    if (scene->playing == 0) {
        quiet = func_ov039_02096c44(EasyTask_GetTaskData(OTU_POOL1(scene), stage->gameoverId));

        if (quiet == 0) {
            EasyTask_GetTaskData(OTU_POOL2(scene), stage->badgeIds[func_ov039_02088418(scene->multiplayer)]);

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
 * The timer starts at 0x258 frames; the wireless steps count it down and flag
 * a link timeout when it runs out. This is the only place the child count is
 * not derived from the match, which is why 02088b80 (not 02088a8c) is the
 * spawner this entry point needs.
 */
void func_ov039_020898a8(TinPinSlammer_Scene* scene) {
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));

    stage->active     = 1;
    stage->timer      = 0x258;
    stage->badgeCount = scene->playerCount;
    stage->wriconId   = OtosuGame_wricon_CreateTask(OTU_POOL1(scene), scene->spareDataType);
    stage->wrwaitId   = OtosuGame_wrwait_CreateTask(OTU_POOL1(scene), scene->spareDataType);
}

/**
 * @brief Leaves that variant: delete the pool-1 sound task this entry point made,
 *        then the common teardown.
 *
 * Only one of the two pool-1 tasks is deleted. The other, at +0x2CC, has no delete
 * anywhere in the overlay; it is presumably cleaned up with pool 1 itself by
 * whatever owns the pool.
 */
void func_ov039_02089918(TinPinSlammer_Scene* scene) {
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));

    EasyTask_DeleteTask(OTU_POOL1(scene), stage->wriconId);
    func_ov039_02088f18(scene);
}
