#include "OtuFieldAccessShared.h"

/*
 * ov039 region 0x02089950 - 0x0208acc0. One translation unit of the
 * overlay; dsd gives each file a single contiguous `.text` claim. The
 * shared types, externs and prototypes are in OtuFieldAccessShared.h.
 */
// Size: 0xF0 (only 0xE8..0xEF are known)

/* ------------------------------------------------------------------ */
/* Addressing helpers whose *shape* is load-bearing.                   */
/* ------------------------------------------------------------------ */

/*
 * The rotating child index, split as `task + 0x100 + 0xA0`.
 *
 * The target forms the address with an `add` and a `+0xA0` displacement rather
 * than one folded `ldrsh [task, #0x1A0]`, and that is a valid alternative
 * encoding this build does not produce. OtuFieldAccess.c records the same thing
 * for the four halfwords at `task + 0x100 + 0x78..0x7E`, which is why the split
 * is spelled out here rather than left to a field access.
 */
#define OTU_PIN_SLOT(task) (*(s16*)((u8*)(task) + 0x100 + 0xA0))

/*
 * The pin tray, reached as `scene + 0x4C + 0x44000` -- two adds.
 *
 * The target never folds the pair into one displacement, and 0x4404C is not
 * encodable as an ARM immediate, so mwcc has to split it somehow. Writing it
 * split is what keeps it splitting the same way; the same reasoning is recorded
 * for OTU_DIGIT_COLUMN in the shared header. Note this is *not*
 * `OTU_PIN_TRAY(scene, slot)`, whose slot multiply folds the 0x44000 away.
 */
#define OTU_TRAY(scene) ((u8*)(scene) + 0x4C + 0x44000)

/** The wireless save record, reached the way 02082c50 reaches it. */
#define OTU_BOARD_RECORD OTU_WIRELESS_RECORD(0x3000, 0)

/* ================================================================== */
/* 0x02089950 -- the first board variant's pre-update stage.          */
/* ================================================================== */

/**
 * @brief Counts the stage's 0x258-frame timer down, then reacts to the
 *        wireless link state.
 *
 * The timer is a saturating counter: it is decremented while positive and the
 * scene's `state.unk_EE8` is raised on the frame it runs out, which is the same
 * flag `func_ov039_02098a50`'s neighbours read to force a transition.
 *
 * The `switch` on `func_ov040_0209cb78()` is a switch rather than an
 * if/else-if chain: the three compares all precede the first body, and each body
 * ends in the shared epilogue rather than branching past its successors. Cases
 * are in ascending order, which is also their emission order.
 *
 * Case 7 is the only one that arms anything: it stores 1 into the block's +0x06
 * and hands `&block[0x06]` to the wireless stack, so +0x06 is a two-byte state
 * word rather than a flag followed by unrelated padding.
 */
void func_ov039_02089950(TinPinSlammer_Scene* scene) {
    OtuWirelessStage* stage = (OtuWirelessStage*)func_ov039_02098b70(OTU_STAGE(scene));

    if (stage->timer > 0) {
        stage->timer = stage->timer - 1;
    } else {
        scene->state.unk_EE8 = 1;
    }

    switch (func_ov040_0209cb78()) {
        case 1:
            func_ov040_0209d990();
            func_ov040_0209caac(0x400548);
            func_ov040_0209cb9c();
            return;

        case 5:
            if (stage->childCount != func_ov040_0209cb68()) {
                return;
            }
            stage->state = 0;
            func_ov039_02098a50(OTU_STAGE(scene));
            return;

        case 7: {
            s32 a = func_ov040_0209cde4();
            s32 b = func_020442f8();

            func_ov040_0209d818(3, 6, 0x10, 2, 0x10, 2);
            func_ov040_0209d848(3);
            // The two receive callbacks have different arities -- 0x020885f4
            // takes (record, scene) and 0x02088688, which OtuFieldAccess.c
            // already defines, takes (scene) alone -- so both are passed through
            // the stack's untyped callback slot. The target stores nothing but the
            // bare address in each case.
            func_ov040_0209d40c((void (*)())func_ov039_020885f4, scene);
            func_ov040_0209d420((void (*)())func_ov039_02088688, scene);

            stage->unk_06 = 1;
            func_ov040_0209cb08(&stage->unk_06, 2);
            func_ov040_0209d0a8(4, b, a);
            return;
        }

        default:
            return;
    }
}

/* ================================================================== */
/* 0x02089a80 -- the first board variant's update stage.               */
/* ================================================================== */

/**
 * @brief The stage machine proper: one switch over the block's `state`, 0 to 8.
 *
 * Stages 1, 4 and 6 are the same loop three times over -- walk the children
 * until one is *not* in the phase the stage is waiting for, and bail if the walk
 * ran off the end. All three have the identical shape: an unconditional branch
 * into the condition (a rotated loop with no entry guard), the phase test with a
 * forward exit, and the count re-read at the bottom beside the increment. They
 * differ only in the phase byte they compare against and the
 * `func_ov039_02087c8c` menu they advance to, which is why three of the eleven
 * case bodies are byte-identical to each other. Stages 0 and 4's twins in
 * 0x0208a098 use the same loop; only 0x02089d6c and 0x0208a354, whose phase-4
 * wait carries a `count > 0` guard, are written as a guarded do/while.
 *
 * Stage 2 is the only body that loops over something other than the children:
 * it builds a bitmask of `1 << i` for `i` in 1..count-1 and hands it to the
 * wireless stack. The mask is a `u16`, which is what the `lsl #0x10 / lsr #0x10`
 * round trip inside the loop is; it is *not* a cast added by this transcription.
 *
 * Stage 8 is the teardown, and it is the one case that does not end in a
 * `func_ov039_02087c8c`: it deletes the fade task and hands the container back.
 */
void func_ov039_02089a80(TinPinSlammer_Scene* scene) {
    OtuWirelessStage* stage = (OtuWirelessStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32               i;
    u16               mask;

    switch (stage->unk_00) {
        case 0:
            func_ov039_02087c8c(scene, 1);
            return;

        case 1:
            data_ov039_0209ad00 = 1;
            // A do-while, not a for: the target enters the loop unconditionally
            // and tests at the bottom, which shifts its literal pool 12 bytes
            // later and is worth ~0.6% here. Still short of 100% -- the
            // residual is pool placement, not code.
            i = 0;
            do {
                if (((OtuChildRecord*)func_ov039_02088440(i))->phase != 1) {
                    break;
                }
                i = i + 1;
            } while (i < OTU_CHILD_COUNT(stage));
            if (i != OTU_CHILD_COUNT(stage)) {
                return;
            }
            func_ov039_02087c8c(scene, 2);
            return;

        case 2:
            stage->unk_04 = 1;
            func_020471ec(0x10, 0x10, &data_ov038_0209edc0);
            func_ov040_0209d970((s32)&data_ov038_0209eef4);
            func_ov003_0209d434((s32)&data_ov038_0209ef48, (s32)scene);
            func_ov040_0209ec5c(func_ov039_02087cac, scene, data_ov039_0209b120, 0x10);
            scene->state.unk_EEC = 1;
            func_ov040_0209ef88();

            mask = 0;
            for (i = 1; i < OTU_CHILD_COUNT(stage); i++) {
                mask |= 1 << i;
            }
            func_ov040_0209ed58(mask);

            func_ov039_02087c8c(scene, 3);
            return;

        case 3:
            if (stage->childCount != func_02047e84(stage->unk_04)) {
                return;
            }
            func_ov040_0209ece8(OTU_TRAY(scene), 0x38);
            func_ov039_02087c8c(scene, 4);
            return;

        case 4:
            data_ov039_0209ad00 = 2;
            for (i = 0; i < OTU_CHILD_COUNT(stage); i++) {
                if (((OtuChildRecord*)func_ov039_02088440(i))->phase != 2) {
                    break;
                }
            }
            if (i != OTU_CHILD_COUNT(stage)) {
                return;
            }
            func_ov039_02087c8c(scene, 6);
            return;

        case 6:
            data_ov039_0209ad00 = 3;
            for (i = 0; i < OTU_CHILD_COUNT(stage); i++) {
                if (((OtuChildRecord*)func_ov039_02088440(i))->phase != 3) {
                    break;
                }
            }
            if (i != OTU_CHILD_COUNT(stage)) {
                return;
            }
            func_ov039_02087c8c(scene, 7);
            return;

        case 7:
            EasyFade_FadeMainDisplay(2, 0x10, 0x1000);
            EasyFade_FadeSubDisplay(2, 0x10, 0x1000);
            func_ov039_02087c8c(scene, 8);
            return;

        case 8:
            func_ov040_0209ed30();
            func_ov040_0209ece4();
            func_ov040_0209ed20();
            func_02047338();
            func_ov040_0209d970(0);
            func_ov003_0209d434(0, 0);
            scene->state.unk_EEC = 0;
            EasyTask_DeleteTask(OTU_POOL1(scene), stage->fadeTask);
            func_ov039_02098a50(OTU_STAGE(scene));
            return;

        default:
            // Case 5 is a bare `pop`: the jump table's slot for it is the
            // function epilogue, so the source has no case 5 at all.
            return;
    }
}

/* ================================================================== */
/* 0x02089d3c -- the first board variant's enter stage.                */
/* ================================================================== */

/**
 * @brief Re-seeds the board, three calls and nothing else.
 *
 * The first call's result is discarded: the target keeps it in r0 and never
 * reads it, so this is a bare call rather than a discarded assignment mwcc might
 * have optimised away differently.
 *
 * Byte-identical to func_ov039_0208a324.
 */
void func_ov039_02089d3c(TinPinSlammer_Scene* scene) {
    func_ov039_02098b70(OTU_STAGE(scene));
    func_ov039_02088ec4(scene);
    func_ov039_02098a50(OTU_STAGE(scene));
}

/* ================================================================== */
/* 0x02089d6c -- "every child has reached phase 4".                    */
/* ================================================================== */

/**
 * @brief Waits for the whole pool to reach phase 4, then fades out.
 *
 * The loop is the guarded form -- `if (count > 0) { do { ... } while (i < count); }`
 * -- which is what produces the target's leading `cmp r0, #0 / ble` before the
 * body. The break on a phase mismatch leaves `i` short of the count, and the
 * `i != count` test after the loop is what turns that into the early return.
 *
 * `state.unk_698` is the flag 0x02089e0c polls to decide whether the board is
 * mid-teardown, so this stage raising it is what stops the physics.
 *
 * Byte-identical to func_ov039_0208a354.
 */
void func_ov039_02089d6c(TinPinSlammer_Scene* scene) {
    OtuWirelessStage* stage = (OtuWirelessStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32               i     = 0;

    data_ov039_0209ad00 = 4;

    if (OTU_CHILD_COUNT(stage) > 0) {
        do {
            if (((OtuChildRecord*)func_ov039_02088440(i))->phase != 4) {
                break;
            }
            i++;
        } while (i < OTU_CHILD_COUNT(stage));
    }

    if (i != OTU_CHILD_COUNT(stage)) {
        return;
    }

    scene->state.unk_698 = 1;
    CriSndMgr_PlayFile(0x18);
    EasyFade_FadeBothDisplays(3, 0, 0x1E);
    func_ov039_02098a50(OTU_STAGE(scene));
}

/* ================================================================== */
/* 0x02089e0c -- the first board variant's tick.                       */
/* ================================================================== */

/**
 * @brief Tears the link down once the last child has gone, then runs the physics.
 *
 * The physics call is outside the teardown guard: the target reaches it from
 * both the `unk_698` path and the fall-through, so it is the function's last
 * statement rather than the tail of the `if`.
 */
void func_ov039_02089e0c(TinPinSlammer_Scene* scene) {
    OtuWirelessStage* stage = (OtuWirelessStage*)func_ov039_02098b70(OTU_STAGE(scene));

    if (scene->state.unk_698 == 0) {
        if (func_ov039_02096c44(EasyTask_GetTaskData(OTU_POOL1(scene), stage->tickTask)) == 0) {
            func_ov040_0209d420(NULL, NULL);
            func_ov039_02098a50(OTU_STAGE(scene));
        }
    }

    func_ov039_020894cc(scene);
}

/* ================================================================== */
/* 0x02089e78 -- the first board variant's wireless interrupt handler. */
/* ================================================================== */

/**
 * @brief Reacts to a wireless interrupt.
 *
 * Written as a `switch` because both case bodies are emitted out of line, past
 * the dispatch -- the shape mwcc only produces for a switch. Case 5's body is
 * emitted first and case 1's last, so that is the order they are written in.
 *
 * The `!= 1` early return inside case 5 is what the target's conditional pop
 * (`cmp r0, #1 / popne {r4, pc}`) corresponds to; writing it as
 * `if (... == 1) { ... }` instead would leave mwcc an unconditional branch.
 */
void func_ov039_02089e78(TinPinSlammer_Scene* scene) {
    switch (func_ov040_0209cb78()) {
        case 5:
            if (func_ov040_0209cb68() != 1) {
                return;
            }
            func_ov040_0209d588();
            return;

        case 1:
            func_ov039_02098a40(OTU_STAGE(scene), NULL);
            return;

        default:
            return;
    }
}

/* ================================================================== */
/* 0x02089ec0 -- the board stage's init.                               */
/* ================================================================== */

/**
 * @brief Arms the stage: clears the sub-state, starts the timer, snapshots the
 *        pool count, and creates the two child tasks.
 *
 * `state.unk_EE0` is latched into the block's child count -- so the count the
 * rest of the band polls is the count as it was at entry, not a live read. Both
 * handle-creating calls take the same `base.spareDataType`, which is what ties
 * the two children to one wireless session.
 */
void func_ov039_02089ec0(TinPinSlammer_Scene* scene) {
    OtuWirelessStage* stage = (OtuWirelessStage*)func_ov039_02098b70(OTU_STAGE(scene));

    stage->state      = 0;
    stage->timer      = 0x258;
    stage->childCount = scene->state.unk_EE0;

    stage->initTask = func_ov039_02096e4c(OTU_POOL1(scene), scene->base.spareDataType);
    stage->fadeTask = func_ov039_020989f0(OTU_POOL1(scene), scene->base.spareDataType);
}

/* ================================================================== */
/* 0x02089f30 -- the board stage's cleanup.                            */
/* ================================================================== */

/**
 * @brief Deletes the init task and tears the board down.
 *
 * The child count is *not* refreshed from `state.unk_EE0` here, unlike the init
 * stage: the target loads it straight into the delete call.
 */
void func_ov039_02089f30(TinPinSlammer_Scene* scene) {
    OtuWirelessStage* stage = (OtuWirelessStage*)func_ov039_02098b70(OTU_STAGE(scene));

    EasyTask_DeleteTask(OTU_POOL1(scene), stage->initTask);
    func_ov039_02088f18(scene);
}

/* ================================================================== */
/* 0x02089f68 -- the second board variant's pre-update stage.          */
/* ================================================================== */

/**
 * @brief The same timer and wireless switch as 0x02089950, with three
 *        differences: the state cases are 1, 2 and 5 rather than 1, 5 and 7;
 *        case 1 splits on the block's +0x08; and case 2 tears the link down.
 *
 * Case 1's two arms are the interesting part. With +0x08 clear (the entry state
 * set by 0x02089ec0) the board is still negotiating: it starts the wireless
 * session, stores the save record's byte at +0x40A into the block's +0x06, and
 * hands the six-byte copy at `scene + 0x41ED8` to the receive callback. With
 * +0x08 set -- which 0x02088698 raises once a packet has landed -- it instead
 * pushes the block's +0x06 as two bytes of state and sends at index 5.
 *
 * `OTU_BOARD_RECORD` is the `gSaveData + 0x3000` base and the `+ 0x40A`
 * displacement is the byte the target reads; the handle it feeds is
 * 0x02088698, the packet receive callback, not the 4-argument player the
 * neighbouring stage hands to 0x0209ec5c.
 */
void func_ov039_02089f68(TinPinSlammer_Scene* scene) {
    OtuWirelessStage* stage = (OtuWirelessStage*)func_ov039_02098b70(OTU_STAGE(scene));

    if (stage->timer > 0) {
        stage->timer = stage->timer - 1;
    } else {
        scene->state.unk_EE8 = 1;
    }

    switch (func_ov040_0209cb78()) {
        case 1:
            if (stage->state == 0) {
                func_ov040_0209d990();
                func_ov040_0209caac(0x400548);
                func_ov040_0209d848(3);
                stage->unk_06 = OTU_BOARD_RECORD[0x40A];
                func_ov040_0209ba04(func_ov039_02088698, scene, (u8*)scene + 0x41ED8, 0);
                return;
            }

            func_ov040_0209d818(3, 6, 0x10, 2, 0x10, 2);
            func_ov040_0209cabc(&stage->unk_06, 2);
            func_ov040_0209d290(5, (u8*)stage + 0x0C);
            return;

        case 2:
            if (stage->state == 0) {
                return;
            }
            func_ov040_0209c158();
            return;

        case 5:
            stage->unk_00 = 0;
            func_ov039_02098a50(OTU_STAGE(scene));
            return;

        default:
            return;
    }
}

/* ================================================================== */
/* 0x0208a098 -- the second board variant's update stage.              */
/* ================================================================== */

/**
 * @brief The second board variant's stage machine.
 *
 * The same nine-case shape as 0x02089a80 with the numbers shuffled, and the
 * differences are all real rather than cosmetic:
 *
 *   * stage 2 clears +0x04 with a `strh` where the first variant *set* it, and
 *     runs the three 02047xxx/0209d7xx teardown calls the other variant does in
 *     its stage 8;
 *   * stage 3 is the bare `pop` here -- the second variant has no equivalent of
 *     the first variant's `func_02047e84` round-trip, and goes straight from 2
 *     to 4;
 *   * stage 4 advances to menu 5 rather than 6, and calls
 *     `func_ov040_0209ed58(1)` -- with a literal 1, where the first variant
 *     hands that call the mask it built in its stage 2;
 *   * stage 5 tests +0x04, which stage 2 cleared, so it is the wait for the
 *     first variant's stage-3 handshake to complete.
 *
 * Cases 1, 4 and 6 repeat the same child-polling loop, with the same rotated-`for`
 * shape as their twins above.
 */
void func_ov039_0208a098(TinPinSlammer_Scene* scene) {
    OtuWirelessStage* stage = (OtuWirelessStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32               i;

    switch (stage->unk_00) {
        case 0:
            func_ov039_02087c8c(scene, 1);
            return;

        case 1:
            data_ov039_0209ad00 = 1;
            for (i = 0; i < OTU_CHILD_COUNT(stage); i++) {
                if (((OtuChildRecord*)func_ov039_02088440(i))->phase != 1) {
                    break;
                }
            }
            if (i != OTU_CHILD_COUNT(stage)) {
                return;
            }
            func_ov039_02087c8c(scene, 2);
            return;

        case 2:
            stage->unk_04 = 0;
            func_020472a8(&data_ov038_0209edc0);
            func_ov040_0209d728();
            func_0204737c();
            func_ov040_0209d970((s32)&data_ov038_0209eef4);
            func_ov003_0209d434((s32)&data_ov038_0209ef6c, (s32)scene);
            func_ov040_0209ec5c(func_ov039_02087cac, scene, data_ov039_0209b120, 0x10);
            func_ov040_0209ece8(OTU_TRAY(scene), 0x0E);
            scene->state.unk_EEC = 1;
            func_ov040_0209efc0();
            func_ov039_02087c8c(scene, 4);
            return;

        case 4:
            data_ov039_0209ad00 = 2;
            for (i = 0; i < OTU_CHILD_COUNT(stage); i++) {
                if (((OtuChildRecord*)func_ov039_02088440(i))->phase != 2) {
                    break;
                }
            }
            if (i != OTU_CHILD_COUNT(stage)) {
                return;
            }
            func_ov040_0209ed58(1);
            func_ov039_02087c8c(scene, 5);
            return;

        case 5:
            if (stage->unk_04 == 0) {
                return;
            }
            func_ov039_02087c8c(scene, 6);
            return;

        case 6:
            data_ov039_0209ad00 = 3;
            for (i = 0; i < OTU_CHILD_COUNT(stage); i++) {
                if (((OtuChildRecord*)func_ov039_02088440(i))->phase != 3) {
                    break;
                }
            }
            if (i != OTU_CHILD_COUNT(stage)) {
                return;
            }
            func_ov039_02087c8c(scene, 7);
            return;

        case 7:
            EasyFade_FadeMainDisplay(2, 0x10, 0x1000);
            EasyFade_FadeSubDisplay(2, 0x10, 0x1000);
            func_ov039_02087c8c(scene, 8);
            return;

        case 8:
            func_ov040_0209ed30();
            func_ov040_0209ece4();
            func_ov040_0209ed20();
            func_02047338();
            func_ov040_0209d970(0);
            func_ov003_0209d434(0, 0);
            scene->state.unk_EEC = 0;
            EasyTask_DeleteTask(OTU_POOL1(scene), stage->fadeTask);
            func_ov039_02098a50(OTU_STAGE(scene));
            return;

        default:
            // Case 3 is the bare `pop`, same as case 5 in the other variant.
            return;
    }
}

/* ================================================================== */
/* 0x0208a324 -- the second board variant's enter stage.               */
/* ================================================================== */

/** Byte-identical to func_ov039_02089d3c. */
void func_ov039_0208a324(TinPinSlammer_Scene* scene) {
    func_ov039_02098b70(OTU_STAGE(scene));
    func_ov039_02088ec4(scene);
    func_ov039_02098a50(OTU_STAGE(scene));
}

/* ================================================================== */
/* 0x0208a354 -- the second variant's "phase 4" waiter.                */
/* ================================================================== */

/** Byte-identical to func_ov039_02089d6c. */
void func_ov039_0208a354(TinPinSlammer_Scene* scene) {
    OtuWirelessStage* stage = (OtuWirelessStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32               i     = 0;

    data_ov039_0209ad00 = 4;

    if (OTU_CHILD_COUNT(stage) > 0) {
        do {
            if (((OtuChildRecord*)func_ov039_02088440(i))->phase != 4) {
                break;
            }
            i++;
        } while (i < OTU_CHILD_COUNT(stage));
    }

    if (i != OTU_CHILD_COUNT(stage)) {
        return;
    }

    scene->state.unk_698 = 1;
    CriSndMgr_PlayFile(0x18);
    EasyFade_FadeBothDisplays(3, 0, 0x1E);
    func_ov039_02098a50(OTU_STAGE(scene));
}

/* ================================================================== */
/* 0x0208a3f4 -- the second board variant's tick.                      */
/* ================================================================== */

/**
 * @brief 0x02089e0c without the callback reset.
 *
 * Identical but 12 bytes shorter, and the 12 bytes are exactly the
 * `mov r0, #0 / mov r1, r0 / bl func_ov040_0209d420` pair. Nothing else differs,
 * so the second board variant's children must be tearing their own callbacks
 * down.
 */
void func_ov039_0208a3f4(TinPinSlammer_Scene* scene) {
    OtuWirelessStage* stage = (OtuWirelessStage*)func_ov039_02098b70(OTU_STAGE(scene));

    if (scene->state.unk_698 == 0) {
        if (func_ov039_02096c44(EasyTask_GetTaskData(OTU_POOL1(scene), stage->tickTask)) == 0) {
            func_ov039_02098a50(OTU_STAGE(scene));
        }
    }

    func_ov039_020894cc(scene);
}

/* ================================================================== */
/* 0x0208a454 -- the second variant's wireless interrupt handler.      */
/* ================================================================== */

/**
 * @brief 0x02089e78 without the `func_ov040_0209cb68() != 1` test.
 *
 * Twelve bytes shorter for the same reason as 0x0208a3f4: the two `bl` /
 * `cmp` / `popne` instructions guarding `func_ov040_0209d588` are gone. The
 * dispatch is identical, so the case order (5 then 1) is the same.
 */
void func_ov039_0208a454(TinPinSlammer_Scene* scene) {
    switch (func_ov040_0209cb78()) {
        case 5:
            func_ov040_0209d588();
            return;

        case 1:
            func_ov039_02098a40(OTU_STAGE(scene), NULL);
            return;

        default:
            return;
    }
}

/* ================================================================== */
/* 0x0208a490 -- add to a pin's score and hand it on.                  */
/* ================================================================== */

/**
 * @brief Credits `value` to a pin, then spreads it over the pin's two children.
 *
 * Three things are worth calling out:
 *
 *   * the empty-slot guard is `>= 0x130`, and the target exits on it with a
 *     *conditional pop* (`cmp r0, #0x130 / pophs`), so the comparison is unsigned
 *     -- hence the `(u32)` on the tray slot, which costs nothing because the load
 *     is already a zero-extending `ldrh`. A tray slot holding the "no pin"
 *     sentinel and anything above it both drop the credit. That is the same 0x130
 *     sentinel func_ov039_0208f00c tests against from the other side;
 *   * the clamp is a post-store `if (total > 0x3E7)`, reading back the value it
 *     just wrote, not a pre-store clamp -- the field really is written twice on
 *     an overflowing credit;
 *   * the round-robin is a post-increment with a separate `>= 2` reset, which
 *     is why `OTU_PIN_SLOT` is read three times rather than kept in a local.
 *
 * The two children are the words at +0x228 and +0x22C, indexed with the
 * `((s32*)((u8*)task + 0x228))[i]` subscript form -- the same one
 * func_ov039_0208f104 uses, and load-bearing: written as a byte-offset
 * expression mwcc strength-reduces it into a walked pointer instead.
 *
 * The parameter and return types are `void*` and `s32` because that is what
 * OtuPinSprites's placeholder declaration says, and the two must agree exactly or
 * mwcc rejects the pair. `s32` is a fiction: the target never materialises a
 * return value, so there is deliberately no `return` statement. The cast to
 * `OtuPinLogic` is the one place the declared type is looser than the truth.
 */
s32 func_ov039_0208a490(void* task, s32 value) {
    OtuPinLogic* self = (OtuPinLogic*)task;

    if ((u32)*self->pinID >= 0x130) {
        return;
    }

    self->total = self->total + value;
    if (self->total > 0x3E7) {
        self->total = 0x3E7;
    }

    func_ov039_02093d68(EasyTask_GetTaskData(self->pool, self->counterId), self->total);
    func_ov039_02095cd4(EasyTask_GetTaskData(self->pool, ((s32*)((u8*)self + 0x228))[OTU_PIN_SLOT(self)]), value);

    OTU_PIN_SLOT(self) = OTU_PIN_SLOT(self) + 1;
    if (OTU_PIN_SLOT(self) >= 2) {
        OTU_PIN_SLOT(self) = 0;
    }

    func_ov039_02087d04(0x340, &self->pos, &self->anchor);
}

/* ================================================================== */
/* 0x0208a530 -- perpendicular distance from a point to a line.        */
/* ================================================================== */

/**
 * @brief How far `c` sits off the line through `a` and `b`, in Q12.12.
 *
 * All three points are Q12.12, so every product is a 64-bit multiply rounded to
 * nearest before the shift; the two squares in the length are likewise rounded
 * *separately* and then added in 32 bits, which is why the target has two
 * independent `smull`/`umull` sequences and a single plain `add` joining them.
 *
 * The cross product is accumulated as `(dy*a.x - dx*a.y) + (-dy*c.x + dx*c.y)`
 * with `dx = a.x - b.x` and `dy = a.y - b.y`, i.e. the target negates `dy` once
 * (`rsb lr, r5, #0`) and reuses the negation for both the third term and the
 * square. That is why the square is taken of `-dy` and not of `dy`: same
 * arithmetic, but the negation is already in a register. The whole expression is
 * `cross(a - b, c - b)`, so the returned value is the distance from `c` to the
 * line, and it is clamped only by the `abs` -- nothing here rejects a point that
 * is nowhere near the segment.
 *
 * The degenerate case is checked first and short-circuits on `a->x == b->x`
 * alone, which is why the target uses conditional loads rather than two
 * unconditional ones.
 */
s32 func_ov039_0208a530(OtuPoint* a, OtuPoint* b, OtuPoint* c) {
    s32 dx, dy, ny, cross, len;

    if (a->x == b->x && a->y == b->y) {
        return 0;
    }

    dx = a->x - b->x;
    dy = a->y - b->y;
    ny = -dy;

    cross = (OtuQ12Mul(dy, a->x) - OtuQ12Mul(dx, a->y)) + (OtuQ12Mul(ny, c->x) + OtuQ12Mul(dx, c->y));
    if (cross < 0) {
        cross = -cross;
    }

    len = OtuQ12Mul(ny, ny) + OtuQ12Mul(dx, dx);

    return FX_Divide(cross, FX_Sqrt(len));
}

/* ================================================================== */
/* 0x0208a624 -- is `c` close to the segment a-b?                      */
/* ================================================================== */

/**
 * @brief True when `c` is within 0x24000 of the line a-b.
 *
 * Three tests, all at the same 0x24000 threshold, and which one runs depends on
 * the sign of the dot product of the two edge vectors: if `c` is on the far side
 * of the line from `b` it is compared against `a` directly, otherwise against
 * `b`. So this is a distance-to-segment test expressed as three distance-to-
 * line tests, and it is the predicate 0x0208a988's corner checks are the
 * boundary cases of.
 *
 * The 0x24000 threshold is 9.0 in Q12.12 -- a cell is 0x1000 wide, so this is
 * nine cells.
 */
s32 func_ov039_0208a624(OtuPoint* a, OtuPoint* b, OtuPoint* c) {
    OtuPoint ab;
    OtuPoint ac;
    s32      result = 0;

    func_ov039_02098bb0(b, a, &ab);
    func_ov039_02098bb0(c, a, &ac);

    if (func_ov039_02098c40(&ab, &ac) >= 0) {
        if (func_ov039_0208a530(a, b, c) <= 0x24000) {
            result = 1;
        }
    } else if (func_ov039_02098ca8(c, a) <= 0x24000) {
        result = 1;
    } else if (func_ov039_02098ca8(c, b) <= 0x24000) {
        result = 1;
    }

    return result;
}

/* ================================================================== */
/* 0x0208a6c4 -- clamp a velocity to length 0x8000.                     */
/* ================================================================== */

/**
 * @brief Normalises a velocity and rescales it to exactly 0x8000 if it is longer.
 *
 * `func_ov039_02098bd4` is the Q12.12 scale-and-replace, so passing the same
 * point as both source and destination is how the target writes it -- there is
 * no separate "scale in place" helper in the overlay.
 *
 * `func_ov039_02098d10` is declared `s32` by band 3 but defined `void` in
 * OtuVecOps.c; the value is used here, so this band relies on band 3's
 * declaration. That inconsistency is pre-existing and not something this band
 * can fix from inside an include.
 */
void func_ov039_0208a6c4(OtuPoint* v) {
    if (func_ov039_02098d10(v) <= 0x8000) {
        return;
    }

    func_ov039_02098d3c(v, v);
    func_ov039_02098bd4(0x8000, v, v);
}

/* ================================================================== */
/* 0x0208a6f8 -- drive a pin's velocity from its speed.                 */
/* ================================================================== */

/**
 * @brief Sets a pin's velocity from the angle it is travelling at.
 *
 * The speed is a two-step lookup: the pin's tray slot indexes a 0x1C-stride
 * board record (a *byte* stride, from the `mov r0, #0x1C / mla` pair), that
 * record's byte at +4 indexes a sixteen-byte-stride speed table, and the table's
 * Q12.12 word is multiplied by the caller's Q12.12 angle. Three different
 * indexing shapes, and only the last one is a word stride.
 *
 * After the speed is applied the velocity is clamped, and then re-normalised
 * into a *second* point only when the clamped velocity is non-zero -- an
 * all-zero velocity stays zero rather than becoming a unit vector, which is what
 * the `beq` over the second `func_ov039_02098d3c` is for.
 *
 * The final blend mode is chosen from +0x1D0: exactly 0x10 selects 0x34D and
 * everything else 0x32E. That is an `==` against a constant, not a `>=`, which
 * is why the `ldreq / ldrne` pair straddles the argument setup.
 */
void func_ov039_0208a6f8(OtuPinLogic* self, OtuPoint* dir, s32 angle) {
    u8  speedIndex = ((OtuBoardSlot*)(self->slots + *self->pinID * 0x1C))->speedIndex;
    s32 speed      = data_ov039_0209a3dc[speedIndex].speed;

    func_ov039_02098c00(OtuQ12Mul(speed, angle), dir, &self->vel, &self->vel);

    func_ov039_0208a6c4(&self->vel);

    if (self->vel.x != 0 || self->vel.y != 0) {
        func_ov039_02098d3c(&self->vel, &self->dir);
    }

    func_ov039_02087d04(self->mode == 0x10 ? 0x34D : 0x32E, &self->pos, &self->anchor);
}

/* ================================================================== */
/* 0x0208a794 -- look a point up in the board grid.                     */
/* ================================================================== */

/**
 * @brief Maps a Q12.12 point to a board cell and returns that cell's bit flags.
 *
 * Three nested reductions, all of which have to be right for the instruction
 * count to match:
 *
 *   1. `>> 12` off both coordinates -- the point becomes a cell coordinate;
 *   2. `/ 32` on both, giving the *coarse* cell. The row term is scaled by
 *      `grid->rowScale` and the pair addresses a four-byte cell record;
 *   3. `/ 16` and then `& 1` on both, giving a *finer* pair used only to pick a
 *      two-by-two sub-table. Note this is *not* `/ 32` again -- the two reductions
 *      genuinely differ, so the fine pair is the low bit of the cell coordinate
 *      divided by sixteen. The `+ (x >> 4 >>> 27)` shape is mwcc's signed
 *      divide-by-32 and the `+ (x >> 3 >>> 28)` one its signed divide-by-16, so
 *      both are written as plain `/`. The `& 1` is the three-instruction
 *      `lsr #31 / rsb lsl #31 / add ror #31` sequence; the rotate is a rotate
 *      *right by 31*, i.e. a rotate left by one, and reading it as
 *      `(x >>> 1) | (x << 31)` makes the whole derivation come out as nonsense.
 *
 * The cell record's first byte short-circuits: a non-zero value is returned
 * as-is, which is how an explicitly-marked cell overrides the type table. Only
 * when it is zero does the type byte at +1 (less 0x10) choose one of the twelve
 * tables.
 *
 * The switch is on an *unsigned* value, which is what makes the target's range
 * check `cmp lr, #0x1C / addls` rather than a signed pair, and it covers 0..28
 * with 0 falling into the default along with everything above 28. The case
 * bodies are in ascending source order, which is what the jump table's shared
 * tails (9..13, 14..18, 19..23, 24..28) show.
 */
u8 func_ov039_0208a794(OtuPoint* p, OtuCellGrid* grid) {
    s32 gx   = p->x >> 12;
    s32 gy   = p->y >> 12;
    s32 row  = (gy / 32) * grid->rowScale + (gx / 32);
    u8* cell = grid->cells + row * 2;
    s32 gx32;
    s32 gy32;
    u32 kind;

    if (cell[0] != 0) {
        return cell[0];
    }

    gx32 = (gx / 16) & 1;
    gy32 = (gy / 16) & 1;
    kind = cell[1] - 0x10;

    switch (kind) {
        case 1:
            return data_ov039_02099248[gy32 * 2 + gx32];
        case 2:
            return data_ov039_02099244[gy32 * 2 + gx32];
        case 3:
            return data_ov039_02099250[gy32 * 2 + gx32];
        case 4:
            return data_ov039_0209923c[gy32 * 2 + gx32];
        case 5:
            return data_ov039_0209924c[gy32 * 2 + gx32];
        case 6:
            return data_ov039_02099240[gy32 * 2 + gx32];
        case 7:
            return data_ov039_02099268[gy32 * 2 + gx32];
        case 8:
            return data_ov039_02099264[gy32 * 2 + gx32];
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
            return data_ov039_02099260[gy32 * 2 + gx32];
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
            return data_ov039_0209925c[gy32 * 2 + gx32];
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
            return data_ov039_02099258[gy32 * 2 + gx32];
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
            return data_ov039_02099254[gy32 * 2 + gx32];
        default:
            return cell[0];
    }
}

/* ================================================================== */
/* 0x0208a988 -- can a pin be placed here?                              */
/* ================================================================== */

/**
 * @brief The board's placement test: is this point on the board and clear?
 *
 * The shape is a nest of cheap rejections:
 *
 *   * the point's own cell must be neither empty (0) nor the "solid" answer
 *     (2), which is one call and two compares -- the target tests the single
 *     return value against both rather than calling twice;
 *   * the four axis neighbours at +/-0xC000 must not be solid, in right, left,
 *     up, down order;
 *   * the four corners at +/-0x20000 are then snapped to the cell corner they
 *     fall in, and if the point is within 0xC000 of that corner it is rejected.
 *     The snap masks are `~0x1FFFF`, `|0x1FFFF` and, for the last corner only,
 *     `& (0x1FFFF << 17)`. That last one is genuinely a mask *with* the shifted
 *     constant and not `~(0x1FFFF << 17)`, so it is written as the shift of a
 *     local rather than as a folded literal -- a folded constant makes mwcc
 *     materialise 0x3FFFE0000 instead of emitting `and r2, r1, r0, lsl #17`;
 *   * finally every child is checked, and the point is rejected if it is closer
 *     to any child than that child's own radius plus 0xC000.
 *
 * The child's point is read out by func_ov039_02092744 and its radius by
 * func_ov039_02092758, so the loop's threshold varies per child -- which is why
 * the `+ 0xC000` is folded into the comparison's right-hand side rather than
 * being applied to the distance.
 *
 * The child point lives at `sp + 0` and the probe at `sp + 8`, so the probe is
 * declared first: mwcc hands out stack slots in reverse declaration order.
 */
s32 func_ov039_0208a988(OtuPoint* self, OtuCellGrid* grid, TinPinSlammer_Scene* scene) {
    OtuPoint probe;
    OtuPoint childPos;
    s32      mask = 0x1FFFF;
    s32      cell;
    s32      count;
    s32      i;

    cell = func_ov039_0208a794(self, grid);
    if (cell != 0 && cell != 2) {
        probe.x = self->x + 0xC000;
        probe.y = self->y;
        if (func_ov039_0208a794(&probe, grid) == 2) {
            return 0;
        }

        probe.x = self->x - 0xC000;
        probe.y = self->y;
        if (func_ov039_0208a794(&probe, grid) == 2) {
            return 0;
        }

        probe.x = self->x;
        probe.y = self->y + 0xC000;
        if (func_ov039_0208a794(&probe, grid) == 2) {
            return 0;
        }

        probe.x = self->x;
        probe.y = self->y - 0xC000;
        if (func_ov039_0208a794(&probe, grid) == 2) {
            return 0;
        }

        probe.x = self->x + 0x20000;
        probe.y = self->y - 0x20000;
        if (func_ov039_0208a794(&probe, grid) == 2) {
            probe.x &= ~mask;
            probe.y |= mask;
            if (func_ov039_02098ca8(self, &probe) < 0xC000) {
                return 0;
            }
        }

        probe.x = self->x + 0x20000;
        probe.y = self->y + 0x20000;
        if (func_ov039_0208a794(&probe, grid) == 2) {
            probe.x &= ~mask;
            probe.y &= ~mask;
            if (func_ov039_02098ca8(self, &probe) < 0xC000) {
                return 0;
            }
        }

        probe.x = self->x - 0x20000;
        probe.y = self->y - 0x20000;
        if (func_ov039_0208a794(&probe, grid) == 2) {
            probe.x |= mask;
            probe.y |= mask;
            if (func_ov039_02098ca8(self, &probe) < 0xC000) {
                return 0;
            }
        }

        probe.x = self->x - 0x20000;
        probe.y = self->y + 0x20000;
        if (func_ov039_0208a794(&probe, grid) == 2) {
            probe.x |= mask;
            probe.y &= mask << 17;
            if (func_ov039_02098ca8(self, &probe) < 0xC000) {
                return 0;
            }
        }

        count = func_ov039_020883ac(scene);
        for (i = 0; i < count; i++) {
            void* child = func_ov039_020883c8(scene, i);
            s32   radius;

            func_ov039_02092744(child, &childPos);
            radius = func_ov039_02092758(child);

            if (func_ov039_02098ca8(self, &childPos) < radius + 0xC000) {
                return 0;
            }
        }

        return 1;
    }

    return 0;
}

/* ================================================================== */
/* 0x0208ac98 -- the change-mask latch.                                 */
/* ================================================================== */

/**
 * @brief Records which bits of a live state word have changed since last time.
 *
 * `now & (last ^ now)` is the standard idiom, and the second store re-reads the
 * live word through the pointer rather than reusing the value already in a
 * register -- the target reloads the pointer, so the source reads it twice.
 *
 * Both fields are halfwords and both stores are `strh`, so they are genuinely
 * `u16` and not `s32` with a cast: a cast to a narrower type on a store to a
 * 32-bit field is what produces the `lsl #0x10 / lsr #0x10` round trip the
 * target does *not* have here.
 */
void func_ov039_0208ac98(OtuInputLatch* self) {
    u16 now = self->state[2];

    self->changed = now & (self->last ^ now);
    self->last    = self->state[2];
}
