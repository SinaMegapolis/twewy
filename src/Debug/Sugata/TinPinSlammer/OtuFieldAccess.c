#include "OtuFieldAccessShared.h"

/*
 * ov039 region file, 0x020883ac - 0x02088698. One of the overlay's translation
 * units; dsd gives each file a single contiguous `.text` claim. The types,
 * externs and prototypes these region files share live in
 * OtuFieldAccessShared.h. The notes below describe the overlay's field-accessor
 * family, whose functions are spread across this and the neighbouring regions.
 */

/**
 * @file OtuFieldAccess.c
 *
 * The overlay's small helpers, in two groups that happen to interleave in
 * address space and so have to share one translation unit:
 *
 *   1. the field accessors -- short routines that read, write or clear one or
 *      two words of a structure and return;
 *   2. the sprite-cell builders -- twenty-two identical routines that fill in
 *      one of the base game's sprite slots, plus the depth-key packer they
 *      share.
 *
 * They interleave, which is why they are in one file rather than two: a delinks
 * entry may claim a `.text` range only once, and a range may not overlap
 * another, so functions that share a file cannot be split across files by
 * address band.
 *
 * There are around forty of these and they are not scattered at random. They
 * cluster tightly by which structure they touch, and the clustering is the
 * useful part -- it is how the overlay's objects can be told apart at all,
 * since almost nothing here carries a name. Grouped by the highest offset each
 * one reaches:
 *
 *   0x110 - 0x1B4   the stage's main task, touched by nine of them in a row at
 *                   0x0208e6cc - 0x0208f03c. The +0x110/+0x114 and +0x118/+0x11C
 *                   pairs each have a getter, a setter and a copy-out, which is
 *                   what makes them look like two more OtuPoints.
 *   0x100 + 0x78..  the same object again, one 0x100 further on: four `ldrsh`
 *                   reads at 0x78, 0x7A, 0x7C, 0x7E, two bytes apart, which is
 *                   four consecutive s16s.
 *   0x40 - 0x54     a second, smaller object with the same getter/setter pairs.
 *   0x0C, 0x04,     a third: two one-word getters and a setter that also raises
 *   0x08, 0x14      a flag, plus the only one here that increments a field.
 *
 * Four more -- 0x020921f4, 0x02092bf0, 0x020933f0 and 0x02096d98 -- compile to
 * `mov r0, #1; bx lr` and are deliberately left out. Four identical bodies at
 * four addresses is at least as likely to be four different predicates reached
 * by four different routes as four copies of one predicate, and naming them
 * would assert something the code does not say.
 *
 * These are modelled with plain offsets rather than by widening the structures
 * they touch. Every structure involved is shared with code that already matches,
 * and naming fields on them would move stack slots and register choices in
 * functions that are currently byte-for-byte correct.
 *
 * That reasoning was tested rather than assumed. Where a struct already carried
 * a field at the right offset *and* the right width -- `OtuBadge`, `OtuShadowTask`,
 * `OtuEntryTask`, `OtuObstacle`, `OtuStageDispatch` and the rest -- naming the
 * field is codegen-neutral, and those conversions are in the tree. The two
 * cases where it is *not* neutral are both recorded at their use sites:
 *
 *   - a `Sprite sprite[3]` array member, which makes mwcc scale the index and
 *     emit `add r0, r4, #0x3800` where the target has `add r0, r4, #0x40`
 *     (OtuFieldAccessShared.h, OtuTripleSprite);
 *   - a field whose declared width disagrees with the access -- `stateFlags`
 *     is a `u16` but one store is a full word, and `OtuBadgeState.home` is a
 *     pointer but is read here as a word flag (OtuMeters.c).
 *
 * So the rule is narrower than "naming fields is safe": it is safe when offset
 * and width already agree, and it is checked per access rather than assumed for
 * a whole structure.
 */

/* ------------------------------------------------------------------ */
/* The stage block's child list.                                       */
/* ------------------------------------------------------------------ */

/** Number of children in the child list (`stageBlock` + 0x144). */
s32 func_ov039_020883ac(TinPinSlammer_Scene* scene) {
    return *(s32*)((u8*)func_ov039_02098b70(OTU_STAGE(scene)) + 0x144);
}

/** The `index`th child's task data, resolved out of the second task pool. */
void* func_ov039_020883c8(TinPinSlammer_Scene* scene, s32 index) {
    u8* block = (u8*)func_ov039_02098b70(OTU_STAGE(scene));

    return EasyTask_GetTaskData(OTU_POOL2(scene), *(u32*)((u8*)block + index * 4 + 0x28C));
}

/* ==================================================================== */
/* The sprite-cell builders.  See the section header below.           */
/* ==================================================================== */

/**
 * @brief Pack a Q12.12 coordinate pair into one sortable key, 0x02088400.
 *
 * `(x << 23) | ((y >> 12) & 0x7FF)`, shifted up by twelve more -- so the
 * returned word is `(x << 23) + (y << 12)` with the low bits of `y`'s integer
 * part kept. Comparing two of these compares `x` first and only breaks the tie
 * on `y`, which is what a painter's-algorithm depth sort needs, and it is why
 * the result lands in the same slot as a pointer: the cell is a small tagged
 * union rather than a struct with a depth field.
 *
 * Note this is a *quantised* key, not an interleave: eleven bits of `y`'s integer
 * part and none of its fraction, so two sprites within 1/2048 of each other
 * vertically compare equal. The callers here are placing whole cells on a grid,
 * so that is presumably deliberate, but it is a lossy comparison and worth
 * knowing before trusting the ordering.
 */
s32 func_ov039_02088400(s32 x, s32 y, s32 axis) {
    // The axis argument is passed and then never read: the target loads it
    // into r2 and uses r2 only as the mask source, so the last instruction is a
    // plain shift and nothing is added. Callers still supply it -- several pass 3,
    // one passes 7 -- and it makes no difference to the result.
    (void)axis;

    return (((x << 0x0B) + ((y >> 0x0C) & 0x7FF)) << 0x0C);
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
    *(s32*)((u8*)scene + 0x41000 + 0xEE4) = 1;
}
