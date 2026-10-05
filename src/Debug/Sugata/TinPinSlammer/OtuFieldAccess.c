/**
 * @file OtuFieldAccess.c
 * @brief The board's obstacle list, the OAM sort-key packer and the wireless
 *        touch-pad input.
 */

#include "OtuFieldAccessShared.h"

extern OtuPadState data_ov039_0209af20[];
extern s32         data_ov039_0209ad04;
extern s32         data_ov039_0209ad08[4];
extern OtuPadState data_ov039_0209ad20;

/* ------------------------------------------------------------------ */
/* The board's obstacles.                                              */
/* ------------------------------------------------------------------ */

s32 func_ov039_020883ac(TinPinSlammer_Scene* scene) {
    return ((OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene)))->obstacleCount;
}

void* func_ov039_020883c8(TinPinSlammer_Scene* scene, s32 index) {
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));

    return EasyTask_GetTaskData(OTU_POOL2(scene), stage->obstacleIds[index]);
}

/**
 * Packs a layer and a Q12.12 y into an OAM sort key: the layer above eleven
 * bits of y's integer part, so sprites sort by layer first and then top to
 * bottom. The third argument (callers pass a z, or 0) is not read.
 */
s32 func_ov039_02088400(s32 layer, s32 y, s32 z) {
    return (((layer << 0x0B) + ((y >> 0x0C) & 0x7FF)) << 0x0C);
}

/**
 * @brief Maps the scene's mode selector onto a local player index.
 *
 * Zero for mode 0, ov040's index for mode 1, zero for anything else. The target
 * tests `!= 0` and then `== 1` rather than just `== 1`, so both tests are kept.
 */
s32 func_ov039_02088418(s32 mode) {
    s32 r = 0;

    if (mode != 0 && mode == 1) {
        r = func_ov040_0209d728();
    }

    return r;
}

/** The six-byte touch-pad descriptor for player slot `slot`. */
void* func_ov039_02088440(s32 slot) {
    return &data_ov039_0209af20[slot];
}

/**
 * @brief Refreshes the four child-records from the wireless link.
 *
 * Only runs while ov040's link state is 5. When it is, a second ov040 query
 * decides between "records are live" and "records are gone": live records are
 * copied out of the returned pointers and the per-slot live flag raised, gone
 * ones are zeroed and the flag cleared, and the board's latch at
 * `data_ov039_0209ad04` is set to match. Any other link state clears all four
 * flags and the latch.
 */
void func_ov039_02088454(void) {
    s32 i;
    s32 src;

    if (func_ov040_0209cb78() != 5) {
        goto clear;
    }

    if (func_ov040_0209d4c4(&data_ov039_0209ad20) != 0) {
        for (i = 0; i < 4; i++) {
            src = func_ov040_0209d4a4((u16)i);
            if (src != 0) {
                MI_CpuCopyU8(src, &data_ov039_0209af20[i], 6);
                data_ov039_0209ad08[i] = 1;
            } else {
                MI_CpuSet(&data_ov039_0209af20[i], 0, 6);
                data_ov039_0209ad08[i] = 0;
            }
        }

        data_ov039_0209ad04 = 0;
        return;
    }

    for (i = 0; i < 4; i++) {
        data_ov039_0209ad08[i] = 0;
    }
    data_ov039_0209ad04 = 1;
    return;

clear:
    for (i = 0; i < 4; i++) {
        data_ov039_0209ad08[i] = 0;
    }
    data_ov039_0209ad04 = 0;
}

/**
 * @brief Polls the touch screen into one of those descriptors.
 *
 * `TouchInput_GetCoord` writes two words, so the coordinate bytes are read from
 * a stack pair rather than a single value. The "was touched" latch at +0 is set
 * from `TouchInput_IsTouchActive` through a select, and the `SysControl` word is
 * copied to +4 before the shared latch `data_ov039_0209ad00` is cleared.
 */
void func_ov039_02088564(void* padPtr) {
    OtuPadState* pad = (OtuPadState*)padPtr;
    TouchCoord   coord;

    TouchInput_GetCoord(&coord);

    pad->x = (u8)coord.x;
    pad->y = (u8)coord.y;

    if (TouchInput_IsTouchActive() != 0) {
        pad->touched = 1;
    } else {
        pad->touched = 0;
    }

    pad->sysControl     = *(u16*)&SysControl;
    pad->phase          = data_ov039_0209ad00;
    data_ov039_0209ad00 = 0;
}

/** Polls player 0 unless the board has already latched the pads. */
void func_ov039_020885cc(void) {
    if (data_ov039_0209ad04 != 0) {
        return;
    }

    func_ov039_02088564((OtuPadState*)&data_ov039_0209ad20);
}

/**
 * @brief The wireless board's per-pad step: advance this child's phase when the
 *        pad has been tapped.
 *
 * The record's +0x10 word is a cursor into the round's phases and +0x14 is the
 * phase the pad must already be on; the store increments the record's counter
 * at +6 and calls the ov040 stepper with the "still in range" flag.
 */
s32 func_ov039_020885f4(void* record, TinPinSlammer_Scene* scene) {
    u8* self   = (u8*)record;
    u8* target = (u8*)func_ov039_02098b70(OTU_STAGE(scene));
    s32 flag;

    if (*(u16*)(self + 0x10) >= *(s32*)(target + 0x140)) {
        return 0;
    }

    if (*(u16*)(self + 0x14) == *(u16*)(target + 6)) {
        (*(u16*)(target + 6))++;

        if (*(s32*)(target + 6) < *(s32*)(target + 0x140)) {
            flag = 1;
        } else {
            flag = 0;
        }

        func_ov040_0209d788(0, 0, target + 6, 2, flag);
        return 1;
    }

    return 0;
}

/** Raises the scene's +0x41EE4 flag. Registered as a callback, so the scene
 *  arrives in the second argument slot -- the target's base is r1. */
void func_ov039_02088688(s32 unused, TinPinSlammer_Scene* scene) {
    scene->linkLost = 1;
}
