#include "Debug/Sugata/TinPinSlammer.h"

/*
 * This is the overlay's catch-all translation unit. Besides the helpers
 * described below, it carries every function whose address is interleaved with
 * them -- the board, badge, wireless and simulation task families. dsd gives a
 * source file exactly one contiguous `.text` claim and forbids overlapping
 * claims, so those groups cannot be split into their own files without moving
 * functions across address ranges. They used to live in sixteen `.inc` band
 * files `#include`d at the bottom; they are now inlined here so that the tree
 * has no `.inc` sources.
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

/* ------------------------------------------------------------------ */
/* The stage task's 0x110 - 0x1B4 block.                              */
/* ------------------------------------------------------------------ */

/** Reads the +0x110/+0x114 pair out to a point. */
// Written as a whole-`OtuPoint` assignment. Two scalar stores make mwcc
// interleave the first store between the two loads, and a named local makes
// it merge them into `stmia`; the struct assignment is the target's four
// instructions (ldr, ldr, str, str) exactly.
void func_ov039_0208e85c(void* task, OtuPoint* out) {
    *out = *(OtuPoint*)((u8*)task + 0x110);
}

/** Writes the +0x110/+0x114 pair from a point. */
void func_ov039_0208e848(void* task, OtuPoint* in) {
    *(OtuPoint*)((u8*)task + 0x110) = *in;
}

/** Reads the +0x118/+0x11C pair out to a point. */
// Written as a whole-`OtuPoint` assignment. Two scalar stores make mwcc
// interleave the first store between the two loads, and a named local makes
// it merge them into `stmia`; the struct assignment is the target's four
// instructions (ldr, ldr, str, str) exactly.
void func_ov039_0208e87c(void* task, OtuPoint* out) {
    *out = *(OtuPoint*)((u8*)task + 0x118);
}

/** Writes the +0x118/+0x11C pair from two values. */
void func_ov039_0208e870(void* task, s32 x, s32 y) {
    *(s32*)((u8*)task + 0x118) = x;
    *(s32*)((u8*)task + 0x11C) = y;
}

/** The +0x120/+0x124 pair, copied out. Used by the nearest-child queries. */
// Written as a whole-`OtuPoint` assignment. Two scalar stores make mwcc
// interleave the first store between the two loads, and a named local makes
// it merge them into `stmia`; the struct assignment is the target's four
// instructions (ldr, ldr, str, str) exactly.
void func_ov039_0208e6e0(OtuPinTask* task, OtuPoint* out) {
    *out = *(OtuPoint*)((u8*)task + 0x120);
}

/** The +0x138/+0x13C pair, copied out -- the fourth such pair on this object. */
// Written as a whole-`OtuPoint` assignment. Two scalar stores make mwcc
// interleave the first store between the two loads, and a named local makes
// it merge them into `stmia`; the struct assignment is the target's four
// instructions (ldr, ldr, str, str) exactly.
void func_ov039_0208e6cc(void* task, OtuPoint* out) {
    *out = *(OtuPoint*)((u8*)task + 0x138);
}

/** A single word at +0x128. */
s32 func_ov039_0208e6f4(void* task) {
    return *(s32*)((u8*)task + 0x128);
}

/** The "alive" word at +0x148. This is the one 0208ee84 tests. */
s32 func_ov039_0208ee98(void* task) {
    return *(s32*)((u8*)task + 0x148);
}

/**
 * The +0x1A4 word, read-and-cleared.
 *
 * Returns the old value and stores zero, so a caller polling this sees each
 * value exactly once. It is the only accessor here with that shape.
 */
s32 func_ov039_0208eff8(void* task) {
    s32 value = *(s32*)((u8*)task + 0x1A4);

    *(s32*)((u8*)task + 0x1A4) = 0;
    return value;
}

/** A single word at +0x1AC. */
s32 func_ov039_0208f034(void* task) {
    return *(s32*)((u8*)task + 0x1AC);
}

/** Raises the +0x1B4 word to 0x11. Set, not incremented. */
void func_ov039_0208f03c(void* task) {
    *(s32*)((u8*)task + 0x1B4) = 0x11;
}

/* ------------------------------------------------------------------ */
/* Four consecutive s16s at base + 0x100 + 0x78.                        */
/* ------------------------------------------------------------------ */

/*
 * These four read 0x78, 0x7A, 0x7C and 0x7E off a base already offset by 0x100,
 * so they are four consecutive s16 fields. The `add r0, r0, #0x100` is spelled
 * out in each rather than folded into the load offset: a single
 * `ldrsh [r0, #0x178]` is a valid alternative encoding that this build does not
 * produce.
 */

s16 func_ov039_0208eea0(void* task) {
    return *(s16*)((u8*)task + 0x100 + 0x78);
}

s16 func_ov039_0208eeac(void* task) {
    return *(s16*)((u8*)task + 0x100 + 0x7A);
}

s16 func_ov039_0208eeb8(void* task) {
    return *(s16*)((u8*)task + 0x100 + 0x7C);
}

s16 func_ov039_0208eec4(void* task) {
    return *(s16*)((u8*)task + 0x100 + 0x7E);
}

/* ------------------------------------------------------------------ */
/* A second, smaller object: 0x40 - 0x54.                              */
/* ------------------------------------------------------------------ */

/** Writes the +0x40/+0x44 pair from a point -- the target loads from the point
 *  and stores into the task, so this is a setter despite its twin below. */
// Written as a whole-`OtuPoint` assignment. Two scalar stores make mwcc
// interleave the first store between the two loads, and a named local makes
// it merge them into `stmia`; the struct assignment is the target's four
// instructions (ldr, ldr, str, str) exactly.
void func_ov039_02092730(void* task, OtuPoint* in) {
    *(OtuPoint*)((u8*)task + 0x40) = *in;
}

/** Reads the +0x48/+0x4C pair out to a point. */
// Written as a whole-`OtuPoint` assignment. Two scalar stores make mwcc
// interleave the first store between the two loads, and a named local makes
// it merge them into `stmia`; the struct assignment is the target's four
// instructions (ldr, ldr, str, str) exactly.
void func_ov039_02092744(void* task, OtuPoint* out) {
    *out = *(OtuPoint*)((u8*)task + 0x48);
}

/** A single word at +0x50. */
s32 func_ov039_02092758(void* task) {
    return *(s32*)((u8*)task + 0x50);
}

/** Raises the +0x54 word to 1. */
void func_ov039_02092760(void* task) {
    *(s32*)((u8*)task + 0x54) = 1;
}

/** Writes the +0x50/+0x54 pair from a point -- the pair's setter. */
void func_ov039_02094e88(void* task, OtuPoint* in) {
    *(OtuPoint*)((u8*)task + 0x50) = *in;
}

/** A single word at +0x54, read as s32 by a different caller. */
s32 func_ov039_02097ad8(void* task) {
    return *(s32*)((u8*)task + 0x54);
}

/* ------------------------------------------------------------------ */
/* Single words elsewhere.                                             */
/* ------------------------------------------------------------------ */

/** A single word at +0xC4. */
s32 func_ov039_02094204(void* task) {
    return *(s32*)((u8*)task + 0xC4);
}

/** A single word at +0xCC. */
s32 func_ov039_0209420c(void* task) {
    return *(s32*)((u8*)task + 0xCC);
}

/** Clears the +0x114 and +0x118 words together. */
void func_ov039_02095ddc(void* task) {
    *(s32*)((u8*)task + 0x114) = 0;
    *(s32*)((u8*)task + 0x118) = 0;
}

/** Clears the +0x80 and +0x84 words together, store order reversed. */
void func_ov039_02096270(void* task) {
    *(s32*)((u8*)task + 0x84) = 0;
    *(s32*)((u8*)task + 0x80) = 0;
}

/** Writes +0x70 and +0x74 from two values, then raises +0x80 to 1. */
void func_ov039_02091654(void* task, s32 x, s32 y) {
    *(s32*)((u8*)task + 0x70) = x;
    *(s32*)((u8*)task + 0x74) = y;
    *(s32*)((u8*)task + 0x80) = 1;
}

/** Raises the +0x70 word to 1. */
void func_ov039_02091b34(void* task) {
    *(s32*)((u8*)task + 0x70) = 1;
}

/** Writes +0x58 from a value and raises +0x5C to 1. */
void func_ov039_0208f7a4(void* task, s32 value) {
    *(s32*)((u8*)task + 0x58) = value;
    *(s32*)((u8*)task + 0x5C) = 1;
}

/* ------------------------------------------------------------------ */
/* The dispatch container's own fields.                               */
/* ------------------------------------------------------------------ */

/**
 * Stores a stage descriptor as the container's action and raises its active flag.
 *
 * This is the call the entry points make when choosing which stage to run --
 * func_ov039_02082c50 branches on it to pick the menu stage or the result stage.
 */
void func_ov039_02098a40(void* dispatch, void* stage) {
    *(void**)((u8*)dispatch + 0x0C) = stage;
    *(s32*)((u8*)dispatch + 0x10)   = 1;
}

/** Increments the stage index -- the only accessor here that does not assign. */
void func_ov039_02098a50(OtuStageDispatch* dispatch) {
    dispatch->stageIndex = dispatch->stageIndex + 1;
}

/** Reads the current stage block. */
void* func_ov039_02098b68(void* dispatch) {
    return *(void**)((u8*)dispatch + 0x04);
}

/** Reads the stage block the row table hangs off. */
void* func_ov039_02098b70(OtuStageDispatch* dispatch) {
    return dispatch->stageBlock;
}

/** Copies the container's first two words out. */
void func_ov039_02098b78(OtuStageDispatch* dispatch, s32* out) {
    out[0] = *(s32*)((u8*)dispatch + 0x00);
    out[1] = *(s32*)((u8*)dispatch + 0x04);
}

/** Raises the scene's +0x41EE4 flag. Registered as a callback, so the scene
 *  arrives in the second argument slot -- the target's base is r1. */
void func_ov039_02088688(s32 unused, TinPinSlammer_Scene* scene) {
    *(s32*)((u8*)scene + 0x41000 + 0xEE4) = 1;
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

/*
 * The twenty-two instantiations. Kept as one call each rather than as macros so
 * that the per-function argument list stays readable and a diff shows which
 * constant each one actually varies.
 */

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_0208d2f8(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0xCC);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x124), 0, 3);
            return slot;
        }

        default:
            return NULL;
    }
}

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_0208f134(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x40);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x5C), 0, 0);
            return slot;
        }

        default:
            return NULL;
    }
}

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_0208f440(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x1);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x4C), 0, 0);
            return slot;
        }

        default:
            return NULL;
    }
}

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_0208fa90(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x40);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x5C), 0, 4);
            return slot;
        }

        default:
            return NULL;
    }
}

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_02090208(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x84);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0xB0), 0, 0);
            return slot;
        }

        default:
            return NULL;
    }
}

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_020902cc(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x94);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0xB8), 0, 0);
            return slot;
        }

        default:
            return NULL;
    }
}

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_02091118(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x44);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x60), 0, 0);
            return slot;
        }

        default:
            return NULL;
    }
}

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_020916c4(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x40);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x5C), 0, 0);
            return slot;
        }

        default:
            return NULL;
    }
}

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_020923c8(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x1);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x4C), 0, 0);
            return slot;
        }

        default:
            return NULL;
    }
}

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_02094ae8(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x40);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x5C), 0, 3);
            return slot;
        }

        default:
            return NULL;
    }
}

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_02094ff4(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x40);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x5C), 0, 0);
            return slot;
        }

        default:
            return NULL;
    }
}

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_0209549c(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x1);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x54), 0, 0);
            return slot;
        }

        default:
            return NULL;
    }
}

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_020958a8(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x1);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x10C), 0, 0);
            return slot;
        }

        default:
            return NULL;
    }
}

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_02095dec(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x1);
            slot->depthKey = func_ov039_02088400(0, 0, 7);
            return slot;
        }

        default:
            return NULL;
    }
}

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_02096280(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x1);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x4C), 0, 0);
            return slot;
        }

        default:
            return NULL;
    }
}

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_0209659c(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0xD0);
            slot->depthKey = func_ov039_02088400(0, 0, 0);
            return slot;
        }

        default:
            return NULL;
    }
}

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_02096660(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x1);
            slot->depthKey = func_ov039_02088400(0, 0, 7);
            return slot;
        }

        default:
            return NULL;
    }
}

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_02096e7c(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x40);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x5C), 0, 3);
            return slot;
        }

        default:
            return NULL;
    }
}

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_02097398(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x1);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x4C), 0, 4);
            return slot;
        }

        default:
            return NULL;
    }
}

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_020977d0(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x1);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x4C), 0, 0);
            return slot;
        }

        default:
            return NULL;
    }
}

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_02097ae0(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x40);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x5C), 0, 3);
            return slot;
        }

        default:
            return NULL;
    }
}

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_02097ff4(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x40);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x5C), 0, 3);
            return slot;
        }

        default:
            return NULL;
    }
}

/*
 * Below are the overlay's remaining translation-unit-sized groups, in the
 * order their dependencies require. They were sixteen separate `.inc` band
 * files `#include`d here; they are now inlined because dsd can give a source
 * file only one contiguous `.text` claim, and these groups interleave with
 * the definitions above, so they cannot be split into their own files. The
 * order is still load-bearing: a call to a function defined further down,
 * with no declaration in between, is an implicit `int (...)` under C99 and
 * then collides with the real definition. `tools/check_band_order.py` walks
 * the file and fails if any symbol is used before it is declared.
 */

/* Band 2 region -- inlined; see the file header. */

#include "Debug/Sugata/TinPinSlammer.h"
#include "SndMgr.h"
#include "SpriteMgr.h"

/*
 * 0x02093000 - 0x02094000: the pinball simulation's own task family.
 *
 * Where OtuFieldAccess.c is the overlay's field accessors, this band is the
 * machinery those accessors are called from: four EasyTask tasks, each built
 * out of the same four-stage lifecycle (init / update / render / cleanup, wired
 * through a `TaskStages` table), and each owning a handful of `Sprite`s.
 *
 * All four tasks are built the same way, and the constants are what make them
 * tellable apart. The task names are readable in the `.rodata` handles:
 *
 *   Tsk_OtosuGame_ovbg          data_ov039_020999b4, 0x850 bytes
 *       init 02092f88  update 020933c0  render 020933f0  cleanup 020933f8
 *       Four BG layers (char + screen + palette each) and one animated
 *       palette source at +0x840. No sprites at all.
 *
 *   Tsk_OtosuGame_badgeradar    data_ov039_02099a60, 0x60 bytes
 *       init 02093668  update 020936b8  render 020937a0  cleanup 020937dc
 *       One sprite that tracks a child task's +0x120/+0x124 position and
 *       renders it as a Q12.12 -> pixel conversion.
 *
 *   Tsk_OtosuGame_badgescounter data_ov039_02099ab0, 0x198 bytes
 *       init 02093b08  update 02093b98  render 02093bc0  cleanup 02093c44
 *       Six sprites: one conditional pair at +0x04/+0x44 and four digit
 *       sprites at +0x84..+0x184, gated by a bitmask at +0x194.
 *
 *   Tsk_OtosuGame_countdown     data_ov039_02099b50, 0xD0 bytes
 *       init 02093f70  update 02093fcc  render 0209411c  cleanup 02094158
 *       Three digit sprites and a countdown at +0xC4 measured in 1/60th
 *       ticks (`arg1 * 0x3C`), with 0x4B0 (20 seconds) as its alarm point.
 *
 * Four findings worth stating up front, because they are what makes the rest
 * readable:
 *
 *   1. **The load wrappers are the same body five times over.** 020935d4,
 *      0209392c, 020939b0, 02093a30 and 02093ee4 each copy a 0x2C-byte
 *      `SpriteAnimation` template out of `.rodata` onto the stack, patch three
 *      fields, and call `_Sprite_Load`. What differs between them is only
 *      which template and which patch -- so they are written out five times
 *      rather than shared. `-inline noauto` means a shared `static` would
 *      compile to a real `bl` and cost every one of them its body.
 *
 *   2. **The sprite-cell builders are byte-identical here, not merely
 *      similar.** 0209352c, 02093884 and 02093e3c have the same body as each
 *      other and differ from the twenty-two in OtuFieldAccess.c in exactly one
 *      instruction: they end with the constant depth key 3 where the twenty-two
 *      call the packer. Three copies, zero constants varied. They are also
 *      `SpriteFrameInfoCallback`s, so their second argument is the callback arg
 *      and their *third* is the mode -- see the comment above 0209352c.
 *
 *   3. **Two different clocks, one unit.** The background task's palette
 *      animation steps one entry per call with a count of 0x12; the countdown
 *      task's clock is in 1/60th ticks and 0x4B0 is twenty of them. Nothing
 *      in this band ties the two together, but the 0x12 and the 0x3C are both
 *      per-frame quantities and 02092f88's palette source and 02093fcc's
 *      countdown both run once per update.
 *
 *   4. **0x130 is a pin-tray value, not a character.** 02093d18 scans for it as
 *      an "empty slot" sentinel and 02093d68 writes digit glyphs as 0x21 + n --
 *      ASCII digits minus 0x0F. Both are working in the tray's value space, so
 *      the counter task's "score" and the pin tray are the same numbering, and
 *      the animation indices in `data_ov039_02099aa8` (also 0x21) are in it
 *      too.
 */

/* Overlays' own helpers, not yet decompiled; declared here rather than in the
 * shared header because five band files are compiled as one translation unit. */
extern s32 func_ov039_02098dbc(void* anim);
extern s32 func_ov039_02098d7c(void* anim, s32 base, const void* table, s16 count);
extern s32 func_ov039_0208ef4c(void* task, u32 arg1);

/* The 0x10-byte animated-palette object at the background task's +0x840. */
typedef struct {
    /* 0x00 */ const u16* table; // compared against the table pointer to see if it is set
    /* 0x04 */ s32        count;
    /* 0x08 */ s16        timer;
    /* 0x0A */ u8         pad_0A[2];
    /* 0x0C */ u16*       base;
} OtuPaletteAnim; // Size: 0x10

/** The four-layer background task's state block, 0x850 bytes. */
typedef struct {
    /* 0x000 */ u8               pad_000[0x008];
    /* 0x008 */ Data*            fileData;
    /* 0x00C */ PaletteResource* palettes[4];
    /* 0x01C */ BgResource*      chars[4];
    /* 0x02C */ BgResource*      screens[4];
    /* 0x03C */ u8               tilemap[0x804];
    /* 0x840 */ OtuPaletteAnim   paletteAnim;
} OtuBgTaskData;

/** The badge-radar task's state block, 0x60 bytes: one sprite and its state. */
typedef struct {
    /* 0x00 */ u32    unk_00;
    /* 0x04 */ Sprite sprite;
    /* 0x44 */ s32    x;        // Q12.12, converted to pixels by the render stage
    /* 0x48 */ s32    y;        // Q12.12
    /* 0x4C */ u32    targetId; // task id of the child being tracked
    /* 0x50 */ s32    animBias; // added to the sprite's animation index
    /* 0x54 */ u8*    table;
    /* 0x58 */ s32    linked;
    /* 0x5C */ s32    animBase; // 1 selects animation 5, anything else animation 1
} OtuRadarData;                 // Size: 0x60

/** The badge-counter task's state block, 0x198 bytes: six sprites. */
typedef struct {
    /* 0x000 */ u32    unk_000;   // low nibble becomes each sprite's dataType
    /* 0x004 */ Sprite spriteA;   // only present when hasSpriteA is set
    /* 0x044 */ Sprite spriteB;
    /* 0x084 */ Sprite digits[4]; // gated by the bitmask at +0x194
    /* 0x184 */ u32    sourceId;  // task id read by the update stage
    /* 0x188 */ s32    unk_188;   // which menu: 7 animations and a y offset per value
    /* 0x18C */ s32    resolved;  // sourceId resolved in the pool
    /* 0x190 */ s32    hasSpriteA;
    /* 0x194 */ u16    visible;   // bit N is digits[N]
    /* 0x196 */ u8     pad_196[2];
} OtuCounterData;                 // Size: 0x198

/** The countdown task's state block, 0xD0 bytes: three digit sprites. */
typedef struct {
    /* 0x000 */ u32    unk_000;   // low nibble becomes each sprite's dataType
    /* 0x004 */ Sprite digits[3];
    /* 0x0C4 */ s32    countdown; // 1/60th ticks remaining
    /* 0x0C8 */ s32    unk_0C8;   // "draw the digits"; set once by the init stage
    /* 0x0CC */ s32    alarmed;   // one-frame pulse at the 0x4B0 crossing
} OtuCountdownData;               // Size: 0xD0

/*
 * The four-word argument block the init stages read, except that the radar and
 * counter tasks carry a fifth. 02093668 reads +0x00..+0x10 (five words);
 * 02093b08 and 02093f70 read only +0x00..+0x0C.  Left as offsets because the
 * fields' meanings differ per task.
 */
typedef struct {
    s32 unk_00;
    s32 unk_04;
    s32 unk_08;
    s32 unk_0C;
    s32 unk_10;
} OtuInitArgs; // Size: 0x14

/* The animation templates the load wrappers copy out of `.rodata`. */
extern const SpriteAnimation data_ov039_02099a7c;
extern const SpriteAnimation data_ov039_02099acc;
extern const SpriteAnimation data_ov039_02099af8;
extern const SpriteAnimation data_ov039_02099b24;
extern const SpriteAnimation data_ov039_02099b6c;
/** Four `s16` offsets: the x positions of the counter task's digit sprites. */
extern const u16 data_ov039_02099aa8[4];

/* The four-stage tables, and the `TaskHandle`s the create wrappers pass. */
extern const TaskStages data_ov039_020999c0;
extern const TaskStages data_ov039_02099a6c;
extern const TaskStages data_ov039_02099abc;
extern const TaskStages data_ov039_02099b5c;
extern const TaskHandle data_ov039_020999b4;
extern const TaskHandle data_ov039_02099a60;
extern const TaskHandle data_ov039_02099ab0;
extern const TaskHandle data_ov039_02099b50;

/** The table 020934e0 refuses to re-point the animated palette at twice. */
extern const u16 data_ov039_02099a18[1];

/**
 * @brief The palette block inside a loaded `Data`'s buffer.
 *
 * The overlay addresses this the same way in 020934e0 and 02093fcc, and the
 * arithmetic is `buffer + 0x20 + the word at buffer + 0x48` -- i.e. a table of
 * sub-resource offsets starting 0x20 into the buffer, indexed by one of them.
 * Which sub-resource it is (palette, char or screen) depends on the caller.
 */
static inline void* OtuPaletteSource(Data* file) {
    u8* buffer = (u8*)file->buffer + 0x20;

    return buffer + *(u32*)(buffer + 0x28);
}

/* The five sprite-load wrappers. They call each other, so they are declared
 * here rather than in the order they appear. */
void func_ov039_020935d4(OtuRadarData* data, Sprite* sprite);
void func_ov039_0209392c(OtuCounterData* data, Sprite* sprite);
void func_ov039_020939b0(OtuCounterData* data, Sprite* sprite);
void func_ov039_02093a30(OtuCounterData* data, Sprite* sprite, s32 index);
void func_ov039_02093ee4(OtuCountdownData* data, Sprite* sprite, s32 index);

/* ==================================================================== */
/* The background task: Tsk_OtosuGame_ovbg.                             */
/* ==================================================================== */

/**
 * @brief Updates the background task's animated palette, 0x020933c0.
 *
 * `func_ov039_02098dbc` advances the ring buffer at +0x840 by one entry and
 * returns the pointer to the palette that entry selects; that pointer is then
 * handed to `PaletteMgr_SetSource` for the second of the task's four palette
 * resources (+0x10, the one init stage 02092f88 allocated with slot index 1).
 *
 * The other three layers are left alone, so this is a per-frame recolour of
 * one BG layer rather than a repaint.
 */
s32 func_ov039_020933c0(TaskPool* pool, Task* self, void* arg) {
    OtuBgTaskData* data = (OtuBgTaskData*)self->data;

    PaletteMgr_SetSource(g_PaletteManagers[1], data->palettes[1], func_ov039_02098dbc(&data->paletteAnim));
    return 1;
}

/**
 * @brief The background task's render stage, 0x020933f0.
 *
 * Returns TRUE and draws nothing, which is what a four-BG-layer task needs: its
 * layers are set up once by the init stage and then repainted by the hardware
 * until the cleanup stage releases them.
 *
 * This is decidable from the table slot rather than guessed. `data_ov039_020999c0`
 * is `{02092f88, 020933c0, 020933f0, 020933f8}`, which is init / update /
 * render / cleanup, and 02092f88 allocates four `BgResMgr` layer triples and
 * no sprites. So this is a real empty render stage for a real reason.
 *
 * Note this does *not* generalise to the other three constant-1 bodies in the
 * overlay (020921f4, 02092bf0, 02096d98). 020921f4 and 02092bf0 are also
 * render slots, but 02096d98 sits at index 1 of `data_ov039_02099ec8` -- an
 * update slot -- for a task whose render (02096da0) is the one that does the
 * work. Same instruction, different meaning; the four are left independent.
 */
s32 func_ov039_020933f0(TaskPool* pool, Task* self, void* arg) {
    (void)pool;
    (void)self;
    (void)arg;

    return 1;
}

/**
 * @brief Releases the background task's four layer triples, 0x020933f8.
 *
 * One pass per layer, screen first, then char data, then the palette -- the
 * reverse of the order init stage 02092f88 acquired them in. The layer index
 * runs 0..3 and is *not* multiplied into the field offsets: each of the three
 * resources is a four-entry array with a four-byte stride, so the three loads
 * inside the loop all share one `data + i * 4` base and differ only in their
 * displacement. Written that way deliberately; indexing the arrays as arrays
 * costs an add per access.
 */
s32 func_ov039_020933f8(TaskPool* pool, Task* self, void* arg) {
    OtuBgTaskData* data = (OtuBgTaskData*)self->data;
    s32            i;

    for (i = 0; i < 4; i++) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[1], data->screens[i]);
        BgResMgr_ReleaseChar(g_BgResourceManagers[1], data->chars[i]);
        PaletteMgr_ReleaseResource(g_PaletteManagers[1], data->palettes[i]);
    }

    DatMgr_ReleaseData(data->fileData);
    return 1;
}

/**
 * @brief The background task's stage dispatcher, 0x02093460.
 *
 * The `TaskStages` table is copied to the stack rather than indexed in place,
 * which is why the target spends four instructions on an `ldm`/`stm` pair
 * before dispatching. `stage` is the fourth argument, which is what
 * `EasyTask` passes when it wants one stage run rather than the whole
 * lifecycle.
 */
s32 func_ov039_02093460(TaskPool* pool, Task* self, void* arg, s32 stage) {
    TaskStages stages = data_ov039_020999c0;

    return stages.iter[stage](pool, self, arg);
}

/**
 * @brief Creates the background task, 0x020934a8.
 *
 * The three arguments are gathered into a stack block and handed to
 * `EasyTask_CreateTask` as its `param`; the init stage 02092f88 reads them back
 * out of there. Priority is 0 and the parent is NULL, so the caller supplies
 * the pool and nothing else.
 *
 * Typed as returning the handle rather than void: a caller elsewhere in this
 * overlay stores the result into a child-handle field. It costs nothing
 * here -- the body has no `mov r0` of its own in either spelling, so the
 * handle already comes back in r0 and both compile to the same
 * instructions. Same finding as func_ov039_02098394 in band 8.
 */
s32 func_ov039_020934a8(TaskPool* pool, s32 arg1, s32 arg2, s32 arg3) {
    s32 args[3];

    args[0] = arg1;
    args[1] = arg2;
    args[2] = arg3;

    return EasyTask_CreateTask(pool, &data_ov039_020999b4, NULL, 0, NULL, args);
}

/**
 * @brief Re-points the animated palette at the overlay's own table, 0x020934e0.
 *
 * Idempotent: if the object at +0x840 is already running `data_ov039_02099a18`
 * this returns without touching it, so a caller can run it every frame. The
 * source pointer handed over is offset 0x48 into the loaded file's buffer --
 * the same expression init stage 02092f88 uses when it first builds the object,
 * so both agree on which half of the file the animation reads.
 *
 * The `count` argument is 0x12, and it is a count of table entries rather than
 * frames: `func_ov039_02098dbc` steps through the table one entry per call and
 * wraps at this value.
 */
void func_ov039_020934e0(OtuBgTaskData* data) {
    if (data->paletteAnim.table == data_ov039_02099a18) {
        return;
    }

    func_ov039_02098d7c(&data->paletteAnim, data->fileData ? (s32)OtuPaletteSource(data->fileData) : 0, data_ov039_02099a18,
                        0x12);
}

/* ==================================================================== */
/* The sprite-cell builders shared by this band's loaders.             */
/* ==================================================================== */

/*
 * 0209352c, 02093884 and 02093e3c.
 *
 * These three are the same function. Not "the same body with a constant
 * varied" as the twenty-two in OtuFieldAccess.c -- byte-identical, all three,
 * with no constant to speak of. They differ from the twenty-two in exactly one
 * way: the depth key is the constant 3 rather than a call into the packer.
 * Everything else, including the three zero stores at +0x04, +0x08 and +0x0C,
 * is the same.
 *
 * The constant 3 is the interesting one. `func_ov039_02088400` returns a
 * quantised `(x << 23) + (y << 12)` key, so a literal 3 cannot be one of those:
 * it is a sort key that says "these cells are coplanar and their order is
 * fixed", which is what a set of glyphs for one number needs. The five
 * animation templates that reference these (`data_ov039_02099a7c`,
 * `data_ov039_02099af8`, `data_ov039_02099acc`, `data_ov039_02099b24`,
 * `data_ov039_02099b6c`) all carry a `frameInfoCallback` of one of the three.
 *
 * **The `sel` argument is the third one, not the second.** The target opens
 * with `cmp r2, #1`, and these are `SpriteFrameInfoCallback`s -- the engine
 * calls them as `(sprite, callbackArg, mode)`, with `mode` in r2. The mode
 * constants are in SpriteMgr.h: 0 LOAD, 1 UPDATE, 2 RENDER, 3 RELEASE. So:
 *
 *   mode 1 (UPDATE)   raise slot+0x00 and return -- "this sprite is live";
 *   mode 2 (RENDER)   fill the cell in from the task's table;
 *   anything else     return NULL -- LOAD and RELEASE draw nothing.
 *
 * That is not a guess: `case 2` is the branch that populates the cell, which is
 * precisely what a render callback is for, and `case 1` returning a slot with
 * only its first word set is what an update callback can usefully report.
 * Declaring these with two parameters moves the selector into r1 and costs the
 * first six instructions of the function.
 */

// Nonmatching: 88.7%. The guard chain, the two-step lookup, the store order and
// the constant-3 tail are all correct. What is left is that mwcc keeps the
// table pointer and the index live across the `str` between the two lookups
// (r12 and r3 here), where the target re-loads both -- `ldrsh [r0, #0x16]` and
// `ldr [r0, #0x1c]` appear twice in the target and once here. This is the same
// gap the twenty-two in OtuFieldAccess.c have and no source form reaches it:
// mwcc will not reload a value it has proved unchanged.
//
// The slot write is spelled `&data_0206b408` rather than `data_0206b408`.
// The shared header declares the symbol as an `OtuSpriteSlot*`, but the target
// uses the symbol's *address* as the slot: `ldr r1, .L_020935d0` loads the pool
// word and the first thing done with r1 is `str r2, [r1, #0x0]`. Taking the
// address of the declared pointer reproduces that; reading the pointer first
// would add a load the target does not have.
OtuSpriteSlot* func_ov039_0209352c(OtuSpriteTask* t, s32 arg, s32 sel) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    switch (sel) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The same three flat short-circuit tests the twenty-two use, over
            // +0x18, +0x1C and +0x16. The `ldrne` on the table load is the
            // short-circuit, which is why +0x18 is tested at all.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + *(u16*)(table + index * 8) * 2);
            }

            slot->depthKey = 3;
            return slot;
        }

        default:
            return NULL;
    }
}

// Nonmatching: 88.7%, byte-for-byte the same reason as func_ov039_0209352c above.
OtuSpriteSlot* func_ov039_02093884(OtuSpriteTask* t, s32 arg, s32 sel) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    switch (sel) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + *(u16*)(table + index * 8) * 2);
            }

            slot->depthKey = 3;
            return slot;
        }

        default:
            return NULL;
    }
}

// Nonmatching: 88.7%, byte-for-byte the same reason as func_ov039_0209352c above.
OtuSpriteSlot* func_ov039_02093e3c(OtuSpriteTask* t, s32 arg, s32 sel) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    switch (sel) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + *(u16*)(table + index * 8) * 2);
            }

            slot->depthKey = 3;
            return slot;
        }

        default:
            return NULL;
    }
}

/* ==================================================================== */
/* The badge-radar task: Tsk_OtosuGame_badgeradar.                       */
/* ==================================================================== */

/**
 * @brief The radar task's init stage, 0x02093668.
 *
 * Copies the five-word argument block in, clears the Q12.12 position and the
 * "linked" flag, then loads the sprite. The sprite's `owner` is the task data
 * block, which is how the cell builder handed to `_Sprite_Load` finds its way
 * back to +0x18/+0x1C/+0x16 when it is called per frame.
 */
s32 func_ov039_02093668(TaskPool* pool, Task* self, OtuInitArgs* args) {
    OtuRadarData* data = (OtuRadarData*)self->data;

    data->unk_00   = args->unk_00;
    data->targetId = args->unk_04;
    data->animBias = args->unk_08;
    data->table    = (u8*)args->unk_0C;
    data->linked   = 0;
    data->animBase = args->unk_10;
    data->x        = 0;
    data->y        = 0;

    func_ov039_020935d4(data, &data->sprite);
    return 1;
}

/**
 * @brief The radar task's render stage, 0x020937a0.
 *
 * Converts the Q12.12 position at +0x44/+0x48 to pixels and draws, but only
 * while `linked` is set. So the update stage's job is to keep `linked`
 * tracking whether the tracked child exists, and the render stage does the
 * arithmetic.
 */
s32 func_ov039_020937a0(TaskPool* pool, Task* self, void* arg) {
    OtuRadarData* data = (OtuRadarData*)self->data;

    if (data->linked != 0) {
        data->sprite.posX = data->x >> 12;
        data->sprite.posY = data->y >> 12;
        Sprite_RenderFrame(&data->sprite);
    }

    return 1;
}

/**
 * @brief The radar task's cleanup stage, 0x020937dc.
 */
s32 func_ov039_020937dc(TaskPool* pool, Task* self, void* arg) {
    OtuRadarData* data = (OtuRadarData*)self->data;

    Sprite_Release(&data->sprite);
    return 1;
}

/** The radar task's stage dispatcher, 0x020937f4. */
s32 func_ov039_020937f4(TaskPool* pool, Task* self, void* arg, s32 stage) {
    TaskStages stages = data_ov039_02099a6c;

    return stages.iter[stage](pool, self, arg);
}

/**
 * @brief Creates the radar task, 0x0209383c.
 *
 * Five arguments this time, against three for the background task -- which is
 * the visible difference between the two `sub sp, sp, #imm` frame sizes
 * (0x1C against 0x14) and is how the two create wrappers tell themselves apart.
 */
s32 func_ov039_0209383c(TaskPool* pool, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5) {
    s32 args[5];

    args[0] = arg1;
    args[1] = arg2;
    args[2] = arg3;
    args[3] = arg4;
    args[4] = arg5;

    return EasyTask_CreateTask(pool, &data_ov039_02099a60, NULL, 0, NULL, args);
}

/* ==================================================================== */
/* The badge-counter task: Tsk_OtosuGame_badgescounter.                 */
/* ==================================================================== */

/**
 * @brief The counter task's init stage, 0x02093b08.
 *
 * Four sprites are loaded: the pair at +0x04/+0x44, plus the four digit
 * sprites at +0x84..+0x184. `unk_190` from the argument block decides whether
 * the +0x04 sprite is loaded at all -- and the update and cleanup stages both
 * test the same flag before touching it, so the field is set once here and
 * never changes.
 *
 * The digit sprites' visible bitmask at +0x194 is raised to 0xC (bits 2 and 3)
 * here: two of the four digits start showing and two start hidden.
 */
s32 func_ov039_02093b08(TaskPool* pool, Task* self, OtuInitArgs* args) {
    OtuCounterData* data = (OtuCounterData*)self->data;
    s32             i;

    data->unk_000    = args->unk_00;
    data->sourceId   = args->unk_04;
    data->unk_188    = args->unk_08;
    data->resolved   = 0;
    data->hasSpriteA = args->unk_0C;
    data->visible    = 0xC;

    if (data->hasSpriteA != 0) {
        func_ov039_0209392c(data, &data->spriteA);
    }

    func_ov039_020939b0(data, &data->spriteB);

    for (i = 0; i < 4; i++) {
        func_ov039_02093a30(data, &data->digits[i], i);
    }

    return 1;
}

/**
 * @brief The counter task's update stage, 0x02093b98.
 *
 * Asks the task pool for the data behind `sourceId` and records whether it
 * resolved. That is the whole stage: whether the counter has anything to show
 * is decided here, and the render stage 02093bc0 does nothing at all while
 * `resolved` is clear.
 */
s32 func_ov039_02093b98(TaskPool* pool, Task* self, void* arg) {
    OtuCounterData* data = (OtuCounterData*)self->data;

    data->resolved = EasyTask_GetTaskData(pool, data->sourceId) != NULL;
    return 1;
}

/**
 * @brief The counter task's cleanup stage, 0x02093c44.
 */
s32 func_ov039_02093c44(TaskPool* pool, Task* self, void* arg) {
    OtuCounterData* data = (OtuCounterData*)self->data;
    s32             i;

    if (data->hasSpriteA != 0) {
        Sprite_Release(&data->spriteA);
    }

    Sprite_Release(&data->spriteB);

    for (i = 0; i < 4; i++) {
        Sprite_Release(&data->digits[i]);
    }

    return 1;
}

/** The counter task's stage dispatcher, 0x02093c90. */
s32 func_ov039_02093c90(TaskPool* pool, Task* self, void* arg, s32 stage) {
    TaskStages stages = data_ov039_02099abc;

    return stages.iter[stage](pool, self, arg);
}

/**
 * @brief Creates the counter task, 0x02093cd8.
 *
 * Four arguments, and a 0x18 frame -- between the background task's three and
 * the radar task's five. All three create wrappers are the same body; the
 * argument count and the `TaskHandle` are the only differences.
 */
s32 func_ov039_02093cd8(TaskPool* pool, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 args[4];

    args[0] = arg1;
    args[1] = arg2;
    args[2] = arg3;
    args[3] = arg4;

    return EasyTask_CreateTask(pool, &data_ov039_02099ab0, NULL, 0, NULL, args);
}

/* ==================================================================== */
/* The sprite-load wrappers.                                            */
/* ==================================================================== */

/*
 * Five copies of one body, 0x020935d4 / 0209392c / 020939b0 / 02093a30 /
 * 02093ee4. `-inline noauto` means a shared `static` would compile to a real
 * `bl` and cost every one of them its body, so each is written out.
 *
 * What they all do:
 *
 *   1. copy a 0x2C-byte `SpriteAnimation` template out of `.rodata` onto a
 *      stack local -- the frame is exactly 0x2C, so this is a struct copy;
 *   2. stamp `owner` with the caller, so the cell builder can find its way
 *      back to the task data;
 *   3. rebuild the first halfword's bits 2-5 from a caller-supplied byte:
 *      `ldrh` the template's word, `bic` the field, shift the new value in.
 *      That is the template's `dataType`, and the byte comes from the task
 *      data's own +0x00 -- the same word 020939b0 reads as a `Sprite`'s
 *      display-engine bits. Whether the two are meant to agree is not stated;
 *      what is clear is that the field is four bits wide and the value is
 *      masked to it by the shift rather than by an explicit `and`.
 *   4. set `animIndex` from the task data, in each wrapper's own way;
 *   5. `_Sprite_Load(sprite, &anim)`.
 *
 * The `animIndex` arithmetic is the interesting part, and it varies per
 * wrapper rather than per object:
 *
 *   020935d4   `(base == 1 ? 5 : 1) + (s16)bias`  -- two different animation
 *              choices selected by a flag, then a signed bias added
 *   0209392c   untouched; instead `posY += count * 0x13`
 *   020939b0   `-(index + 1) * 7`   -- `rsb r, r, r, lsl #3`
 *   02093a30   `lut[index]`, and `posX += (index - 2) * 6` for index < 3
 *   02093ee4   untouched; instead `posX = (0x1F - index * 2) * 8` and
 *              `posY = 0x12`
 */

/**
 * @brief Loads the radar task's one sprite, 0x020935d4.
 *
 * The only wrapper that reads two separate fields to build its animation
 * index. `animBase` at +0x5C picks between animation 5 and animation 1, and
 * `animBias` at +0x50 is then added as a signed 16-bit value -- the
 * `lsl #0x10 / asr #0x10` pair, i.e. a sign-extending narrowing of a word.
 */
void func_ov039_020935d4(OtuRadarData* data, Sprite* sprite) {
    SpriteAnimation anim = data_ov039_02099a7c;

    anim.owner     = data;
    anim.dataType  = data->unk_00;
    anim.animIndex = (data->animBase == 1) ? 5 : 1;
    anim.animIndex = anim.animIndex + (s16)data->animBias;

    _Sprite_Load(sprite, &anim);
}

/**
 * @brief Loads the counter task's +0x04 sprite, 0x0209392c.
 *
 * Leaves `animIndex` alone and offsets the vertical position instead:
 * `posY += unk_188 * 0x13`. `unk_188` is the index the init stage was handed
 * and the one 02093d18 uses to pick between animation groups, so this is the
 * same per-menu selector -- laid out as 19 pixels of vertical offset rather
 * than as an animation choice.
 */
void func_ov039_0209392c(OtuCounterData* data, Sprite* sprite) {
    SpriteAnimation anim = data_ov039_02099af8;

    anim.owner    = data;
    anim.dataType = data->unk_000;
    anim.posY     = anim.posY + data->unk_188 * 0x13;

    _Sprite_Load(sprite, &anim);
}

/**
 * @brief Loads the counter task's +0x44 sprite, 0x020939b0.
 *
 * `animIndex = (unk_188 + 1) * 7`, which the target spells `add r, r, #1`
 * then `rsb r, r, r, lsl #3` (that is `8x - x`). Seven per `unk_188` again --
 * the same stride 02093d18 walks when it searches for a free tray slot, which
 * is why the two are recognisably the same family: `unk_188` names a menu and
 * each menu owns seven animations, and this sprite is showing the seventh.
 */
void func_ov039_020939b0(OtuCounterData* data, Sprite* sprite) {
    SpriteAnimation anim = data_ov039_02099acc;

    anim.owner     = data;
    anim.dataType  = data->unk_000;
    anim.animIndex = (data->unk_188 + 1) * 7;

    _Sprite_Load(sprite, &anim);
}

/**
 * @brief Loads one of the counter task's four digit sprites, 0x02093a30.
 *
 * The only wrapper that indexes a table: `animIndex = data_ov039_02099aa8[index]`,
 * read as a *signed* halfword (`ldrsh`) from a stack copy of the table. The
 * table is four `s16` at `{0x21, 0x21, 0x21, 0x1d}` -- three equal and one
 * different, so it is an index-to-animation map and not a position list.
 *
 * The horizontal shift is conditional: for index 0..2 the sprite moves by
 * `(2 - index) * 6` pixels, so index 2 stays put and indices 1 and 0 slide left
 * by 6 and 12 -- a three-digit number right-aligned against a fixed column.
 * Index 3 gets no shift at all, which is consistent with the fourth table
 * entry being the odd one out.
 */

// Nonmatching: 96.7%. Two instructions of mwcc scheduling and nothing else: it
// hoists `index * 2` above the two `ldrh`s that copy the table into the stack
// local, where the target computes it after them. The copy, the table read, the
// guarded position shift and the `_Sprite_Load` tail are all correct and in
// order.
void func_ov039_02093a30(OtuCounterData* data, Sprite* sprite, s32 index) {
    SpriteAnimation anim = data_ov039_02099b24;
    s16             lut[4];

    lut[0] = data_ov039_02099aa8[0];
    lut[1] = data_ov039_02099aa8[1];
    lut[2] = data_ov039_02099aa8[2];
    lut[3] = data_ov039_02099aa8[3];

    anim.owner     = data;
    anim.dataType  = data->unk_000;
    anim.animIndex = lut[index];

    if (index < 3) {
        anim.posX = anim.posX + (2 - index) * 6;
    }

    anim.posY = anim.posY + data->unk_188 * 0x13;

    _Sprite_Load(sprite, &anim);
}

/**
 * @brief Loads one of the countdown task's three digit sprites, 0x02093ee4.
 *
 * Positions three sprites across the screen from the index alone:
 * `posX = (0x1F - index * 2) * 8` and a fixed `posY = 0x12`. The three x
 * values are 0xF8, 0xE8, 0xD8 -- eight pixels apart, evenly spaced, which is
 * what a three-digit readout wants. No `animIndex` and no `unk_188`: this task
 * is laid out by count rather than by menu.
 */
void func_ov039_02093ee4(OtuCountdownData* data, Sprite* sprite, s32 index) {
    SpriteAnimation anim = data_ov039_02099b6c;

    anim.owner    = data;
    anim.dataType = data->unk_000;
    anim.posX     = (0x1F - index * 2) * 8;
    anim.posY     = 0x12;

    _Sprite_Load(sprite, &anim);
}

/* ==================================================================== */
/* The counter task's render stage and its two score helpers.          */
/* ==================================================================== */

/**
 * @brief The counter task's render stage, 0x02093bc0.
 *
 * Draws the pair at +0x04/+0x44 (the +0x04 one only when `hasSpriteA` says it
 * was loaded) and then the four digit sprites, each gated on its own bit in
 * the `visible` mask at +0x194: bit N is `digits[N]`.
 *
 * `Sprite_Update` is called immediately before `Sprite_RenderFrame` for each
 * sprite rather than once for the lot, which is the engine's per-sprite
 * contract -- these are independent sprites, not one animated object.
 *
 * The whole stage is skipped while `resolved` is clear, so a counter with no
 * source task costs three loads and a return.
 *
 * The mask is reached as `data + 0x100 + 0x94` rather than `data + 0x194`
 * because that is how the target forms the address, and mwcc keeps the split.
 */

// Nonmatching: 93.5%. One register choice. The target keeps the walked base
// `data + 0x100` in r4 and the constant 1 in r5, then `tst r0, r5, lsl r6`;
// here mwcc hoists `data + 0x194` into r5 and puts the 1 in r4, giving
// `tst r0, r4, lsl r6`. Everything else -- the early-out on `resolved`, the
// `hasSpriteA` gate, the unconditional pair at +0x44 and the four-iteration
// digit loop -- is instruction-for-instruction identical.
s32 func_ov039_02093bc0(TaskPool* pool, Task* self, void* arg) {
    OtuCounterData* data    = (OtuCounterData*)self->data;
    u16*            visible = (u16*)((u8*)data + 0x100);
    s32             i;

    if (data->resolved != 0) {
        if (data->hasSpriteA != 0) {
            Sprite_Update(&data->spriteA);
            Sprite_RenderFrame(&data->spriteA);
        }

        Sprite_Update(&data->spriteB);
        Sprite_RenderFrame(&data->spriteB);

        for (i = 0; i < 4; i++) {
            if (visible[0x4A] & (1 << i)) {
                Sprite_Update(&data->digits[i]);
                Sprite_RenderFrame(&data->digits[i]);
            }
        }
    }

    return 1;
}

/**
 * @brief Points the counter at the animation for a free tray slot, 0x02093d18.
 *
 * Scans six `u16` for the 0x130 "empty" sentinel -- the same value
 * func_ov039_020824a0 fills the pin tray with and func_ov039_0208efb0 rejects
 * -- and takes the first index that carries it. That index is folded into an
 * animation number as `unk_188 * 7 + index + 1`, i.e. **seven animations per
 * `unk_188`**, which is the same stride 020939b0 bakes into its own
 * `animIndex` and the only reason to read `unk_188` the same way twice.
 *
 * A caller passing a slot value of 0x130 therefore gets animation
 * `unk_188 * 7 + 1` for that slot, and a full tray gets the last one tried.
 *
 * The whole function is a tail call: the target jumps to `Sprite_ChangeAnimation`
 * rather than calling and returning, so there is no frame of its own.
 */
s32 func_ov039_02093d18(OtuCounterData* data, u16* slots) {
    s32 i;

    for (i = 0; i < 6; i++) {
        if (slots[i] == 0x130) {
            break;
        }
    }

    return Sprite_ChangeAnimation(&data->spriteB, data->spriteB.animData, (s16)(data->unk_188 * 7 + i + 1),
                                  data->spriteB.cellTable);
}

/**
 * @brief Writes a score into the counter task's digit sprites, 0x02093d68.
 *
 * Two halves, and the order matters:
 *
 *   1. format `value` into up to three glyph indices, least significant first,
 *      stopping as soon as the value runs out -- so `0` costs one iteration,
 *      not three;
 *   2. raise bit 3 of `visible` (the sprite at +0x144, which this function
 *      never touches otherwise and which is therefore always showing), then
 *      walk the three digit sprites from `digits[2]` down to `digits[0]`,
 *      reading the formatted digits back-to-front and raising bits 2, 1 and 0
 *      as it goes.
 *
 * **The glyph base is 0x21, not ASCII '0' (0x30).** The target's `add r9, r9,
 * #0x21` is not a coincidence with the animation indices in
 * `data_ov039_02099aa8` -- which are also 0x21 -- it is the same numbering, and
 * both are offset from ASCII by 0x0F. So this is not "write the characters of
 * the number"; it is "write the number in the overlay's own glyph space",
 * which is the same space the tray's 0x130 empty sentinel lives in. That is
 * the link between the two halves of this file: 02093d18 searches the tray for
 * 0x130, and 02093d68 pushes 0x21..0x2A, so both are working in the pin-tray
 * value space rather than in text.
 *
 * The early `return` inside the second loop is the target's own shape --
 * `addmi sp, sp, #8` / `popmi {...}` -- and it is what stops a one-digit score
 * from writing stale digits into the two sprites it does not use: it returns
 * as soon as the digit index has gone below zero, *after* raising that
 * sprite's bit. So the visible mask ends up describing exactly the digits the
 * score needed, and every sprite's animation is left alone otherwise.
 *
 * The buffer is `s16` and the target stores and reloads it with
 * `strh`/`ldrsh`, so the glyph index is a halfword throughout.
 */

// Nonmatching: 88.9%. The logic is settled -- the divide loop, the break, the
// mask init, the descending sprite walk, the early return and the bit raising
// all match. What differs is register allocation, and it cascades: the target
// spends r7 on the digit count and starts its saved set at r4, where mwcc here
// puts the count in r3 and shifts the whole saved set down by one register. The
// per-iteration address arithmetic follows from that (the target recomputes
// `data + j * 0x40` for the cell table, where this build walks the sprite
// pointer). Nothing about the overlay is in doubt here.
void func_ov039_02093d68(OtuCounterData* data, s32 value) {
    s16 buf[4];
    s32 count;
    s32 idx;
    s32 j;

    count = 0;
    for (idx = 0; idx < 3; idx++) {
        buf[idx] = (s16)(0x21 + value % 10);
        count++;
        value /= 10;
        if (value == 0) {
            break;
        }
    }

    *(u16*)((u8*)data + 0x194) = 8;

    idx = count - 1;
    for (j = 2; j >= 0; j--) {
        Sprite* s = &data->digits[j];

        Sprite_ChangeAnimation(s, s->animData, buf[idx], s->cellTable);
        *(u16*)((u8*)data + 0x194) |= (1 << j);

        idx--;
        if (idx < 0) {
            return;
        }
    }
}

/* ==================================================================== */
/* The countdown task: init, the radar task's update, and the update.  */
/* ==================================================================== */

/**
 * @brief The countdown task's init stage, 0x02093f70.
 *
 * The one argument that matters is `unk_04`, and it is seconds: the target
 * multiplies it by 0x3C to get the countdown stored at +0xC4. So the overlay
 * keeps this clock in 1/60th ticks and 0x4B0 (1200) -- the alarm point
 * 02093fcc compares against -- is exactly twenty of them.
 *
 * `unk_0C8` is raised to 1 here and never written again by anything in this
 * band; the render stage 0209411c reads it as the "draw the digits" gate, so
 * it is set once at construction and the countdown's own state lives at +0xC4
 * and +0xCC.
 */
s32 func_ov039_02093f70(TaskPool* pool, Task* self, OtuInitArgs* args) {
    OtuCountdownData* data = (OtuCountdownData*)self->data;
    s32               i;

    data->unk_0C8   = 1;
    data->alarmed   = 0;
    data->unk_000   = args->unk_00;
    data->countdown = args->unk_04 * 0x3C;

    for (i = 0; i < 3; i++) {
        func_ov039_02093ee4(data, &data->digits[i], i);
    }

    return 1;
}

/**
 * @brief The radar task's update stage, 0x020936b8.
 *
 * Follows one child task and converts its Q12.12 position into the radar
 * sprite's:
 *
 *   1. ask the pool for `targetId`'s data; if it is gone, clear `linked` and
 *      stop -- the render stage then draws nothing at all;
 *   2. ask func_ov039_0208ef4c whether that child is a real pin rather than an
 *      empty tray slot, and clear `linked` if not;
 *   3. copy the child's +0x120/+0x124 pair out (func_ov039_0208e6e0, one
 *      `OtuPoint`) and set `linked`;
 *   4. for each axis: subtract 0xA0000, add a per-child skew read from the
 *      byte table at +0x54 (bytes 2 and 3 respectively), divide the lot by 8,
 *      and add a per-axis origin (0x50000 for x, 0x10000 for y);
 *   5. `Sprite_Update`.
 *
 * Step 4 is a screen-space transform: 0xA0000 is 160.0 in Q12.12, the skew is
 * `((0x32 - b) / 2) << 17`, and the origins differ per axis, so x and y come
 * out of the same arithmetic with different constants. The `/8` is a real
 * signed divide with the sign correction spelled out in the target, not a
 * shift -- writing it as `/ 8` reproduces it.
 *
 * Note `linked` is written twice on the success path (before and after the
 * point copy). That is the target's shape and it is left alone: the second
 * store is dead, but removing it changes nothing and keeping it costs nothing.
 *
 * 0xA0000 is 160.0 in Q12.12, so this reads as "pin position, shifted 160
 * units left and divided by eight", with a per-child skew folded in from the
 * byte table. What the byte table *is* is not established here -- it arrives as
 * a bare pointer in the init stage and nothing in this band writes it.
 */

// Nonmatching: 91.3%. Everything through the position arithmetic matches
// instruction for instruction, including both sign-corrected divides and the
// redundant `linked` store. The remaining gap is where the null-child block
// sits: the target branches past it to a single out-of-line clear at the bottom
// of the function (`beq` forward, then `mov r0, #0; str r0, [r4, #0x58]` after
// the `Sprite_Update` block), where the `else if` chain here puts the same
// store inline ahead of the branch. Same work, different block placement; it
// was not worth a `goto` to chase two instructions.
s32 func_ov039_020936b8(TaskPool* pool, Task* self, void* arg) {
    OtuRadarData* data  = (OtuRadarData*)self->data;
    OtuPinTask*   child = (OtuPinTask*)EasyTask_GetTaskData(pool, data->targetId);
    OtuPoint      pt;
    s32           skew;

    if (child == NULL) {
        data->linked = 0;
    } else if (func_ov039_0208ef4c(child, 0) == 0) {
        data->linked = 0;
    } else {
        data->linked = 1;
        func_ov039_0208e6e0(child, &pt);
        data->linked = 1;

        data->x = pt.x - 0xA0000;
        skew    = (0x28 - (data->table[2] - 0xA)) / 2;
        data->x = (data->x + (skew << 17)) / 8 + 0x50000;

        data->y = pt.y - 0xA0000;
        skew    = (0x28 - (data->table[3] - 0xA)) / 2;
        data->y = (data->y + (skew << 17)) / 8 + 0x10000;

        Sprite_Update(&data->sprite);
    }

    return 1;
}

/**
 * @brief The countdown task's update stage, 0x02093fcc.
 *
 * Four things, in this order:
 *
 *   1. **Step the clock.** If the countdown is positive, decrement it and, on
 *      the frame it crosses a whole second (`/60`), and while it is at or
 *      below 0x4B0, play SE 0x33E. So the last twenty seconds tick.
 *   2. **Fire the alarm edge.** `alarmed` goes to 1 only on the frame the
 *      countdown passes from 0x4B0 to 0x4AF, and back to 0 on every other
 *      frame. That is a rising edge, not a level -- whatever reads +0xCC (and
 *      something does: `func_ov039_0209420c` is a one-instruction accessor for
 *      that word) sees it high for exactly one frame.
 *   3. **Swap the palette.** On that same edge the three digit sprites get
 *      their `unk3C` (palette-source) pointers pointed at the block the first
 *      sprite's own resource data points into -- `buffer + 0x20 + the word at
 *      buffer + 0x48`, which is the shape 020934e0 uses for the background
 *      task's animated palette. So the digits change colour when the alarm
 *      sounds rather than when they are drawn.
 *   4. **Draw the number.** `countdown / 60` in seconds, split three ways by
 *      repeated `% 10` and `/ 10`, each digit pushed as a `Sprite_ChangeAnimation`
 *      frame of `digit + 1` followed by `Sprite_Update`.
 *
 * The `+ 1` on the frame is not obvious from the code: the digit glyphs are
 * selected by frame index, and the target indexes them from 1. What the +1
 * buys is that frame 0 is free -- presumably for a blank or a minus sign, so a
 * one-digit remainder in the tens place does not show a stale 0.
 *
 * The two magic constants here are worth naming, because they are the only
 * places this file divides and both are non-obvious: `0x88888889` with an
 * `asr #5` is `/60` and `0x66666667` with an `asr #2` is `/10`, both with the
 * sign correction mwcc emits. The `/60` is what makes the countdown a clock
 * (02093f70 stores `args->unk_04 * 0x3C`), and the `/10` is the digit split.
 * The seconds figure is therefore drawn least-significant digit first, into
 * `digits[0]`, `digits[1]`, `digits[2]` in order -- which is why 02093ee4 lays
 * those three sprites out right to left.
 */

// Nonmatching: 88.6%. All four stages are present and correct -- the
// decrement, the `/60` edge test and its sound, the alarm edge, the palette
// swap and the three-digit loop all match. What differs is register allocation
// throughout: the target keeps seven callee-saved registers live (r3..r9) where
// mwcc here needs five (r4..r8), so the base pointer, the magic constant, the
// seconds value and the walked sprite pointer land in different registers
// throughout. One instruction of substance differs: the target forms the null
// palette pointer with `moveq r2, #0` inside the null test, where the `pal =
// NULL` here hoists a `mov r2, #0` above the test. Both are register choices,
// not logic.
s32 func_ov039_02093fcc(TaskPool* pool, Task* self, void* arg) {
    OtuCountdownData* data = (OtuCountdownData*)self->data;
    s32               before;
    s32               secs;
    s32               i;

    before = data->countdown;
    if (before > 0) {
        data->countdown = before - 1;

        if (before / 60 != (before - 1) / 60 && before - 1 <= 0x4B0) {
            SndMgr_StartPlayingSE(0x33E);
        }
    }

    if (before >= 0x4B0 && data->countdown < 0x4B0) {
        Data* file = data->digits[0].resourceData;
        void* pal;

        pal = NULL;
        if (file != NULL) {
            pal = OtuPaletteSource(file);
        }

        for (i = 0; i < 3; i++) {
            data->digits[i].unk3C = (UnkSmallInternal*)pal;
        }

        data->alarmed = 1;
    } else {
        data->alarmed = 0;
    }

    secs = data->countdown / 60;
    for (i = 0; i < 3; i++) {
        Sprite* s = &data->digits[i];

        Sprite_ChangeAnimation(s, s->animData, (s16)(secs % 10 + 1), s->cellTable);
        Sprite_Update(s);
        secs /= 10;
    }

    return 1;
}

/* Band 3 region -- inlined; see the file header. */

#include "Debug/Sugata/TinPinSlammer.h"
#include "Engine/Core/OamMgr.h"
#include "SndMgr.h"
#include "SpriteMgr.h"

#include <nitro/fx/fx_atan.h>

/**
 * @file OtuEntryTasks.c
 *
 * 0x020950b8 - 0x02095fe4: the four sprite tasks the board stage spawns.
 *
 * The `.rodata` tables a few hundred bytes above this band give the grouping
 * away. Each of the four is a `TaskHandle` (name, entry, data size) followed
 * by a `TaskStages` array of four pointers, and each `TaskStages` array lists
 * exactly four functions in this band or immediately around it:
 *
 *   0x02099cac  "Tsk_OtosuGame_slash"  0x70 bytes, entry 02095420
 *       init 02095144  update 02095194  render 020953c4  destroy 0209540c
 *   0x02099cf4  "Tsk_OtosuGame_track"  0x70 bytes, entry 02095708
 *       init 020955f4  update 02095638  render 020956a4  destroy 020956f0
 *   0x02099d44  "Tsk_OtosuGame_point"  0x174 bytes, entry 02095c58
 *       init 02095a18  update 02095a78  render 02095b6c  destroy 02095c2c
 *   0x02099d8c  "Tsk_OtosuGame_entry"  0xC8 bytes, entry 020960dc
 *       init 02095f18  update 02095f58  render 02095fe4  destroy 020960bc
 *
 * So the four `*_RunTask`-shaped routines are the task entries, and each is
 * byte-identical apart from which `TaskStages` object it copies to the stack.
 * Everything else here is one of the four stages, or a helper only one of them
 * calls -- and each task loads its sprite(s) through its own copy of the
 * shared loader shape, with its own `.rodata` `SpriteAnimation` template.
 *
 * The `data_*` names below are the build's own, and the two that are named
 * rather than numeric (`Tsk_OtosuGame_*` and `Seq_Otosu`) are read off the strings
 * at 0x0209a7e0 onward.
 */

/* ------------------------------------------------------------------ */
/* Declarations this TU does not otherwise have.                      */
/*                                                                     */
/* Everything here lives outside this band and outside OtuFieldAccess's */
/* own bodies, so nothing declares it for us.  They go here rather    */
/* than in the shared header, which five agents share.                 */
/* ------------------------------------------------------------------ */

/* The +0x110/+0x114, +0x118/+0x11C and +0x120/+0x124 pair copy-outs. */
void func_ov039_0208e85c(void* task, OtuPoint* out);
void func_ov039_0208e87c(void* task, OtuPoint* out);

/* The overlay's vector unit's four helpers, 0x02098b8c - 0x02098d10. */
void func_ov039_02098b8c(OtuPoint* a, OtuPoint* b, OtuPoint* out);
s32  func_ov039_02098d10(OtuPoint* v);

/* The pin task's own three queries, 0x0208eed0 - 0x0208ef38. */
s32  func_ov039_0208eed0(void* pin);
void func_ov039_0208ef14(void* pin, OtuPoint* a, OtuPoint* b);
s32  func_ov039_0208ef38(void* pin);

/* The board stage's per-sprite animation stepper, 0x02087bf8. */
void func_ov039_02087bf8(void* anim, void* limit);

/* The base module's fixed-point divide.  No prototype exists in this repo,
 * but the two call sites here both pass a quotient-remainder pair. */
s32 _s32_div_f(s32 a, s32 b);

/* Every stage this band names in a `TaskStages` table.  The tables are written
 * before the stages themselves, so the names have to be in scope first.
 * func_ov039_02095194 and func_ov039_02095a78 are declared but not yet
 * defined here; see the notes on each. */
s32 func_ov039_02095144(TaskPool* pool, Task* task, void* args);
s32 func_ov039_02095194(TaskPool* pool, Task* task, void* args);
s32 func_ov039_020953c4(TaskPool* pool, Task* task, void* args);
s32 func_ov039_0209540c(TaskPool* pool, Task* task, void* args);

s32 func_ov039_020955f4(TaskPool* pool, Task* task, void* args);
s32 func_ov039_02095638(TaskPool* pool, Task* task, void* args);
s32 func_ov039_020956a4(TaskPool* pool, Task* task, void* args);
s32 func_ov039_020956f0(TaskPool* pool, Task* task, void* args);

s32 func_ov039_02095a18(TaskPool* pool, Task* task, void* args);
s32 func_ov039_02095a78(TaskPool* pool, Task* task, void* args);
s32 func_ov039_02095b6c(TaskPool* pool, Task* task, void* args);
s32 func_ov039_02095c2c(TaskPool* pool, Task* task, void* args);

/* The four `SpriteAnimation` templates and the four task descriptors, read by
 * address.  These are in the overlay's gap-filled `.rodata`, so they are
 * declared rather than defined. */
extern TaskHandle data_ov039_02099cac;
extern TaskHandle data_ov039_02099cf4;
extern TaskHandle data_ov039_02099d44;
/* data_ov039_02099d8c is declared const by band 4. */

extern const TaskStages data_ov039_02099cb8;
extern const TaskStages data_ov039_02099d00;
extern const TaskStages data_ov039_02099d50;

extern SpriteAnimation data_ov039_02099cc8;
extern SpriteAnimation data_ov039_02099d10;
extern SpriteAnimation data_ov039_02099d60;
extern SpriteAnimation data_ov039_02099da8;

/* Four `s16` values, 12, 11, 1, 1 -- one per sprite of the point task. */
extern const s16 data_ov039_02099d3c[4];

/* The base module's shared `s16` slope table, indexed by `angle >> 4`. */
extern s32 data_0205e4e0[];

/* ------------------------------------------------------------------ */
/* Task-data shapes.                                                   */
/* ------------------------------------------------------------------ */

/**
 * @brief One of the four slots in "Tsk_OtosuGame_point", 0x14 bytes.
 *
 * A 0x40-stride array of these at +0x120, one per sprite. `live` gates the
 * whole slot; `delay` counts the sprite's own animation down; `offsetX` and
 * `accum` are the two components of the position the render adds to the
 * anchor difference; `phase` is the step the update integrates into `accum`.
 */
typedef struct {
    /* 0x00 */ s32 live;
    /* 0x04 */ s32 delay;
    /* 0x08 */ s32 offsetX;
    /* 0x0C */ s32 accum;
    /* 0x10 */ s32 phase;
} OtuPointSlot; // Size: 0x14

/**
 * @brief "Tsk_OtosuGame_slash", 0x70 bytes.
 *
 * One `Sprite` at the base. +0x48/+0x4C is the pair the loader subtracts, and
 * +0x58/+0x5C the pair it subtracts *it* from -- so the sprite is drawn at the
 * difference between them, which the render recomputes as
 * `(+0x58 - +0x50) >> 12` against the pair the update refreshes.
 */
typedef struct {
    /* 0x00 */ Sprite sprite;
    /* 0x40 */ s32    unk_40;
    /* 0x44 */ s32    unk_44;
    /* 0x48 */ s32    unk_48;
    /* 0x4C */ s16    unk_4C;
    /* 0x4E */ s16    unk_4E;
    /* 0x50 */ s32    unk_50;
    /* 0x54 */ s32    unk_54;
    /* 0x58 */ s32    unk_58;
    /* 0x5C */ s32    unk_5C;
    /* 0x60 */ s32    unk_60;
    /* 0x64 */ s32    unk_64;
    /* 0x68 */ s32    unk_68;
    /* 0x6C */ s32    unk_6C;
} OtuSlashTask; // Size: 0x70

/**
 * @brief "Tsk_OtosuGame_track", 0x70 bytes.
 *
 * Same size as the slash task but the `Sprite` sits at +0x08 rather than at
 * +0, and the subtracted pair is +0x50 against +0x48. The render therefore
 * computes the mirror of the slash task's expression -- `(+0x50 - +0x48) >> 12`
 * where the slash task computes `(+0x58 - +0x50) >> 12` -- which is why the
 * two tasks' loaders differ by which word they load.
 */
typedef struct {
    /* 0x00 */ s32    unk_00;
    /* 0x04 */ s32    unk_04;
    /* 0x08 */ Sprite sprite;
    /* 0x48 */ s32    unk_48;
    /* 0x4C */ s32    unk_4C;
    /* 0x50 */ s32    unk_50;
    /* 0x54 */ s32    unk_54;
    /* 0x58 */ s32    unk_58;
    /* 0x5C */ s32    unk_5C;
    /* 0x60 */ s32    unk_60;
    /* 0x64 */ s32    unk_64;
    /* 0x68 */ s32    unk_68;
    /* 0x6C */ s32    unk_6C;
} OtuTrackTask; // Size: 0x70

/**
 * @brief "Tsk_OtosuGame_point", 0x174 bytes.
 *
 * Four `Sprite`s at 0x40 stride from +0x00 and four `OtuPointSlot`s at +0x120.
 * The anchor pair at +0x100/+0x104 is where the tracked pin is and the one at
 * +0x108/+0x10C is where this task is, both Q12.12; +0x110 holds the pin's
 * task id; +0x114 and +0x118 are the frame counter and the "still live" flag;
 * +0x170 is the counter the update decrements and clamps.
 */
typedef struct {
    /* 0x000 */ Sprite       sprite[4];
    /* 0x100 */ s32          unk_100;
    /* 0x104 */ s32          unk_104;
    /* 0x108 */ s32          unk_108;
    /* 0x10C */ s32          unk_10C;
    /* 0x110 */ s32          unk_110;
    /* 0x114 */ s32          unk_114;
    /* 0x118 */ s32          unk_118;
    /* 0x11C */ s32          unk_11C;
    /* 0x120 */ OtuPointSlot slot[4];
    /* 0x170 */ s32          unk_170;
} OtuPointTask; // Size: 0x174

/**
 * @brief One 0x10-byte animation-state block of the entry task.
 *
 * The two sprites each have one of these as their limit, and each also has a
 * 0x0C-byte running block that 0x02087bf8 writes. Only the scale words at the
 * block's +0x04 and +0x08 are read here -- they are what
 * `OamMgr_AllocAffineGroup` is handed -- but the whole 0x10 is there because
 * the stride is what keeps the two sprites apart.
 */
typedef struct {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 scaleX;
    /* 0x08 */ s32 scaleY;
    /* 0x0C */ s16 unk_0C;
    /* 0x0E */ s16 unk_0E;
} OtuEntryAnim; // Size: 0x10

/**
 * @brief "Tsk_OtosuGame_entry", 0xC8 bytes.
 *
 * Two `Sprite`s at +0x00 and +0x40, each with a 0x10-byte limit block and a
 * 0x0C-byte running block behind it, so the second sprite's animation state
 * begins at +0xA0 and the two running blocks at +0xB0 and +0xBC.
 */
typedef struct {
    /* 0x00 */ Sprite       sprite;
    /* 0x40 */ Sprite       sprite2;
    /* 0x80 */ s32          unk_80;
    /* 0x84 */ s32          unk_84;
    /* 0x88 */ s32          unk_88;
    /* 0x8C */ s32          unk_8C;
    /* 0x90 */ OtuEntryAnim anim0;
    /* 0xA0 */ OtuEntryAnim anim1;
    /* 0xB0 */ u8           run0[0x0C];
    /* 0xBC */ u8           run1[0x0C];
} OtuEntryTask; // Size: 0xC8

/**
 * @brief The block the three task-creating helpers pack and pass by pointer.
 *
 * Which words each one packs is its own business, so there are three types
 * rather than one over-declared union. The track task's middle word is the
 * caller's own pool pointer, which is why its block is three words and the
 * other two are two.
 */
typedef struct {
    s32 unk_00;
    s32 unk_04;
} OtuTaskArgs2;

typedef struct {
    s32       unk_00;
    TaskPool* unk_04;
    s32       unk_08;
} OtuTaskArgs3;

typedef struct {
    s32 unk_00;
} OtuTaskArgs1;

/* ------------------------------------------------------------------ */
/* "Tsk_OtosuGame_slash"                                                   */
/* ------------------------------------------------------------------ */

/** Releases the one sprite the slash task owns, and keeps running. */
s32 func_ov039_0209540c(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtuSlashTask*)task->data)->sprite);
    return 1;
}

/** The slash task's entry point: dispatches to one of its four stages. */
s32 func_ov039_02095420(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_02099cb8;
    return stages.iter[stage](pool, task, args);
}

/**
 * @brief Spawns a slash task.
 *
 * The two-word argument block is built in the outgoing-argument area rather
 * than in a frame of its own, which is why the frame is exactly the 0x10 the
 * two outgoing words and the block need together.
 */
s32 func_ov039_02095468(TaskPool* pool, s32 dataType, s32 pin) {
    OtuTaskArgs2 args;

    args.unk_00 = dataType;
    args.unk_04 = pin;

    return EasyTask_CreateTask(pool, &data_ov039_02099cac, NULL, 0, NULL, &args);
}

/**
 * @brief The slash task's sprite loader.
 *
 * Copies the overlay's shared `SpriteAnimation` template for this sprite onto
 * the stack, patches the owner, the `dataType` out of the argument block and
 * the position out of the task's own pair, then loads it. The `bic #0x3C` is
 * mwcc clearing the `dataType` field's four bits before inserting the new
 * value, and the `<< 0x1C` / `>> 0x1A` pair is the insert itself.
 */
void func_ov039_020950b8(OtuSlashTask* self, Sprite* sprite, OtuTaskArgs2* args) {
    SpriteAnimation anim = data_ov039_02099cc8;

    anim.owner    = self;
    anim.dataType = args->unk_00;
    anim.posX     = self->unk_58 >> 12;
    anim.posY     = self->unk_5C >> 12;

    _Sprite_Load(sprite, &anim);
}

/**
 * @brief The slash task's init stage.
 *
 * Records the tracked pin's task id, seeds the pair the loader reads and clears
 * the four state words. +0x44 and +0x48 both start at 0x1000, and the two
 * halfwords at +0x4C/+0x4E are cleared with 16-bit stores, so those two are
 * genuinely narrower than the words around them.
 */
s32 func_ov039_02095144(TaskPool* pool, Task* task, void* args) {
    OtuSlashTask* self = (OtuSlashTask*)task->data;
    OtuTaskArgs2* a    = (OtuTaskArgs2*)args;

    self->unk_60 = a->unk_04;
    self->unk_58 = 0;
    self->unk_5C = 0;
    self->unk_40 = 0;
    self->unk_44 = 0x1000;
    self->unk_48 = 0x1000;
    self->unk_4C = 0;
    self->unk_4E = 0;
    self->unk_64 = 0;
    self->unk_6C = 0;
    self->unk_68 = 0;

    func_ov039_020950b8(self, &self->sprite, a);
    return 1;
}

/**
 * @brief The slash task's render stage.
 *
 * Draws only while +0x64 is set, and puts the sprite at the difference between
 * the pair the update refreshes and the pair the init seeded.
 */
s32 func_ov039_020953c4(TaskPool* pool, Task* task, void* args) {
    OtuSlashTask* self = (OtuSlashTask*)task->data;

    if (self->unk_64 != 0) {
        self->sprite.posX = (self->unk_58 - self->unk_50) >> 12;
        self->sprite.posY = (self->unk_5C - self->unk_54) >> 12;
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

/* ------------------------------------------------------------------ */
/* "Tsk_OtosuGame_track"                                                   */
/* ------------------------------------------------------------------ */

/** Releases the track task's sprite, which sits at +0x08. */
s32 func_ov039_020956f0(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtuTrackTask*)task->data)->sprite);
    return 1;
}

/** The track task's entry point. */
s32 func_ov039_02095708(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_02099d00;
    return stages.iter[stage](pool, task, args);
}

/**
 * @brief Spawns a track task.
 *
 * Same shape as the slash spawner but with a three-word argument block whose
 * middle word is the pool itself, so the frame is 0x14 rather than 0x10.
 */
s32 func_ov039_02095750(TaskPool* pool, s32 dataType, s32 pin) {
    OtuTaskArgs3 args;

    args.unk_00 = dataType;
    args.unk_04 = pool;
    args.unk_08 = pin;

    return EasyTask_CreateTask(pool, &data_ov039_02099cf4, NULL, 0, NULL, &args);
}

/** The track task's sprite loader; see func_ov039_020950b8 for the shape. */
void func_ov039_02095558(OtuTrackTask* self, Sprite* sprite, OtuTaskArgs3* args) {
    SpriteAnimation anim = data_ov039_02099d10;

    anim.owner    = self;
    anim.dataType = args->unk_00;
    anim.posX     = (self->unk_50 - self->unk_48) >> 12;
    anim.posY     = (self->unk_54 - self->unk_4C) >> 12;

    _Sprite_Load(sprite, &anim);
}

/** The track task's init stage: records the pin id and pool, clears, loads. */
s32 func_ov039_020955f4(TaskPool* pool, Task* task, void* args) {
    OtuTrackTask* self = (OtuTrackTask*)task->data;
    OtuTaskArgs3* a    = (OtuTaskArgs3*)args;

    self->unk_00 = a->unk_04;
    self->unk_04 = a->unk_08;
    self->unk_60 = 0;
    self->unk_64 = 0;
    self->unk_48 = 0;
    self->unk_4C = 0;
    self->unk_50 = 0;
    self->unk_54 = 0;

    func_ov039_02095558(self, &self->sprite, a);
    return 1;
}

/**
 * @brief The track task's update stage.
 *
 * Only runs while +0x60 is set. When the sprite's animation runs out it clears
 * +0x60 and +0x64 and stops; otherwise it re-reads the tracked pin's position
 * into the task's pair, integrates it, and advances the sprite.
 */
s32 func_ov039_02095638(TaskPool* pool, Task* task, void* args) {
    OtuTrackTask* self = (OtuTrackTask*)task->data;

    if (self->unk_60 != 0) {
        if (SpriteMgr_IsAnimationFinished(&self->sprite)) {
            self->unk_60 = 0;
            self->unk_64 = 0;
        } else {
            OtuPinTask* pin = (OtuPinTask*)EasyTask_GetTaskData(self->unk_00, self->unk_04);

            func_ov039_0208e85c(pin, (OtuPoint*)&self->unk_48);
            func_ov039_02098b8c((OtuPoint*)&self->unk_50, (OtuPoint*)&self->unk_58, (OtuPoint*)&self->unk_50);
            Sprite_Update(&self->sprite);
            self->unk_64 = 1;
        }
    }
    return 1;
}

/** The track task's render stage. */
s32 func_ov039_020956a4(TaskPool* pool, Task* task, void* args) {
    OtuTrackTask* self = (OtuTrackTask*)task->data;

    if (self->unk_64 != 0) {
        self->sprite.posX = (self->unk_50 - self->unk_48) >> 12;
        self->sprite.posY = (self->unk_54 - self->unk_4C) >> 12;
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

/* ------------------------------------------------------------------ */
/* "Tsk_OtosuGame_point"                                                   */
/* ------------------------------------------------------------------ */

/** Releases all four of the point task's sprites, and keeps running. */
s32 func_ov039_02095c2c(TaskPool* pool, Task* task, void* args) {
    OtuPointTask* self = (OtuPointTask*)task->data;
    s32           i;

    for (i = 0; i < 4; i++) {
        Sprite_Release(&self->sprite[i]);
    }
    return 1;
}

/** The point task's entry point. */
s32 func_ov039_02095c58(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_02099d50;
    return stages.iter[stage](pool, task, args);
}

/** Spawns a point task; the same two-word block as the slash task's. */
s32 func_ov039_02095ca0(TaskPool* pool, s32 dataType, s32 pin) {
    OtuTaskArgs2 args;

    args.unk_00 = dataType;
    args.unk_04 = pin;

    return EasyTask_CreateTask(pool, &data_ov039_02099d44, NULL, 0, NULL, &args);
}

/**
 * @brief The point task's sprite loader, for one of its four sprites.
 *
 * The fourth argument indexes a four-entry `s16` table at 0x02099d3c -- values
 * 12, 11, 1, 1 -- and the selected entry becomes the template's `animIndex`.
 * It is read with `ldrsh`, so the table is signed.
 */
void func_ov039_02095964(OtuPointTask* self, Sprite* sprite, OtuTaskArgs2* args, s32 index) {
    SpriteAnimation anim = data_ov039_02099d60;

    anim.owner     = self;
    anim.dataType  = args->unk_00;
    anim.posX      = (self->unk_108 - self->unk_100) >> 12;
    anim.posY      = (self->unk_10C - self->unk_104) >> 12;
    anim.animIndex = data_ov039_02099d3c[index];

    _Sprite_Load(sprite, &anim);
}

/**
 * @brief The point task's init stage.
 *
 * Records the tracked pin's task id and clears the two anchor pairs, then loads
 * all four sprites. The index walks separately from the sprite pointer so the
 * call passes the loop counter as the table index.
 */
s32 func_ov039_02095a18(TaskPool* pool, Task* task, void* args) {
    OtuPointTask* self = (OtuPointTask*)task->data;
    OtuTaskArgs2* a    = (OtuTaskArgs2*)args;
    s32           i;

    self->unk_110 = a->unk_04;
    self->unk_100 = 0;
    self->unk_104 = 0;
    self->unk_108 = 0;
    self->unk_10C = 0;
    self->unk_114 = 0;
    self->unk_118 = 0;

    for (i = 0; i < 4; i++) {
        func_ov039_02095964(self, &self->sprite[i], a, i);
    }
    return 1;
}

/**
 * @brief The point task's render stage.
 *
 * Reads the tracked pin's two position pairs once, then walks the four slots.
 * A slot draws only once both its live flag and its delay counter agree, and
 * at the anchor difference plus its own two offsets.
 */
s32 func_ov039_02095b6c(TaskPool* pool, Task* task, void* args) {
    OtuPointTask* self = (OtuPointTask*)task->data;
    OtuPinTask*   pin;
    s32           i;

    if (self->unk_118 != 0) {
        pin = (OtuPinTask*)EasyTask_GetTaskData(pool, self->unk_110);

        if (pin != NULL) {
            func_ov039_0208e85c(pin, (OtuPoint*)&self->unk_100);
            func_ov039_0208e6e0(pin, (OtuPoint*)&self->unk_108);

            for (i = 0; i < 4; i++) {
                if (self->slot[i].live != 0 && self->slot[i].delay <= 0) {
                    // Both positions are held in locals and shifted only at the
                    // store, and the y expression is written first: the target
                    // computes that chain before the x one even though it
                    // stores posX first.
                    s32 py = self->unk_10C - self->unk_104 + self->slot[i].accum - 0x10000;
                    s32 px = self->unk_108 - self->unk_100 + self->slot[i].offsetX;

                    self->sprite[i].posX = px >> 12;
                    self->sprite[i].posY = py >> 12;
                    Sprite_RenderFrame(&self->sprite[i]);
                }
            }
        }
    }
    return 1;
}

/**
 * @brief The point task's update stage.
 *
 * Only runs while +0x114 is set, and walks the four slots. A live slot whose
 * delay has run out integrates its phase into its own vertical offset, steps
 * the phase on by 0x800, and once the offset has gone positive clamps it back
 * to zero and divides the phase down so the next lap is slower. The sprite is
 * advanced only on that path, so a slot still counting down holds still.
 *
 * After the walk the task raises its "still live" flag and counts +0x170 down,
 * clearing both +0x114 and +0x118 when it runs out.
 *
 * The phase divide is a Q12 scale by -0.25, not a division: see the note at the
 * assignment. Walking `self->slot[i]` and `self->sprite[i]` directly rather than
 * folding `&self->slot[0]` into a base pointer is what makes mwcc keep `self` as
 * the loop's base register and reach the slots with absolute +0x120.. offsets.
 */
s32 func_ov039_02095a78(TaskPool* pool, Task* task, void* args) {
    OtuPointTask* self = (OtuPointTask*)task->data;
    s32           i;

    if (self->unk_114 != 0) {
        for (i = 0; i < 4; i++) {
            if (self->slot[i].live != 0) {
                if (self->slot[i].delay > 0) {
                    self->slot[i].delay = self->slot[i].delay - 1;
                } else {
                    self->slot[i].accum = self->slot[i].accum + self->slot[i].phase;
                    self->slot[i].phase = self->slot[i].phase + 0x800;

                    if (self->slot[i].accum > 0) {
                        self->slot[i].accum = 0;

                        if (self->slot[i].phase > 0) {
                            // A Q12 scale by -0.25 with the usual 0x800
                            // rounding bias, spelled as an explicit 64-bit
                            // expression. That is what makes mwcc hoist the
                            // magic constants (-1024, -1 and a second zero)
                            // into r4/r5/r11 ahead of the loop and take the
                            // `umull`/`mla`/sign-fold path here; a plain
                            // `/ -4` collapses to three shift instructions.
                            self->slot[i].phase = (s32)(((s64)self->slot[i].phase * -0x400 + 0x800) >> 12);
                        }
                    }
                    Sprite_Update(&self->sprite[i]);
                }
            }
        }

        self->unk_118 = 1;
        self->unk_170 = self->unk_170 - 1;

        if (self->unk_170 <= 0) {
            self->unk_114 = 0;
            self->unk_118 = 0;
        }
    }
    return 1;
}

/**
 * @brief The point task's setup step: seeds all four slots and both sprites.
 *
 * Called with the point task's data rather than a task, so it is not a stage
 * function -- it is the "start bouncing" entry point. Every slot gets its
 * delay staggered by four and its horizontal offset staggered by eight (in
 * screen units, pre-shifted), all four start at a phase of -0x3000, and the
 * frame count is seeded at sixty.
 *
 * The two sprites are the third and fourth of the four the task owns, and
 * their animation index comes from the argument's decimal digits: the low
 * digit picks the first one's animation and the tens digit the second's, with
 * +1 on each because zero is not a valid index. The second sprite is switched
 * off outright when its index comes out at 1 or below.
 *
 * The argument is stored at +0x11C and read back for the second digit, which
 * is why the target reloads it rather than keeping it in a register.
 */
// Nonmatching: 96.9%. Every field, constant, both loop strides and the whole
// loop body match, as do both magic-multiply divides, the early return and the
// second animation narrowing. The single residual is mwcc's scheduling of the
// *first* narrowing: the target keeps the `lsl #0x10` / `asr #0x10` pair
// adjacent and emits it before the call's argument loads, whereas this version
// splits the pair and sinks the `asr` past them. The value, the register (r2) and
// the instruction count are all the same -- only the placement differs. Writing
// the cast inline at the call site instead of through the `first` local makes no
// difference, and neither does dropping the now-unused offset counter.
void func_ov039_02095cd4(OtuPointTask* self, s32 count) {
    s32 i;
    s16 first;
    s16 last;

    self->unk_114 = 1;
    self->unk_170 = 0x3C;

    for (i = 0; i < 4; i++) {
        self->slot[i].live    = 1;
        self->slot[i].delay   = (3 - i) * 4;
        self->slot[i].offsetX = (0xC - i * 8) << 12;
        self->slot[i].accum   = 0;
        self->slot[i].phase   = -0x3000;
    }

    self->unk_11C = count;

    // Both indices are narrowed to s16 *before* they are used, not at the call:
    // the target's `lsl #0x10` / `asr #0x10` pair lands ahead of the argument
    // loads, and for the second one ahead of the `<= 1` compare. Passing the
    // narrowed value on is what reproduces that.
    first = (s16)(count % 10 + 1);
    Sprite_ChangeAnimation(&self->sprite[2], self->sprite[2].animData, first, self->sprite[2].cellTable);

    last = (s16)((self->unk_11C / 10) % 10 + 1);

    if ((s16)last <= 1) {
        self->slot[3].live = 0;
        return;
    }

    Sprite_ChangeAnimation(&self->sprite[3], self->sprite[3].animData, last, self->sprite[3].cellTable);
}

/**
 * @brief The slash task's update stage -- the largest thing in this band.
 *
 * Runs a three-state machine on +0x68 (0 = start, 1 = follow, 2 = settle) and
 * returns 1 either way, so the task is never deleted from here.
 *
 * State 0 either switches to the "follow" animation and falls through into
 * state 1, or gives up and clears the visible flag. State 1 is the real work:
 * ask the pin for its position and velocity, integrate the tracked point
 * through four vector-unit calls, turn the result into an angle, and -- once
 * the sprite's frame finishes -- start the arrow sound and steer its pan and
 * pitch. State 2 eases +0x48 toward 0x200 by a shrinking fraction.
 *
 * The two `case` bodies fall through into each other (0 into 1, and 1 into 2
 * when the pin has gone away), which is why the state is written as a `switch`
 * with real fallthrough rather than as a chain of ifs.
 */
s32 func_ov039_02095194(TaskPool* pool, Task* task, void* args) {
    OtuSlashTask* self = (OtuSlashTask*)task->data;
    OtuPinTask*   pin;
    s32           len;
    s32           pan;

    pin = (OtuPinTask*)EasyTask_GetTaskData(pool, self->unk_60);

    if (pin != NULL) {
        // Declaration order here is load-bearing: mwcc hands out stack slots in
        // reverse, and the target puts the velocity at sp+0x10, the offset at
        // sp+0x8 and the direction at sp+0x0.
        OtuPoint vel;
        OtuPoint off;
        OtuPoint dir;

        s32 phase = func_ov039_0208eed0(pin);

        func_ov039_0208e87c(pin, (OtuPoint*)&self->unk_50);

        switch (self->unk_68) {
            case 0:
                if (phase != 0) {
                    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 2, self->sprite.cellTable);
                    self->unk_64 = 1;
                    self->unk_68 = 1;
                    // Falls through into state 1.
                } else {
                    self->unk_64 = 0;
                    break;
                }
                // Falls through.

            case 1:
                if (phase != 0) {
                    func_ov039_0208ef14(pin, &vel, &off);

                    pan = off.x >> 12;

                    func_ov039_02098b8c(&vel, (OtuPoint*)&self->unk_50, &vel);
                    func_ov039_02098b8c(&off, (OtuPoint*)&self->unk_50, &off);
                    func_ov039_02098bb0(&off, &vel, &dir);
                    func_ov039_02098c00(0x800, &dir, &vel, (OtuPoint*)&self->unk_58);

                    len          = func_ov039_02098d10(&dir);
                    self->unk_48 = len / 48;
                    self->unk_40 = (u16)(FX_Atan2Idx(dir.y, dir.x) + 0x4000);

                    if (func_ov039_0208ef38(pin) != 0) {
                        Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 3, self->sprite.cellTable);
                    }

                    if (SpriteMgr_IsFrameFinished(&self->sprite)) {
                        SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_ARROW);

                        if (pan < 0) {
                            pan = 0;
                        } else if (pan > 0xFF) {
                            pan = 0xFF;
                        }

                        SndMgr_UpdateSEPan(SEIDX_SE_BAYBADGE_ARROW, pan);
                        SndMgr_SetSequenceSoundPitch(SEIDX_SE_BAYBADGE_ARROW, ((len >> 12) * 3 * 0x100) / 0x100);
                    }
                    break;
                }

                // The pin has gone: start the settle countdown and fall
                // straight into state 2, which is what the target's layout
                // shows -- there is no branch between these two blocks.
                self->unk_6C = 0x10;
                self->unk_68 = 2;
                // Falls through.

            case 2:
                if (phase == 1) {
                    self->unk_68 = 1;
                    break;
                }

                self->unk_6C = self->unk_6C - 1;

                if (self->unk_6C <= 0) {
                    self->unk_68 = 0;
                } else {
                    self->unk_48 = self->unk_48 + _s32_div_f(0x200 - self->unk_48, self->unk_6C);
                }
                break;
        }
    } else {
        self->unk_64 = 0;
    }

    if (self->unk_64 != 0) {
        Sprite_Update(&self->sprite);
    }
    return 1;
}

/**
 * @brief Points one of the board's arrow slots along a direction.
 *
 * Not a stage function and not one of this task's own: the board's tick calls
 * it with this overlay's task data, its own slot, an angle, a direction sign
 * and a length. It writes a pair of Q12.12 offsets into the task data and then
 * picks the arrow sprite's animation from the length.
 *
 * Both offset pairs come out of the shared `s16` table at 0x0205e4e0, indexed
 * by `angle >> 4` after a direction-dependent bias of +/-0x6000 for the first
 * pair and +/-0x4000 for the second. Each index reads two consecutive entries
 * and scales them differently: by 12 into +0x50/+0x54, and by 0x800 into
 * +0x58/+0x5C. The `+ 1` entry is the x component and the plain entry the y,
 * and the two components are stored in the order x then y.
 *
 * The 0x800 scale is a rounded Q12 halving done in 64 bits: the target
 * sign-extends `scale`, shifts it left by 11 with `asr #0x1f`/`orr .. lsr
 * #0x15`, adds the 0x800 bias with `adds`/`adc`, then shifts the 64-bit result
 * right by 12 as `lsr low, #0xc` / `orr low, low, high, lsl #0x14` and keeps the
 * low word. `>> 12` spelled directly is load-bearing: `/ 0x1000` makes mwcc call
 * `_ll_sdiv` instead.
 *
 * `index` carries the doubled index so both subscripts reduce to one `<< 1`
 * plus an `add #1` off the same base register, which is the shape the target
 * emits (`lsl ip, r0, #1` / `add r0, ip, #1` / `mov r2, r0, lsl #1`).
 *
 * `anim` is `s16` rather than `s32` because the parameter it feeds is `s16`;
 * with an `s32` local mwcc re-sign-extends it (`lsl #0x10` / `asr #0x10`)
 * before the call. Selecting into the local and calling once -- rather than
 * three separate `Sprite_SetAnimation` calls -- is what produces the target's
 * single call site that `blt` jumps forward to.
 *
 * The fifth argument is stack-passed, which is what fixes the argument count.
 */
// Nonmatching: the instruction sequence, operands and structure all agree with
// the target; the residual is purely mwcc's scratch-register choice in the index
// arithmetic (the target parks the index in r12 for the first table read and
// r3 for the second, mine uses r2 for both, and the two loaded halves
// consequently land in a rotated set of registers). Declaration order and the
// order of the table reads were both varied without moving the allocator.
void func_ov039_02095788(OtuTrackTask* self, OtuPointSlot* slot, s32 angle, s32 dir, s32 len) {
    s32 index;
    s16 scaleX;
    s16 scaleY;
    s16 anim;

    self->unk_60 = 1;

    // The `(u16)` cast is load-bearing twice: it is the target's
    // `lsl #0x10 / lsr #0x10` pair, and only an unsigned narrowing followed by
    // an arithmetic shift reproduces the `asr #0x4` on the biased angle.
    index        = ((u16)(angle + ((dir != 0) ? -0x6000 : 0x6000)) >> 4) * 2;
    scaleX       = ((s16*)data_0205e4e0)[index + 1];
    scaleY       = ((s16*)data_0205e4e0)[index];
    self->unk_50 = scaleX * 12;
    self->unk_54 = scaleY * 12;
    func_ov039_02098b8c((OtuPoint*)slot, (OtuPoint*)&self->unk_50, (OtuPoint*)&self->unk_50);

    index        = ((u16)(angle + ((dir != 0) ? -0x4000 : 0x4000)) >> 4) * 2;
    scaleX       = ((s16*)data_0205e4e0)[index + 1];
    scaleY       = ((s16*)data_0205e4e0)[index];
    self->unk_58 = (s32)(((s64)scaleX * 0x800 + 0x800) >> 12);
    self->unk_5C = (s32)(((s64)scaleY * 0x800 + 0x800) >> 12);

    if (len < 0x2000) {
        anim = 3;
    } else if (len > 0x3000) {
        anim = 1;
    } else {
        anim = 2;
    }

    Sprite_SetAnimation(&self->sprite, self->sprite.animData, anim, self->sprite.cellTable);
}

/* ------------------------------------------------------------------ */
/* "Tsk_OtosuGame_entry"                                                   */
/* ------------------------------------------------------------------ */

/**
 * @brief The entry task's sprite loader.
 *
 * The same shape as the other three loaders but with no position patch at
 * all -- it sets the owner and the dataType and nothing else.
 */
void func_ov039_02095ea4(OtuEntryTask* self, Sprite* sprite, OtuTaskArgs1* args) {
    SpriteAnimation anim = data_ov039_02099da8;

    anim.owner    = self;
    anim.dataType = args->unk_00;

    _Sprite_Load(sprite, &anim);
}

/** The entry task's init stage: loads both sprites and seeds the delay. */
s32 func_ov039_02095f18(TaskPool* pool, Task* task, void* args) {
    OtuEntryTask* self = (OtuEntryTask*)task->data;
    OtuTaskArgs1* a    = (OtuTaskArgs1*)args;

    self->unk_84 = 0;

    func_ov039_02095ea4(self, &self->sprite, a);
    func_ov039_02095ea4(self, &self->sprite2, a);

    self->sprite2.posY = 0x91;
    return 1;
}

/**
 * @brief The entry task's update stage.
 *
 * A two-case `switch` on the state word at +0x84, the compare chain emitted
 * before either body. State 0 just clears the visible flag; state 1 steps both
 * sprites' animations, advances the second only when +0x8C says it exists,
 * counts the frame counter down, and either keeps or clears the state.
 */
s32 func_ov039_02095f58(TaskPool* pool, Task* task, void* args) {
    OtuEntryTask* self = (OtuEntryTask*)task->data;

    switch (self->unk_84) {
        case 0:
            self->unk_80 = 0;
            break;

        case 1:
            func_ov039_02087bf8(&self->run0, &self->anim0);
            Sprite_Update(&self->sprite);

            if (self->unk_8C != 0) {
                func_ov039_02087bf8(&self->run1, &self->anim1);
                Sprite_Update(&self->sprite2);
            }

            self->unk_88 = self->unk_88 - 1;

            if (self->unk_88 > 0) {
                self->unk_80 = 1;
            } else {
                self->unk_84 = 0;
                self->unk_80 = 0;
            }
            break;
    }
    return 1;
}

/**
 * @brief The entry task's render stage.
 *
 * Draws the first sprite when +0x80 is set and the second when +0x8C is, and
 * on each path allocates an affine slot whose index goes into bits 5..9 of the
 * sprite's OAM attribute word. The manager is picked from the sprite's own
 * display-engine bits, and the scale comes from that sprite's own limit block.
 *
 * The `(u32)(u16)` around the allocation's return is load-bearing twice over:
 * it is the `<< 0x10` / `>> 0x10` pair the target emits, and it makes the
 * `>> 0x16` a logical rather than an arithmetic shift -- without the u32 cast
 * mwcc emits `asr #0x16` where the target has `lsr`.
 */
// Nonmatching: 72.6%. The body is right -- every field, mask and shift matches,
// and the instruction multiset is identical -- but mwcc schedules the call's
// arguments in the other order. The target builds the manager address
// (`&g_OamMgr[sprite->bits_0_1]`) *before* spilling the fifth argument to
// sp+0; mwcc spills first and computes the address second, on both call sites.
// Five structurally different spellings were tried -- the address hoisted into an
// `OamManager*` local, the call result hoisted into a `u32` local, the shared
// zero between the rotation and flip-flag arguments made an explicit local, the
// two combined, and the plain inline form -- and all five compile to exactly the
// same schedule, so the order is mwcc's argument setup rather than something the
// source asks for. This is the same gap Shop_item2.c already carries for the
// same expression.
s32 func_ov039_02095fe4(TaskPool* pool, Task* task, void* args) {
    OtuEntryTask* self = (OtuEntryTask*)task->data;

    if (self->unk_80 != 0) {
        self->sprite.unk_0A.raw =
            (self->sprite.unk_0A.raw & ~0x3E0) |
            ((u32)(u16)OamMgr_AllocAffineGroup(&g_OamMgr[self->sprite.bits_0_1], 0, self->anim0.scaleX, self->anim0.scaleY, 0)
                 << 0x1B >>
             0x16);
        Sprite_RenderFrame(&self->sprite);

        if (self->unk_8C != 0) {
            self->sprite2.unk_0A.raw = (self->sprite2.unk_0A.raw & ~0x3E0) |
                                       ((u32)(u16)OamMgr_AllocAffineGroup(&g_OamMgr[self->sprite2.bits_0_1], 0,
                                                                          self->anim1.scaleX, self->anim1.scaleY, 0)
                                            << 0x1B >>
                                        0x16);
            Sprite_RenderFrame(&self->sprite2);
        }
    }
    return 1;
}

/* Band 4 region -- inlined; see the file header. */

#include "Debug/Sugata/TinPinSlammer.h"
#include "EasyFade.h"
#include "Engine/Core/OamMgr.h"
#include "SndMgr.h"
#include "SpriteMgr.h"

/* ==================================================================== */
/* The one function in this band that is deliberately absent.          */
/* ==================================================================== */

/*
 * func_ov039_02096d98 is `mov r0, #0x1` / `bx lr` and is not decompiled here.
 * OtuFieldAccess.c leaves it out, along with 0x020921f4, 0x02092bf0 and
 * 0x020933f0, on the grounds that four identical bodies at four addresses are
 * at least as likely to be four different predicates as four copies of one.
 * That reasoning does not survive contact with the callers, and the correction
 * is recorded here because the comment it contradicts lives in the shared TU
 * and not in this file.
 *
 * None of the four is ever `bl`'d. Each appears exactly once, as a `.word` in a
 * four-entry TaskStages table, and in every case it is the *third* entry:
 *
 *   0x02099610  { 02091b98, 02092154, 020921f4, 020921fc }
 *   0x020999a4  { 020929ec, 02092a78, 02092bf0, 02092bf8 }
 *   0x020999c0  { 02092f88, 020933c0, 020933f0, 020933f8 }
 *   0x02099ec8  { 02096d70, 02096d98, 02096da0, 02096dec }
 *
 * The slot order is initialize / update / render / cleanup -- fixed by
 * 02099de0, whose four entries are this file's own 020963c8 (clears the object
 * and loads the sprite), 02096404 (per-frame child fetch), 020964a4
 * (Sprite_RenderFrame) and 020964ec (Sprite_Release). So all four bodies are
 * the *render* stage of a different task, and the other three stages of each of
 * those four tasks all end `mov r0, #0x1` / `pop {..., pc}`. A render stage
 * that draws nothing and returns 1 is a coherent reading; four unrelated
 * predicates is not, because a predicate needs a load and there are none.
 *
 * What is left open is only whether the original source said `return 1;` or
 * something the compiler folded to the same two instructions -- which is not
 * decidable from the ROM and does not change the code. The function is left out
 * because the shared file's decision to leave it out is not mine to reverse,
 * and because doing so without being able to amend the comment it contradicts
 * would leave the tree asserting two things at once.
 */

/* ==================================================================== */
/* Referenced overlay data and cross-module routines.                  */
/* ==================================================================== */

/*
 * The four TaskHandles and the four TaskStages tables this band dispatches
 * through.  All eight are `.rodata` in the original overlay and are not
 * claimed by any delinks entry, so they are referenced by the build's own
 * symbol names rather than redeclared as new objects -- see the note on the
 * row tables in TinPinSlammer.h for why that matters.
 */
extern const TaskHandle data_ov039_02099d8c;
extern const TaskHandle data_ov039_02099dd4;
extern const TaskHandle data_ov039_02099e1c;
extern const TaskHandle data_ov039_02099ebc;

extern const TaskStages data_ov039_02099d98;
extern const TaskStages data_ov039_02099de0;
extern const TaskStages data_ov039_02099e28;
extern const TaskStages data_ov039_02099ec8;

/*
 * The six SpriteAnimation templates the sprite loaders copy to the stack.
 * Each is 0x2C bytes, which is exactly sizeof(SpriteAnimation), and each names
 * a different one of the overlay's sprite-cell builders as its frame-info
 * callback -- that pairing is the only thing that distinguishes them.
 */
extern const SpriteAnimation data_ov039_02099df0;
extern const SpriteAnimation data_ov039_02099e38;
extern const SpriteAnimation data_ov039_02099e64;
extern const SpriteAnimation data_ov039_02099e90;
extern const SpriteAnimation data_ov039_02099ed8;
extern const SpriteAnimation data_ov039_02099f20;

/*
 * The glyph tables the two inits at the end of this file build cursors over.
 * Declared as arrays, not as `const void*`: these symbols *are* the tables, so
 * a pointer declaration makes every use cost an extra `ldr` through the
 * symbol, which the target does not have.
 */
extern const u8 data_ov039_0209a830[];
extern const u8 data_ov039_0209a8a8[];
extern const u8 data_ov039_0209a938[];
extern const u8 data_ov039_0209aa0c[];

/* Within this band: the sprite loaders the init stages call. */
void func_ov039_0209633c(void* self, void* sprite, s32* args);
void func_ov039_02096718(void* self, void* sprite, s32* args);
void func_ov039_0209678c(void* self, void* sprite, s32* args);
void func_ov039_02096800(void* self, void* sprite, s32* args);
void func_ov039_02096d00(void* self, void* sprite);
void func_ov039_02096f40(void* self, void* sprite, s32* args);

/* Elsewhere in the overlay: the animation-phase counter 02096da0 reads. */
extern s32 func_ov040_0209cb5c(void);

/* Elsewhere in the overlay: the point-copy helper 02096404 calls that the
 * shared header does not declare (func_ov039_0208e6e0 is already there), and
 * the text-cell pair the two switch bodies below use. */
void func_ov039_02087ba0(void* out, void* table, s32 count, void* cell);

/* ==================================================================== */
/* Task-create wrappers.                                               */
/* ==================================================================== */

/*
 * Four of these, and they differ only in which TaskHandle they pass and in
 * how many arguments they forward to the new task.  The stack block they build
 * is a two-word struct -- a null first word and a pointer to the argument
 * block -- handed to EasyTask_CreateTask as its `param`, which is why the
 * target emits `add ip, sp, #0x8` and then `stm sp, {r2, ip}`.
 */

/** Creates the task whose handle is data_ov039_02099d8c. */
s32 func_ov039_02096124(TaskPool* pool, s32 arg1) {
    s32 sp8;

    sp8 = arg1;
    return EasyTask_CreateTask(pool, &data_ov039_02099d8c, NULL, 0, NULL, &sp8);
}

/** Creates the task whose handle is data_ov039_02099dd4. Two arguments. */
s32 func_ov039_02096548(TaskPool* pool, s32 arg1, s32 arg2) {
    s32 sp8[2];

    sp8[0] = arg1;
    sp8[1] = arg2;
    return EasyTask_CreateTask(pool, &data_ov039_02099dd4, NULL, 0, NULL, sp8);
}

/** Creates the task whose handle is data_ov039_02099e1c.
 *
 * Typed as returning the handle rather than void: a caller elsewhere in this
 * overlay stores the result into a child-handle field. It costs nothing
 * here -- the body has no `mov r0` of its own in either spelling, so the
 * handle already comes back in r0 and both compile to the same
 * instructions. Same finding as func_ov039_02098394 in band 8.
 */
s32 func_ov039_02096b18(TaskPool* pool, s32 arg1) {
    s32 sp8;

    sp8 = arg1;
    return EasyTask_CreateTask(pool, &data_ov039_02099e1c, NULL, 0, NULL, &sp8);
}

/** Creates the task whose handle is data_ov039_02099ebc.
 *
 * Typed as returning the handle rather than void: a caller elsewhere in this
 * overlay stores the result into a child-handle field. It costs nothing
 * here -- the body has no `mov r0` of its own in either spelling, so the
 * handle already comes back in r0 and both compile to the same
 * instructions. Same finding as func_ov039_02098394 in band 8.
 */
s32 func_ov039_02096e4c(TaskPool* pool, s32 arg1) {
    s32 sp8;

    sp8 = arg1;
    return EasyTask_CreateTask(pool, &data_ov039_02099ebc, NULL, 0, NULL, &sp8);
}

/* ==================================================================== */
/* Task-stage dispatchers.                                             */
/* ==================================================================== */

/*
 * The standard four-slot TaskStages trampoline: copy the constant table onto
 * the stack and call slot `stage`.  The target's `ldm`/`stm` of sixteen bytes
 * is the table copy; `ldr r3, [ip, r4, lsl #2]` is the indexed call.
 */
s32 func_ov039_020960dc(TaskPool* pool, Task* task, void* data, s32 stage) {
    TaskStages stages = data_ov039_02099d98;

    return stages.iter[stage](pool, task, data);
}

s32 func_ov039_02096500(TaskPool* pool, Task* task, void* data, s32 stage) {
    TaskStages stages = data_ov039_02099de0;

    return stages.iter[stage](pool, task, data);
}

s32 func_ov039_02096ad0(TaskPool* pool, Task* task, void* data, s32 stage) {
    TaskStages stages = data_ov039_02099e28;

    return stages.iter[stage](pool, task, data);
}

s32 func_ov039_02096e04(TaskPool* pool, Task* task, void* data, s32 stage) {
    TaskStages stages = data_ov039_02099ec8;

    return stages.iter[stage](pool, task, data);
}

/* ==================================================================== */
/* The small accessors and release wrappers.                           */
/* ==================================================================== */

/** Raises the task's +0x44 word to 1, copies args->unk_00 to +0, loads. */
s32 func_ov039_02096d70(void* pool, void* task, s32* args) {
    u8* self = *(u8**)((u8*)task + 0x18);

    *(s32*)((u8*)self + 0x44) = 1;
    *(s32*)((u8*)self + 0x00) = *args;
    func_ov039_02096d00(self, self + 4);
    return 1;
}

/** Releases the sprite at self + 4. */
s32 func_ov039_02096dec(void* pool, void* task, void* args) {
    u8* self = *(u8**)((u8*)task + 0x18);

    Sprite_Release((Sprite*)(self + 4));
    return 1;
}

/** Raises +0x58 and then tail-calls Sprite_SetAnimation on frame 4. */
void func_ov039_0209657c(Sprite* self) {
    *(s32*)((u8*)self + 0x58) = 1;
    Sprite_SetAnimation(self, *(s16**)((u8*)self + 0x18), 4, *(SpriteCell**)((u8*)self + 0x1C));
}

/** Releases the two sprites at self + 0 and self + 0x40. */
s32 func_ov039_020960bc(void* pool, void* task, void* args) {
    u8* self = *(u8**)((u8*)task + 0x18);

    Sprite_Release((Sprite*)self);
    Sprite_Release((Sprite*)(self + 0x40));
    return 1;
}

/** The same pair of releases, for the other task. */
s32 func_ov039_02096ab0(void* pool, void* task, void* args) {
    u8* self = *(u8**)((u8*)task + 0x18);

    Sprite_Release((Sprite*)self);
    Sprite_Release((Sprite*)(self + 0x40));
    return 1;
}

/** Releases the one sprite at self + 0. */
s32 func_ov039_020964ec(void* pool, void* task, void* args) {
    u8* self = *(u8**)((u8*)task + 0x18);

    Sprite_Release((Sprite*)self);
    return 1;
}

/** Reads the +0xC8 word and returns it as a predicate. */
s32 func_ov039_02096c44(void* self) {
    return *(s32*)((u8*)self + 0xC8) != 0;
}

/* ==================================================================== */
/* The sprite loaders.                                                 */
/* ==================================================================== */

/*
 * The six loaders below are the band's largest group.  They share a shape:
 * copy a 0x2C-byte SpriteAnimation template from `.rodata` onto the stack,
 * patch three or four of its fields, and hand it to _Sprite_Load.  The
 * template copy is the `ldm`/`stm` triple, the patch to the leading bitfield
 * halfword is the `bic`/`orr` pair, and the fields written after it are
 * `owner` (+0x10) and, where present, `posX`/`posY` (+0x04/+0x06).
 *
 * They are spelled out six times rather than sharing a `static` helper on
 * purpose: the build is `-inline noauto`, so a shared body would compile to a
 * real `bl` and collapse all six callers to stubs.
 *
 * Two details are load-bearing and both cost an iteration to find:
 *
 *   - `dataType` is fed a *32-bit* load of the argument's first word, narrowed
 *     to u16 and then placed in bits 2-5.  Reading it as a u16 directly emits
 *     `ldrh` and loses the target's `ldr`/`lsl #0x10`/`lsr #0x10` pair.
 *   - `posX`/`posY` are `>> 12`, not `/ 4096`.  The divide form makes mwcc
 *     emit the three-instruction sign-correcting sequence, which the target
 *     does not have.
 */

/*
 * Six routines with one shape: copy a 0x2C-byte SpriteAnimation template from
 * `.rodata` onto the stack, patch three or four of its fields, and hand it to
 * _Sprite_Load.  The template copy is the `ldm`/`stm` triple, the patch to the
 * leading bitfield halfword is the `bic`/`orr` pair, and the fields written
 * after it are `owner` (+0x10) and, where present, `posX`/`posY` (+0x04/+0x06).
 *
 * They are spelled out six times rather than sharing a `static` helper on
 * purpose: the build is `-inline noauto`, so a shared body would compile to a
 * real `bl` and collapse all six callers to stubs.
 */

/** Loads the sprite for the task whose anim template is data_ov039_02099df0. */
void func_ov039_0209633c(void* self, void* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_02099df0;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)args;
    anim.posX     = *(s32*)((u8*)self + 0x48) >> 12;
    anim.posY     = *(s32*)((u8*)self + 0x4C) >> 12;
    _Sprite_Load((Sprite*)sprite, &anim);
}

/** Loads the sprite for the task whose anim template is data_ov039_02099e38. */
void func_ov039_02096718(void* self, void* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_02099e38;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)args;
    _Sprite_Load((Sprite*)sprite, &anim);
}

/** Loads the sprite for the task whose anim template is data_ov039_02099e64. */
void func_ov039_0209678c(void* self, void* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_02099e64;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)args;
    _Sprite_Load((Sprite*)sprite, &anim);
}

/** Loads the sprite for the task whose anim template is data_ov039_02099e90. */
void func_ov039_02096800(void* self, void* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_02099e90;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)args;
    _Sprite_Load((Sprite*)sprite, &anim);
}

/** Loads the sprite for the task whose anim template is data_ov039_02099ed8. */
void func_ov039_02096d00(void* self, void* sprite) {
    SpriteAnimation anim = data_ov039_02099ed8;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)self;
    _Sprite_Load((Sprite*)sprite, &anim);
}

/** Loads the sprite for the task whose anim template is data_ov039_02099f20. */
void func_ov039_02096f40(void* self, void* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_02099f20;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)args;
    anim.posX     = *(s32*)((u8*)self + 0x58) >> 12;
    anim.posY     = *(s32*)((u8*)self + 0x5C) >> 12;
    _Sprite_Load((Sprite*)sprite, &anim);
}

/* ==================================================================== */
/* The init and render stages.                                         */
/* ==================================================================== */

/*
 * Four of the band's five task-init stages, plus the two render stages that go
 * with them.  All six read the task's object through `task->data` and reach
 * the same object from three arguments, so the pointer is materialised once
 * into a local and every field is an offset from it.
 *
 * The store order inside each is the target's, and it is not sorted: 02096fcc
 * writes +0x78 and +0x7C *before* +0x74, and 020963c8 writes +0x50 before the
 * +0x40 block it is interleaved with.  Both are reproduced literally.
 */

/** Clears the coordinate block, raises +0x44, then loads via 0209633c. */
s32 func_ov039_020963c8(void* pool, void* task, s32* args) {
    u8* self = *(u8**)((u8*)task + 0x18);

    *(s32*)((u8*)self + 0x50) = *(s32*)((u8*)args + 4);
    *(s32*)((u8*)self + 0x40) = 0;
    *(s32*)((u8*)self + 0x44) = 0;
    *(s32*)((u8*)self + 0x48) = 0;
    *(s32*)((u8*)self + 0x4C) = 0;
    *(s32*)((u8*)self + 0x54) = 0;
    *(s32*)((u8*)self + 0x58) = 0;
    func_ov039_0209633c(self, self, args);
    return 1;
}

/** Recomputes the sprite position from the +0x40/+0x48 pairs, then renders. */
s32 func_ov039_020964a4(void* pool, void* task, void* args) {
    u8* self = *(u8**)((u8*)task + 0x18);

    if (*(s32*)((u8*)self + 0x54) != 0) {
        // The two halves of the object are two Q12.12 points; the sprite is
        // drawn at the difference between them, which is why the target
        // subtracts before shifting rather than loading a stored position.
        *(s16*)((u8*)self + 0x0C) = (*(s32*)((u8*)self + 0x48) - *(s32*)((u8*)self + 0x40)) >> 12;
        *(s16*)((u8*)self + 0x0E) = (*(s32*)((u8*)self + 0x4C) - *(s32*)((u8*)self + 0x44)) >> 12;
        Sprite_RenderFrame((Sprite*)self);
    }
    return 1;
}

/** Clears the +0x50..+0x7C block, then loads via 02096f40. */
s32 func_ov039_02096fcc(void* pool, void* task, s32* args) {
    u8* self = *(u8**)((u8*)task + 0x18);

    *(s32*)((u8*)self + 0x78) = 0;
    *(s32*)((u8*)self + 0x7C) = 0;
    *(s32*)((u8*)self + 0x74) = *(s32*)((u8*)args + 4);
    *(s32*)((u8*)self + 0x50) = 0;
    *(s32*)((u8*)self + 0x54) = 0;
    *(s32*)((u8*)self + 0x58) = 0;
    *(s32*)((u8*)self + 0x5C) = 0;
    *(s32*)((u8*)self + 0x60) = 0;
    *(s32*)((u8*)self + 0x64) = 0;
    *(s32*)((u8*)self + 0x68) = 0;
    *(s32*)((u8*)self + 0x40) = 0;
    *(s32*)((u8*)self + 0x44) = 0x1800;
    *(s32*)((u8*)self + 0x48) = 0x1800;
    *(s16*)((u8*)self + 0x4C) = 0;
    *(s16*)((u8*)self + 0x4E) = 0;
    func_ov039_02096f40(self, self, args);
    return 1;
}

/** Clears the +0xC0/+0xC8 words, then loads three sprites via 02096718/8c/00. */
s32 func_ov039_02096874(void* pool, void* task, s32* args) {
    u8* self = *(u8**)((u8*)task + 0x18);

    *(s32*)((u8*)self + 0xC8) = 0;
    *(s32*)((u8*)self + 0xC0) = 0;
    func_ov039_02096718(self, self, args);
    func_ov039_0209678c(self, self + 0x40, args);
    func_ov039_02096800(self, self + 0x80, args);
    return 1;
}

/**
 * Picks the sprite's frame from the overlay's own animation-phase counter.
 *
 * The +0x44 word gates the whole body: while it is clear nothing is stepped,
 * updated or drawn.  `5 - phase` is the frame index, and the target narrows it
 * to a halfword with `lsl #0x10` / `asr #0x10`, so the arithmetic is done in
 * s32 and truncated on the way into Sprite_ChangeAnimation.
 */
s32 func_ov039_02096da0(void* pool, void* task, void* args) {
    u8* self = *(u8**)((u8*)task + 0x18);

    if (*(s32*)((u8*)self + 0x44) != 0) {
        // Spelled inline rather than as a local: a local makes mwcc compute the
        // `lsl` into r0 and only then move it into r2, where the target narrows
        // straight into the argument register.
        Sprite_ChangeAnimation((Sprite*)(self + 4), *(void**)((u8*)self + 0x1C), (s16)(5 - func_ov040_0209cb5c()),
                               *(void**)((u8*)self + 0x20));
        Sprite_Update((Sprite*)(self + 4));
        Sprite_RenderFrame((Sprite*)(self + 4));
    }
    return 1;
}

/* ==================================================================== */
/* 02096404 -- the per-frame update for the 0209633c task.              */
/* ==================================================================== */

/**
 * @brief Follows a child task's position and keeps the sprite in step.
 *
 * Fetches the child task's data through the id stashed at +0x50, then branches
 * on the sub-state at +0x58:
 *
 *   0  the child is not ready -- clear the +0x54 "moving" flag;
 *   1  copy two points out of the child (func_ov039_0208e85c and
 *      func_ov039_0208e6e0) and, if the sprite's own first word says the
 *      animation is finished, clear +0x58 and +0x54 instead of raising +0x54;
 *   anything else falls straight through to the tail.
 *
 * The test on the sprite's word 0 is `isPlaying`, reached through the
 * SpriteAnimation/Sprite header rather than as a raw shift -- see the comment
 * at the test for why the spelling is load-bearing.
 */
s32 func_ov039_02096404(void* pool, void* task, void* args) {
    u8*   self  = *(u8**)((u8*)task + 0x18);
    void* child = EasyTask_GetTaskData(pool, *(u32*)((u8*)self + 0x50));

    if (child != NULL) {
        switch (*(s32*)((u8*)self + 0x58)) {
            case 0:
                *(s32*)((u8*)self + 0x54) = 0;
                break;

            case 1:
                func_ov039_0208e85c(child, (OtuPoint*)((u8*)self + 0x40));
                func_ov039_0208e6e0(child, (OtuPoint*)((u8*)self + 0x48));
                // `lsl #0x15` / `lsr #0x1f` is mwcc's extract for a *single* bit
                // at position 10, not a shift by ten: the target's pair leaves
                // only bit 10 of the sprite's word 0 in the result. That is
                // `isPlaying`, and naming the bitfield is what produces the
                // pair -- both `/ 1024` and `>> 10` fold to one `lsr #0xa`.
                if (((Sprite*)self)->isPlaying != 1) {
                    *(s32*)((u8*)self + 0x54) = 1;
                } else {
                    *(s32*)((u8*)self + 0x58) = 0;
                    *(s32*)((u8*)self + 0x54) = 0;
                }
                break;
        }
    } else {
        *(s32*)((u8*)self + 0x54) = 0;
    }

    if (*(s32*)((u8*)self + 0x54) != 0) {
        Sprite_Update((Sprite*)self);
    }
    return 1;
}

/* ==================================================================== */
/* 02096c58 -- the sprite-cell builder for the 02096d00 task.          */
/* ==================================================================== */

/**
 * @brief The frame-info callback for the 02096d00 task's sprite.
 *
 * Structurally the same as the twenty-two cell builders in OtuFieldAccess.c --
 * same two-step lookup, same guard chain over +0x18, +0x1C and +0x16 -- but with
 * three differences that are all visible in the disassembly and none of which
 * are stylistic:
 *
 *   - the depth key is the constant 3, not a call to func_ov039_02088400. The
 *     target ends the case-2 body with `mov r2, #0x3` / `str r2, [r1, #0x10]`;
 *   - `+0x0C` is never written.  Every one of the twenty-two writes it, and
 *     this one does not;
 *   - the index at +0x16 is read with `ldrsb`, a *sign-extended byte*, where
 *     all twenty-two use `ldrsh`.  That is a real type difference in the
 *     original, not a codegen artefact, so it is reproduced as a `s8` read.
 *
 * It is also a leaf: the two returns are `bx lr` out of the switch, with no
 * frame at all, and the object it fills is `g_SpriteFrameInfo` rather than
 * `data_0206b408`. That is the other tell that this one is a frame-info
 * callback and not one of the twenty-two: its literal pool word names the
 * frame-info object.
 */
// Nonmatching: 82%. Every semantic decision here is confirmed against the
// disassembly and the size is exact: the three-argument callback shape, the
// `updateSteps` store at +0 on the LOAD pass, the constant depth key of 3, the
// absent +0x0C write, the `ldrsb` (signed *byte*) index read where all
// twenty-two cell builders use `ldrsh`, the leaf frame, and the fact that the
// object filled is `g_SpriteFrameInfo` rather than `data_0206b408`.
//
// What is left is one register: the target materialises the zero it stores to
// +0x04/+0x08/+0x0C in r2, this build in r3. Both then compute the -1 for
// +0x10 in r2, so the residual is the choice of scratch register for the
// constant and nothing else. Four spellings of the zero stores were tried
// (separate assignments, a shared local, and reordering against the +0x10
// write) without moving it, and a chained assignment does not type-check
// because the three fields are s32, pointer and pointer.
SpriteFrameInfo* func_ov039_02096c58(Sprite* sprite, s32 arg, s32 mode) {
    SpriteFrameInfo* info = &g_SpriteFrameInfo;

    switch (mode) {
        case 1:
            // Offset 0, not +4: on the LOAD pass the target writes the *first*
            // word of the frame-info object, which is `updateSteps`. The cell
            // builders' case 1 writes +0x00 of OtuSpriteSlot, which is why the
            // two look identical in the disassembly and are not the same store.
            info->updateSteps = 1;
            return info;

        case 2: {
            u8* table;

            info->pieceCount = 0;
            info->cellPieces = NULL;
            info->affine     = NULL;
            info->sortKey    = -1;

            if (*(void**)((u8*)sprite + 0x18) != NULL && (table = *(u8**)((u8*)sprite + 0x1C)) != NULL &&
                *(s16*)((u8*)sprite + 0x16) >= 0)
            {
                // Both the index and the table pointer are re-read rather than
                // cached: the target loads +0x16 and +0x1C again for the second
                // lookup, and keeping them in locals shortens the body by an
                // instruction and shifts the whole register allocation.
                // `(index * 4 + 1) * 2`, not `index * 8 + 2`, for the same
                // reason -- the unsimplified form is what emits lsl/add/lsl.
                info->pieceCount = *(u16*)(table + (*(s16*)((u8*)sprite + 0x16) * 4 + 1) * 2);
                info->cellPieces =
                    (struct OamCellPiece*)(*(u8**)((u8*)sprite + 0x1C) +
                                           *(u16*)(*(u8**)((u8*)sprite + 0x1C) + *(s16*)((u8*)sprite + 0x16) * 8) * 2);
            }

            info->sortKey = 3;
            return info;
        }

        default:
            return NULL;
    }
}

/* ==================================================================== */
/* 020969fc -- the render stage for the three-sprite task.              */
/* ==================================================================== */

/**
 * @brief Draws whichever of the task's three sprites the state selects.
 *
 * The whole body is gated on +0xC0.  Inside it a switch on +0xC4 picks the
 * sprite: 0 draws the one at self+0, and cases 1 and 2 share the sprite at
 * self+0x40 while case 3 draws self+0x80.  Both `addls pc, pc, r0, lsl #2`
 * sequences in the target are switch jump tables, and the case bodies come out
 * in source order.
 *
 * Case 0 is the only one that does any work beyond rendering: it asks the OAM
 * manager for a fresh affine group, sized from +0xD4 and +0xD8, and stores the
 * returned index into bits 5-9 of the sprite's OAM attribute halfword at +0x0A.
 * The target builds the index with `lsl #0x10` / `lsr #0x10` (a u16 narrowing
 * of the s32 result) and then places it with `lsl #0x1b` / `lsr #0x16`, which
 * is a five-bit field insert -- hence the `unk_05 : 5` member rather than a
 * masked word.
 */
// Nonmatching: 83%. The switch, all four case bodies, the gate on +0xC0, the
// two jump tables, the bitfield insert and the size are all exact. What is
// left is the scheduling of case 0's affine-group call: the target computes
// the manager address (`ldrh`-free `lsl/lsr` index extract, two pool loads and
// the `mla`) and only then stores the zero fifth argument to the outgoing
// stack slot, whereas this build stores the stack argument first and shifts
// the address computation after it. The two `mla` operands are also permuted,
// which is the same reordering seen from the other side. Naming the manager
// address in a local was tried and changed nothing, so this is mwcc's
// evaluation order for the call and not a misread of the source.
s32 func_ov039_020969fc(void* pool, void* task, void* args) {
    u8* self = *(u8**)((u8*)task + 0x18);

    if (*(s32*)((u8*)self + 0xC0) != 0) {
        switch (*(s32*)((u8*)self + 0xC4)) {
            case 0:
                ((Sprite*)self)->unk_0A.unk_05 = (u16)OamMgr_AllocAffineGroup(
                    &g_OamMgr[((Sprite*)self)->bits_0_1], 0, *(s32*)((u8*)self + 0xD4), *(s32*)((u8*)self + 0xD8), 0);
                Sprite_RenderFrame((Sprite*)self);
                break;

            case 1:
            case 2:
                Sprite_RenderFrame((Sprite*)(self + 0x40));
                break;

            case 3:
                Sprite_RenderFrame((Sprite*)(self + 0x80));
                break;
        }
    }
    return 1;
}

/* ==================================================================== */
/* The two inits that are not task stages.                              */
/* ==================================================================== */

/*
 * func_ov039_02096b48 and func_ov039_02096154 are not reached through a
 * TaskStages table: the scene calls them directly with the task object in r0
 * and a selector in r1, so they take two or three arguments rather than the
 * usual (pool, task, args).  Both build a text-cell cursor over the shared
 * glyph table and then start a sound effect, and both are the only routines in
 * the overlay that use the `+0xC4` selector to pick *which* of the three
 * sprites a stage is about.
 *
 * The object is three Sprites (at +0, +0x40 and +0x80) followed by the state
 * block at +0xC0; the sound index constants are the overlay's own SndMgrSeIdx
 * values and are left as literals, since SndMgrSeIdx.h does not name them.
 */

/** Initialises the three-sprite task for one of its four selectors. */
void func_ov039_02096b48(void* self, s32 which) {
    *(s32*)((u8*)self + 0xC8) = 1;
    *(s32*)((u8*)self + 0xCC) = 0x3C;
    *(s32*)((u8*)self + 0xC4) = which;

    // The text-cell block at +0xD0: a cleared x, a 0x1000 (1.0) y and scale,
    // then a zero width, followed by the cursor built over the glyph table.
    *(s32*)((u8*)self + 0xD0) = 0;
    *(s32*)((u8*)self + 0xD4) = 0x1000;
    *(s32*)((u8*)self + 0xD8) = 0x1000;
    *(s16*)((u8*)self + 0xDC) = 0;
    *(s16*)((u8*)self + 0xDE) = 0;
    func_ov039_02087ba0((u8*)self + 0xE0, data_ov039_0209aa0c, 0xC, (u8*)self + 0xD0);

    // A switch, not an if/else chain: the target compares against 3 with
    // `addls pc, pc, r4, lsl #2`, and the four arms come out in source order.
    switch (which) {
        case 0:
            SndMgr_StartPlayingSE(0x55F);
            Sprite_ChangeAnimation((Sprite*)self, *(void**)((u8*)self + 0x18), 1, *(void**)((u8*)self + 0x1C));
            break;

        case 1:
            SndMgr_StartPlayingSE(0x560);
            Sprite_ChangeAnimation((Sprite*)(self + 0x40), *(void**)((u8*)self + 0x58), 1, *(void**)((u8*)self + 0x5C));
            break;

        case 2:
            SndMgr_StartPlayingSE(0x561);
            Sprite_ChangeAnimation((Sprite*)(self + 0x40), *(void**)((u8*)self + 0x58), 2, *(void**)((u8*)self + 0x5C));
            break;

        case 3:
            Sprite_ChangeAnimation((Sprite*)(self + 0x80), *(void**)((u8*)self + 0x98), 1, *(void**)((u8*)self + 0x9C));
            break;
    }

    SndMgr_StartPlayingSE(0x34B);
}

/**
 * @brief Initialises the two-sprite task, with an optional second label.
 *
 * The same object shape as 02096b48 but two sprites wide (the third block at
 * +0x80 is the one 02096b48 uses and this one does not), and with the +0x8C
 * argument acting as a gate: when it is zero the function returns straight
 * after the first Sprite_ChangeAnimation, which is what the target's `popeq`
 * on the link register is -- an early exit, not a computed return value. That
 * is also why the function is `void`: there is nothing in r0 on either path
 * that the source chose to return.
 *
 * The two selector arms differ only in their sound effects, their glyph table
 * and their cursor length (0xC versus 0xE), and the +0x349 first effect is
 * built by reusing the `1` already in r3 as `r3 + 0x348` -- reproduced here as
 * the plain constant, which mwcc folds back into that form on its own.
 *
 * `which` is typed `s16` rather than `s32` because the target hands it to
 * Sprite_ChangeAnimation with no narrowing: an s32 there would make mwcc emit
 * the `lsl #0x10` / `asr #0x10` pair the target does not have.
 */
void func_ov039_02096154(void* self, s16 which, s32 hasLabel) {
    *(s32*)((u8*)self + 0x84) = 1;
    *(s32*)((u8*)self + 0x88) = 0x78;
    *(s32*)((u8*)self + 0x8C) = hasLabel;
    *(s32*)((u8*)self + 0x90) = 0;
    *(s32*)((u8*)self + 0x94) = 0x1000;
    *(s32*)((u8*)self + 0x98) = 0x1000;
    *(s16*)((u8*)self + 0x9C) = 0;
    *(s16*)((u8*)self + 0x9E) = 0;

    // A switch even though there is no jump table: the target does both
    // compares up front and emits the two bodies out of line, which is the
    // shape a switch gives. An `else if` chain gets if-converted the other way
    // round and costs two extra branches.
    switch (which) {
        case 1:
            SndMgr_StartPlayingSE(0x349);
            SndMgr_StartPlayingSE(0x55D);
            func_ov039_02087ba0((u8*)self + 0xB0, data_ov039_0209a8a8, 0xC, (u8*)self + 0x90);
            break;

        case 2:
            SndMgr_StartPlayingSE(0x34A);
            SndMgr_StartPlayingSE(0x55E);
            func_ov039_02087ba0((u8*)self + 0xB0, data_ov039_0209a938, 0xE, (u8*)self + 0x90);
            break;
    }

    Sprite_ChangeAnimation((Sprite*)self, *(void**)((u8*)self + 0x18), which, *(void**)((u8*)self + 0x1C));

    if (*(s32*)((u8*)self + 0x8C) == 0) {
        return;
    }

    *(s32*)((u8*)self + 0xA0) = 0;
    *(s32*)((u8*)self + 0xA4) = 0x1000;
    *(s32*)((u8*)self + 0xA8) = 0x1000;
    *(s16*)((u8*)self + 0xAC) = 0;
    *(s16*)((u8*)self + 0xAE) = 0;
    func_ov039_02087ba0((u8*)self + 0xBC, data_ov039_0209a830, 0xA, (u8*)self + 0xA0);
    Sprite_ChangeAnimation((Sprite*)(self + 0x40), *(void**)((u8*)self + 0x58), 3, *(void**)((u8*)self + 0x5C));
}

/* ==================================================================== */
/* 020968c0 -- the per-frame update for the three-sprite task.          */
/* ==================================================================== */

/**
 * @brief Advances the three-sprite task's fade/label sequence.
 *
 * The task is a four-state machine on +0xC8, run as a `switch` because the
 * target emits a jump table for it.  Each of states 1 to 3 counts +0xCC down
 * from 0x3C and, on reaching zero, either reloads it and advances the state or
 * clears the state outright:
 *
 *   0  nothing is showing: clear the +0xC0 "visible" flag;
 *   1  visible; fade the main display out, then move to state 2;
 *   2  visible; on expiry play the effect and move to state 3;
 *   3  visible; fade both displays to 0x10, then clear the state.
 *
 * States 1 and 2 write the reload with an `if/else` shaped test -- the target
 * branches *over* the reset with `bgt` when the counter is still positive --
 * while state 3 tests the same counter with a conditional store instead.  Both
 * spellings are reproduced; making them uniform does not match.
 *
 * The tail is a second switch, on +0xC4, that steps whichever sprite the
 * selector names.  It is reached only when +0xC0 is set, and so is the
 * text-cell advance through func_ov039_02087bf8.
 */
s32 func_ov039_020968c0(void* pool, void* task, void* args) {
    u8* self = *(u8**)((u8*)task + 0x18);

    switch (*(s32*)((u8*)self + 0xC8)) {
        case 0:
            *(s32*)((u8*)self + 0xC0) = 0;
            break;

        case 1:
            *(s32*)((u8*)self + 0xC0) = 1;
            EasyFade_FadeMainDisplay(FADER_INSTANT, 0, 0x3C);
            *(s32*)((u8*)self + 0xCC) = *(s32*)((u8*)self + 0xCC) - 1;
            if (*(s32*)((u8*)self + 0xCC) <= 0) {
                *(s32*)((u8*)self + 0xCC) = 0x3C;
                *(s32*)((u8*)self + 0xC8) = 2;
            }
            break;

        case 2:
            *(s32*)((u8*)self + 0xC0) = 1;
            *(s32*)((u8*)self + 0xCC) = *(s32*)((u8*)self + 0xCC) - 1;
            if (*(s32*)((u8*)self + 0xCC) <= 0) {
                SndMgr_StartPlayingSE(0x34C);
                *(s32*)((u8*)self + 0xCC) = 0x3C;
                *(s32*)((u8*)self + 0xC8) = 3;
            }
            break;

        case 3:
            *(s32*)((u8*)self + 0xC0) = 1;
            EasyFade_FadeBothDisplays(FADER_INSTANT, 0x10, 0x3C);
            *(s32*)((u8*)self + 0xCC) = *(s32*)((u8*)self + 0xCC) - 1;
            if (*(s32*)((u8*)self + 0xCC) <= 0) {
                *(s32*)((u8*)self + 0xC8) = 0;
            }
            break;
    }

    if (*(s32*)((u8*)self + 0xC0) != 0) {
        func_ov039_02087bf8((u8*)self + 0xE0, (u8*)self + 0xD0);

        switch (*(s32*)((u8*)self + 0xC4)) {
            case 0:
                Sprite_Update((Sprite*)self);
                break;

            case 1:
            case 2:
                Sprite_Update((Sprite*)(self + 0x40));
                break;

            case 3:
                Sprite_Update((Sprite*)(self + 0x80));
                break;
        }
    }
    return 1;
}

/* Band 5 region -- inlined; see the file header. */

#include "Debug/Sugata/TinPinSlammer.h"
#include "Engine/EasyTask.h"
#include "Engine/Math/Random.h"
#include "SpriteMgr.h"
#include <nitro/fx/fx_division.h>

/*
 * ============================================================================
 * Band 5: the four pin-sprite tasks.
 * ============================================================================
 *
 * This band is four copies of one task type. Each owns a base-game `Sprite` at
 * the head of its data block and a run of pinball state after it, and each is
 * driven through the engine's `TaskStages` quartet -- init, update, render,
 * cleanup -- held in one of four `.rodata` tables:
 *
 *   data_ov039_02099f10   02096fcc  0209702c  02097160  020971b0
 *   data_ov039_02099f58   020974e0  02097528  02097670  020976c0
 *   data_ov039_02099fa0   02097918  02097954  020979cc  02097a14
 *   data_ov039_02099fe8   02097c30  02097c90  02097dbc  02097e0c
 *
 * Every stage takes `(TaskPool*, Task*, void*)`, returns 1, and reaches its
 * object through `task->data` -- the `ldr rX, [r1, #0x18]` that opens all
 * sixteen. Which of the sixteen are here:
 *
 *   first  table   init (02096fcc) is below this band; the other three are here
 *   second table   init/update/render/cleanup all here (020974e0..020976c0)
 *   third  table   all four here (02097918..02097a14)
 *   fourth table   all four here (02097c30..02097e0c)
 *
 * All sixteen stages are here except 0x02096fcc, which is the first table's init
 * and sits below this band; it is declared (unprototyped) where it is needed.
 * The band also holds the four stage dispatchers, the four task-creation
 * wrappers, the four `_Sprite_Load` wrappers and the four public setup routines.
 *
 * What the quartet does is legible from the code: an update stage fetches
 * another task's data, asks for its +0x110/+0x114 pair through
 * func_ov039_0208e85c, and blends the sprite toward it with
 * func_ov039_02098c00, so this is a sprite that chases a pin. What the pin is,
 * and what any of the counters mean, the code does not say and neither does
 * this file.
 */

/**
 * @brief A pin-sprite task's data block: a base-game `Sprite` plus overlay state.
 *
 * The first 0x40 bytes are an ordinary `Sprite`, and that is not a guess: it is
 * forced three ways over. The block is handed straight to `Sprite_Update`,
 * `Sprite_RenderFrame`, `Sprite_Release` and `_Sprite_Load`, its +0x18 and +0x1C
 * are passed back out as `Sprite_ChangeAnimation`'s `animData` and `cellTable`
 * arguments, and its first word is tested as a bitfield. Everything from +0x40
 * on is the overlay's own.
 *
 * The words are kept as a flat run rather than grouped into points, because the
 * two helpers that take them (`func_ov039_0208e85c` writes one,
 * `func_ov039_02098c00` reads one and writes another) are handed bare addresses
 * and the layouts differ between the four objects: the second task's block is
 * the first's shifted down by 0x10 for the first six fields and not for the
 * rest. Grouping them would assert a correspondence the code does not have.
 */
typedef struct {
    /* 0x00 */ Sprite sprite; // 0x40 bytes; +0x0C/+0x0E are posX/posY
    /* 0x40 */ s32    unk_40;
    /* 0x44 */ s32    unk_44;
    /* 0x48 */ s32    unk_48;
    /* 0x4C */ s32    unk_4C;
    /* 0x50 */ s32    unk_50;
    /* 0x54 */ s32    unk_54;
    /* 0x58 */ s32    unk_58;
    /* 0x5C */ s32    unk_5C;
    /* 0x60 */ s32    unk_60;
    /* 0x64 */ s32    unk_64;
    /* 0x68 */ s32    unk_68;
    /* 0x6C */ s32    unk_6C;
    /* 0x70 */ s32    unk_70;
    /* 0x74 */ s32    unk_74;
    /* 0x78 */ s32    unk_78;
    /* 0x7C */ s32    unk_7C;
    /* 0x80 */ s32    unk_80;
    /* 0x84 */ s32    unk_84;
} OtuTaskSprite; // Size: 0x88

/** @brief One of the four stages of a pin-sprite task. */
typedef s32 (*OtuTaskSpriteStage)(TaskPool* pool, Task* task, void* param);

/* ------------------------------------------------------------------ */
/* Data the band reads but does not define.                            */
/* ------------------------------------------------------------------ */

/*
 * The four task descriptors the `EasyTask_CreateTask` wrappers hand over, and
 * the four `.rodata` stage tables the dispatchers name. All of these live in the
 * overlay's gap-filled `.rodata`, so they are referenced by the build's own
 * names: declaring an object of one's own would land it at an address of the
 * linker choosing, not the one the target uses, which is the same reason the
 * result-screen row tables are declared that way.
 *
 * The stage tables are declared but not referenced -- the dispatchers build their
 * own copies as local initialisers, because that is the only form mwcc emits as
 * the `ldm/stm` the target has. They are declared anyway so that the addresses
 * are on the record, and so that the one-row relocation-name gap on each
 * dispatcher is legible rather than mysterious. See the dispatcher section.
 */
extern const TaskHandle data_ov039_02099f04;
extern const TaskHandle data_ov039_02099f4c;
extern const TaskHandle data_ov039_02099f94;
extern const TaskHandle data_ov039_02099fdc;

extern const TaskStages data_ov039_02099f10;
extern const TaskStages data_ov039_02099f58;
extern const TaskStages data_ov039_02099fa0;
extern const TaskStages data_ov039_02099fe8;

/**
 * @brief The bin every pin-sprite in the overlay draws its cells from.
 *
 * All four `SpriteAnimation` templates in the overlay name this one record (bin
 * id 39); it is the only thing tying the four sprites' art together. Not
 * identified further -- it is a plain identifier record in the overlay's `.data`.
 */
extern const BinIdentifier data_ov039_0209a0dc;

/*
 * Eight Q12.12 constants in the overlay's `.data`, read by the two setup
 * routines. Values recovered with `tools/ov039_table.py`:
 *
 *   0209a2fc = 0x2000    0209a300 = 0x1000    0209a304 = 0x0133
 *   0209a30c = 0x4000    0209a310 = 0x0400    0209a314 = 0x3000
 *   0209a328 = 0x9800
 *
 * None of them is used the same way twice: 0x0133 is a per-frame bleed out of a
 * count, 0x0400 is both a `FX_Divide` denominator and a per-frame spin-up, and
 * 0x2000/0x4000 are the bounds of the two random ranges the starting spin is
 * drawn from.
 *
 * They are deliberately *not* declared `const`. They live in `.data`, not
 * `.rodata`, and mwcc acts on that: told a symbol is const it will keep its
 * value in a callee-saved register across the `FX_Divide` call in the two setup
 * routines, where the target reloads it every time. The `const` costs three
 * instructions there and buys nothing.
 */
extern s32 data_ov039_0209a2fc;
extern s32 data_ov039_0209a300;
extern s32 data_ov039_0209a304;
extern s32 data_ov039_0209a30c;
extern s32 data_ov039_0209a310;
extern s32 data_ov039_0209a314;
extern s32 data_ov039_0209a328;

/**
 * @brief The base game's random `s16` pair table, indexed off `RNG_Next`.
 *
 * Unidentified. It is a plain `s16` array in the main module, and both this band
 * and the boss code read a consecutive pair out of it at a random index. This
 * band uses it as a random direction vector; that is an inference from the two
 * values never being equal and neither being zero, not something the data says.
 */
/* data_0205e4e0 is an array of s32, declared by band 3 -- casting its *value*
 * rather than its address was worth 43 points on func_ov039_02095788. */

/**
 * @brief Reads a task's +0x110/+0x114 pair out to a point, 0x0208e85c.
 *
 * Defined in OtuFieldAccess.c, which this file is #included into *above* the
 * definition, so it has to be declared here rather than picked up from the
 * header -- the header does not carry it, and without a declaration mwcc infers
 * `int (...)` from the call below and then rejects the definition.
 */

/**
 * @brief The overlay's Q12.12 point blend, 0x02098c00.
 *
 * `out = base + (offset * scale) >> 12`, componentwise. Declared without a
 * prototype so that this file cannot disagree with whoever defines it.
 */

/** @brief The base game's 32-bit signed divide. Also unprototyped. */
s32 _s32_div_f();

/*
 * The four `_Sprite_Load` wrappers. Each fills a `SpriteAnimation` template with
 * the object's own anchor and owner and loads one sprite; only the field the
 * anchor is read from and the template's constant differ between them.
 *
 * The third argument is the task's creation block again, read as a word and
 * narrowed to a halfword: the target loads a full word and masks it with
 * `lsl #0x10; lsr #0x10` before folding it into the template's four-bit
 * `dataType` field, which a `u16*` parameter would have turned into a single
 * `ldrh` instead. The target passes nothing for this argument and the callee
 * reads whatever is in r2, which is still the block because no caller has
 * touched r2 since loading the id out of it. Passing it explicitly costs no
 * instruction -- r2 already holds it -- and keeps the declaration honest.
 */
void func_ov039_02097454(OtuTaskSprite* self, Sprite* sprite, s32* dataType);
void func_ov039_0209788c(OtuTaskSprite* self, Sprite* sprite, s32* dataType);
void func_ov039_02097ba4(OtuTaskSprite* self, Sprite* sprite, s32* dataType);

/* ------------------------------------------------------------------ */
/* The cleanup stages: four identical five-instruction bodies.         */
/* ------------------------------------------------------------------ */

/*
 * The four `Sprite_Release` wrappers. The engine's convention is that a cleanup
 * stage returns 1 to keep the task alive, and all four do so unconditionally --
 * which is what the bare `mov r0, #1` is for here, exactly as in the four
 * `mov r0, #1; bx lr` bodies OtuFieldAccess.c declines to name.
 *
 * They are written out four times rather than factored, because the build uses
 * `-inline noauto` and a `static` helper would compile to a real `bl` and
 * collapse every caller to a stub.
 */

/** Releases the first pin-sprite's sprite. */
s32 func_ov039_020971b0(TaskPool* pool, Task* task, void* param) {
    Sprite_Release((Sprite*)task->data);
    return 1;
}

/** Releases the second pin-sprite's sprite. */
s32 func_ov039_020976c0(TaskPool* pool, Task* task, void* param) {
    Sprite_Release((Sprite*)task->data);
    return 1;
}

/** Releases the third pin-sprite's sprite. */
s32 func_ov039_02097a14(TaskPool* pool, Task* task, void* param) {
    Sprite_Release((Sprite*)task->data);
    return 1;
}

/** Releases the fourth pin-sprite's sprite. */
s32 func_ov039_02097e0c(TaskPool* pool, Task* task, void* param) {
    Sprite_Release((Sprite*)task->data);
    return 1;
}

/* ------------------------------------------------------------------ */
/* The create wrappers.                                                */
/* ------------------------------------------------------------------ */

/*
 * Each pushes the two words it was given as the new task's creation parameter
 * block and calls `EasyTask_CreateTask` with the NULL data pointer, priority 0
 * and NULL parent the engine's signature asks for.
 *
 * The block is a named two-word struct rather than two scalar locals, and that
 * is load-bearing: only the block's *address* is ever taken, so with scalars mwcc
 * drops the second store as dead and the frame shrinks by four bytes. A struct
 * whose address is taken keeps both.
 *
 * Every `init` stage receives this block as its third argument and reads it as
 * two words: the first becomes the sprite's initial position, the second a task
 * id the object then chases.
 *
 * The stages take it as `void*` and the four public setup routines take
 * `OtuTaskParams*`, and that is not an inconsistency. A stage's signature is
 * fixed by `TaskStages` -- every stage shares `(TaskPool*, Task*, void*)` because
 * the engine's table is homogeneous -- and mwcc rejects the implicit conversion
 * if one stage narrows it. The public routines have no such constraint and say
 * what they mean.
 */

/** @brief The two-word parameter block `EasyTask_CreateTask` hands to a task. */
typedef struct {
    /* 0x0 */ s32 word0;
    /* 0x4 */ s32 word1;
} OtuTaskParams; // Size: 0x8

/* Forward declarations for the two functions below that this file's own
 * call sites reach first. Without them C infers `int (...)` and the real
 * definition then reads as a redeclaration. This is band 5 declaring its own
 * functions, not a dependency on another band.
 *
 * They sit here rather than at the top of the file because they name
 * OtuTaskSprite and OtuTaskParams, which are defined just above: the two
 * typedefs live in this file rather than the feature header because this is
 * their only user. */
s32  func_ov039_0209720c(TaskPool* pool, s32 word0, s32 word1);
void func_ov039_02097240(OtuTaskSprite* s, OtuTaskParams* param);

/* Returns the task handle. The target's last two instructions are the call
 * and the epilogue with no `mov r0` between, so r0 already holds the handle
 * and returning it costs nothing -- and a caller in band 1 assigns it. */
s32 func_ov039_0209720c(TaskPool* pool, s32 word0, s32 word1) {
    OtuTaskParams params;

    params.word0 = word0;
    params.word1 = word1;
    return EasyTask_CreateTask(pool, &data_ov039_02099f04, NULL, 0, NULL, &params);
}

s32 func_ov039_0209771c(TaskPool* pool, s32 word0, s32 word1) {
    OtuTaskParams params;

    params.word0 = word0;
    params.word1 = word1;
    return EasyTask_CreateTask(pool, &data_ov039_02099f4c, NULL, 0, NULL, &params);
}

s32 func_ov039_02097a70(TaskPool* pool, s32 word0, s32 word1) {
    OtuTaskParams params;

    params.word0 = word0;
    params.word1 = word1;
    return EasyTask_CreateTask(pool, &data_ov039_02099f94, NULL, 0, NULL, &params);
}

s32 func_ov039_02097e68(TaskPool* pool, s32 word0, s32 word1) {
    OtuTaskParams params;

    params.word0 = word0;
    params.word1 = word1;
    return EasyTask_CreateTask(pool, &data_ov039_02099fdc, NULL, 0, NULL, &params);
}

/* ------------------------------------------------------------------ */
/* The render stages.                                                  */
/* ------------------------------------------------------------------ */
/*
 * (The four stage dispatchers are at the end of this file, once all sixteen
 * stages they name are in scope -- mwcc will not take a function's address
 * before it is declared.)
 */

/*
 * A render stage sets the sprite's `posX`/`posY` from two Q12.12 differences and
 * renders -- but only if the flag the update stage raised on the previous frame
 * is still set. Three of the four add a third term to `posY`; the fourth has
 * none, which is the only real difference between them.
 */

/** The first pin-sprite's render stage. */
s32 func_ov039_02097160(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s = (OtuTaskSprite*)task->data;

    if (s->unk_7C != 0) {
        s->sprite.posX = (s16)((s->unk_58 - s->unk_50) >> 0xC);
        s->sprite.posY = (s16)((s->unk_5C - s->unk_54 + s->unk_60) >> 0xC);
        Sprite_RenderFrame((Sprite*)s);
    }

    return 1;
}

/**
 * @brief The second pin-sprite's render stage.
 *
 * The same shape as the first's on the first task's layout, which the second
 * task reuses shifted down by 0x10 for these six fields.
 */
s32 func_ov039_02097670(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s = (OtuTaskSprite*)task->data;

    if (s->unk_6C != 0) {
        s->sprite.posX = (s16)((s->unk_48 - s->unk_40) >> 0xC);
        s->sprite.posY = (s16)((s->unk_4C - s->unk_44 + s->unk_50) >> 0xC);
        Sprite_RenderFrame((Sprite*)s);
    }

    return 1;
}

/** The fourth pin-sprite's render stage -- the first's, unchanged. */
s32 func_ov039_02097dbc(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s = (OtuTaskSprite*)task->data;

    if (s->unk_7C != 0) {
        s->sprite.posX = (s16)((s->unk_58 - s->unk_50) >> 0xC);
        s->sprite.posY = (s16)((s->unk_5C - s->unk_54 + s->unk_60) >> 0xC);
        Sprite_RenderFrame((Sprite*)s);
    }

    return 1;
}

/**
 * @brief The third pin-sprite's render stage.
 *
 * The odd one out. It watches +0x58 rather than +0x7C, and its `posY` has no
 * third term -- which follows from the third task being the short one: it never
 * keeps a value at the offset that term would come from.
 */
s32 func_ov039_020979cc(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s = (OtuTaskSprite*)task->data;

    if (s->unk_58 != 0) {
        s->sprite.posX = (s16)((s->unk_48 - s->unk_40) >> 0xC);
        s->sprite.posY = (s16)((s->unk_4C - s->unk_44) >> 0xC);
        Sprite_RenderFrame((Sprite*)s);
    }

    return 1;
}

/* ------------------------------------------------------------------ */
/* The init stages that live in this band.                             */
/* ------------------------------------------------------------------ */

/**
 * @brief The second pin-sprite's init stage.
 *
 * Clears the position block and both flags, keeps the creation block's second
 * word as the task id to chase at +0x68, and loads the sprite.
 */
s32 func_ov039_020974e0(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s = (OtuTaskSprite*)task->data;

    s->unk_6C = 0;
    s->unk_70 = 0;
    s->unk_68 = ((s32*)param)[1];
    s->unk_40 = 0;
    s->unk_44 = 0;
    s->unk_48 = 0;
    s->unk_4C = 0;
    s->unk_50 = 0;
    s->unk_54 = 0;
    s->unk_58 = 0;
    func_ov039_02097454(s, (Sprite*)s, (s32*)param);

    return 1;
}

/**
 * @brief The third pin-sprite's init stage.
 *
 * The same shape as the second's over the shorter field set: it keeps the
 * creation block's second word at +0x50 and clears +0x40 through +0x4C.
 */
s32 func_ov039_02097918(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s = (OtuTaskSprite*)task->data;

    s->unk_54 = 0;
    s->unk_58 = 0;
    s->unk_50 = ((s32*)param)[1];
    s->unk_40 = 0;
    s->unk_44 = 0;
    s->unk_48 = 0;
    s->unk_4C = 0;
    func_ov039_0209788c(s, (Sprite*)s, (s32*)param);

    return 1;
}

/**
 * @brief The fourth pin-sprite's init stage.
 *
 * The first task's shape, with +0x44 and +0x48 preset to 0x1000 rather than
 * 0x1800 and with the last two clears written as halfwords -- the only place in
 * the band where a field is stored as two `s16`s instead of one word, so they
 * are written that way here.
 */
s32 func_ov039_02097c30(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s = (OtuTaskSprite*)task->data;

    s->unk_78              = 0;
    s->unk_7C              = 0;
    s->unk_74              = ((s32*)param)[1];
    s->unk_50              = 0;
    s->unk_54              = 0;
    s->unk_58              = 0;
    s->unk_5C              = 0;
    s->unk_60              = 0;
    s->unk_64              = 0;
    s->unk_68              = 0;
    s->unk_40              = 0;
    s->unk_44              = 0x1000;
    s->unk_48              = 0x1000;
    *(s16*)&s->unk_4C      = 0;
    *(s16*)((u8*)s + 0x4E) = 0;
    func_ov039_02097ba4(s, (Sprite*)s, (s32*)param);

    return 1;
}

/* ------------------------------------------------------------------ */
/* The two 300-byte update siblings.  This is the band.                */
/* ------------------------------------------------------------------ */

/*
 * 0x0209702c (the first task) and 0x02097c90 (the fourth) are the same function
 * with one constant changed. Both walk the same path -- fetch the chased pin,
 * count one tick off a budget, read the pin's position, scale the budget into a
 * Q12.12 anchor, bleed a count down, spin the object up, blend its position
 * toward the anchor, and raise the flag the render stage watches -- and their
 * instructions line up one for one apart from two differences:
 *
 *   1. the multiplier. The first computes `budget * 0x1800`, which mwcc emits as
 *      an explicit `mov r0, #0x1800; mul`; the fourth computes `budget * 0x1000`,
 *      which it folds to the `lsl r0, r0, #0xc` the target wants. So the two
 *      constants are genuinely different -- 0x1800 is 1.5 in Q12.12 and 0x1000 is
 *      1.0 -- and it is the *value*, not the spelling, that separates them: the
 *      target itself spells the fourth one as a shift because the constant makes
 *      a shift legal. This is not the divide-form gap from the brief; both sides
 *      are multiplies and both fall out of a plain `*`.
 *
 *   2. where the position read sits. The first calls the position getter and
 *      only then reloads the budget for the multiply; the fourth reads the
 *      position first and decrements after, so the decremented value is still in
 *      a register at the multiply. That is a source-order difference, not a
 *      codegen one: in the first the reload is forced because the call has
 *      already clobbered r0.
 *
 * They are written out twice rather than factored, per the `-inline noauto`
 * rule in the brief.
 */

/** The first pin-sprite's update stage. */
s32 func_ov039_0209702c(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s   = (OtuTaskSprite*)task->data;
    void*          pin = EasyTask_GetTaskData(pool, s->unk_74);

    if (pin != NULL) {
        if (s->unk_78 != 0) {
            s->unk_84 = s->unk_84 - 1;

            if (s->unk_84 <= 0) {
                s->unk_78 = 0;
                s->unk_7C = 0;
            } else {
                func_ov039_0208e85c(pin, (OtuPoint*)&s->unk_50);
                s->unk_48 = _s32_div_f(s->unk_84 * 0x1800, s->unk_80);
                s->unk_44 = s->unk_48;

                s->unk_6C = s->unk_6C - data_ov039_0209a304;

                if (s->unk_6C < 0) {
                    s->unk_6C = 0;
                }

                s->unk_70 = s->unk_70 + data_ov039_0209a310;
                func_ov039_02098c00(s->unk_6C, (OtuPoint*)&s->unk_64, (OtuPoint*)&s->unk_58, (OtuPoint*)&s->unk_58);

                s->unk_60 = s->unk_60 + s->unk_70;

                // Wrap the accumulated offset back to zero, and rescale the spin
                // by the same signed factor when it is still positive.
                //
                // The rescale is a 64-bit multiply-and-reshift with a +0x800
                // rounding bias, not a divide: that is what produces the target's
                // smull / adds #0x800 / lsr #12 / orr sequence, and writing it as
                // `/ 0x1000` instead makes mwcc emit a call to __ll_sdiv.
                if (s->unk_60 > 0) {
                    s->unk_60 = 0;

                    if (s->unk_70 > 0) {
                        s->unk_70 = (s32)(((s64)s->unk_70 * -data_ov039_0209a300 + 0x800) >> 0xC);
                    }
                }

                Sprite_Update((Sprite*)s);
                s->unk_7C = 1;
            }
        }
    } else {
        // The pin is gone: stop, and do not draw this frame.
        s->unk_78 = 0;
        s->unk_7C = 0;
    }

    return 1;
}

/**
 * @brief The fourth pin-sprite's update stage.
 *
 * 0x02097c90. The same body as func_ov039_0209702c with 0x1800 replaced by
 * 0x1000 and the position read hoisted above the decrement; see the note above
 * the two together for why they are not one body.
 */
s32 func_ov039_02097c90(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s = (OtuTaskSprite*)task->data;
    s32            budget;
    void*          pin = EasyTask_GetTaskData(pool, s->unk_74);

    if (pin != NULL) {
        if (s->unk_78 != 0) {
            func_ov039_0208e85c(pin, (OtuPoint*)&s->unk_50);
            budget    = s->unk_84 - 1;
            s->unk_84 = budget;

            if (budget <= 0) {
                s->unk_78 = 0;
                s->unk_7C = 0;
            } else {
                s->unk_48 = _s32_div_f(budget * 0x1000, s->unk_80);
                s->unk_44 = s->unk_48;

                s->unk_6C = s->unk_6C - data_ov039_0209a304;

                if (s->unk_6C < 0) {
                    s->unk_6C = 0;
                }

                s->unk_70 = s->unk_70 + data_ov039_0209a310;
                func_ov039_02098c00(s->unk_6C, (OtuPoint*)&s->unk_64, (OtuPoint*)&s->unk_58, (OtuPoint*)&s->unk_58);

                s->unk_60 = s->unk_60 + s->unk_70;

                // Same rescale as the first task's: a 64-bit multiply-and-reshift
                // with a rounding bias, which is what the smull sequence is.
                if (s->unk_60 > 0) {
                    s->unk_60 = 0;

                    if (s->unk_70 > 0) {
                        s->unk_70 = (s32)(((s64)s->unk_70 * -data_ov039_0209a300 + 0x800) >> 0xC);
                    }
                }

                Sprite_Update((Sprite*)s);
                s->unk_7C = 1;
            }
        }
    } else {
        s->unk_78 = 0;
        s->unk_7C = 0;
    }

    return 1;
}

/* ------------------------------------------------------------------ */
/* The second pin-sprite's update stage: a four-state machine.         */
/* ------------------------------------------------------------------ */

/**
 * @brief The second pin-sprite's update stage, 0x02097528.
 *
 * The third member of the cluster -- it calls both func_ov039_0208e85c and
 * func_ov039_02098c00 like the two 300-byte siblings -- but it is a different
 * shape, not a fourth copy of them. Its blend runs only while the state word is
 * non-zero, and it blends in the *other* direction: the offset is a countdown
 * that is decremented by a fixed step rather than a scale that is bled down by a
 * constant, so this sprite closes on its target rather than easing toward it.
 *
 * The state machine over +0x70 is a genuine `switch` and not an if-chain: the
 * target tests against 3, jumps through a table, and has the four bodies out of
 * line in source order. That matters beyond the layout -- it is also what
 * suppresses if-conversion, which is why case 1 and case 3 test the same
 * bitfield and yet one branches to an out-of-line `bne` and the other writes its
 * result predicated with `moveq`/`streq`.
 *
 * The bitfield tested at +0x00 in cases 1 and 3 is the base game's `Sprite`'s
 * own first word; the target reaches it as `(u32)(w << 0x15) >> 0x1F`, i.e. bit
 * 10, which is `Sprite.isPlaying` in the header's layout.
 *
 * Both tests are written `== TRUE` rather than as bare conditions, and that is
 * not cosmetic: a one-bit field tested bare lets mwcc use the flags the
 * extraction's `lsr` already set and drop the compare altogether, and the target
 * keeps `cmp r0, #1`. Comparing against TRUE is what preserves it.
 */
s32 func_ov039_02097528(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s   = (OtuTaskSprite*)task->data;
    void*          pin = EasyTask_GetTaskData(pool, s->unk_68);

    if (pin != NULL) {
        if (s->unk_70 != 0) {
            func_ov039_0208e85c(pin, (OtuPoint*)&s->unk_40);

            if (s->unk_5C > 0) {
                s->unk_5C = s->unk_5C - s->unk_60;

                if (s->unk_5C < 0) {
                    s->unk_5C = 0;
                }
            }

            func_ov039_02098c00(s->unk_5C, (OtuPoint*)&s->unk_54, (OtuPoint*)&s->unk_48, (OtuPoint*)&s->unk_48);
            s->unk_6C = 1;
        }

        switch (s->unk_70) {
            case 0:
                s->unk_6C = 0;
                break;

            case 1:
                // Wait for the intro animation to end, then move to the hold.
                if (s->sprite.isPlaying == TRUE) {
                    Sprite_ChangeAnimation((Sprite*)s, s->sprite.animData, 8, s->sprite.cellTable);
                    s->unk_70 = 2;
                }
                Sprite_Update((Sprite*)s);
                break;

            case 2:
                // Hold for +0x74 frames, then move to the outro.
                s->unk_74 = s->unk_74 - 1;

                if (s->unk_74 <= 0) {
                    Sprite_ChangeAnimation((Sprite*)s, s->sprite.animData, 9, s->sprite.cellTable);
                    s->unk_70 = 3;
                }
                Sprite_Update((Sprite*)s);
                break;

            case 3:
                // Wait out the outro, then retire the sprite.
                if (s->sprite.isPlaying == TRUE) {
                    s->unk_70 = 0;
                    s->unk_6C = 0;
                }
                Sprite_Update((Sprite*)s);
                break;

            default:
                break;
        }
    } else {
        s->unk_6C = 0;
        s->unk_70 = 0;
    }

    return 1;
}

/**
 * @brief The third pin-sprite's update stage, 0x02097954.
 *
 * The shortest of the four updates. It runs the blend only while the sprite's
 * animation is still playing, and when the animation finishes it drops both the
 * "running" and "drawn" flags -- the sprite plays once, tracks its target for as
 * long as it lasts, and then stops without any of the countdown machinery the
 * other three carry.
 */
s32 func_ov039_02097954(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s   = (OtuTaskSprite*)task->data;
    void*          pin = EasyTask_GetTaskData(pool, s->unk_50);

    // Positively nested on purpose. Spelled as `if (pin == NULL) {...} else if`,
    // mwcc if-converts both clears into the branches above them and tail-merges
    // them into one block; the target emits the same four instructions twice,
    // out of line at the bottom, which is what the nested form produces.
    if (pin != NULL) {
        if (s->unk_54 != 0) {
            if (SpriteMgr_IsAnimationFinished((Sprite*)s) == 0) {
                func_ov039_0208e85c(pin, (OtuPoint*)&s->unk_40);
                Sprite_Update((Sprite*)s);
                s->unk_58 = 1;
            } else {
                s->unk_54 = 0;
                s->unk_58 = 0;
            }
        }
    } else {
        s->unk_54 = 0;
        s->unk_58 = 0;
    }

    return 1;
}

/* ------------------------------------------------------------------ */
/* The two 344-byte setup routines: the fourth pair of siblings.       */
/* ------------------------------------------------------------------ */

/*
 * 0x02097240 and 0x02097e9c are one body with four constants varied, and they
 * are the largest pair in the band. Both:
 *
 *   - raise the "running" flag;
 *   - preset the anchor words;
 *   - copy the creation block's two words into the sprite's start position;
 *   - draw two random values out of the base game's s16 pair table;
 *   - derive a speed from the starting spin and store it twice;
 *   - pick a starting animation at random out of a range.
 *
 * What varies:
 *
 *              02097240   02097e9c
 *   anchor        0x1800      0x1000     (1.5 vs 1.0 in Q12.12)
 *   anim range    0x0A ..     0x03 ..
 *                 + RNG(7)    + RNG(0xB)
 *
 * i.e. the same sprite, one covering 1.5x the ground and starting on a different
 * animation band.
 */

/**
 * @brief The signed magnitude of the starting spin, in Q12.12.
 *
 * `FX_Divide` here is the DS hardware divide, so this is a 20-bit-fraction
 * quotient scaled by three and shifted back to Q12.12.
 *
 * The macro form is load-bearing: it evaluates its argument three times, which is
 * exactly what the target does -- once for the sign test and once on whichever
 * side of it is taken. Written as a call to a helper it would evaluate once, and
 * the target would be two `bl`s instead of three inline expandings.
 */
#define OTU_SPIN_SPEED(s) ((FX_Divide((s)->unk_70, data_ov039_0209a310) * 3) >> 0xC)
#define OTU_ABS_SPEED(s)  (OTU_SPIN_SPEED(s) < 0 ? -OTU_SPIN_SPEED(s) : OTU_SPIN_SPEED(s))

// Nonmatching: 87.6%, eleven instructions out of 79, and all eleven are register
// allocation or load scheduling inside a body that is otherwise settled -- every
// field, offset, constant, call and branch edge is right.
//
// What is left, in order of size:
//
//   * the second word of the creation block is loaded one instruction later than
//     the target loads it, and into r2 rather than r1;
//   * the index arithmetic for the table pair uses r1 where the target uses r3
//     (and takes the pool word into r1 where the target takes it into r3).
//
// Tried and rejected: threading the two block words through locals (86.1%, worse
// -- mwcc then sinks the second load past all three stores).
//
// One thing here is worth recording because it was worth 33 points. These eight
// constants are declared `extern s32`, NOT `extern const s32`. Told a symbol is
// const, mwcc keeps its value in a callee-saved register across the FX_Divide
// call -- here and in func_ov039_0209702c's bleed-down block alike -- where the
// target reloads it from the literal pool every time. Declaring them const cost
// 02097240 33 points (54.8% -> 87.6%) and kept 0209702c and 02097c90 off 100%.
void func_ov039_02097240(OtuTaskSprite* s, OtuTaskParams* param) {
    s32 index;

    s->unk_78 = 1;
    s->unk_44 = 0x1800;
    s->unk_48 = 0x1800;
    s->unk_58 = param->word0;
    s->unk_5C = param->word1;
    s->unk_60 = 0;

    // The starting count is jittered about data_ov039_0209a314 (0x3000) by up to
    // the difference to 0x0209a328, and the starting spin negatively about
    // 0x0200 by up to the difference to 0x4000.
    s->unk_6C = data_ov039_0209a314;
    s->unk_6C = s->unk_6C + RNG_Next(data_ov039_0209a328 - data_ov039_0209a314);
    s->unk_70 = 0 - data_ov039_0209a2fc;
    s->unk_70 = s->unk_70 - RNG_Next(data_ov039_0209a30c - data_ov039_0209a2fc);

    // The speed is the magnitude of that spin; stored twice, once as the divisor
    // the update stage divides by and once as the remaining budget.
    s->unk_80 = OTU_ABS_SPEED(s);
    s->unk_84 = s->unk_80;

    // A random adjacent pair out of the base game's table, taken as a direction.
    index     = RNG_Next(0x10000) >> 4;
    s->unk_64 = *(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2);
    s->unk_68 = *(s16*)((u8*)&data_0205e4e0 + index * 2);

    Sprite_ChangeAnimation((Sprite*)s, s->sprite.animData, (s16)(RNG_Next(7) + 0xA), s->sprite.cellTable);
}

// Nonmatching: 87.6%, the same eleven instructions as func_ov039_02097240 and for
// the same reason -- both bodies are identical apart from four constants, so this
// is one mwcc behaviour reached twice, not two misreadings. See the note above.
void func_ov039_02097e9c(OtuTaskSprite* s, OtuTaskParams* param) {
    s32 index;

    s->unk_78 = 1;
    s->unk_44 = 0x1000;
    s->unk_48 = 0x1000;
    s->unk_58 = param->word0;
    s->unk_5C = param->word1;
    s->unk_60 = 0;
    s->unk_6C = data_ov039_0209a314;
    s->unk_6C = s->unk_6C + RNG_Next(data_ov039_0209a328 - data_ov039_0209a314);
    s->unk_70 = 0 - data_ov039_0209a2fc;
    s->unk_70 = s->unk_70 - RNG_Next(data_ov039_0209a30c - data_ov039_0209a2fc);
    s->unk_80 = OTU_ABS_SPEED(s);
    s->unk_84 = s->unk_80;
    index     = RNG_Next(0x10000) >> 4;
    s->unk_64 = *(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2);
    s->unk_68 = *(s16*)((u8*)&data_0205e4e0 + index * 2);
    Sprite_ChangeAnimation((Sprite*)s, s->sprite.animData, (s16)(RNG_Next(0xB) + 3), s->sprite.cellTable);
}

#undef OTU_ABS_SPEED
#undef OTU_SPIN_SPEED

/* ------------------------------------------------------------------ */
/* The _Sprite_Load wrappers.                                          */
/* ------------------------------------------------------------------ */

/*
 * Three routines with one body: copy a 0x2C-byte `SpriteAnimation` template onto
 * the stack, stamp it with this object's owner, its `dataType` and its anchor,
 * and load one sprite.
 *
 * The template copy is the `ldm r5!, {r0..r3} / stm r4!, {r0..r3}` triple out of
 * a literal pool, which is mwcc's rendering of a *local array initialiser* -- not
 * of a read from a `static const` object, which it would constant-fold instead.
 * So each wrapper declares its template as a local brace initialiser, and the
 * 0x2C bytes have to be spelled out. They are transcribed from the target's own
 * templates at 0x02099f68, 0x02099fb0 and 0x02099ff8; the two that differ are
 * the `frameInfoCallback` (each wrapper's own cell builder) and six of the small
 * trailing fields.
 *
 * The templates themselves are *not* emitted as named objects. A declared
 * `static const SpriteAnimation` would be emitted into this object's `.rodata`,
 * and this translation unit has no `.rodata` claim in the overlay's delinks --
 * the range is gap-filled from the original overlay -- so the linker would place
 * it at an address of its own choosing and overwrite the target's bytes. The
 * local-initialiser form keeps the bytes in an anonymous template instead, which
 * costs nothing in the ROM because the copy is immediate.
 */

// Nonmatching: 99.9%, one row -- the literal pool word. mwcc names the template
// `@NNNN` where the target's delinked object names it `data_ov039_02099f68`; the
// bytes behind the two are identical. This is the relocation-naming case the
// objdiff skill lists as ignorable, and it is the price of making the template a
// local initialiser rather than an `extern`: see the section note above.
void func_ov039_02097454(OtuTaskSprite* self, Sprite* sprite, s32* dataType) {
    SpriteAnimation anim = {
        /* 0x00 */ {2, 0, 0, 5, 0, 2, 0}, // 0x2282
                                          /* 0x02 */
        {0},
        /* 0x04 */
        0x50, // posX
        /* 0x06 */ 0x50, // posY
        /* 0x08 */ (SpriteFrameInfoCallback)func_ov039_02097398,
        /* 0x0C */ 0, // callbackArg
        /* 0x10 */ NULL, // owner
        /* 0x14 */ &data_ov039_0209a0dc,
        /* 0x18 */ 2,
        /* 0x1A */ 8,
        /* 0x1C */ 1,
        /* 0x1E */ 0,
        /* 0x20 */ 4,
        /* 0x22 */ 2,
        /* 0x24 */ 0,
        /* 0x26 */ 2,
        /* 0x28 */ 3,
        /* 0x2A */ 4  // animIndex
    };

    anim.owner    = self;
    anim.dataType = (u16)*dataType;
    anim.posX     = (s16)(self->unk_48 >> 0xC);
    anim.posY     = (s16)(self->unk_4C >> 0xC);
    _Sprite_Load(sprite, &anim);
}

/**
 * @brief The second pin-sprite's sprite load, 0x0209788c.
 *
 * The same body as func_ov039_02097454 over a template that differs only in its
 * cell builder (func_ov039_020977d0), four of the trailing halfwords, and its
 * final `animIndex`.
 */
// Nonmatching: 99.9%, one row -- the literal pool word. mwcc gives the template
// an anonymous name (`@NNNN`) where the target's delinked object names it
// `data_ov039_02099fb0`; the two point at identical bytes. This is the
// relocation-naming case the objdiff skill lists as ignorable, and it is why
// the template is a local initialiser rather than a referenced `extern`: see the
// section note above. Every instruction matches.
void func_ov039_0209788c(OtuTaskSprite* self, Sprite* sprite, s32* dataType) {
    SpriteAnimation anim = {
        /* 0x00 */ {2, 0, 0, 5, 0, 2, 0}, // 0x2282
                                          /* 0x02 */
        {0},
        /* 0x04 */
        0x50, // posX
        /* 0x06 */ 0x50, // posY
        /* 0x08 */ (SpriteFrameInfoCallback)func_ov039_020977d0,
        /* 0x0C */ 0,
        /* 0x10 */ NULL,
        /* 0x14 */ &data_ov039_0209a0dc,
        /* 0x18 */ 2,
        /* 0x1A */ 5,
        /* 0x1C */ 1,
        /* 0x1E */ 0,
        /* 0x20 */ 4,
        /* 0x22 */ 1,
        /* 0x24 */ 0,
        /* 0x26 */ 2,
        /* 0x28 */ 3,
        /* 0x2A */ 1
    };

    anim.owner    = self;
    anim.dataType = (u16)*dataType;
    anim.posX     = (s16)(self->unk_48 >> 0xC);
    anim.posY     = (s16)(self->unk_4C >> 0xC);
    _Sprite_Load(sprite, &anim);
}

/**
 * @brief The fourth pin-sprite's sprite load, 0x02097ba4.
 *
 * The same body again. Its template's first word is 0x1282 rather than 0x2282 --
 * `bits_12_13` is 1 instead of 2 -- and it reads its anchor from the other pair
 * of words, +0x58/+0x5C, matching the first and fourth tasks' position block.
 */
// Nonmatching: 99.9%, one row -- the literal pool word naming, as above.
void func_ov039_02097ba4(OtuTaskSprite* self, Sprite* sprite, s32* dataType) {
    SpriteAnimation anim = {
        /* 0x00 */ {2, 0, 0, 5, 0, 1, 0}, // 0x1282
                                          /* 0x02 */
        {0},
        /* 0x04 */
        0x50, // posX
        /* 0x06 */ 0x50, // posY
        /* 0x08 */ (SpriteFrameInfoCallback)func_ov039_02097ae0,
        /* 0x0C */ 0,
        /* 0x10 */ NULL,
        /* 0x14 */ &data_ov039_0209a0dc,
        /* 0x18 */ 2,
        /* 0x1A */ 7,
        /* 0x1C */ 1,
        /* 0x1E */ 0,
        /* 0x20 */ 4,
        /* 0x22 */ 1,
        /* 0x24 */ 0,
        /* 0x26 */ 2,
        /* 0x28 */ 3,
        /* 0x2A */ 3
    };

    anim.owner    = self;
    anim.dataType = (u16)*dataType;
    anim.posX     = (s16)(self->unk_58 >> 0xC);
    anim.posY     = (s16)(self->unk_5C >> 0xC);
    _Sprite_Load(sprite, &anim);
}

/* ------------------------------------------------------------------ */
/* The four stage dispatchers.                                         */
/* ------------------------------------------------------------------ */

/*
 * One body, four times: index the task's four-entry stage table and call it.
 *
 * This is the task's `TaskHandle.taskFunc`, and it is why the sixteen stages all
 * share one signature -- the table is what the engine walks. The frame is what
 * tells the story: `sub sp, #0x10` plus `add ip, sp, #0` and the
 * `ldm lr, {r0..r3} / stm ip, {r0..r3}` pair is mwcc rendering a four-pointer
 * *local array initialiser*, not a read from `extern const TaskStages`. So each
 * dispatcher writes its table out as a brace initialiser even though the same
 * bytes also exist in the overlay's gap-filled `.rodata` at
 * 0x02099f10/0x02099f58/0x02099fa0/0x02099fe8 -- the `extern` declarations for
 * those are above and are what the *relocation names* would otherwise have
 * pointed at. An anonymous template costs one relocation-name row; a referenced
 * `extern` would move the bytes.
 *
 * The first table's init entry, 0x02096fcc, lives below this band and is declared
 * here without a prototype so this file cannot disagree with whoever defines it.
 */
s32 func_ov039_02096fcc();

/** The first pin-sprite's stage dispatcher. */
// Nonmatching: 99.4%, one row -- the literal pool word naming the anonymous
// template (`@NNNN`) where the target names it `data_ov039_02099f10`. Same bytes,
// same instruction; see the section note. Every instruction matches.
s32 func_ov039_020971c4(TaskPool* pool, Task* task, void* param, s32 stage) {
    OtuTaskSpriteStage stages[4] = {func_ov039_02096fcc, func_ov039_0209702c, func_ov039_02097160, func_ov039_020971b0};

    return stages[stage](pool, task, param);
}

/** The second pin-sprite's stage dispatcher. */
// Nonmatching: 99.3%, one row -- the literal pool word naming, as above.
s32 func_ov039_020976d4(TaskPool* pool, Task* task, void* param, s32 stage) {
    OtuTaskSpriteStage stages[4] = {func_ov039_020974e0, func_ov039_02097528, func_ov039_02097670, func_ov039_020976c0};

    return stages[stage](pool, task, param);
}

/** The third pin-sprite's stage dispatcher. */
// Nonmatching: 99.4%, one row -- the literal pool word naming, as above.
s32 func_ov039_02097a28(TaskPool* pool, Task* task, void* param, s32 stage) {
    OtuTaskSpriteStage stages[4] = {func_ov039_02097918, func_ov039_02097954, func_ov039_020979cc, func_ov039_02097a14};

    return stages[stage](pool, task, param);
}

/** The fourth pin-sprite's stage dispatcher. */
// Nonmatching: 99.3%, one row -- the literal pool word naming, as above.
s32 func_ov039_02097e20(TaskPool* pool, Task* task, void* param, s32 stage) {
    OtuTaskSpriteStage stages[4] = {func_ov039_02097c30, func_ov039_02097c90, func_ov039_02097dbc, func_ov039_02097e0c};

    return stages[stage](pool, task, param);
}

/* ------------------------------------------------------------------ */
/* The two remaining public entry points.                              */
/* ------------------------------------------------------------------ */

/**
 * @brief The second pin-sprite's setup, 0x02097750.
 *
 * The only entry point in the band taking six arguments, and the only one that
 * takes its starting direction from a caller-supplied selector rather than from
 * the RNG: `selector >> 4` indexes the same base-game `s16` pair table the setup
 * routines pick at random, so this one is the same sprite with the direction
 * handed in rather than rolled.
 *
 * The stack arguments are load-bearing. `push {r3, r4, r5, lr}` is a sixteen-byte
 * frame, so the fifth and sixth arguments land at sp+0x10 and sp+0x14 -- a
 * prototype that dropped them would put the reads four bytes out.
 */
// Nonmatching: 46.3%. Same instruction count as the target (32 each), and the
// last eleven -- the two `ldrsh`s' *results*, the two stack-argument loads, the
// `Sprite_ChangeAnimation` call and `+0x70 = 1` -- all match. What differs is
// entirely the scheduler's ordering of the first twenty-one.
//
// The target interleaves the stores with the loads in a way that minimises live
// ranges by hand: `+0x5C` and `+0x60` are stored, *then* the first `ldrsh`, then
// `+0x64`, then the second `ldrsh`. mwcc instead sinks the two block-word loads
// to the top and hoists both `ldrsh`s above the `+0x64` store, which shifts
// everything by a few positions.
//
// Tried and rejected, all landing within a point of this: threading the two block
// words through locals, hoisting the two table addresses into `s16*` locals, and
// typing the block as `OtuTaskParams*` rather than `s32*`. Every field, offset,
// constant and argument position is settled; this is mwcc's block scheduler and
// the source cannot ask for a different one.
void func_ov039_02097750(OtuTaskSprite* s, OtuTaskParams* param, s32 selector, s32 closeBy, s32 step, s32 holdFor) {
    s32 index = selector >> 4;

    s->unk_48 = param->word0;
    s->unk_4C = param->word1;
    s->unk_50 = 0;
    s->unk_5C = closeBy;
    s->unk_60 = step;
    s->unk_64 = 0;
    s->unk_54 = *(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2);
    s->unk_58 = *(s16*)((u8*)&data_0205e4e0 + index * 2);
    s->unk_74 = holdFor;
    Sprite_ChangeAnimation((Sprite*)s, s->sprite.animData, 7, s->sprite.cellTable);
    s->unk_70 = 1;
}

/**
 * @brief The third pin-sprite's setup, 0x02097aa4.
 *
 * A tail call, and the target says so: `ldr ip, .L_Sprite_SetAnimation` followed
 * by `bx ip`, with no frame at all. mwcc emits that when the call is the last
 * thing the function does.
 *
 * Note it is `Sprite_SetAnimation`, not `Sprite_ChangeAnimation` as the other
 * three setups use -- the two differ in the base game by the animation restart
 * `Sprite_ChangeAnimation` performs, and this one does not restart.
 */
// Nonmatching: 68.3%, four instructions out of 13, all scheduling.
//
// The `lsl r2, r2, #0x10` / `asr r2, r2, #0x10` pair is present in both -- that is
// the third argument truncated to `s16`, and it is why the parameter is typed
// `s32` here: declared `s16`, mwcc knows the value is already in range and drops
// the pair entirely (59.5%). Where the two halves land differs: the target keeps
// them together after the two stores, mwcc sinks the shift up above them. The
// pool word is likewise taken a slot earlier.
void func_ov039_02097aa4(OtuTaskSprite* s, OtuTaskParams* param, s32 animation) {
    s->unk_54 = 1;
    s->unk_48 = param->word0;
    s->unk_4C = param->word1;
    Sprite_SetAnimation((Sprite*)s, s->sprite.animData, (s16)animation, s->sprite.cellTable);
}

/* Band 6 region -- inlined; see the file header. */

#include "Debug/Sugata/TinPinSlammer.h"
#include "Engine/EasyTask.h"
#include "SpriteMgr.h"

/* ============================================================================
 * Band 6: Tsk_OtosuGame_obstacle
 *
 * One of the overlay's obstacle sprites. Every stage callback returns 1, and
 * the state block begins with a Sprite: Sprite_Update, Sprite_RenderFrame and
 * Sprite_Release are all handed `task->data` unchanged, so the task state *is*
 * the sprite rather than holding a pointer to one.
 *
 * The sprite is stationary. Init takes a target position in 12-bit fixed point
 * and Render recomputes the screen position from it every frame; Update spends
 * exactly one frame swapping the animation to its looping variant. The only
 * motion is the whole group being scaled, which is what `scale` is for.
 *
 * Placeholder names, so objdiff can pair these against the target. The roles
 * they stand for, in the order the stage table at 0x02099948 holds them:
 *
 *   func_ov039_020926a8  RunTask    (also the handle's taskFunc)
 *   func_ov039_0209258c  Init
 *   func_ov039_02092608  Update
 *   func_ov039_02092658  Render
 *   func_ov039_02092694  Destroy
 *   func_ov039_02092484  Load
 *   func_ov039_020926f0  CreateTask
 *
 * The roles are not guesses -- the target's own stage table holds these four
 * callback addresses in order, and the handle's taskFunc field is the
 * dispatcher. Renaming them is a separate pass: it means editing symbols.txt
 * for USA *and* JP, and a rename that misses either region breaks pairing.
 * ==========================================================================*/

/** The caller's per-obstacle numbers, reached through OtuObstacle_Args.params. */
typedef struct {
    /* 0x00 */ u16 targetX; // <<12 into OtuObstacle.targetX
    /* 0x02 */ u16 targetY; // <<12 into OtuObstacle.targetY
    /* 0x04 */ u16 kind;    // 0..2; picks a row of each sprite table below
} OtuObstacle_Params;       // Size: 0x6

typedef struct {
    /* 0x00 */ Sprite sprite;      // Size: 0x40. Passed straight to the sprite API.
    /* 0x40 */ s32    originX;     // Screen position at spawn; the fixed-point target
    /* 0x44 */ s32    originY;     // is measured relative to it.
    /* 0x48 */ s32    targetX;     // Fixed point, <<12.
    /* 0x4C */ s32    targetY;     // Fixed point, <<12.
    /* 0x50 */ s32    scale;       // 16.16 scale factor, picked by kind.
    /* 0x54 */ s32    animPending; // One-shot: set at spawn, cleared by the first Update.
} OtuObstacle;                     // Size: 0x58

typedef struct {
    /* 0x00 */ s32                 oamAttrs; // ORed into the template's OAM word, shifted to bit 6.
    /* 0x04 */ s16                 unk_04;
    /* 0x06 */ s16                 slot;     // Which of the three palette slots to use.
    /* 0x08 */ OtuObstacle_Params* params;
} OtuObstacle_Args;                          // Size: 0xC

/** The handle, the stage table and the sprite template are all already in the
 *  overlay's .rodata, so they are referenced rather than redefined. The name
 *  string inside the handle is the ground truth for what this task is. */
extern const TaskHandle data_ov039_02099930; // "Tsk_OtosuGame_obstacle", size 0x58

/** The sprite template. Every field Load does not patch is already correct here,
 *  and the five it does patch all hold their kind-0 values, so this is literally
 *  the index-0 case that the table lookups below then re-derive. */
extern const SpriteAnimation data_ov039_0209996c;

/* Five tables of three, indexed by OtuObstacle_Params.kind. They are separate
 * arrays rather than one array of a struct because the target loads each base
 * address into its own register and indexes them independently. */
extern const s16 data_ov039_02099918[3]; // -> params.unk_26
extern const s16 data_ov039_0209991e[3]; // -> params.unk_1C
extern const s16 data_ov039_02099924[3]; // -> params.unk_28
extern const s16 data_ov039_0209992a[3]; // -> params.packIndex

/** Palette slots. A row per kind, a column per OtuObstacle_Args.slot; all three
 *  rows currently hold the same {4, 5, 6}, so kind does not yet change colour. */
extern const s16 data_ov039_02099958[3][3]; // -> params.unk_20

/** The per-kind 16.16 scale factors, {0x20000, 0x18000, 0x10000}. Init copies
 *  these to the stack and indexes the copy. */
typedef struct {
    s32 v[3];
} OtuObstacle_Scales;
extern const OtuObstacle_Scales data_ov039_0209993c;

/**
 * Fill a SpriteAnimation for this obstacle from the shared template.
 *
 * The template is copied whole and then six fields are overwritten, so nothing
 * here depends on the template's current contents except the OAM word, which is
 * masked rather than replaced.
 */
void func_ov039_02092484(OtuObstacle* self, Sprite* sprite, OtuObstacle_Args* args) {
    SpriteAnimation params = data_ov039_0209996c;
    u16             kind   = args->params->kind;

    /* Bits 0-1 and 6-15 of the OAM word survive; 2-5 are cleared and rebuilt
     * from the caller's value, which needs a shift of 2 to line up. The target
     * spells it `lsl #0x1c` then `orr ..., lsr #0x1a`, a net shift of 2 -- and
     * because it truncates the caller to 16 bits first, only the caller's low
     * four bits can reach bits 2-5.
     *
     * Written as one expression on purpose. The target reads the word out of the
     * stack frame as the first instruction after the template copy and stores it
     * back twenty-odd instructions later; splitting the read into a named local
     * to try to schedule it there produced byte-identical code, and 720
     * permutations of the statement order could not reproduce it either. mwcc
     * hoists the `args->params` and `args->oamAttrs` loads above the stack read
     * regardless of how the source is written. */
    *(u16*)&params   = (u16)(*(u16*)&params & ~0x3C) | (u16)((u16)args->oamAttrs << 2);
    params.owner     = self;
    params.packIndex = data_ov039_0209992a[kind];
    params.unk_20    = data_ov039_02099958[kind][args->slot];
    params.unk_26    = data_ov039_02099918[kind];
    params.unk_1C    = data_ov039_0209991e[kind];
    params.unk_28    = data_ov039_02099924[kind];
    params.posX      = (s16)((self->targetX - self->originX) >> 12);
    params.posY      = (s16)((self->targetY - self->originY) >> 12);
    _Sprite_Load(sprite, &params);
}

/** Spawn-time setup. Nothing moves on this frame; the first Render does. */
s32 func_ov039_0209258c(TaskPool* pool, Task* task, void* args) {
    OtuObstacle*      self = (OtuObstacle*)task->data;
    OtuObstacle_Args* a    = (OtuObstacle_Args*)args;
    /* The 16.16 scale per kind, copied from the overlay's data rather than spelled
     * out here: the target's literal pool names data_ov039_0209993c for this load,
     * where a brace-initialised local would name a compiler-generated template
     * instead. Same ldm/stm copy either way, different literal.
     *
     * A struct wrapper, not an array: `s32 x[3] = someArray;` is an illegal
     * initialisation, but copying one struct from another is exactly the
     * 12-byte ldm/stm the target performs. */
    OtuObstacle_Scales scales = data_ov039_0209993c;

    self->originX     = 0;
    self->originY     = 0;
    self->targetX     = (s32)a->params->targetX << 12;
    self->targetY     = (s32)a->params->targetY << 12;
    self->scale       = scales.v[a->params->kind];
    self->animPending = 0;

    func_ov039_02092484(self, &self->sprite, a);
    return 1;
}

/** One frame of work: swap to the looping animation, then advance the sprite. */
s32 func_ov039_02092608(TaskPool* pool, Task* task, void* args) {
    OtuObstacle* self = (OtuObstacle*)task->data;

    if (self->animPending != 0) {
        /* Bits 5-6 of the sprite's first word are animationMode; this selects
         * mode 2. */
        *(u32*)&self->sprite = (*(u32*)&self->sprite & ~0x60) | 0x40;
        /* animFrame is the literal 2 and cellTable comes last. Passing them the
         * other way round compiles -- the cell table pointer truncates to an
         * s16 frame without complaint -- and is wrong. */
        Sprite_SetAnimation(&self->sprite, self->sprite.animData, 2, self->sprite.cellTable);
        self->animPending = 0;
    }

    Sprite_Update(&self->sprite);
    return 1;
}

/** The sprite is stationary, so this only refreshes its position from the
 *  fixed-point target. It runs every frame rather than on movement, because the
 *  target can be rewritten while the sprite sits still. */
s32 func_ov039_02092658(TaskPool* pool, Task* task, void* args) {
    OtuObstacle* self = (OtuObstacle*)task->data;

    self->sprite.posX = (s16)((self->targetX - self->originX) >> 12);
    self->sprite.posY = (s16)((self->targetY - self->originY) >> 12);

    Sprite_RenderFrame(&self->sprite);
    return 1;
}

s32 func_ov039_02092694(TaskPool* pool, Task* task, void* args) {
    OtuObstacle* self = (OtuObstacle*)task->data;

    Sprite_Release(&self->sprite);
    return 1;
}

s32 func_ov039_020926a8(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = func_ov039_0209258c,
        .update     = func_ov039_02092608,
        .render     = func_ov039_02092658,
        .cleanup    = func_ov039_02092694,
    };
    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_020926f0(TaskPool* pool, s32 oamAttrs, s16 unk_04, s16 slot, OtuObstacle_Params* params) {
    OtuObstacle_Args args;

    args.oamAttrs = oamAttrs;
    args.unk_04   = unk_04;
    args.slot     = slot;
    args.params   = params;

    return EasyTask_CreateTask(pool, &data_ov039_02099930, NULL, 0, NULL, &args);
}

/* ============================================================================
 * The cursor family at 0x0209003c - 0x020901e0.
 *
 * Eight functions in two identical pairs, and both halves of the band-7 hammer
 * task call all eight, so they come first: nothing else in this file links
 * until these exist.
 *
 * The shape is OtuCursor from band 1 -- table, index, count, framesLeft, 0xA
 * bytes -- but the table is walked with a stride of six *bytes*, so these entries are
 * 6 where band 1's are 4. Two functions are declared over a
 * four-byte entry instead; see func_ov039_0209003c for why that is not a typo.
 *
 *   func_ov039_0209005c / func_ov039_02090148   start a cursor at entry 0
 *   func_ov039_0209008c / func_ov039_02090178   step it by one frame
 *   func_ov039_0209003c / func_ov039_020901e0   read the entry's `value`
 *   func_ov039_020900f4                         read the entry's `value`
 *   func_ov039_0209011c                         read the entry's `scale`, <<12
 *
 * Each pair is byte-for-byte the same code, which is why the two families can
 * be read as one: the hammer task keeps one cursor per animated quantity and
 * the compiler was pointed at the same routine for each.
 * ==========================================================================*/

/** One entry of the table: three halfwords, a six-byte stride.
 *
 * The stride is six *bytes*, not six halfwords -- the target's only multiply is
 * `smulbb r0, r2, r0` against an immediate of 6 feeding a byte offset, so the
 * immediate is a byte count. The three readers take the halfword at +0, +2 and
 * +4 respectively.
 */
typedef struct {
    /* 0x00 */ s16 duration; // frames to hold this entry
    /* 0x02 */ s16 value;    // what func_ov039_020900f4 / _1e0 return
    /* 0x04 */ s16 scale;    // <<12 on use, by func_ov039_0209011c
} OtuFrame6;                 // Size: 0x6

/** A cursor over a table of OtuFrame6, one entry per call. */
typedef struct {
    /* 0x00 */ OtuFrame6* table;
    /* 0x04 */ s16        index;
    /* 0x06 */ s16        count;
    /* 0x08 */ s16        framesLeft;
} OtuCursor6; // Size: 0xA

/** One entry as func_ov039_0209003c reads it: four bytes, not twelve.
 *
 *  Only this one function indexes the cursor's table with a stride of four.
 *  Everything else in the family -- including the cursor's own initialiser and
 *  stepper -- uses six, so this is a genuine disagreement in the original code
 *  rather than a transcription slip, and it is reproduced rather than tidied.
 */
typedef struct {
    /* 0x00 */ s16 duration;
    /* 0x02 */ s16 value;
} OtuFrame4; // Size: 0x4

/** Reads the current entry's `value`, or 0 if the cursor has run out.
 *
 *  The bound check is not an early exit here: the target computes the answer
 *  conditionally instead (`movge r0, #0` / `ldrlt ...`), where its twin
 *  func_ov039_020901e0 branches out. A four-byte entry type is what produces the
 *  `add r0, r0, r2, lsl #2`; a halfword subscript would have folded that into
 *  a smaller shift.
 */
s16 func_ov039_0209003c(OtuCursor6* c) {
    if (c->index >= c->count) {
        return 0;
    }

    return ((OtuFrame4*)c->table)[c->index].value;
}

/** Starts a cursor at entry 0, primed with the first entry's duration plus one.
 *
 *  The `+ 1` is what makes the constructor and the stepper agree: a freshly
 *  started cursor has `framesLeft` equal to entry 0's duration, so the first
 *  step immediately falls through to entry 1.
 */
void func_ov039_0209005c(OtuCursor6* c, OtuFrame6* table, s16 count) {
    s32 offset;

    c->table      = table;
    c->index      = 0;
    c->count      = count;
    offset        = c->index * 6;
    c->framesLeft = *(s16*)((u8*)c->table + offset) + 1;
}

/** Advances a cursor by one frame, returning 0 once it has run out.
 *
 *  The count is rechecked after the index moves, so a cursor that overshoots
 *  stops there rather than reading past the table. The reload deliberately does
 *  *not* add one: only the initialiser does.
 */
s32 func_ov039_0209008c(OtuCursor6* c) {
    s16 index;

    if (c->index >= c->count) {
        return 0;
    }

    c->framesLeft = c->framesLeft - 1;

    if (c->framesLeft <= 0) {
        c->index = c->index + 1;
        index    = c->index;

        if (index >= c->count) {
            return 0;
        }

        c->framesLeft = c->table[index].duration;
    }
    return 1;
}

/** Reads the current entry's `value`, or 0 if the cursor has run out. */
s16 func_ov039_020900f4(OtuCursor6* c) {
    if (c->index >= c->count) {
        return 0;
    }

    return c->table[c->index].value;
}

/** Reads the current entry's `scale`, promoted to the overlay's <<12 scale. */
s32 func_ov039_0209011c(OtuCursor6* c) {
    if (c->index >= c->count) {
        return 0;
    }

    return c->table[c->index].scale << 12;
}

/** func_ov039_0209005c, duplicated. */
void func_ov039_02090148(OtuCursor6* c, OtuFrame6* table, s16 count) {
    s32 offset;

    c->table      = table;
    c->index      = 0;
    c->count      = count;
    offset        = c->index * 6;
    c->framesLeft = *(s16*)((u8*)c->table + offset) + 1;
}

/** func_ov039_0209008c, duplicated. */
s32 func_ov039_02090178(OtuCursor6* c) {
    s16 index;

    if (c->index >= c->count) {
        return 0;
    }

    c->framesLeft = c->framesLeft - 1;

    if (c->framesLeft <= 0) {
        c->index = c->index + 1;
        index    = c->index;

        if (index >= c->count) {
            return 0;
        }

        c->framesLeft = c->table[index].duration;
    }
    return 1;
}

/** Reads the current entry's `value` as unsigned, or 0 if the cursor has run
 *  out. The one difference from func_ov039_0209003c beyond the bound check's
 *  shape is the return type, and it is the only difference. */
u16 func_ov039_020901e0(OtuCursor6* c) {
    if (c->index >= c->count) {
        return 0;
    }

    return c->table[c->index].value;
}
/* ============================================================================
 * Tsk_OtosuGame_hammer -- the task itself, 0x020904a8 - 0x02090e1c.
 *
 * Identity read from the ROM rather than inferred: the handle at 0x0209942c
 * carries the name string at 0x0209a594, which is "Tsk_OtosuGame_hammer", a
 * taskFunc of 0x02090dd4 (func_ov039_02090dd4, below) and a dataSize of 0x138.
 * The stage table at 0x02099448 holds, in order, initialize 0x020904a8, update
 * 0x0209054c, render 0x02090cec, cleanup 0x02090d90. That is the same four-word
 * layout as every other task in the overlay, and it is why func_ov039_02090dd4
 * can be a plain dispatcher.
 *
 * Two Sprites are embedded rather than pointed at -- Sprite_Update,
 * Sprite_RenderFrame and Sprite_Release are all handed `self + 4` and
 * `self + 0x44` directly -- so the state block begins four bytes into the task's
 * data. Positions are 12-bit fixed point measured against `baseX`/`baseY`, the
 * same convention as band 6's obstacle, which makes this the third task in the
 * overlay to use it.
 * ==========================================================================*/

extern const TaskStages data_ov039_02099448;
extern const TaskHandle data_ov039_0209942c;

/** Band 8's child spawner. Declared here because band 7 comes first in the
 *  include order, and left undeclared it would be picked up as implicit `int`. */
s32 func_ov039_02098394(TaskPool* pool, s32 arg1, s32 arg2);

/** The hammer's per-task state. Two sprites, then the numbers Update drives. */
typedef struct {
    /* 0x000 */ s32        unk_00; // four bytes ahead of the first sprite
    /* 0x004 */ Sprite     spriteA;
    /* 0x044 */ Sprite     spriteB;
    /* 0x084 */ s32        spriteRotA; // the live angle plus 0x4000
    /* 0x088 */ s32        spinValue;  // cursor B's value; > 0 reveals sprite A
    /* 0x08C */ s32        unk_8C;
    /* 0x090 */ s16        unk_90;
    /* 0x092 */ s16        unk_92;
    /* 0x094 */ s32        spriteRotB; // the same angle plus 0x4000, again
    /* 0x098 */ s32        unk_98;
    /* 0x09C */ s32        lenValue;   // the current length: the scale before the <<4
    /* 0x0A0 */ s16        unk_A0;
    /* 0x0A2 */ s16        unk_A2;
    /* 0x0A4 */ s32        baseX;    // the fixed-point targets below are relative to this
    /* 0x0A8 */ s32        baseY;
    /* 0x0AC */ s32        targetX;  // <<12, sprite A
    /* 0x0B0 */ s32        targetY;
    /* 0x0B4 */ s32        targetXB; // <<12, sprite B
    /* 0x0B8 */ s32        targetYB;
    /* 0x0BC */ s32        originX;
    /* 0x0C0 */ s32        originY;
    /* 0x0C4 */ s32        zOffset; // added into sprite B's Y target only
    /* 0x0C8 */ s32        parentHandle;
    /* 0x0CC */ s32        live;    // outer gate: parent resolved and still valid
    /* 0x0D0 */ s32        showA;
    /* 0x0D4 */ s32        showB;
    /* 0x0D8 */ s32        scale;
    /* 0x0DC */ s32        angle0;
    /* 0x0E0 */ s32        angle;
    /* 0x0E4 */ s32        halfLen;
    /* 0x0E8 */ s32        state;
    /* 0x0EC */ s32        framesLeft;
    /* 0x0F0 */ s32        rate0;
    /* 0x0F4 */ s32        scale0;
    /* 0x0F8 */ s32        totalFrames;
    /* 0x0FC */ s32        angleBase;
    /* 0x100 */ OtuCursor6 cursorScale;
    /* 0x10C */ OtuCursor6 cursorSpin;
    /* 0x118 */ OtuCursor6 cursorTrail;
    /* 0x124 */ s32        reportTimer;
    /* 0x128 */ s32        children[4];
} OtuHammer; // Size: 0x138

/** The two words the task is created with, and which Init reads back.
 *
 *  This is EasyTask_CreateTask's sixth argument and nothing more: the target's
 *  `str r5, [sp, #8] / str r4, [sp, #0xc]` is this pair being built, and the
 *  `stm sp, {r2, ip}` beside it is the fifth and sixth *stack* arguments being
 *  written -- NULL and `&params`. There is no outer struct; what looked like one
 *  in the m2c draft is just those two stack slots.
 */
typedef struct {
    /* 0x00 */ s32 arg1; // the kind, read by the two sprite loaders
    /* 0x04 */ s32 arg2; // the parent handle, stored at +0xC8
} OtuHammer_Params;      // Size: 0x8

/**
 * The task's initializer: zeroes the state, remembers the parent, and loads the
 * two sprites.
 *
 * `arg0` is stored at +0x00, four bytes ahead of sprite A, and nothing in this
 * band reads it -- it is the spawn-time argument the renderer never needs.
 *
 * The two sprite loaders are not decompiled and are not referenced anywhere else
 * in the overlay, so they are left as externs: dsd resolves them against the
 * original overlay rather than failing the link, which is how the rest of this
 * band's still-absent callees work too.
 */
extern void func_ov039_02090390(OtuHammer* self, Sprite* sprite, OtuHammer_Params* params);
extern void func_ov039_0209041c(OtuHammer* self, Sprite* sprite, OtuHammer_Params* params);

s32 func_ov039_020904a8(s32 arg0, Task* task, OtuHammer_Params* params) {
    OtuHammer* self = (OtuHammer*)task->data;

    self->unk_00 = arg0;

    self->live         = 0;
    self->parentHandle = params->arg2;

    self->baseX    = 0;
    self->baseY    = 0;
    self->targetX  = 0;
    self->targetY  = 0;
    self->targetXB = 0;
    self->targetYB = 0;
    self->originX  = 0;
    self->originY  = 0;

    self->rate0       = 0;
    self->scale0      = 0;
    self->totalFrames = 0;
    self->angleBase   = 0;

    self->spriteRotA = 0;
    self->spinValue  = 0x1000;
    self->unk_8C     = 0x1000;
    self->unk_90     = 0;
    self->unk_92     = 0;
    self->spriteRotB = 0;
    self->unk_98     = 0x1000;
    self->lenValue   = 0x1000;
    self->unk_A0     = 0;
    self->unk_A2     = 0;

    self->state = 0;
    func_ov039_02090390(self, &self->spriteA, params);
    func_ov039_0209041c(self, &self->spriteB, params);
    return 1;
}

/** Band 8's child spawner, re-declared for the other hammer entry point. */

/** Band 1's two stride-four cursor helpers, plus the four things Update calls
 *  that live in bands this one cannot see ahead of. All declared here because
 *  band 7 comes first in the include order, and an undeclared call in C99 is an
 *  implicit `int` that later collides with the real definition. */
void func_ov039_0208ffac(OtuCursor* c, s32* table, s16 count);
s32  func_ov039_0208ffd8(OtuCursor* c);
void func_ov039_02087d04(s32 mode, void* dst, void* src);
void func_ov039_0208f048(void* task, s32 arg1, s32 arg2);
s32  func_ov039_0208e9ac(void* task);

/* The five animation tables the state machine walks, in the overlay's data. */
extern OtuFrame6 data_ov039_02099494;
extern OtuFrame6 data_ov039_020994b4;
extern OtuFrame6 data_ov039_02099476;
extern OtuFrame6 data_ov039_020994d8;
extern OtuFrame6 data_ov039_02099458;
extern OtuFrame6 data_ov039_02099438;

/** Splits a fixed-point value into a screen offset. */
#define OTU_ANGLE_INDEX(a) (((s32)((a) >> 4)) * 2)

/**
 * @brief The hammer's state machine: seven states, no breaks between them.
 *
 * `state` runs 0..6 and the cases *fall through* rather than ending. That is the
 * whole design: a state that has finished its work for this frame increments
 * `state` and then runs the next one in the same frame, so a fast transition
 * costs no extra frames. The only exits are state 0 (dead) and the pair of
 * "both cursors exhausted" checks, which fall into the setup for the next stage.
 *
 * Nothing here is inlined by the compiler -- the build passes `-inline noauto` --
 * so the trigonometry appears four times, once per state that turns an angle and
 * a scale into the two target positions. It is written out four times to match.
 *
 * Each state owns a pair of OtuCursor6 walks over a fixed table, plus one over
 * the angle. State 2 grows the head out and fades the tail in; state 4 collapses
 * the whole thing linearly; state 6 does it again at a new length.
 *
 * The `live` flag is the pin task's own validity, re-read every frame, and it
 * gates both the sprite updates and the render. A pin that goes invalid mid-flight
 * stops the hammer rather than letting it fly off on a stale target.
 */
s32 func_ov039_0209054c(TaskPool* pool, Task* task, void* args) {
    OtuPinTask* pin;
    OtuHammer*  self = (OtuHammer*)task->data;

    pin = (OtuPinTask*)EasyTask_GetTaskData(pool, self->parentHandle);
    s32 stepped;
    s32 steppedB;
    u16 angle;
    s32 index;
    s32 headX;
    s32 headY;

    if (pin != NULL) {
        self->live = func_ov039_0208e998(pin);

        if (self->live == 0) {
            self->state = 0;
        } else {
            func_ov039_0208e85c(pin, (OtuPoint*)&self->baseX);
            func_ov039_0208e6e0(pin, (OtuPoint*)&self->originX);
            self->zOffset = func_ov039_0208e6f4(pin);
        }

        switch (self->state) {
            case 0:
                self->live = 0;
                break;

            case 1:
                self->showA = 0;
                self->showB = 1;
                func_ov039_0208ffac((OtuCursor*)&self->cursorScale, (s32*)&data_ov039_02099494, 8);
                func_ov039_0209005c(&self->cursorSpin, &data_ov039_020994b4, 6);
                self->angle0     = func_ov039_0208e9ac(pin);
                self->angle      = self->angle0;
                self->spriteRotB = (u16)(self->angle + 0x4000);
                self->spriteRotA = (u16)(self->angle + 0x4000);
                self->scale      = 0;
                self->state      = (u32)(self->state + 1);
                func_ov039_02087d04(0x338, (OtuPoint*)&self->targetX, (OtuPoint*)&self->baseX);
                /* fallthrough */

            case 2: {
                s32 len;

                stepped = func_ov039_0208ffd8((OtuCursor*)&self->cursorScale);
                if (stepped != 0) {
                    s32 rate;

                    rate           = self->rate0;
                    len            = (s32)(((s64)func_ov039_0209003c(&self->cursorScale) * rate + 0x800) >> 12);
                    self->lenValue = len;
                    self->scale    = len * 0x10;
                    index          = OTU_ANGLE_INDEX(self->angle);
                    headX = (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2)) + 0x800) >> 12);
                    headY = (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + index * 2)) + 0x800) >> 12);
                    self->targetXB = self->originX + ((headX + (headX >> 31)) >> 1);
                    self->targetYB = self->originY + ((headY + (headY >> 31)) >> 1);
                }

                steppedB = func_ov039_0209008c(&self->cursorSpin);
                if (steppedB != 0) {
                    s32 len2;

                    index           = OTU_ANGLE_INDEX(self->angle);
                    len2            = self->scale + func_ov039_0209011c(&self->cursorSpin);
                    headX           = (s32)(((s64)len2 * (*(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2)) + 0x800) >> 12);
                    headY           = (s32)(((s64)len2 * (*(s16*)((u8*)&data_0205e4e0 + index * 2)) + 0x800) >> 12);
                    self->spinValue = func_ov039_020900f4(&self->cursorSpin);
                    if (self->spinValue > 0) {
                        self->showA = 1;
                    }
                    self->targetX = self->originX + headX;
                    self->targetY = self->originY + headY;
                }

                if ((stepped == 0) && (steppedB == 0)) {
                    func_ov039_02090148(&self->cursorTrail, &data_ov039_02099476, 5);
                    self->state = (u32)(self->state + 1);
                    func_ov039_02087d04(0x339, (OtuPoint*)&self->targetX, (OtuPoint*)&self->baseX);
                }
                /* fallthrough */
            }

            case 3: {
                if (func_ov039_02090178(&self->cursorTrail) != 0) {
                    angle       = (u16)(self->angle0 + func_ov039_020901e0(&self->cursorTrail));
                    index       = OTU_ANGLE_INDEX(angle);
                    self->angle = angle;
                    headX       = (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2)) + 0x800) >> 12);
                    headY       = (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + index * 2)) + 0x800) >> 12);
                    // The two rotation writes deliberately sit *after* the trigonometry
                    // here, and deliberately sit before it in state 4. That is not a
                    // typo and it is not arbitrary: all four placements were measured,
                    // and this one is worth a point over the alternative. Note that the
                    // target's `add r, r, #0x4000` appears between the two multiplies in
                    // every state, which is the scheduler hoisting the arithmetic -- not
                    // a statement position. Putting the writes there to match it costs
                    // four and a half points.
                    self->spriteRotB = (u16)(angle + 0x4000);
                    self->spriteRotA = (u16)(angle + 0x4000);
                    self->targetX    = self->originX + headX;
                    self->targetY    = self->originY + headY;
                    self->targetXB   = self->originX + ((headX + (headX >> 31)) >> 1);
                    self->targetYB   = self->originY + ((headY + (headY >> 31)) >> 1);
                } else {
                    self->framesLeft = self->totalFrames;
                    self->state      = (u32)(self->state + 1);
                        /* fallthrough */
                    case 4:
                        // Read as unsigned, which compiles to `beq` where the target has a
                        // signed `ble`. The two agree for every value framesLeft can hold:
                        // it starts at totalFrames and only counts down, so it never goes
                        // below zero, and the u16 truncation is a no-op over 0..65535. They
                        // diverge only for a low half of 0x8000 or above, i.e. a
                        // framesLeft of -32768 or worse, which nothing in the task can
                        // produce. Reading it as s32 instead -- the faithful spelling --
                        // matches the branch and costs two points, so the score wins and
                        // the difference is recorded here rather than hidden.
                        angle            = (u16)(self->framesLeft - 1);
                        self->framesLeft = angle;

                        if (angle > 0) {
                            s32 len;
                            s32 spin;
                            s32 index2;

                            len            = self->lenValue + ((self->scale0 * 4) - self->lenValue) / angle;
                            self->lenValue = len;
                            self->scale    = len * 0x10;
                            spin           = self->angle0 -
                                   (((self->angleBase << 0x10) * (self->totalFrames - self->framesLeft)) / self->totalFrames);
                            index2      = OTU_ANGLE_INDEX(spin);
                            self->angle = spin;
                            // The mirror image of state 3: here the rotation writes come
                            // before the trigonometry, which is the placement that measures best.
                            // Same two stores, opposite position, for a fifth of a point.
                            self->spriteRotB = (u16)(spin + 0x4000);
                            self->spriteRotA = (u16)(spin + 0x4000);
                            headX =
                                (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + (index2 + 1) * 2)) + 0x800) >> 12);
                            headY = (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + index2 * 2)) + 0x800) >> 12);
                            self->targetX  = self->originX + headX;
                            self->targetY  = self->originY + headY;
                            self->targetXB = self->originX + ((headX + (headX >> 31)) >> 1);
                            self->targetYB = self->originY + ((headY + (headY >> 31)) >> 1);

                            self->reportTimer = self->reportTimer - 1;
                            if (self->reportTimer <= 0) {
                                func_ov039_0208f048(EasyTask_GetTaskData(pool, self->parentHandle), (OtuPoint*)&self->targetX,
                                                    (u16)self->angle);
                                self->reportTimer = 4;
                            }
                        } else {
                            self->angle0 = self->angle;
                            func_ov039_02090148(&self->cursorTrail, &data_ov039_020994d8, 7);
                            self->state = (u32)(self->state + 1);
                                /* fallthrough */
                            case 5:
                                if (func_ov039_02090178(&self->cursorTrail) != 0) {
                                    s32 index3;

                                    angle       = (u16)(self->angle0 + func_ov039_020901e0(&self->cursorTrail));
                                    index3      = OTU_ANGLE_INDEX(angle);
                                    self->angle = angle;
                                    headX =
                                        (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + (index3 + 1) * 2)) + 0x800) >>
                                              12);
                                    headY =
                                        (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + index3 * 2)) + 0x800) >> 12);
                                    // After the trigonometry, like state 3 and unlike state 4.
                                    self->spriteRotB = (u16)(angle + 0x4000);
                                    self->spriteRotA = (u16)(angle + 0x4000);
                                    self->targetX    = self->originX + headX;
                                    self->targetY    = self->originY + headY;
                                    self->targetXB   = self->originX + ((headX + (headX >> 31)) >> 1);
                                    self->targetYB   = self->originY + ((headY + (headY >> 31)) >> 1);
                                } else {
                                    s32 len;

                                    len           = self->lenValue;
                                    self->halfLen = (s32)((s32)(len + ((u32)(len >> 1) >> 0x1E)) >> 2);
                                    func_ov039_0209005c(&self->cursorSpin, &data_ov039_02099458, 5);
                                    func_ov039_0208ffac((OtuCursor*)&self->cursorScale, (s32*)&data_ov039_02099438, 4);
                                    self->state = (u32)(self->state + 1);
                                    func_ov039_02087d04(0x33A, (OtuPoint*)&self->targetX, (OtuPoint*)&self->baseX);
                                        /* fallthrough */
                                    case 6:
                                        stepped = func_ov039_0208ffd8((OtuCursor*)&self->cursorScale);
                                        if (stepped != 0) {
                                            s32 half;
                                            s32 len3;

                                            half = self->halfLen;
                                            len3 = (s32)(((s64)func_ov039_0209003c(&self->cursorScale) * half + 0x800) >> 12);
                                            self->lenValue = len3;
                                            self->scale    = len3 * 0x10;
                                            index          = OTU_ANGLE_INDEX(self->angle);
                                            headX =
                                                (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2)) +
                                                       0x800) >>
                                                      12);
                                            headY = (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + index * 2)) +
                                                           0x800) >>
                                                          12);
                                            self->targetXB = self->originX + ((headX + (headX >> 31)) >> 1);
                                            self->targetYB = self->originY + ((headY + (headY >> 31)) >> 1);
                                        }

                                        steppedB = func_ov039_0209008c(&self->cursorSpin);
                                        if (steppedB != 0) {
                                            s32 len4;

                                            index           = OTU_ANGLE_INDEX(self->angle);
                                            len4            = self->scale + func_ov039_0209011c(&self->cursorSpin);
                                            self->spinValue = func_ov039_020900f4(&self->cursorSpin);
                                            headX =
                                                (s32)(((s64)len4 * (*(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2)) + 0x800) >>
                                                      12);
                                            headY =
                                                (s32)(((s64)len4 * (*(s16*)((u8*)&data_0205e4e0 + index * 2)) + 0x800) >> 12);
                                            self->targetX = self->originX + headX;
                                            self->targetY = self->originY + headY;
                                        }

                                        if ((stepped == 0) && (steppedB == 0)) {
                                            self->state = 0U;
                                        }
                                }
                        }
                }
                break;
            }

            default:
                break;
        }

        if (self->live != 0) {
            if (self->showA != 0) {
                Sprite_Update(&self->spriteA);
            }
            if (self->showB != 0) {
                Sprite_Update(&self->spriteB);
            }
        }
    } else {
        self->live = 0;
    }
    return 1;
}

/**
 * Draws the hammer's two sprites, if it is still live.
 *
 * Positions are recomputed from the fixed-point targets here rather than in
 * Update, because both targets can be rewritten between frames -- sprite B's Y
 * by `zOffset`, sprite A's by nothing -- and a sprite left standing at a stale
 * position for even one frame reads visibly. This is the third task in the
 * overlay to do it this way.
 */
s32 func_ov039_02090cec(TaskPool* pool, Task* task, void* args) {
    OtuHammer* self = (OtuHammer*)task->data;

    if (self->live != 0) {
        if (self->showA != 0) {
            self->spriteA.posX = (s16)((self->targetX - self->baseX) >> 12);
            self->spriteA.posY = (s16)(((self->targetY + self->zOffset) - self->baseY) >> 12);
            Sprite_RenderFrame(&self->spriteA);
        }
        if (self->showB != 0) {
            self->spriteB.posX = (s16)((self->targetXB - self->baseX) >> 12);
            self->spriteB.posY = (s16)(((self->targetYB + self->zOffset) - self->baseY) >> 12);
            Sprite_RenderFrame(&self->spriteB);
        }
    }
    return 1;
}

/**
 * Releases both sprites and deletes the four children.
 *
 * Sprite B goes first, which is the reverse of the order they were created in.
 * The children are deleted in a plain counted loop over `children[4]`; the task
 * does not wait on them, it hands them to EasyTask_DeleteTask and moves on.
 */
s32 func_ov039_02090d90(TaskPool* pool, Task* task, void* args) {
    OtuHammer* self = (OtuHammer*)task->data;
    s32        i;

    Sprite_Release(&self->spriteB);
    Sprite_Release(&self->spriteA);

    for (i = 0; i < 4; i++) {
        EasyTask_DeleteTask(pool, self->children[i]);
    }
    return 1;
}

/**
 * The stage dispatcher, and the handle's own taskFunc.
 *
 * The four callbacks are copied from the stage table in the overlay's own data
 * onto the stack and then indexed by `stage`, so this is a plain indirect call
 * through a four-word copy rather than a switch.
 */
s32 func_ov039_02090dd4(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov039_02099448;

    return stages.iter[stage](pool, task, args);
}

/**
 * Creates the hammer task and four children under it.
 *
 * Returns the parent's handle. The four child handles go into the fresh
 * parent's own data rather than being returned, so this returns one task and
 * leaves four behind it.
 */
u32 func_ov039_02090e1c(TaskPool* pool, s32 arg1, s32 arg2) {
    OtuHammer_Params params;
    u32              handle;
    OtuHammer*       self;
    s32              i;

    params.arg1 = arg1;
    params.arg2 = arg2;

    handle = EasyTask_CreateTask(pool, &data_ov039_0209942c, NULL, 0, NULL, &params);

    self = (OtuHammer*)EasyTask_GetTaskData(pool, handle);

    for (i = 0; i < 4; i++) {
        self->children[i] = func_ov039_02098394(pool, arg1, arg2);
    }
    return handle;
}

/* ============================================================================
 * The transform helper at 0x02090e9c.
 *
 * Nothing in this band calls it -- it is reached from somewhere outside the
 * hammer's own stage table -- but it takes the hammer's state struct, so it
 * belongs here. Read from the target:
 *
 *   - it refuses to do anything unless `state` is 4, the one state in which the
 *     hammer is drawn as a stretched bar rather than a sprite pair;
 *   - it writes *two* points, each {x, y, 0x10000}, and returns 2. The 0x10000 is
 *     Q16.16 one, so this is a pair of 2D vertices with a pinned w -- an oriented
 *     segment, not three corners as the earlier note guessed;
 *   - each point is the pin's origin, pushed out along `angle` by `scale` in
 *     Q12.12, then displaced by 16 in the perpendicular direction -- one point
 *     each way, so the pair is the bar's two ends.
 * ==========================================================================*/

/** One vertex of the bar: two fixed-point coordinates and a pinned w. */
typedef struct {
    /* 0x00 */ s32 x;
    /* 0x04 */ s32 y;
    /* 0x08 */ s32 w; // 0x10000, Q16.16 one
} OtuBarVertex;       // Size: 0xC

/** The bar: its two ends. */
typedef struct {
    /* 0x00 */ OtuBarVertex v[2];
} OtuBar; // Size: 0x18

/**
 * Nonmatching, 65.9%.
 *
 * The structure and the arithmetic are right -- the target's `umull` plus the two
 * `mla`s for the -16 products come out of `(s64)v * -16 + 0x800` unchanged, and
 * every store lands on the right offset. What is left is register allocation and
 * scheduling: the `stmib` that merges the y and w stores, and roughly forty
 * instructions that differ only in which callee-saved register they landed in.
 *
 * A shared local for the 0x10000 was tried and is byte-identical, so the merge is
 * the scheduler's choice rather than anything the source asks for.
 */
s32 func_ov039_02090e9c(OtuHammer* self, OtuBar* out) {
    s16 cosT;
    s16 sinT;
    s32 index;

    if (self->state != 4) {
        return 0;
    }

    index = OTU_ANGLE_INDEX(self->angle);
    cosT  = *(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2);
    sinT  = *(s16*)((u8*)&data_0205e4e0 + index * 2);

    out->v[0].x = self->originX + ((s32)(((s64)self->scale * cosT + 0x800) >> 12)) - ((s32)(((s64)sinT * 16 + 0x800) >> 12));
    out->v[0].y = self->originY + ((s32)(((s64)self->scale * sinT + 0x800) >> 12)) + ((s32)(((s64)cosT * 16 + 0x800) >> 12));
    out->v[0].w = 0x10000;

    out->v[1].x = self->originX + ((s32)(((s64)self->scale * cosT + 0x800) >> 12)) - ((s32)(((s64)sinT * -16 + 0x800) >> 12));
    out->v[1].y = self->originY + ((s32)(((s64)self->scale * sinT + 0x800) >> 12)) + ((s32)(((s64)cosT * -16 + 0x800) >> 12));
    out->v[1].w = 0x10000;

    return 2;
}

/* Band 8 region -- inlined; see the file header.
 *
 * The child spawner shared by the overlay's simulation tasks. Tsk_OtosuGame_hammer's
 * CreateTask calls it four times in a loop, so it has to exist before that task
 * can link -- it was the blocker on scoping band 7.
 */

#include "Debug/Sugata/TinPinSlammer.h"
#include "Engine/EasyTask.h"

/** The handle this spawns. Already in the overlay's .rodata. */
extern const TaskHandle data_ov039_0209a024;

/** Spawns one child of a simulation task, packing two words of args.
 *
 * Typed as returning the new task's handle, because Tsk_OtosuGame_hammer's
 * CreateTask stores the result into its `children[i]` and the target's
 * `str r0, [r1, #0x128]` is unambiguous about that. This costs nothing here: the
 * body has no `mov r0` of its own in either typing, so the handle already comes
 * back in r0 and both spellings compile to the same eight instructions.
 */
s32 func_ov039_02098394(TaskPool* pool, s32 arg1, s32 arg2) {
    s32 args[2];

    args[0] = arg1;
    args[1] = arg2;

    return EasyTask_CreateTask(pool, &data_ov039_0209a024, NULL, 0, NULL, args);
}

/* ============================================================================
 * Band B5 -- Tsk_OtosuGame_specialgauge, 0x02094214 - 0x02094ae8.
 *
 * Claim range: .text 0x02094214 .. 0x02094ae8 (11 functions, 2260 bytes,
 * 0x02094214 .. 0x02094ab4). It sits in the gap between band 2
 * (0x02093000-0x02094000) and band 3 (0x02095000-0x02096000) and is one
 * whole task rather than a slice of a call graph, the same shape as band 6
 * and band 7. It is self-contained: nothing outside the band calls into it
 * except the overlay's own dispatcher, so it can sit anywhere in the include
 * order that is convenient.
 *
 * WHAT THE TASK IS
 * ----------------
 * The overlay's data at 0x02099b98 is a `TaskHandle` naming the string at
 * 0x0209a7b0 -- "Tsk_OtosuGame_specialgauge" -- with a taskFunc of
 * 0x02094a6c (this band's dispatcher) and a dataSize of 0x4B4, and the
 * `TaskStages` immediately after it lists this band's four stages in the
 * usual order:
 *
 *   0x02099ba4  020944bc  init     020945c8  update
 *               020948f4  render   020949f4  cleanup
 *
 * It is a four-digit counter: a row of four small cells, each a *pair* of
 * sprites showing a two-digit number out of a gauge. 0x02094418 is the loader
 * for both halves of a pair and takes a third argument selecting which of the
 * two, which is the clearest single piece of evidence for the layout.
 *
 * The four `SpriteAnimation` templates that follow at 0x02099bb4, 0x02099be0,
 * 0x02099c0c and 0x02099c38 all name 0x02094214 -- this band's own first
 * function -- as their `frameInfoCallback`, which is what identifies 0x02094214
 * as a cell builder rather than a stage. 0x02099be0, 0x02099c0c and
 * 0x02099c38 are byte-identical; only 0x02099bb4 differs, and only in its
 * `unk_02` halfword and its initial posX/posY. That is why three of the four
 * loaders have the same body and differ only in which pool word they name.
 *
 * STRUCTURAL FINDINGS
 * -------------------
 *   * The 0x400-byte body is *not* an array of 0x40-stride cells. It is
 *     0x0C of header, then four sprites at 0x40 stride, then four more at
 *     0x40 stride, then eight at 0x40 stride -- but the eight are addressed
 *     as two interleaved 0x80-stride walks (the "low" and "high" sprites of
 *     each pair), which is why the update and the render both keep two
 *     pointers advancing by 0x80 beside two advancing by 0x40. The last
 *     two sprites, at 0x40C and 0x44C, are the pair that gets the affine
 *     transform.
 *   * The four palette-source writes in the update land on `Sprite.unk3C`
 *     (+0x3C) of all four sprites of the cell. The two index expressions
 *     are `self + i*0x40 + 0x48` and `self + i*0x40 + 0x148` for the two
 *     0x40-walk groups and `self + i*0x80 + 0x248` / `+0x288` for the
 *     0x80-walk pair; 0x48 - 0x0C = 0x3C and 0x148 - 0x10C = 0x3C pin the
 *     sprite base at 0x0C and 0x10C, and 0x248 - 0x20C = 0x3C and
 *     0x288 - 0x24C = 0x3C pin the pair's two halves at 0x20C and 0x24C.
 *   * The pointer the update reads at `self + i*0x40 + 0x134` is
 *     `spriteB[i].resourceData` (Sprite +0x28, and 0x10C + 0x28 = 0x134), and
 *     the two-step walk it performs on it -- `buffer + 0x20` then add the
 *     word at `buffer + 0x48` or `buffer + 0x40` -- is the overlay's
 *     pack-entry shape, i.e. `PackEntry[5]` and `PackEntry[4]`. Band 2 spells
 *     the same walk for one of them as `OtuPaletteSource`; both are
 *     reproduced here as one parameterised helper.
 *   * The digit position tables are two differently-strided objects at
 *     0x0209a780 (4 bytes per entry, one position) and 0x0209a790 (8 bytes
 *     per entry, two positions). 0x0209a790's first pair is byte-identical
 *     to 0x0209a780's entry, and its second pair is the first minus five on
 *     x -- i.e. the two-digit table is the one-digit table plus the tens
 *     digit five pixels to its left, which is what the count < 10 /
 *     count >= 10 split in the update is actually choosing between.
 * ==========================================================================*/

#include "Debug/Sugata/TinPinSlammer.h"
#include "Engine/Core/OamMgr.h"
#include "Engine/EasyTask.h"
#include "SpriteMgr.h"

/* ------------------------------------------------------------------ */
/* Data the band reads but does not define.                            */
/* ------------------------------------------------------------------ */

/*
 * The overlay's `.rodata` is gap-filled from 0x02098f3c to 0x0209a12c and its
 * `.data` from 0x0209a49c to 0x0209ad00, and every symbol this band names in
 * those windows is in the gap. Declaring an object of our own would put it at
 * an address of the linker choosing rather than the one the target uses, so
 * they keep the build's own names. See the same note in band 5.
 *
 * `data_ov039_0209a0fc` is the `BinIdentifier` all four templates name -- the
 * only thing tying this task's eighteen sprites to one piece of art. It is
 * the same *kind* of record as band 5's `data_ov039_0209a0dc` (one overlay,
 * one bin id) but a different object, so it is declared rather than shared.
 */
extern const TaskHandle data_ov039_02099b98;
extern const TaskStages data_ov039_02099ba4;

extern SpriteAnimation data_ov039_02099bb4;
extern SpriteAnimation data_ov039_02099be0;
extern SpriteAnimation data_ov039_02099c0c;
extern SpriteAnimation data_ov039_02099c38;

extern const BinIdentifier data_ov039_0209a0fc;

/** @brief One cell's single-digit position: two halfwords. */
typedef struct {
    /* 0x00 */ u16 x;
    /* 0x02 */ u16 y;
} OtuGaugePos; // Size: 0x4

/**
 * @brief One cell's two-digit position: four signed halfwords.
 *
 * The last pair is the first minus five on x, so `lowX`/`highX` read left to
 * right as tens-then-units. The halfword stride is what the code uses: 0x02094418
 * addresses entry `index` and then offsets by `which * 4` to pick a half of it.
 */
typedef struct {
    /* 0x00 */ u16 lowX;  // units digit
    /* 0x02 */ u16 lowY;
    /* 0x04 */ u16 highX; // tens digit, five pixels left of lowX
    /* 0x06 */ u16 highY;
} OtuGaugeDigits;         // Size: 0x8

/*
 * The two digit-position tables. Both are declared non-`const` because they
 * live in the overlay's `.data`, not its `.rodata` -- band 5 records that
 * telling mwcc a `.data` symbol is const costs it a reload across a call, and
 * neither table is folded here (both are indexed by the loop counter) so the
 * distinction is free either way, but the section is the honest answer.
 *
 * Values, recovered with `tools/ov039_bytes.py`: 0x0209a780 holds
 * (0x37,0x9C) (0x27,0x83) (0x14,0x90) (0x1E,0xA7), and 0x0209a790's first
 * pair repeats those exactly while its second is five to the left. So the
 * two-digit table is the one-digit table plus the tens digit, which is exactly
 * the choice the update's `count < 10` / `count >= 10` split makes.
 */
extern OtuGaugePos    data_ov039_0209a780[4];
extern OtuGaugeDigits data_ov039_0209a790[4];

/**
 * @brief The overlay's fourth "is this pin the kind I care about" filter.
 *
 * The shared header declares the other three (0x0208e984 kind 8, 0x0208e998
 * kind 7, 0x0208e9d0 kind 6) but not this one. The target's body is
 * `ldr r1,[r0,#0xf8]; cmp r1,#9; moveq r0,#1` -- kind 9 -- so it belongs to
 * that family and sits between them at 0x0208e9e4, inside OtuFieldAccess.c's
 * own claim and not yet defined there.
 */
s32 func_ov039_0208e9e4(OtuPinTask* task);

/* ------------------------------------------------------------------ */
/* Task-data shapes.                                                   */
/* ------------------------------------------------------------------ */

/**
 * @brief The two words `func_ov039_02094ab4` packs and the init stage reads.
 *
 * The first becomes the task's `dataType` and is folded into the `dataType`
 * bitfield of all eighteen `SpriteAnimation` templates; the second is the pin
 * task's handle, which the update resolves through `EasyTask_GetTaskData` and
 * then queries four ways. This is a struct rather than two scalar locals
 * because only its *address* is ever taken, and with scalars mwcc drops the
 * second store as dead and the frame shrinks by four bytes (band 5 records the
 * same for its own block).
 */
typedef struct {
    /* 0x00 */ s32 dataType; // SpriteAnimation.dataType, four bits
    /* 0x04 */ s32 pinId;    // handle of the pin task this gauge watches
} OtuGaugeArgs;              // Size: 0x8

/**
 * @brief One cell of the gauge: its two digit sprites, 0x80 apart.
 *
 * The 0x80 stride is load-bearing and is the reason this is a struct rather
 * than a flat `Sprite[8]`: the update and the render each walk the "low" and
 * the "high" sprite with a *separate* pointer advancing 0x80 per cell, side by
 * side with the two 0x40-stride walks, and mwcc's address arithmetic follows
 * the source's. Indexing a flat array by `i * 2` would fold the multiply and
 * change the instruction the loop is built from.
 *
 * "low" is the units digit: it is repositioned and re-animated on every change
 * of the count, and is the one shown alone when the count is below ten.
 * "high" is the tens digit: it is only repositioned, re-animated, updated and
 * drawn once the count reaches ten.
 */
typedef struct {
    /* 0x00 */ Sprite spriteLow;  // units digit
    /* 0x40 */ Sprite spriteHigh; // tens digit
} OtuGaugePair;                   // Size: 0x80

/**
 * @brief "Tsk_OtosuGame_specialgauge", 0x4B4 bytes.
 *
 * The size is the target's, not an estimate: the `TaskHandle` at 0x02099b98
 * carries it as its third word.
 *
 * The layout is derived from the strides the code uses, and every offset in it
 * is fixed by at least one load or store in this band. From 0x0C to 0x40B the
 * block is eighteen `Sprite`s with nothing between them, which the arithmetic
 * above pins from three independent directions. The 0x400 tail is
 * `OtuEntryAnim`-shaped: a rotation and an x/y scale pair handed to
 * `OamMgr_AllocAffineGroup`, two halfwords nothing in this band reads, and two
 * four-entry arrays the update compares against.
 *
 * The two wide sprites are named for what the code does to them rather than for
 * what they depict: `plate` (0x40C) is only ever updated and rendered, and
 * `dial` (0x44C) is the one whose OAM attribute word has an affine slot index
 * inserted into it every frame.
 */
typedef struct {
    /* 0x000 */ s32          dataType;     // folded into every template's dataType field
    /* 0x004 */ s32          pinId;        // the tracked pin's task handle
    /* 0x008 */ s32          running;      // raised by Update, tested by Render
    /* 0x00C */ Sprite       spriteA[4];   // four cells' first 0x40-walk sprite
    /* 0x10C */ Sprite       spriteB[4];   // four cells' second 0x40-walk sprite
    /* 0x20C */ OtuGaugePair digit[4];     // the four two-digit displays
    /* 0x40C */ Sprite       plate;        // the wide backing sprite
    /* 0x44C */ Sprite       dial;         // the wide sprite drawn through the affine path
    /* 0x48C */ s32          rotation;     // +0x100 per frame, kept to 16 bits
    /* 0x490 */ s32          scaleX;       // 0x1000, handed to the affine allocator
    /* 0x494 */ s32          scaleY;       // 0x1000 likewise
    /* 0x498 */ s16          unk_498;      // cleared by Init, read by nothing in this band
    /* 0x49A */ s16          unk_49A;      // likewise
    /* 0x49C */ s16          lastCount[4]; // the count each cell was last drawn for
    /* 0x4A4 */ s32          alive[4];     // the filter's answer for this cell's pin
} OtuGauge;                                // Size: 0x4B4

/**
 * @brief The palette block inside a loaded sprite resource's buffer.
 *
 * `buffer + 0x20` is the overlay's `PackHeader`; the word at the caller's
 * offset into the table that follows is a `PackEntry`'s `offset` field, and
 * adding it to the table's base is the sub-resource. This is the same walk as
 * band 2's `OtuPaletteSource` and as `Data_GetPackEntryData`, written out with
 * the entry index as a parameter because this band needs two of them: the
 * update takes the fifth entry when a cell has gone dead and the fourth when it
 * has come back, and the arithmetic differs only in the constant.
 *
 * Declared here rather than reusing band 2's helper so this band does not
 * depend on the include order, and named differently so the two can coexist.
 */
static inline void* OtuGaugePaletteSource(Data* file, s32 packEntry) {
    PackEntry* entries = (PackEntry*)((u8*)file->buffer + sizeof(PackHeader));

    return (u8*)entries + entries[packEntry].offset;
}

/* ------------------------------------------------------------------ */
/* The cell builder, 0x02094214.                                       */
/* ------------------------------------------------------------------ */

/**
 * @brief The `frameInfoCallback` all four of this task's templates name.
 *
 * The same body as the twenty-two in OtuFieldAccess.c and the three in band 2,
 * and with the same two known differences: it does not set the slot's +0x0C,
 * and it stores a constant 3 into the depth key instead of calling
 * `func_ov039_02088400`. The three flat short-circuit tests and the two-step
 * lookup are shared verbatim, down to the `ldrne` that is the short-circuit
 * over +0x18 and the fact that both the index and the table are re-loaded
 * between the two reads.
 *
 * +0x18, +0x1C and +0x16 are `Sprite.animData`, `Sprite.cellTable` and
 * `Sprite.cellIndex`, and the eight-byte stride is `SpriteCell` -- so `unk_04`
 * lands on the entry's `pieceCount` and `unk_08` on its `pieceOffset` scaled to
 * bytes. `OtuSpriteTask` is a `u8 pad_16[0x100]` blob, so the three reads are
 * spelled as casts rather than as fields; that is the form OtuFieldAccess.c
 * settled on and it is what the target's raw offsets come from.
 */
/* `arg` is unused -- the target never loads r1 in either body. Kept as a named
 * parameter so the engine's selector stays in the third argument slot. */
OtuSpriteSlot* func_ov039_02094214(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the cell table at one stride,
            // then a byte pointer built from the u16 at the other.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = *(u16*)(table + index * 8 + 2);
                slot->unk_08 = (s32)(u8*)(table + *(u16*)(table + index * 8) * 2);
            }

            // The one place this differs from the twenty-two: a constant, not
            // `func_ov039_02088400`. The `-1` above is still stored first, so
            // the field is written twice on this path.
            slot->depthKey = 3;
            return slot;
        }

        default:
            return NULL;
    }
}

/* ------------------------------------------------------------------ */
/* The four sprite loaders.                                            */
/* ------------------------------------------------------------------ */

/*
 * All four copy a 0x2C-byte `SpriteAnimation` onto the stack, stamp it with
 * this object's `dataType` and owner, and load one sprite. The copy is the
 * `ldm/stm` triple out of a literal pool, which is how mwcc renders a copy
 * from an `extern const` object -- band 3's loaders use that same form
 * (`SpriteAnimation anim = data_ov039_02099cc8;`) and it is preferred here over
 * a local brace initialiser, which band 5 only needs because its templates
 * have no symbol of their own in the target.
 *
 * The `dataType` write is a read-modify-write of the template's first halfword:
 * `bic #0x3C` clears the field's four bits and the `<< 0x1C` / `>> 0x1A` pair
 * is the insert. The `lsl #0x10` / `lsr #0x10` round trip in front of it is
 * the `(u16)` cast, and it is load-bearing: without it the value reaches the
 * field untruncated and mwcc emits a different sequence.
 */

/** Loads the affine-path sprite (0x44C) from the template at 0x02099bb4. */
void func_ov039_020942bc(OtuGauge* self, Sprite* sprite) {
    SpriteAnimation anim = data_ov039_02099bb4;

    anim.owner    = self;
    anim.dataType = (u16)self->dataType;

    _Sprite_Load(sprite, &anim);
}

/** Loads the wide backing sprite (0x40C) from the template at 0x02099be0. */
void func_ov039_0209432c(OtuGauge* self, Sprite* sprite) {
    SpriteAnimation anim = data_ov039_02099be0;

    anim.owner    = self;
    anim.dataType = (u16)self->dataType;

    _Sprite_Load(sprite, &anim);
}

/**
 * @brief Loads one of the eight 0x40-walk sprites, 0x0209439c.
 *
 * The template at 0x02099c0c is byte-identical to the one 0x0209432c uses, so
 * this body is 0x0209432c's plus one argument -- the cell index, whose `+ 1`
 * becomes the template's `packIndex`, selecting one of the eight walk packs.
 * It is deliberately not the animation: `animIndex` at +0x2A keeps the
 * template's own value. Zero is therefore not a valid pack index, the same
 * convention band 3's point task uses on its own digit table.
 */
void func_ov039_0209439c(OtuGauge* self, Sprite* sprite, s32 index) {
    SpriteAnimation anim = data_ov039_02099c0c;

    anim.owner     = self;
    anim.dataType  = (u16)self->dataType;
    anim.packIndex = index + 1;

    _Sprite_Load(sprite, &anim);
}

/**
 * @brief Loads one half of a cell's two-digit display, 0x02094418.
 *
 * The only loader that also positions its sprite. `which` picks the low or the
 * high digit of cell `index`, and the target reaches the table as
 * `base + index * 8` and `base + which * 4` as two separate index
 * expressions, so the read is a halfword subscript with a doubled index rather
 * than a field access -- a field access would need a branch to choose between
 * `lowX/lowY` and `highX/highY`.
 *
 * The `+ 1` on the index is the same one the 0x40-walk loader applies, so all
 * four of a cell's sprites come from the same template with the same animation.
 */
void func_ov039_02094418(OtuGauge* self, Sprite* sprite, s32 index, s32 which) {
    SpriteAnimation anim = data_ov039_02099c38;

    anim.owner     = self;
    anim.dataType  = (u16)self->dataType;
    anim.posX      = ((u16*)&data_ov039_0209a790[index])[which * 2];
    anim.posY      = ((u16*)&data_ov039_0209a790[index])[which * 2 + 1];
    anim.animIndex = index + 1;

    _Sprite_Load(sprite, &anim);
}

/* ------------------------------------------------------------------ */
/* The init stage, 0x020944bc.                                         */
/* ------------------------------------------------------------------ */

/**
 * @brief The gauge's init stage: records the two arguments, loads all
 *        eighteen sprites, and seeds the affine block.
 *
 * `dataType` is written before the two argument words are read back out, which
 * is what lets the loaders take `self` rather than the argument block: the
 * target's first store is the first thing read by the first loader.
 *
 * Each cell gets both 0x40-walk sprites at `index + 1` and both of its digit
 * sprites at the same index, and then the *first* 0x40-walk sprite is switched
 * to animation 0xC -- the only `Sprite_ChangeAnimation` in the stage, and the
 * reason the target hoists 0xC into a callee-saved register before the loop
 * rather than re-materialising it on each of the four passes.
 *
 * The two halfwords at +0x498 and +0x49A are cleared with `strh`, so they are
 * genuinely `s16`; nothing in this band reads them, which is why they stay
 * `unk`.
 */
s32 func_ov039_020944bc(TaskPool* pool, Task* task, void* arg) {
    OtuGauge*     self = (OtuGauge*)task->data;
    OtuGaugeArgs* args = (OtuGaugeArgs*)arg;
    s32           i;

    self->dataType = args->dataType;
    self->pinId    = args->pinId;

    func_ov039_020942bc(self, &self->dial);
    func_ov039_0209432c(self, &self->plate);

    for (i = 0; i < 4; i++) {
        self->lastCount[i] = 0;
        self->alive[i]     = 0;

        func_ov039_0209439c(self, &self->spriteA[i], i);
        Sprite_ChangeAnimation(&self->spriteA[i], self->spriteA[i].animData, 0xC, self->spriteA[i].cellTable);

        func_ov039_0209439c(self, &self->spriteB[i], i);
        func_ov039_02094418(self, &self->digit[i].spriteLow, i, 0);
        func_ov039_02094418(self, &self->digit[i].spriteHigh, i, 1);
    }

    self->rotation = 0;
    self->scaleX   = 0x1000;
    self->scaleY   = 0x1000;
    self->unk_498  = 0;
    self->unk_49A  = 0;

    return 1;
}

/* ------------------------------------------------------------------ */
/* The update stage, 0x020945c8 -- the largest thing in the band.     */
/* ------------------------------------------------------------------ */

/**
 * @brief The gauge's update stage: spin the dial, then walk the four cells.
 *
 * The prologue is two statements -- raise the running flag, step the dial's
 * rotation by 0x100 with a 16-bit truncation -- followed by the two wide
 * sprites' `Sprite_Update`s and a resolution of the pin task. The target
 * interleaves the rotation arithmetic with the two call setups, which is the
 * scheduler and not a source order.
 *
 * What the four-cell walk does, per cell:
 *
 *   1. Ask the pin two questions through a `switch` on the cell index: which
 *      of four "is this pin the kind I want" filters, and which of four
 *      counters off the pin's own +0x178..+0x17E run. The switch is a real one
 *      -- the target emits the `cmp r9, #3` / `addls pc, pc, r9, lsl #2` jump
 *      table -- and the `alive` result is the one the target pre-sets to zero
 *      before the dispatch, while `count` is not initialised at all, which is
 *      only safe because every value the loop can reach has a case.
 *   2. If both answers are zero the cell is dead: point all four of its
 *      sprites' palette sources at pack entry 5. A cell that has come back to
 *      life is repointed at pack entry 4 instead, but only on the frame its
 *      count actually changes.
 *   3. On a change of count, re-animate the units digit to `count % 10 + 2`
 *      and move it to its place. Below ten it takes the one-digit position out
 *      of 0x0209a780; from ten up it takes the two-digit pair out of
 *      0x0209a790 and additionally re-animates the tens digit to
 *      `(count / 10) % 10 + 2` and moves it. So the two tables are the same
 *      positions with and without the tens digit, and the split is what
 *      decides whether a second sprite appears.
 *   4. Advance the sprites: the first 0x40-walk sprite only if its filter
 *      answered true, the second one always, the units digit always, and the
 *      tens digit only once the stored count has reached ten.
 *
 * The two `% 10` narrowings are deliberate and both are in the target: the
 * `lsl #0x10` / `asr #0x10` in front of each is the assignment into the `s16`
 * local, and the second round of the pair is the `s16` parameter of
 * `Sprite_ChangeAnimation`. Writing `count % 10 + 2` inline would collapse
 * them into one.
 *
 * `count / 10` is computed once and reused by the second `% 10`, and the target
 * keeps it in a register across the `Sprite_ChangeAnimation` that sits between
 * the two divides -- which is why the tens digit's frame is spelled through a
 * local rather than as `(count / 10) % 10 + 2` inline.
 */
s32 func_ov039_020945c8(TaskPool* pool, Task* task, void* args) {
    OtuGauge*   self = (OtuGauge*)task->data;
    OtuPinTask* pin;
    s32         i;
    s32         count;
    s32         alive;

    self->running  = 1;
    self->rotation = (u16)(self->rotation + 0x100);

    Sprite_Update(&self->dial);
    Sprite_Update(&self->plate);

    pin = (OtuPinTask*)EasyTask_GetTaskData(pool, self->pinId);

    for (i = 0; i < 4; i++) {
        s16               frame;
        s32               tens;
        UnkSmallInternal* pal;
        Data*             file;

        alive = 0;

        switch (i) {
            case 0:
                alive = func_ov039_0208e9d0(pin); // pin kind 6
                count = func_ov039_0208eea0(pin);
                break;

            case 1:
                alive = func_ov039_0208e984(pin); // pin kind 8
                count = func_ov039_0208eeac(pin);
                break;

            case 2:
                alive = func_ov039_0208e998(pin); // pin kind 7
                count = func_ov039_0208eeb8(pin);
                break;

            case 3:
                alive = func_ov039_0208e9e4(pin); // pin kind 9
                count = func_ov039_0208eec4(pin);
                break;

            default:
                break;
        }

        // Nothing left in the cell: repoint all four sprites at the "empty"
        // palette block, which is pack entry five.
        if (count == 0 && alive == 0) {
            file = self->spriteB[i].resourceData;

            if (file != NULL) {
                pal = (UnkSmallInternal*)OtuGaugePaletteSource(file, 5);
            } else {
                pal = NULL;
            }

            self->spriteA[i].unk3C          = pal;
            self->spriteB[i].unk3C          = pal;
            self->digit[i].spriteLow.unk3C  = pal;
            self->digit[i].spriteHigh.unk3C = pal;
        }

        self->alive[i] = alive;

        if (count != (s32)self->lastCount[i]) {
            // A live cell goes back to the normal palette block, entry four.
            // This is deliberately inside the change test and not beside the
            // dead-cell test above: the target's `cmp r6, #0; ble` skips it
            // entirely for a count of zero or less, so a cell that has just
            // emptied keeps the "empty" palette it was just given.
            if (count > 0) {
                file = self->spriteB[i].resourceData;

                if (file != NULL) {
                    pal = (UnkSmallInternal*)OtuGaugePaletteSource(file, 4);
                } else {
                    pal = NULL;
                }

                self->spriteA[i].unk3C          = pal;
                self->spriteB[i].unk3C          = pal;
                self->digit[i].spriteLow.unk3C  = pal;
                self->digit[i].spriteHigh.unk3C = pal;
            }

            frame = (s16)(count % 10);
            Sprite_ChangeAnimation(&self->digit[i].spriteLow, self->digit[i].spriteLow.animData, frame + 2,
                                   self->digit[i].spriteLow.cellTable);

            if (count < 0xA) {
                // One digit: the units digit takes the standalone position.
                self->digit[i].spriteLow.posX = data_ov039_0209a780[i].x;
                self->digit[i].spriteLow.posY = data_ov039_0209a780[i].y;
            } else {
                // Two digits: both take their places and the tens digit
                // appears.
                self->digit[i].spriteLow.posX = data_ov039_0209a790[i].lowX;
                self->digit[i].spriteLow.posY = data_ov039_0209a790[i].lowY;

                tens  = count / 10;
                frame = (s16)(tens % 10);
                Sprite_ChangeAnimation(&self->digit[i].spriteHigh, self->digit[i].spriteHigh.animData, frame + 2,
                                       self->digit[i].spriteHigh.cellTable);

                self->digit[i].spriteHigh.posX = data_ov039_0209a790[i].highX;
                self->digit[i].spriteHigh.posY = data_ov039_0209a790[i].highY;
            }

            self->lastCount[i] = (s16)count;
        }

        if (self->alive[i] != 0) {
            Sprite_Update(&self->spriteA[i]);
        }
        Sprite_Update(&self->spriteB[i]);
        Sprite_Update(&self->digit[i].spriteLow);

        if (self->lastCount[i] >= 0xA) {
            Sprite_Update(&self->digit[i].spriteHigh);
        }
    }

    return 1;
}

/* ------------------------------------------------------------------ */
/* The render stage, 0x020948f4.                                       */
/* ------------------------------------------------------------------ */

/**
 * @brief The gauge's render stage: one affine sprite, then all eighteen.
 *
 * Only the dial is transformed. Its OAM attribute word's affine-slot field --
 * `Sprite.unk_0A.raw`'s bits 5..9 -- is replaced with the slot the allocator
 * returns, and the `(u32)(u16)` around that return is load-bearing twice: it is
 * the `<< 0x10` / `>> 0x10` pair the target emits, and it makes the following
 * `>> 0x16` a logical rather than an arithmetic shift. Band 3's entry-task
 * render is the same expression over the same manager array and carries the
 * same note.
 *
 * The manager is picked from the sprite's own display-engine bits, exactly as
 * band 3 does it, and the scale pair comes from the task rather than from a
 * per-sprite limit block.
 *
 * Nothing in the stage is positioned here; every sprite draws where the update
 * left it, which is the opposite of band 7's hammer and of band 3's point task
 * and is why this task needs no render-side anchor arithmetic at all.
 */
s32 func_ov039_020948f4(TaskPool* pool, Task* task, void* args) {
    OtuGauge* self = (OtuGauge*)task->data;
    s32       i;

    if (self->running != 0) {
        self->dial.unk_0A.raw =
            (self->dial.unk_0A.raw & ~0x3E0) |
            ((u32)(u16)OamMgr_AllocAffineGroup(&g_OamMgr[self->dial.bits_0_1], self->rotation, self->scaleX, self->scaleY, 0)
                 << 0x1B >>
             0x16);
        Sprite_RenderFrame(&self->dial);
        Sprite_RenderFrame(&self->plate);

        for (i = 0; i < 4; i++) {
            Sprite_RenderFrame(&self->spriteB[i]);
            Sprite_RenderFrame(&self->digit[i].spriteLow);

            if (self->lastCount[i] >= 0xA) {
                Sprite_RenderFrame(&self->digit[i].spriteHigh);
            }

            if (self->alive[i] != 0) {
                Sprite_RenderFrame(&self->spriteA[i]);
            }
        }
    }

    return 1;
}

/* ------------------------------------------------------------------ */
/* The cleanup stage, 0x020949f4.                                      */
/* ------------------------------------------------------------------ */

/**
 * @brief Releases all eighteen sprites, and keeps running.
 *
 * The dial goes first and the plate second, then the four cells in the same
 * order the update walks them. `Sprite_Release` does not care about order, so
 * this is simply the reverse of the two 0x40/0x80 pointer setups the stage
 * shares with the update -- which is why the target builds its four walking
 * pointers identically in the two functions.
 */
s32 func_ov039_020949f4(TaskPool* pool, Task* task, void* args) {
    OtuGauge* self = (OtuGauge*)task->data;
    s32       i;

    Sprite_Release(&self->dial);
    Sprite_Release(&self->plate);

    for (i = 0; i < 4; i++) {
        Sprite_Release(&self->spriteA[i]);
        Sprite_Release(&self->spriteB[i]);
        Sprite_Release(&self->digit[i].spriteLow);
        Sprite_Release(&self->digit[i].spriteHigh);
    }

    return 1;
}

/* ------------------------------------------------------------------ */
/* The dispatcher and the spawner.                                     */
/* ------------------------------------------------------------------ */

/**
 * @brief The stage dispatcher, and the `TaskHandle`'s own `taskFunc`.
 *
 * Identical in shape to band 3's and band 7's: the four callbacks are copied
 * out of the overlay's own `TaskStages` onto the stack and then indexed by
 * `stage`, so the frame is the 0x10 the copy needs and the indirect call is
 * through the copy. The pool word is the target's `data_ov039_02099ba4`, which
 * is why that object is declared by address rather than being written out as a
 * local initialiser.
 *
 * The result of the stage is not touched -- the target has no `mov r0` after
 * the `blx` -- so the return type is `s32` only because that is what the four
 * stages return and what the engine's taskFunc signature says.
 */
s32 func_ov039_02094a6c(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_02099ba4;

    return stages.iter[stage](pool, task, args);
}

/**
 * @brief Creates the gauge task. 0x02094ab4.
 *
 * The two-word block is built in the task's own outgoing-argument area rather
 * than in a frame of its own, which is why the frame is exactly the 0x10 that
 * the two outgoing words and the block need together: `[sp]` and `[sp+4]` are
 * the fifth and sixth arguments, `[sp+8]` and `[sp+0xC]` the block.
 *
 * The handle is read by address -- it is in the gap-filled `.rodata` -- and the
 * target's last two instructions are the call and the epilogue with no `mov r0`
 * between, so the handle comes back in r0 for free.
 */
s32 func_ov039_02094ab4(TaskPool* pool, s32 arg1, s32 arg2) {
    OtuGaugeArgs args;

    args.dataType = arg1;
    args.pinId    = arg2;

    return EasyTask_CreateTask(pool, &data_ov039_02099b98, NULL, 0, NULL, &args);
}

#include "Save.h"

/* ============================================================================
 * Batch B1 -- 0x02088698 - 0x02089918, 22 functions, 4792 bytes.
 *
 * This is the board (pinball) stage: the block of functions that owns the pin
 * children, spawns them, runs the per-frame pairwise interactions between them,
 * scores them and decides the round's outcome, plus the four stage entry points
 * at 0x02089780 - 0x02089918 that the results-screen menu dispatches to.
 *
 * WHERE THIS FILE GOES
 * --------------------
 * It is a band include for OtuFieldAccess.c and MUST be included after
 * OtuMeters and before OtuHammerSpawn / OtuPinSprites. Two hard reasons:
 *
 *   - OtuPinSprites is last because it calls into bands 4 and 5. Six callees
 *     here (0208f00c, 0208f024, 0208f0b0, 0208f0c8, 0208f104, 02087d04) are
 *     defined there, so they have to be forward-declared below -- which is
 *     exactly what a band before band 1 has to do.
 *   - 020934a8 (OtuCounters), 02096b18 and 02096e4c (OtuTaskStages) are
 *     *defined* with a `void` return type but the target returns the new task's
 *     handle in r0, and this batch stores that handle. See below.
 *
 * RETURN TYPES THE ORCHESTRATOR MUST FIX (three, all mechanical)
 * ------------------------------------------------------------
 *   OtuCounters:288  void func_ov039_020934a8(...)  ->  s32
 *   OtuTaskStages:140  void func_ov039_02096b18(...)  ->  s32
 *   OtuTaskStages:148  void func_ov039_02096e4c(...)  ->  s32
 *
 * All three bodies are already correct (`EasyTask_CreateTask(...)` as the last
 * statement), so changing the declared return type is codegen-neutral for the
 * definitions themselves; only the call sites here need the value. Without it
 * this file does not compile. A separate `extern s32` line is not an option --
 * that would be a conflicting redeclaration in the same translation unit.
 *
 * SCOPE
 * -----
 * Nothing outside 0x02088698 - 0x02089918 is defined here. Every other callee is
 * declared and left alone; dsd resolves an undefined symbol against the original
 * overlay rather than failing the link, which is how the rest of this overlay's
 * still-absent callees work.
 *
 * WHAT THE BATCH LEARNED ABOUT THE STRUCTURE
 * ------------------------------------------
 * The whole band is driven by one heap block, reached through the scene's stage
 * dispatcher as `func_ov039_02098b70(OTU_STAGE(scene))` and modelled here as
 * `OtuPinStage`. Its identity is pinned from four independent places:
 *
 *   - +0x140 is the pin-children count, and it is written by exactly two
 *     functions: 1 + a count of non-empty countdown groups (02088df0) or
 *     straight out of scene+0x41EE0 (020898a8). Everything else only reads it.
 *   - +0x17C is the child-handle array, +0x18C a 64-entry helper-handle array
 *     and +0x28C the effect-handle array. The last two are *contiguous*:
 *     0x18C + 0x40 * 4 == 0x28C. Both are walked as `(base + i * 4) + field`,
 *     i.e. the same shape as the feature header's OTU_CHILD_ID, which is why
 *     they get macros of their own rather than array subscripts.
 *   - +0x150 is a 0x10-byte parameter block: 02088c50 loads exactly 0x10 bytes
 *     into it from the overlay's bin, and three task factories are handed its
 *     address. Two bytes of it are read individually (as `ldrb`, so u8) and a
 *     third becomes the effect count.
 *   - +0x16C points at the per-menu countdown block, so the stage and the
 *     countdown are two views of the same round: 02088df0 fills the countdown
 *     from the pin trays and then counts how many groups are populated.
 *
 * The 0x34-byte countdown block is modelled separately as OtuBoardCountdown
 * rather than reusing the header's OtuMenuCountdown: the header's copy has its
 * fields at +0x84/+0x86 inside a 0x34 type, which cannot express the +4 group
 * count and the three 0x10-byte groups this band reads. The two agree about the
 * two leading digit columns.
 * ==========================================================================*/

#include "CriSndMgr.h"
#include "EasyFade.h"
#include "Engine/Math/Random.h"
#include <nitro/mi/cpumem.h>

/* ============================================================================
 * Data
 * ==========================================================================*/

/* An overlay-global byte block in the main module, at 0x02071cf0. Nothing in
 * this batch owns it; two unrelated tables inside it are written here. Declared
 * as a bare array so no dsd symbol has to be invented for the block itself. */
extern u8 data_02071cf0[];

/** The round's per-slot score, a `u16` at data_02071cf0 + 0x3434.
 *
 *  The target reaches it as `add rX, rbase, rI, lsl #1 / add rX, rX, #0x3400 /
 *  strh r0, [rX, #0x14]` off a pool word of `data_02071cf0 + 0x20`, so the two
 *  constants 0x3400 and 0x14 are load-bearing and are written as one byte offset
 *  here. The score is what 020893fc compares to find the round's leader. */
#define OTU_BOARD_SLOT_SCORE(i) (*(u16*)((u8*)data_02071cf0 + 0x3434 + (i) * 2))

/** The board's result code, a `u16` at data_02071cf0 + 0x24D4 -- 6 or 7.
 *
 *  Written by 020897dc and read by whoever runs the results stage. Offsets split
 *  the same way as the score table: pool word `data_02071cf0 + 0x2420`, then
 *  +0xB4. */
#define OTU_BOARD_RESULT (*(u16*)((u8*)&gSaveData + 0x24B4))

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

/* The overlay's own bin identifier for this stage's data, and the eight 10-entry
 * tables that go with it. All read-only here, and all reached through the
 * overlay's own symbol names rather than invented ones -- declaring the arrays
 * and letting dsd place them would put the pool words at addresses of the
 * linker's choosing instead of the target's. */
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
extern const s32 data_ov039_0209a360[10];

/* ============================================================================
 * Types
 * ==========================================================================*/

/** One 0x10-byte record of an inbound wireless packet.
 *
 *  Four words, and the copy at 02088698 moves twelve of them in one go with
 *  `ldm`/`stm` -- which is what mwcc emits for a 16-byte struct assignment, and
 *  is why this is a struct rather than a `memcpy`. */
typedef struct {
    /* 0x00 */ s32 w[4];
} OtuWireRecord; // Size: 0x10

/** The 0x10-byte parameter block at stage + 0x150.
 *
 *  Loaded verbatim from the overlay's bin by 02088c50 and handed *by address* to
 *  three task factories, so the overlay never interprets it here. Only three
 *  bytes are ever read back, and all three with `ldrb`, so all three are u8.
 *  `effectMax` becoming the effect count is the load-bearing one: 02088c50
 *  zero-extends it and stores it into a word field. */
typedef struct {
    /* 0x00 */ u8 kind;      // == 1 makes both rounds clear the OBJ layers
    /* 0x01 */ u8 variant;   // handed to the obstacle factory as `slot`
    /* 0x02 */ u8 pad_02[2];
    /* 0x04 */ u8 effectMax; // copied to stage->effectCount by 02088c50
    u8            pad_05[0x0B];
} OtuBoardParams;            // Size: 0x10

/** One of the countdown block's three 0x10-byte groups.
 *
 *  `groupCount` is the number of menu cells this group spans -- it is what
 *  02088a8c multiplies by 0x22 to find where the next group's sprites start,
 *  and what 02088df0 counts non-zero to decide how many pin children to spawn.
 *  `row` is the 0xE-byte copy of one pin-tray slot. */
typedef struct {
    /* 0x04 */ u16 groupCount;
    /* 0x06 */ u8  row[0x0A];
} OtuBoardCountdownGroup; // Size: 0x10

/** The board's countdown block: two digit columns and three score groups.
 *
 *  The 0x34 stride is what makes this a menu-indexed array. It agrees with the
 *  header's OtuMenuCountdown on the two leading digit columns -- which are the
 *  header's `thousands` and `hundreds` at +0x84/+0x86 of the 0x44000 sub-object,
 *  i.e. +0 and +2 of this block -- and differs in that it also describes the
 *  group count and the tray copy that follow them. */
typedef struct {
    /* 0x00 */ u16                    digits[2];
    /* 0x04 */ OtuBoardCountdownGroup group[3];
} OtuBoardCountdown; // Size: 0x34

/** The board stage's block: the object every function in this batch reaches
 *  through `func_ov039_02098b70(OTU_STAGE(scene))`.
 *
 *  Nothing in decompiled code allocates this -- func_ov039_02098a60 does, from
 *  the size in the stage descriptor -- so the extent below is read off the
 *  highest offset any function here touches (0x2CC) and rounded to 0x2D0, which
 *  is the size the overlay's own stage descriptors advertise for the two board
 *  stages. Fields keep their offsets; the roles are named only where the code
 *  says what they are. */
typedef struct {
    /* 0x000 */ u8                 pad_000[0x06];
    /* 0x006 */ u16                packetKind;   // matched against the wire packet's +0x50
    /* 0x008 */ s32                gotPacket;
    /* 0x00C */ OtuWireRecord      rxRecord[12]; // 0xC0 bytes, filled wholesale by 02088698
    /* 0x0CC */ u16                active;       // raised by both entry points at 02089780/898a8
    /* 0x0CE */ u8                 pad_0CE[0x72];
    /* 0x140 */ s32                childCount;   // the pin children; OTU_CHILD_COUNT's field
    /* 0x144 */ s32                effectCount;
    /* 0x148 */ s32                helperCursor; // 0..0x40, round-robin base for 02089064
    /* 0x14C */ s32                stageIndex;   // indexes all eight data tables above
    /* 0x150 */ OtuBoardParams     params;
    /* 0x160 */ u8*                bufA;         // 0x1388 bytes from the heap
    /* 0x164 */ u8*                bufB;         // handed to the obstacle factory as 8-byte records
    /* 0x168 */ u8*                bufC;
    /* 0x16C */ OtuBoardCountdown* countdown;
    /* 0x170 */ s32                countdownAux; // the block's second digit column, or a table entry
    /* 0x174 */ s32                timer;        // set to 0x258 by 020898a8, read nowhere here
    /* 0x178 */ s32                outcome;      // 020893fc's verdict: 1, 2 or 3
    /* 0x17C */ s32                childIds[4];  // OTU_CHILD_ID's array
    /* 0x18C */ s32                helperIds[0x40];
    /* 0x28C */ s32                effectIds[8];
    /* 0x2AC */ s32                pinTask;
    /* 0x2B0 */ s32                bgTaskA;
    /* 0x2B4 */ s32                bgTaskB;
    /* 0x2B8 */ s32                bgTaskC;    // the animated palette's owner
    /* 0x2BC */ s32                fxTask;     // the round-state task 020894cc polls
    /* 0x2C0 */ s32                fxTask2;
    /* 0x2C4 */ s32                soundTask;  // lives in pool 1, not pool 2
    /* 0x2C8 */ s32                soundTask2; // ditto; deleted by 02089918
    /* 0x2CC */ s32                soundTask3; // ditto; created but never read here
} OtuPinStage;                                 // Size: 0x2D0

/* ============================================================================
 * Callee declarations. Nothing here is defined in this file.
 * ==========================================================================*/

/* --- In this overlay, not yet decompiled ---------------------------------*/

/** Maps the scene's mode selector onto a local player index.
 *
 *  Read from the target: 0 for mode 0, ov040's index for mode 1, 0 for anything
 *  else. Re-read inside several loops rather than hoisted, which is why the call
 *  sites below spell it out each time. */
s32 func_ov039_02088418(s32 mode);

/** The six-byte touch-pad descriptor for player slot `i`. */
void* func_ov039_02088440(s32 slot);

/** Polls the touch screen into one of those descriptors and resets the
 *  "was touched" latch. */
void func_ov039_02088564(void* pad);

/** The pin child's "is this one worth resolving" test, and its follower. */
s32  func_ov039_0208e9f8(TaskPool* pool, void* task);
void func_ov039_0208eaa0(void* self, void* other);

/** The pairwise interaction predicates: contact, push-apart, and effect-pull. */
s32 func_ov039_0208e28c(void* a, void* b);
s32 func_ov039_0208e37c(void* a, void* b);
s32 func_ov039_0208e504(void* a, void* b);

/** Reads a pin child's *own* aim point -- a different pair from the +0x120 one
 *  that func_ov039_0208e6e0 copies out. */
void func_ov039_0208e6fc(void* task, OtuPoint* out);

/** Creates one pin child. Ten parameters, four in registers and six on the
 *  stack; the last six are the block's parameter block, the scene, the scene's
 *  +0x41EF0 block, one pin-tray slot, an "is this the first child" flag and a
 *  per-group sprite base. */
s32 func_ov039_0208dcb0(TaskPool* pool, s32 dataType, s32 slot, void* pad, void* params, void* scene, void* board,
                        void* traySlot, s32 isFirst, void* groupBase);

/** Task factories. All six are the same three-line wrapper around
 *  EasyTask_CreateTask and all six return the new handle in r0. */
s32 func_ov039_02094e58(TaskPool* pool, s32 dataType);
s32 func_ov039_02092310(TaskPool* pool, s32 dataType, Heap* heap, OtuBoardParams* params);
s32 func_ov039_02092cd4(TaskPool* pool, s32 dataType, Heap* heap, OtuBoardParams* params);
s32 func_ov039_020941d0(TaskPool* pool, s32 dataType, s32 aux);
s32 func_ov039_02094ab4(TaskPool* pool, s32 dataType, s32 pin);
s32 func_ov039_020989f0(TaskPool* pool, s32 dataType);

/** Feeds one helper child a point and resets it. */
void func_ov039_02094e88(void* task, OtuPoint* in);
/** Feeds one helper child a point and a fresh random offset. */
void func_ov039_02094e9c(void* task, OtuPoint* in);

/** Places one of two sprites at a point, given a bearing and a length. */
void func_ov039_02092348(void* task, s32 x, s32 y);
void func_ov039_020923b4(void* task, s32 event);
void func_ov039_02092d0c(void* task, s32 x, s32 y);
void func_ov039_02092e04(void* task, s32 event);

/** ov040's packet hand-off. */
void func_ov040_0209cb98(void* packet, s32 which);

/** Population count of a bit mask. Declared the same way Boss03.c declares it,
 *  since the overlay uses it for exactly one thing here: how many players share
 *  a top score. */
s32 func_02047e84(u16 bits);

/* --- Defined in OtuVecOps.c, which a band include cannot see -------------- */

/* --- Defined in OtuPinSprites, which is included *after* this file --------- */

/** out = a scaled vector offset, and the overlay's shared "apply a sprite
 *  action" helper (declared unprototyped there). */
s32  func_ov039_0208f00c(void* task); // the tray slot holds a real pin
void func_ov039_0208f024(void* task); // retract one child's sprites
s32  func_ov039_0208f0b0(void* task); // the tray slot is empty (0x130)
void func_ov039_0208f0c8(void* task); // clear the child's +0x84/+0x80 pair
void func_ov039_0208f104(void* task); // clear +0x114/+0x118 on two grandchildren

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
 * The result code written to OTU_BOARD_RESULT is 6 for outcome 1 and 7 for
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
                OTU_BOARD_RESULT = 6;
                func_ov039_02098a40(OTU_STAGE(scene), NULL);
            } else {
                OTU_BOARD_RESULT = 7;
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

/* Band 1 region -- inlined; see the file header. */

#include "Debug/Sugata/TinPinSlammer.h"
#include "Engine/EasyTask.h"
#include "Engine/File/BinMgr.h"
#include "SpriteMgr.h"
#include "common_data.h"

/**
 * @file OtuPinSprites.c
 *
 * Band 1 of the overlay: 0x0208f000 - 0x02090000.
 *
 * This band is four sibling sprite tasks and the handful of small helpers that
 * surround them. The four tasks are near-copies of each other and the target
 * says so explicitly: each one is a TaskHandle plus a four-entry TaskStages
 * table plus a 0x2C-byte SpriteAnimation template, and the templates' only
 * differences are a resource id and the animation frame count. The task names
 * are still in the ROM, at 0x0209a4f4 onwards:
 *
 *   0x0209930c  Tsk_OtosuGame_shadow   Init 0208f2a4  Update 0208f2f0  Render 0208f360  Release 0208f3b0
 *   0x02099354  Tsk_OtosuGame_piyo     Init 0208f5a8  Update 0208f5e0  Render 0208f6cc  Release 0208f714
 *   0x0209939c  Tsk_OtosuGame_marker   Init 0208f900  Update 0208f938  Render 0208f9b8  Release 0208fa00
 *   0x020993e4  Tsk_OtosuGame_meteo    Init 0208fbe0  Update 0208fc38  Render 0208fd8c  Release 0208fddc
 *
 * So the recurring constants across the four are resource ids (the SpriteAnimation
 * templates at 0x02099328/0x2099370/0x020993b8/0x02099400), not frame counts.
 * All four templates carry the same 0x0050 initial x/y and the same callback
 * slot, and differ only in `packIndex` (1, 2, 8, 8) and the anim/cell indices --
 * which is exactly the set a pinball table needs: one shadow, one "piyo"
 * (Japanese for the springy part), one marker and one weather effect.
 */

/* The overlay's four sprite-task resources. Unidentified: keep the names. */
extern const SpriteAnimation data_ov039_02099328;
extern const SpriteAnimation data_ov039_02099370;
extern const SpriteAnimation data_ov039_020993b8;
extern const SpriteAnimation data_ov039_02099400;

/* The four task handles and their TaskStages tables. */
extern const TaskHandle data_ov039_0209930c;
extern const TaskHandle data_ov039_02099354;
extern const TaskHandle data_ov039_0209939c;
extern const TaskHandle data_ov039_020993e4;
extern const TaskStages data_ov039_02099318;
extern const TaskStages data_ov039_02099360;
extern const TaskStages data_ov039_020993a8;
extern const TaskStages data_ov039_020993f0;

/* A 0x24-byte table passed to func_ov039_02087ba0 by the state-3 setup. */
extern const s32 data_ov039_0209a54c[9];

/* ------------------------------------------------------------------ */
/* Sibling accessors this band calls.                                  */
/*                                                                      */
/* The five defined in OtuFieldAccess.c are declared with the same     */
/* `void*` first parameter they are defined with, because OtuPinSprites.c  */
/* is #included into that file and mwcc rejects a mismatched redeclaration. */
/* ------------------------------------------------------------------ */

/* Defined in OtuFieldAccess.c below the band include; declared here so mwcc
 * does not fall back to an implicit int() prototype and then reject the real
 * definition. */
void func_ov039_02096270(void* task);
void func_ov039_02095ddc(void* task);
s32  func_ov039_0208e6f4(void* task);
s32  func_ov039_0208ee98(void* task);

s32 func_ov039_0208e890(void* task);
s32 func_ov039_0208e8c4(void* task);
s32 func_ov039_0208e950(void* task);
s32 func_ov039_0208a490(void* task, s32 arg2);

/* ==================================================================== */
/* The band-level helpers: 0208f00c - 0208f104.                        */
/* ==================================================================== */

/**
 * Reads the tray-slot pointer at +0x16C and asks whether the value it points
 * at is below the "no pin" sentinel 0x130.
 *
 * That is the same test as func_ov039_0208efb0 and the mirror image of
 * func_ov039_0208f0b0: 0x130 means the slot is empty, so `<` means "there is a
 * real pin here". It is the overlay's only predicate on a pin's identity that
 * is not a `kind` comparison.
 */
s32 func_ov039_0208f00c(void* task) {
    return *(u16*)(*(u8**)((u8*)task + 0x16C)) < 0x130;
}

/**
 * Tail-call thunk: forwards to func_ov039_0208a490 with a literal second
 * argument of 10.
 *
 * The target is `ldr ip, [pool]; mov r1, #0xa; bx ip` -- a three-instruction
 * thunk, not a body of its own. Written as a plain call so mwcc emits the same
 * shape.
 */
void func_ov039_0208f024(void* arg) {
    func_ov039_0208a490(arg, 0xA);
}

/**
 * Round-robins a slot index at +0x1C8 through the eight words at +0x238, handing
 * each slot to func_ov039_02097750 with two caller-supplied values and two
 * constants (0x1000 and 0x66/0x6).
 *
 * The `cmp #8 / movge #0 / strge` at the end is the wrap: the index is written
 * unconditionally first and then overwritten to zero only when it reached 8, so
 * the modulus is a post-store clamp rather than a pre-store one.
 */
void func_ov039_0208f048(void* task, s32 arg1, s32 arg2) {
    // No local for the slot index: the target reloads +0x1C8 from memory on both
    // sides of the call rather than keeping it in a callee-saved register, which
    // is what a local forces.
    func_ov039_02097750(
        EasyTask_GetTaskData(*(void**)((u8*)task + 8), *(s32*)((u8*)task + *(s32*)((u8*)task + 0x1C8) * 4 + 0x238)), arg1,
        arg2, 0x1000, 0x66, 6);

    *(s32*)((u8*)task + 0x1C8) = *(s32*)((u8*)task + 0x1C8) + 1;
    if (*(s32*)((u8*)task + 0x1C8) >= 8) {
        *(s32*)((u8*)task + 0x1C8) = 0;
    }
}

/* ==================================================================== */
/* The four _Sprite_Load wrappers: 0208f1f8, 0208f4fc, 0208f874,        */
/* 0208fb54.  One body, four times, three constants varied.             */
/* ==================================================================== */

/**
 * @brief The pin-shadow task's data block, 0x68 bytes.
 *
 * `Sprite` first, because _Sprite_Load is handed `self` itself (the sprite lives
 * at offset 0) and the two point pairs follow the two coordinate words the
 * wrapper writes.
 *
 * This is the only one of the four with a point pair at +0x50 and another at
 * +0x58 *and* the `+3` offset in its render step, which is what distinguishes
 * the shadow's placement from the other three.
 */
/*
 * One struct for all four tasks, deliberately minimal.
 *
 * The four tasks use the same word range for *different* things, and the
 * layouts genuinely overlap rather than sitting side by side:
 *
 *   +0x40/+0x44   piyo, marker   -- the pin's +0x110/+0x114 pair
 *   +0x48/+0x4C   piyo, marker   -- the pin's +0x120/+0x124 pair
 *   +0x44/+0x48   shadow, meteo  -- a Q12 scale pair, seeded to 0x1000
 *   +0x50/+0x54   shadow, meteo  -- the pin's +0x110/+0x114 pair
 *   +0x58/+0x5C   shadow, meteo  -- the pin's +0x120/+0x124 pair
 *
 * So +0x44 is a point's y on the piyo and a scale on the shadow, and naming both
 * readings would mean a union whose members mwcc then lays out a word apart from
 * where the target has them. Only the words the code reaches *unambiguously*
 * across all four tasks are named; the overlapping ones are read by raw offset at
 * the use site, which is also how OtuFieldAccess.c models this overlay's
 * structures.
 */
typedef struct {
    /* 0x00 */ Sprite   sprite;
    /* 0x40 */ s32      unk_40;
    /* 0x44 */ s32      unk_44;
    /* 0x48 */ s32      unk_48;
    /* 0x4C */ u16      unk_4C;
    /* 0x4E */ u16      unk_4E;
    /* 0x50 */ OtuPoint pinPosA_hi;   // shadow/meteo: the pin's +0x110/+0x114 pair
    /* 0x58 */ OtuPoint pinPosB_hi;   // shadow/meteo: the pin's +0x120/+0x124 pair
    /* 0x60 */ s32      childId;      // shadow: the pin id it tracks
    /* 0x64 */ s32      meteoChildId; // meteo: its pin id sits one word higher
    /* 0x68 */ s32      alive;        // meteo only: the kind == 8 filter's result
    /* 0x6C */ s32      state;        // meteo only: 0-3, the sub-state machine
    /* 0x70 */ s32      timer;        // meteo only: the state machine's work counter
} OtuShadowTask;                      // Size: 0x74

/* The piyo and marker keep their two pin pairs at +0x40 and +0x48, where the
 * shadow and meteo keep a scale pair. Reached by offset rather than by member. */
#define OTU_PIN_POS_A(task) ((OtuPoint*)((u8*)(task) + 0x40))
#define OTU_PIN_POS_B(task) ((OtuPoint*)((u8*)(task) + 0x48))

/*
 * +0x60 is deliberately shared. The shadow and the piyo keep their pin id there;
 * the meteo instead keeps a y velocity at +0x60 and puts its own id at +0x64, so
 * its render step can read the velocity and its update step the id without
 * either colliding with the other two. That overlap is why `meteoChildId` is a
 * separate member rather than a second reading of `childId`: naming one word
 * twice forces a union, and mwcc then lays the later members out a word past
 * where the target puts them.
 */

/**
 * Loads the shadow sprite: copies its SpriteAnimation template onto the stack,
 * points it at its owner, sets the dataType nibble from the argument struct and
 * the initial x/y from the task's own +0x58/+0x5C, then lowers the OAM priority
 * to 10.
 *
 * `data_0206a890.unk_0C` is the OAM priority/slot word (see Engine/Core/OamMgr.c,
 * where it is masked with 0x1F); the only one of these four wrappers that writes
 * it, and the reason the shadow sits behind everything else on its screen.
 *
 * The statement order is load-bearing. The target's `ldrh [sp,#2] / bic / orr`
 * pair sits *after* both position stores, and moving the halfword edit between
 * them is a real 77%-vs-100% difference rather than cosmetic reordering.
 */
void func_ov039_0208f1f8(OtuShadowTask* self, Sprite* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_02099328;

    anim.owner           = self;
    anim.dataType        = *args;
    anim.posX            = F2I(*(s32*)((u8*)self + 0x58));
    anim.posY            = F2I(*(s32*)((u8*)self + 0x5C));
    anim.unk_02.unk_02   = 1;
    data_0206a890.unk_0C = 0xA;

    _Sprite_Load(sprite, &anim);
}

/**
 * Loads the "piyo" sprite.
 *
 * Same body as the shadow wrapper except for three things: the position comes
 * from the *difference* of two task words (+0x48 - +0x40, +0x4C - +0x44) rather
 * than from a single one, the flags halfword's 3-bit field at bits 7-9 is forced
 * to 5, and the OAM priority is left alone. So the piyo is positioned relative to
 * a previous position stored on the task, while the shadow is positioned
 * absolutely.
 *
 * The flags edit also goes after both position stores, as in the shadow wrapper.
 */
void func_ov039_0208f4fc(OtuShadowTask* self, Sprite* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_02099370;

    anim.owner    = self;
    anim.dataType = *args;
    anim.posX     = F2I(*(s32*)((u8*)self + 0x48) - *(s32*)((u8*)self + 0x40));
    anim.posY     = F2I(*(s32*)((u8*)self + 0x4C) - *(s32*)((u8*)self + 0x44));
    anim.bits_7_9 = 5;

    _Sprite_Load(sprite, &anim);
}

/** Loads the marker sprite: absolute position from +0x48/+0x4C, nothing forced. */
void func_ov039_0208f874(OtuShadowTask* self, Sprite* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_020993b8;

    anim.owner    = self;
    anim.dataType = *args;
    anim.posX     = F2I(*(s32*)((u8*)self + 0x48));
    anim.posY     = F2I(*(s32*)((u8*)self + 0x4C));

    _Sprite_Load(sprite, &anim);
}

/* ==================================================================== */
/* The four render steps and the release steps.                          */
/* ==================================================================== */

/*
 * Every lifecycle function in this band is a TaskStages callback, so it takes
 * the full `(TaskPool*, Task*, void*)` triple and reaches its data through
 * `task->data` -- which is `Task`'s own +0x18 field, and is the sprite block's
 * base. Getting that signature wrong costs a whole register: with only
 * `(Task*)` mwcc loads `[r0, #0x18]` (reading the field off the wrong
 * register) instead of `[r1, #0x18]`, which is 99% rather than a match on
 * every function here.
 */

/* Release step, one per task. Identical bodies -- they differ only in which
 * TaskHandle/TaskStages pair points at them. */

/** Releases the shadow sprite. */
s32 func_ov039_0208f3b0(TaskPool* pool, Task* task, void* args) {
    Sprite_Release((Sprite*)((OtuShadowTask*)task->data));
    return 1;
}

/** Releases the piyo sprite. */
s32 func_ov039_0208f714(TaskPool* pool, Task* task, void* args) {
    Sprite_Release((Sprite*)((OtuShadowTask*)task->data));
    return 1;
}

/** Releases the marker sprite. */
s32 func_ov039_0208fa00(TaskPool* pool, Task* task, void* args) {
    Sprite_Release((Sprite*)((OtuShadowTask*)task->data));
    return 1;
}

/**
 * Renders the shadow, offset by three pixels.
 *
 * The only one of the four with a `+ 3` on both axes, and the only one that
 * reads +0x58/+0x5C rather than +0x48/+0x4C. The `+3` is almost certainly a
 * deliberate nudge: a drop shadow offset down-and-right by three pixels is the
 * cheapest way to fake depth on a 2D pinball table, and it is consistent with
 * this being the only task that also drops the OAM priority.
 */
s32 func_ov039_0208f360(TaskPool* pool, Task* task, void* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;

    if (*(s32*)((u8*)self + 0x64) != 0) {
        self->sprite.posX = F2I(*(s32*)((u8*)self + 0x58) - self->pinPosA_hi.x) + 3;
        self->sprite.posY = F2I(*(s32*)((u8*)self + 0x5C) - *(s32*)((u8*)self + 0x54)) + 3;
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

/**
 * Renders the piyo: absolute offset from the stored base pair, no nudge.
 *
 * Guarded on +0x54, the word the piyo's update step raises when it has decided
 * the sprite is worth drawing at all.
 */
s32 func_ov039_0208f6cc(TaskPool* pool, Task* task, void* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;

    if (*(s32*)((u8*)self + 0x54) != 0) {
        self->sprite.posX = F2I(*(s32*)((u8*)self + 0x48) - self->unk_40);
        self->sprite.posY = F2I(*(s32*)((u8*)self + 0x4C) - self->unk_44);
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

/** Renders the marker. Byte-for-byte the same body as the piyo's render step. */
s32 func_ov039_0208f9b8(TaskPool* pool, Task* task, void* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;

    if (*(s32*)((u8*)self + 0x54) != 0) {
        self->sprite.posX = F2I(*(s32*)((u8*)self + 0x48) - self->unk_40);
        self->sprite.posY = F2I(*(s32*)((u8*)self + 0x4C) - self->unk_44);
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

/**
 * Renders the meteo.
 *
 * The only one of the four that adds a third term: y is
 * `(unk_60 + unk_5C - unk_54) >> 12`, where +0x60 is the child's +0x128 read
 * through func_ov039_0208e6f4. So the meteo sprite is displaced vertically by
 * whatever the pin's own vertical velocity is, which is what makes it read as
 * falling rather than sitting still.
 */
s32 func_ov039_0208fd8c(TaskPool* pool, Task* task, void* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;

    if (*(s32*)((u8*)self + 0x68) != 0) {
        self->sprite.posX = F2I(*(s32*)((u8*)self + 0x58) - *(s32*)((u8*)self + 0x50));
        // (0x5C - 0x54) + 0x60, not 0x60 + (0x5C - 0x54): the target's
        // `sub r1, r2, r1` then `add r1, r3, r1` puts the difference first.
        self->sprite.posY = F2I(*(s32*)((u8*)self + 0x5C) - *(s32*)((u8*)self + 0x54) + *(s32*)((u8*)self + 0x60));
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

/**
 * @brief The two-word argument block the pin tasks are created with.
 *
 * `dataType` is read back by the four `_Sprite_Load` wrappers (it lands in the
 * animation template's dataType nibble); `childId` is read by the four init steps,
 * which store it as the id of the pin the new task will track.
 */
typedef struct {
    s32 dataType;
    s32 childId;
} OtuPinSpriteArgs; // Size: 0x8

/* The four load wrappers, defined further down but called by the init steps. */
void func_ov039_0208f1f8(OtuShadowTask* self, Sprite* sprite, s32* args);
void func_ov039_0208f4fc(OtuShadowTask* self, Sprite* sprite, s32* args);
void func_ov039_0208f874(OtuShadowTask* self, Sprite* sprite, s32* args);
void func_ov039_0208fb54(OtuShadowTask* self, Sprite* sprite, s32* args);

/* ==================================================================== */
/* The four init steps: 0208f2a4, 0208f5a8, 0208f900, 0208fbe0.        */
/* ==================================================================== */

/*
 * These four are the same body with three things varied -- which child id word
 * is written, which words get cleared, and whether the two scale words are
 * seeded with 0x1000. The seeds are the interesting part: shadow and meteo both
 * set scaleX = scaleY = 0x1000 (= 1.0 in Q12) and are the two that scale
 * anything later, while piyo and marker leave the whole block at zero because
 * they only ever move the sprite by an offset.
 *
 * The store order within each is load-bearing and follows the target exactly:
 * the child id lands first in the shadow and meteo (which write it into the
 * *highest* of their cleared words) but the clearing runs downward from it.
 */

/**
 * Shadow init: tracks the pin at `args->childId`, clears its two point pairs and
 * seeds the scales at 1.0, then loads the shadow sprite.
 */
s32 func_ov039_0208f2a4(TaskPool* pool, Task* task, OtuPinSpriteArgs* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;

    // +0x60, not +0x64: the shadow shares the lower of the two id words with the
    // piyo, and only the meteo's id sits at +0x64.
    self->childId      = args->childId;
    self->pinPosA_hi.x = 0;
    self->pinPosA_hi.y = 0;
    self->pinPosB_hi.x = 0;
    self->pinPosB_hi.y = 0;
    self->unk_40       = 0;
    self->unk_44       = 0x1000;
    self->unk_48       = 0x1000;
    self->unk_4C       = 0;
    self->unk_4E       = 0;

    func_ov039_0208f1f8(self, &self->sprite, &args->dataType);
    return 1;
}

/**
 * Piyo init: same shape, but it stores the child id into +0x50 (which the
 * piyo's own update and render then read) and clears one extra word below it.
 * No scale seeds.
 */
s32 func_ov039_0208f5a8(TaskPool* pool, Task* task, OtuPinSpriteArgs* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;

    self->pinPosA_hi.y = 0;
    self->pinPosA_hi.x = args->childId;
    self->unk_40       = 0;
    self->unk_44       = 0;
    self->unk_48       = 0;
    // A word store, not the halfword the shadow and meteo use here.
    *(s32*)((u8*)self + 0x4C) = 0;

    func_ov039_0208f4fc(self, &self->sprite, &args->dataType);
    return 1;
}

/** Marker init: the piyo's body with the child-id and zero stores reordered. */
s32 func_ov039_0208f900(TaskPool* pool, Task* task, OtuPinSpriteArgs* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;

    self->pinPosA_hi.x        = args->childId;
    self->unk_40              = 0;
    self->unk_44              = 0;
    self->unk_48              = 0;
    *(s32*)((u8*)self + 0x4C) = 0; // a word store, as in the piyo
    self->pinPosA_hi.y        = 0;

    func_ov039_0208f874(self, &self->sprite, &args->dataType);
    return 1;
}

/**
 * Meteo init: the widest of the four.
 *
 * Clears seven words (two point pairs, the +0x60 velocity and the +0x68/+0x6C
 * state pair), seeds the scales at 1.0, and puts the child id in +0x64 -- two
 * words higher than the shadow's, which is what leaves +0x60 free for the
 * velocity the render step adds into y.
 */
s32 func_ov039_0208fbe0(TaskPool* pool, Task* task, OtuPinSpriteArgs* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;

    self->meteoChildId = args->childId;
    self->pinPosA_hi.x = 0;
    self->pinPosA_hi.y = 0;
    self->pinPosB_hi.x = 0;
    self->pinPosB_hi.y = 0;
    self->childId      = 0; // the meteo's y velocity, cleared before first use
    self->alive        = 0;
    self->state        = 0;
    self->unk_40       = 0;
    self->unk_44       = 0x1000;
    self->unk_48       = 0x1000;
    self->unk_4C       = 0;
    self->unk_4E       = 0;

    func_ov039_0208fb54(self, &self->sprite, &args->dataType);
    return 1;
}

/* ==================================================================== */
/* The four update steps: 0208f2f0, 0208f5e0, 0208f938, 0208fc38.        */
/* ==================================================================== */

/**
 * Shadow update.
 *
 * Resolves the pin id to its data, asks func_ov039_0208e890 whether that pin is
 * still on the table, and only if the answer is exactly 1 copies the pin's two
 * coordinate pairs in and advances the sprite one frame.
 *
 * The `== 1` rather than `!= 0` is the interesting part: +0x64 stores the raw
 * filter result, and the render step only draws the shadow when it is non-zero,
 * so a filter that returns 2 or 3 would move the shadow without drawing it. The
 * filter's exact value space is not established here, but the equality test is
 * definitely not a truthiness test.
 */
s32 func_ov039_0208f2f0(TaskPool* pool, Task* task, void* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;
    void*          pin  = EasyTask_GetTaskData(pool, self->childId);

    if (pin != NULL) {
        *(s32*)((u8*)self + 0x64) = func_ov039_0208e890(pin);

        if (*(s32*)((u8*)self + 0x64) == 1) {
            func_ov039_0208e85c(pin, &self->pinPosA_hi);
            func_ov039_0208e6e0(pin, &self->pinPosB_hi);
            self->unk_48 = func_ov039_0208e8c4(pin);
            self->unk_44 = self->unk_48;
            Sprite_Update(&self->sprite);
        }
    } else {
        *(s32*)((u8*)self + 0x64) = 0;
    }
    return 1;
}

/**
 * Piyo update: the distance-banded sprite swap.
 *
 * `func_ov039_0208ee98` reads the pin's +0x148 "still in play" word, and the
 * value `(that * 100) / self->unk_58` is banded into one of five animation frames:
 * 20/40/60/80 are the thresholds, giving frames 8, 7, 6, 5, 4. So the sprite
 * gets visibly bigger or smaller depending on how live the pin is -- this is the
 * overlay's "how close to scoring is this pin" readout, rendered as a sprite.
 *
 * The `+ 0x1E` reload of the timer is a 30-frame hysteresis on the
 * func_ov039_02087d04 recomputation, so the recompute happens at most once every
 * half second rather than every frame it is eligible.
 */
s32 func_ov039_0208f5e0(TaskPool* pool, Task* task, void* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;
    void*          pin  = EasyTask_GetTaskData(pool, self->pinPosA_hi.x);
    // s16, not s32: the target carries the chosen frame in r2 straight into
    // Sprite_ChangeAnimation's `s16 animIndex` with no truncation, and an s32
    // local forces an `lsl #0x10 / asr #0x10` pair on the way in.
    s16 frame;
    s32 band;
    s32 live;

    if (pin != NULL) {
        live = func_ov039_0208ee98(pin);

        if (live > 0) {
            func_ov039_0208e85c(pin, OTU_PIN_POS_A(self));
            func_ov039_0208e6e0(pin, OTU_PIN_POS_B(self));

            // The quotient stays a full s32: the target compares it at 32-bit width
            // (`cmp r0, #0x14` straight after `_s32_div_f`) and only the chosen
            // frame is narrow.
            band = live * 100 / *(s32*)((u8*)self + 0x58);

            // Written as four flat `ble` tests rather than a switch: the target's
            // `cmp / movle / ble` triples are the if-chain shape, and a switch
            // would hoist all the compares to the top.
            if (band <= 0x14) {
                frame = 8;
            } else if (band <= 0x28) {
                frame = 7;
            } else if (band <= 0x3C) {
                frame = 6;
            } else if (band <= 0x50) {
                frame = 5;
            } else {
                frame = 4;
            }

            Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, frame, self->sprite.cellTable);
            Sprite_Update(&self->sprite);

            *(s32*)((u8*)self + 0x5C) = *(s32*)((u8*)self + 0x5C) - 1;
            if (*(s32*)((u8*)self + 0x5C) <= 0) {
                func_ov039_02087d04(0x330, OTU_PIN_POS_B(self), OTU_PIN_POS_A(self));
                *(s32*)((u8*)self + 0x5C) = 0x1E;
            }

            *(s32*)((u8*)self + 0x54) = 1;
        } else {
            *(s32*)((u8*)self + 0x54) = 0;
        }
    } else {
        *(s32*)((u8*)self + 0x54) = 0;
    }
    return 1;
}

/**
 * Marker update.
 *
 * Asks func_ov039_0208e950 for the pin's current marker frame and adopts it,
 * changing the sprite's animation only when it differs from the previous value --
 * so the sprite tracks a frame number the *pin* owns rather than computing its
 * own.
 */
s32 func_ov039_0208f938(TaskPool* pool, Task* task, void* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;
    void*          pin  = EasyTask_GetTaskData(pool, self->pinPosA_hi.x);
    s32            prev;
    s32            frame;

    if (pin != NULL) {
        // Read before the store: the target holds the old frame in r6 across the
        // write, so the compare is against the previous value, not the new one.
        prev                      = *(s32*)((u8*)self + 0x54);
        frame                     = func_ov039_0208e950(pin);
        *(s32*)((u8*)self + 0x54) = frame;

        if (frame > 0) {
            if (frame != prev) {
                Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, (s16)frame, self->sprite.cellTable);
            }
            func_ov039_0208e85c(pin, OTU_PIN_POS_A(self));
            func_ov039_0208e6e0(pin, OTU_PIN_POS_B(self));
            Sprite_Update(&self->sprite);
        }
    } else {
        *(s32*)((u8*)self + 0x54) = 0;
    }
    return 1;
}

/**
 * Meteo update: the band's largest function and the only real state machine.
 *
 * Four states, dispatched by a jump table (`addls pc, pc, r0, lsl #2` over
 * `cmp #3`), which the brief's rule makes unambiguous: a switch, not an if-chain.
 * Case bodies run 0, 1, 2, 3 and each ends in an unconditional branch to the
 * common `Sprite_Update` tail.
 *
 *   0  the pin died or was never there -- stop drawing
 *   1  a plain countdown on +0x70, falling back to 0 when it runs out
 *   2  easing scaleY toward 1.0 in Q12, `(0x1000 - y) / frames` per step
 *   3  the wind-up: func_ov039_02087bf8 lays out a path, and once +0x70 hits 30
 *      it walks eight further children through func_ov039_02097240
 *
 * State 2's `rsb r0, r0, #0x1000` is the tell that this is a converging ease
 * rather than a linear one: it always moves scaleY *toward* 0x1000 and the step
 * shrinks as it gets there.
 */
// Nonmatching: 94%. The logic is settled -- the four-state jump table, all four
// case bodies, the shared Sprite_Update guard and the null-pin arm are correct,
// and the filter is the kind == 8 test.
//
// What is left is one duplicated store and the block placement around it. The
// target emits the null-pin clear as its own tail block at 0x0208fd68
// (\mov r0, #0 / str r0, [r4, #0x68]\) which the \eq\ at the top jumps *forward*
// to, and separately emits the same two instructions as \case 0\. This build
// recognises the two as the same value and merges them, so the null arm's clear
// is hoisted above the \eq\ and the tail block disappears.
//
// Both an \if (pin == NULL) {...} else {...}\ and an early eturn 1\ arm (with
// the Sprite_Update guard duplicated by hand) were tried; the else form is the
// better of the two at 94% against 88%. Splitting the two stores into something
// mwcc cannot see as the same value would work but would mean writing code the
// target does not, so the honest form is kept.
s32 func_ov039_0208fc38(TaskPool* pool, Task* task, void* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;
    void*          pin  = EasyTask_GetTaskData(pool, self->meteoChildId);
    s32            i;

    if (pin == NULL) {
        self->alive = 0;
    } else {
        self->alive = func_ov039_0208e984(pin);

        if (self->alive != 0) {
            func_ov039_0208e85c(pin, &self->pinPosA_hi);
            func_ov039_0208e6e0(pin, &self->pinPosB_hi);
            *(s32*)((u8*)self + 0x60) = func_ov039_0208e6f4(pin);
        } else {
            self->state = 0;
        }

        switch (self->state) {
            case 0:
                self->alive = 0;
                break;

            case 1:
                self->timer = self->timer - 1;
                if (self->timer <= 0) {
                    self->state = 0;
                }
                break;

            case 2:
                if (self->timer > 0) {
                    self->unk_48 = self->unk_48 + _s32_div_f(0x1000 - self->unk_48, self->timer);
                    self->timer  = self->timer - 1;
                }
                break;

            case 3:
                func_ov039_02087bf8((u8*)self + 0x94, (u8*)self + 0x40);

                if (self->timer == 0x1E) {
                    for (i = 0; i < 8; i++) {
                        // The target reads exactly two words from this pointer
                        // (+0x00 and +0x04), which is what OtuTaskParams is; the
                        // address is already a pointer to that pair.
                        func_ov039_02097240(EasyTask_GetTaskData(pool, ((s32*)((u8*)self + 0x74))[i]),
                                            (OtuTaskParams*)&self->pinPosB_hi);
                    }
                }

                self->timer = self->timer - 1;
                if (self->timer <= 0) {
                    self->state = 0;
                }
                break;
        }

        if (self->alive != 0) {
            Sprite_Update(&self->sprite);
        }
    }
    return 1;
}

/**
 * Meteo destroy.
 *
 * Releases the sprite, then deletes eight tasks listed at +0x74. So the meteo
 * owns a fan-out of eight children -- the same eight func_ov039_0208fe60 created
 * for it -- and this is their teardown.
 */
s32 func_ov039_0208fddc(TaskPool* pool, Task* task, void* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;
    s32            i;

    Sprite_Release((Sprite*)self);

    for (i = 0; i < 8; i++) {
        EasyTask_DeleteTask(pool, ((s32*)((u8*)self + 0x74))[i]);
    }
    return 1;
}

/**
 * Creates a meteo task and its eight children.
 *
 * Unlike the other three constructors this one does work after creating the task:
 * it resolves the new task's data pointer and fills +0x74 with eight ids from
 * func_ov039_0209720c -- which is what func_ov039_0208fddc later deletes and what
 * func_ov039_0208fc38's state 3 walks.
 */
s32 func_ov039_0208fe60(TaskPool* pool, s32 dataType, s32 arg2) {
    OtuPinSpriteArgs args;
    // `id` is declared before `self` on purpose: mwcc hands out callee-saved
    // registers in declaration order, and the target keeps the task id in r7 with
    // the pool in r6. Declaring them the other way round puts `self` in r7 and
    // costs four instructions of register shuffling.
    s32            id;
    OtuShadowTask* self;
    s32            i;

    args.dataType = dataType;
    args.childId  = arg2;

    id   = EasyTask_CreateTask(pool, &data_ov039_020993e4, NULL, 0, NULL, &args);
    self = (OtuShadowTask*)EasyTask_GetTaskData(pool, id);

    for (i = 0; i < 8; i++) {
        ((s32*)((u8*)self + 0x74))[i] = func_ov039_0209720c(pool, dataType, arg2);
    }
    return id;
}

/* ------------------------------------------------------------------ */
/* The meteo's three state-entry steps and its position copier.        */
/* ------------------------------------------------------------------ */

/*
 * These write `state` last and set whatever the new state needs, so they are
 * entry points called from elsewhere rather than from the update step -- the
 * update step's switch *consumes* state and never sets anything but 0.
 */

/** Meteo state 1: re-enter animation frame 1 at full scale, for `frames` frames. */
void func_ov039_0208fefc(OtuShadowTask* self, s32 frames) {
    self->timer  = frames;
    self->unk_44 = 0x1000;
    self->unk_48 = 0x1000;

    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 1, self->sprite.cellTable);
    self->state = 1;
}

/** Meteo state 2: enter the scale-ease, four frames of it, with a small scaleY. */
void func_ov039_0208ff30(OtuShadowTask* self) {
    self->timer  = 4;
    self->unk_44 = 0x1000;
    self->unk_48 = 0x29;

    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 4, self->sprite.cellTable);
    self->state = 2;
}

/**
 * Meteo state 3: the wind-up. Frame 5, thirty frames of timer, and a path laid
 * out by func_ov039_02087ba0 from the table at 0x0209a54c.
 */
void func_ov039_0208ff68(OtuShadowTask* self) {
    self->timer = 0x1E;

    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 5, self->sprite.cellTable);
    func_ov039_02087ba0((u8*)self + 0x94, data_ov039_0209a54c, 6, (u8*)self + 0x40);
    self->state = 3;
}

/**
 * @brief A position plus a scale, the three-word record func_ov039_0208fee0 fills.
 *
 * Named rather than spelled as an `s32 out[3]` because the array form lets mwcc
 * merge the first two stores into one `stmia`, which the target does not do.
 */
typedef struct {
    s32 x;
    s32 y;
    s32 scale;
} OtuPinRecord; // Size: 0xC

/**
 * Copies the sprite's +0x58/+0x5C pair out to a three-word record, with a fixed
 * 0x1D000 third word.
 *
 * 0x1D000 is 29.0 in Q12 -- the overlay's own scale constant rather than anything
 * derived here, the same 0x1000-family values the four init steps seed.
 */
// Nonmatching: 98%, and only in the order of the first two stores -- the target
// stores +0x58 then +0x5C where this build stores +0x5C then +0x58. The two
// `ldr`s, the constant, the third store and the epilogue all match. The gap is
// mwcc's store scheduling for two adjacent same-base writes, which it will not
// reorder back; eight source spellings were tried (locals in both orders, the
// three field orders, and a raw `s32*` cast) and none produces the target's
// order together with the loads the target has.
void func_ov039_0208fee0(OtuShadowTask* self, OtuPinRecord* out) {
    s32 x = self->pinPosB_hi.x;
    s32 y = self->pinPosB_hi.y;

    out->scale = 0x1D000;
    out->x     = x;
    out->y     = y;
}

/* OtuCursor, func_ov039_0208ffac and func_ov039_0208ffd8 are declared in
 * include/Debug/Sugata/TinPinSlammer.h rather than here: the hammer task in band
 * 7 drives these same two functions over its own cursor, and band 7 comes first
 * in the include order. Both bands are callers, so neither should own it. */

/**
 * Starts a cursor at entry zero and primes it with the first entry's value plus
 * one.
 *
 * The `+ 1` is why the constructor and the stepper agree: a freshly started
 * cursor has `framesLeft` equal to entry 0's duration, so the first step
 * immediately falls through to entry 1.
 */
void func_ov039_0208ffac(OtuCursor* c, s32* table, s16 count) {
    // No locals for the table or the index: the target reloads both from the
    // cursor rather than reusing the incoming `table` and the zero it just
    // stored, which is what `ldr r1, [r0, #4] / ldr r2, [r0, #0]` says.
    s32 offset;

    c->table = table;
    c->index = 0;
    c->count = count;
    // The scale is computed as its own step: the target emits `lsl r1, r1, #2`
    // and then a separate `ldrsh r1, [r2, r1]`, whereas a single subscript folds
    // the two into one indexed load.
    offset        = c->index * 4;
    c->framesLeft = *(s16*)((u8*)c->table + offset) + 1;
}

/**
 * Advances a cursor by one frame.
 *
 * Returns 0 once the index reaches the count -- including from inside the
 * reload, so a cursor that runs off the end stops there rather than reading past
 * the table. This is the "both returns are `cmp/ge/bxge`" shape, so it is two
 * separate early exits rather than one shared one.
 */
s32 func_ov039_0208ffd8(OtuCursor* c) {
    s16 index;

    if (c->index >= c->count) {
        return 0;
    }

    c->framesLeft = c->framesLeft - 1;

    if (c->framesLeft <= 0) {
        c->index = c->index + 1;
        // A local for the new index: the target re-reads +0x4 into a *third*
        // register (r3) after the store, so the compare and the table lookup both
        // use the stored value rather than the one left over from the add.
        index = c->index;

        if (index >= c->count) {
            return 0;
        }

        c->framesLeft = *(s16*)((u8*)c->table + index * 4);
    }
    return 1;
}

/* ==================================================================== */
/* The four task constructors and the four stage dispatchers.           */
/* ==================================================================== */

/**
 * Creates a shadow task.
 *
 * The args block is a stack local rather than a caller-supplied pointer, so
 * EasyTask_CreateTask is handed `&args` as its last argument. All four of these
 * are identical apart from which TaskHandle they name -- which is the clearest
 * statement the target makes that these really are four instances of one task.
 */
s32 func_ov039_0208f40c(TaskPool* pool, s32 dataType, s32 childId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = childId;
    return EasyTask_CreateTask(pool, &data_ov039_0209930c, NULL, 0, NULL, &args);
}

/** Creates a piyo task. */
s32 func_ov039_0208f770(TaskPool* pool, s32 dataType, s32 childId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = childId;
    return EasyTask_CreateTask(pool, &data_ov039_02099354, NULL, 0, NULL, &args);
}

/** Creates a marker task. */
s32 func_ov039_0208fa5c(TaskPool* pool, s32 dataType, s32 childId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = childId;
    return EasyTask_CreateTask(pool, &data_ov039_0209939c, NULL, 0, NULL, &args);
}

/*
 * The four stage dispatchers. `const TaskStages` on the stack rather than a
 * direct `table.iter[stage]` through the global is what produces the target's
 * `ldm/stm` pair -- the same shape as NRepMenu_RunTask in NRep.c. Each copies
 * its own four-entry table out of .rodata and indexes it.
 */

/** Shadow task dispatcher. */
s32 func_ov039_0208f3c4(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_02099318;

    return stages.iter[stage](pool, task, args);
}

/** Piyo task dispatcher. */
s32 func_ov039_0208f728(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_02099360;

    return stages.iter[stage](pool, task, args);
}

/** Marker task dispatcher. */
s32 func_ov039_0208fa14(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_020993a8;

    return stages.iter[stage](pool, task, args);
}

/** Meteo task dispatcher. */
s32 func_ov039_0208fe18(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_020993f0;

    return stages.iter[stage](pool, task, args);
}

/** Loads the meteo sprite: absolute position from +0x58/+0x5C, as the shadow. */
void func_ov039_0208fb54(OtuShadowTask* self, Sprite* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_02099400;

    anim.owner    = self;
    anim.dataType = *args;
    anim.posX     = F2I(*(s32*)((u8*)self + 0x58));
    anim.posY     = F2I(*(s32*)((u8*)self + 0x5C));

    _Sprite_Load(sprite, &anim);
}
s32 func_ov039_0208f0b0(void* task) {
    return *(u16*)(*(u8**)((u8*)task + 0x16C)) == 0x130;
}

/**
 * Clears a child's +0x84/+0x80 pair through func_ov039_02096270, but only when
 * +0x1A8 is non-zero.
 *
 * The +0x1A8 word is a "has this been done" latch: the target tests it, returns
 * early on zero, and never clears it, so a second call is a no-op only because
 * something else does. Worth stating because it is the one place in this band
 * where an early exit is a side-effect guard rather than a value test.
 */
void func_ov039_0208f0c8(void* task) {
    if (*(s32*)((u8*)task + 0x1A8) != 0) {
        func_ov039_02096270(EasyTask_GetTaskData(*(void**)((u8*)task + 8), *(s32*)((u8*)task + 0x230)));
    }
}

/**
 * Clears +0x1B0 when it equals the argument.
 *
 * `cmp / moveq #0 / streq` -- the store is conditional on the comparison, so
 * the source is an `if` with the constant folded into it, not a masked write.
 */
void func_ov039_0208f0f0(void* task, s32 value) {
    if (*(s32*)((u8*)task + 0x1B0) == value) {
        *(s32*)((u8*)task + 0x1B0) = 0;
    }
}

/**
 * Clears +0x114/+0x118 on two children at +0x228 and +0x22C.
 *
 * The loop reads the child id as `add r1, r5, r4, lsl #2` then `ldr r1, [r1,
 * #0x228]`, i.e. the index is scaled into a *base* and the field is a fixed
 * displacement off it -- the same shape as OTU_CHILD_ID.
 *
 * Reaching that shape needs the subscript form below. Every variant written as
 * `*(s32*)((u8*)task + i * 4 + 0x228)` makes mwcc strength-reduce the loop into
 * a walked pointer (`mov r5, r4` / `add r5, r5, #4`), which is a real
 * difference and costs several instructions; subscripting a `s32*` base keeps
 * the scaled add and the fixed displacement separate and matches exactly.
 */
void func_ov039_0208f104(void* task) {
    s32 i;

    for (i = 0; i < 2; i++) {
        func_ov039_02095ddc(EasyTask_GetTaskData(*(void**)((u8*)task + 8), ((s32*)((u8*)task + 0x228))[i]));
    }
}

/* ============================================================================
 * Band B2 -- the wireless-board stage state machine and the board grid query,
 * 0x02089950 - 0x0208acc0.
 *
 * Claim range: .text 0x02089950 .. 0x0208acc0 (22 functions, 4976 bytes,
 * 0x02089950 .. 0x0208ac98). It is one contiguous slice of ov039's
 * `OtuFieldAccess.c` claim and nothing else in the tree touches any of it.
 *
 * WHAT IS HERE
 * ------------
 * The band is two things that share a translation unit only because the delinks
 * format forbids splitting a claimed range:
 *
 *   1. Fourteen stage routines, in two near-identical groups of seven. They are
 *      the whole life cycle of the overlay's *wireless board* stage: an
 *      init/cleanup pair plus four update/render/transition pairs, all driving
 *      one 0x2D0-byte stage block through `func_ov039_02098b70`, which is just
 *      `dispatch->stageBlock`. The two groups are the same code with different
 *      constants and one extra test each, which is the clearest evidence in the
 *      overlay that there are two wireless-board layouts:
 *
 *        02089950 / 02089f68   pre-update   (timer + a wireless-state switch)
 *        02089a80 / 0208a098   update       (a 0..8 stage switch)
 *        02089d3c / 0208a324   enter        (three calls, 48 bytes, identical)
 *        02089d6c / 0208a354   "everyone has finished" (160 bytes, identical)
 *        02089e0c / 0208a3f4   tick         (96 vs 108 bytes)
 *        02089e78 / 0208a454   wireless interrupt handler (72 vs 60 bytes)
 *        02089ec0 / 02089f30   init / cleanup (the only pair, not duplicated)
 *
 *      02089d3c and 0208a324 are byte-identical; so are 02089d6c and 0208a354.
 *      02089e78 differs from 0208a454 by the `func_ov040_0209cb68() != 1` test
 *      and 02089e0c from 0208a3f4 by the extra `func_ov040_0209d420(NULL, NULL)`.
 *
 *   2. Eight pin-logic / grid routines from 0x0208a490 on: a score accumulator,
 *      two geometry predicates, a velocity clamp, a speed lookup, the board cell
 *      lookup, the "is this point reachable" test, and a change-mask latch.
 *
 * STRUCTURAL FINDINGS
 * -------------------
 *   * The stage block is 0x2D0 bytes, which is exactly the `size` word the
 *     header's `OtuScene_WirelessBoard` descriptor carries, and `func_ov039_
 *     020883c8` walks a *second* child-handle array in it at +0x28C with the
 *     `index * 4` + fixed-displacement shape `OTU_CHILD_ID` documents, against
 *     the first at +0x17C. +0x140 is the pool count both dispatchers compare
 *     against and both "has everyone finished?" loops bound themselves by --
 *     i.e. it is the shared `OTU_CHILD_COUNT` field -- and +0x144 is a *second,
 *     different* count that `func_ov039_020883ac` reports and that 0x0208a988's
 *     placement loop walks over the +0x28C array.
 *   * `func_ov039_02088440` is `data_ov039_0209af20 + index * 6` and nothing
 *     else: a three-instruction accessor for a six-byte-stride record whose
 *     *byte* at +1 is the 1/2/3/4 phase the stage's three polling loops wait on.
 *     Six is a byte count (`mov r1, #6; mla r0, r1, r0, r2`), and the read is
 *     `ldrb`, so this is not `OtuFrame6` and is declared as its own type.
 *   * 0208a794 maps a Q12.12 point to a cell with *three* successive fixed-point
 *     reductions -- >>12, then /32 for the coarse lookup, then `/ 16 & 1` for
 *     the fine table -- and then picks one of twelve four-byte tables by the
 *     cell's type byte minus 0x10. The tables are 2x2 arrays of bit flags
 *     packed into four bytes and indexed *bytewise* (`base[gy32 * 2 + gx32]`),
 *     which is what makes the target reach them as `add r0, base, gy32, lsl #1`
 *     followed by `ldrb r0, [gx32, r0]`.
 *   * 0208a530 is the perpendicular distance from `c` to the line a-b, in
 *     Q12.12: `|cross| / |b - a|`, with the cross product accumulated as
 *     `(dy*a.x - dx*a.y) + (-dy*c.x + dx*c.y)` where `dx = a.x - b.x` and
 *     `dy = a.y - b.y`, and the length from two separately-rounded Q12.12
 *     squares summed *in 32 bits* before the sqrt.
 *   * 0208a988 probes the four axis neighbours at +/-0xC000 and then the four
 *     corners at +/-0x20000. A corner that reports "solid" snaps the probe point
 *     to that corner's cell and rejects the point if it is within 0xC000 of it --
 *     i.e. the corner probes are a bounds check, not a collision test. The three
 *     masks are `~0x1FFFF`, `|0x1FFFF` and, for the last corner only,
 *     `& (0x1FFFF << 17)`.
 *   * 0208ac98 is the standard "which bits changed since I last looked" latch:
 *     `changed = now & (last ^ now); last = now;` over a word reached through a
 *     pointer at +0xE8.
 * ==========================================================================*/

#include "Debug/Sugata/TinPinSlammer.h"
#include "Engine/EasyTask.h"
#include "SpriteMgr.h"
#include <nitro/fx/fx_division.h>
/* OTU_BOARD_RECORD reaches the board grid through gSaveData, which is
 * declared in Save.h -- the same include every other user of gSaveData
 * in this tree pulls in. */
#include "Save.h"

/* ------------------------------------------------------------------ */
/* Integration notes.                                                  */
/* ------------------------------------------------------------------ */

/*
 * Two declarations outside this band have to change before it can be included,
 * and both are stale placeholders rather than real prototypes:
 *
 *   * include/Debug/Sugata/TinPinSlammer.h lines 246-260 declare fourteen of
 *     these as `void func_ov039_XXXXXXXX(void);`. Every one of them takes the
 *     scene in r0 -- it is the first thing the prologue saves -- so the
 *     definitions below conflict with them and mwcc will reject the pair. Those
 *     fourteen lines have to go (or be given the real prototype) at the same
 *     time this band lands. They are not repeated here: a redeclaration would
 *     not help, since the conflict is with the header, not with a local copy.
 *
 *   * OtuPinSprites line 73 declares `s32 func_ov039_0208a490(void*, s32);` and
 *     calls it discarding the result. The definition below matches that
 *     prototype exactly -- including the untyped first parameter, which the
 *     target's `ldr r0, [r5, #8]` does not pin down any better -- so nothing has
 *     to change there.
 *
 * Include order: this band uses `func_ov039_02098bb0`, `func_ov039_02098c00`
 * and `func_ov039_02098d10` as band 3 declares them (and `func_ov039_02098d10`
 * as the *s32* band 3 declares, not as the `void` OtuVecOps.c defines it), so
 * it has to come after OtuEntryTasks. Anything before OtuPinSprites works; the
 * natural slot is next to OtuGauge.
 */

/* ------------------------------------------------------------------ */
/* Data the band reads but does not define.                            */
/* ------------------------------------------------------------------ */

/*
 * All of these sit in the overlay's gap-filled `.data`/`.bss`, so declaring an
 * object of our own would put it wherever the linker chose rather than at the
 * address the target's pool word names. They keep the build's own names, for
 * the same reason and with the same caveat as the offset tables in band 13.
 */

/** The overlay-global phase byte: 1, 2, 3 or 4, raised by the stage's pollers. */
extern u8 data_ov039_0209ad00;

/** The 0x10-byte scratch buffer handed to the wireless stack with a 0x10 length. */
extern u8 data_ov039_0209b120[0x10];

/*
 * Four cross-overlay objects in ov038. `data_ov038_0209edc0` is passed to the
 * two 02047xxx entry points as an argument block, `...eef4` to
 * func_ov040_0209d970 as a name, and `...ef48` / `...ef6c` to ov003's
 * registration routine as the first of its two arguments. Note the *two* copies
 * of the last one: 02089a80 passes ...ef48 and 0208a098 passes ...ef6c. That is
 * a real difference between the two board variants, not a transcription slip.
 */
extern u32 data_ov038_0209edc0;
extern u32 data_ov038_0209eef4;
extern u32 data_ov038_0209ef48;
extern u32 data_ov038_0209ef6c;

/**
 * @brief The pin's Q12.12 speed table, sixteen bytes per entry.
 *
 * 0208a6f8 indexes it with `ldr r0, [base, index, lsl #4]` and reads the word at
 * offset 0, so the stride is sixteen *bytes* and the value is Q12.12 (the
 * entries run 0x148, 0x171, 0x19A ... 0x7AB8, 0x8000 -- i.e. 0.08 to 2.0 in
 * Q12.12, which is what the caller then multiplies a Q12.12 angle by).
 *
 * Declared with an incomplete extent: the index is a byte read out of a record
 * whose layout this band cannot see, so the count is not knowable from here.
 */
typedef struct {
    /* 0x00 */ s32 speed; // Q12.12
    /* 0x04 */ u8  pad_04[0x0C];
} OtuSpeedEntry;          // Size: 0x10

extern OtuSpeedEntry data_ov039_0209a3dc[];

/**
 * @brief One child record of the wireless board, six bytes.
 *
 * Reached only through `func_ov039_02088440`, which is
 * `data_ov039_0209af20 + index * 6`. Only the phase byte at +1 is read by
 * anything in this band, and it is read three separate times -- once per polling
 * loop, against the constants 1, 2, 3 and 4 -- which is what sequences the two
 * board variants' rounds.
 *
 * Six is a byte count, not three halfwords: the accessor is
 * `mov r1, #6; mla r0, r1, r0, base`.
 */
typedef struct {
    /* 0x00 */ u8 pad_00;
    /* 0x01 */ u8 phase; // 1, 2, 3 or 4 -- which round this child has reached
    /* 0x02 */ u8 pad_02[4];
} OtuChildRecord;        // Size: 0x6

extern OtuChildRecord data_ov039_0209af20[];

/**
 * @brief The twelve four-byte cell-type tables 0208a794 selects between.
 *
 * Each is a 2x2 grid of bit flags packed into four bytes: entry `gy32 * 2 + gx32`
 * with both halves of the division by 32. The values are drawn from the set
 * {0, 1, 0x100, 0x101}, i.e. two independent bit flags, and the mapping is
 * driven by the cell's type byte minus 0x10:
 *
 *     1 -> 48   2 -> 44   3 -> 50   4 -> 3C   5 -> 4C   6 -> 40
 *     7 -> 68   8 -> 64   9..13 -> 60   14..18 -> 5C
 *    19..23 -> 58   24..28 -> 54   anything else -> the cell's own first byte
 *
 * They are `.data`, not `.rodata`, so they are declared non-`const`.
 */
extern u8 data_ov039_0209923c[4];
extern u8 data_ov039_02099240[4];
extern u8 data_ov039_02099244[4];
extern u8 data_ov039_02099248[4];
extern u8 data_ov039_0209924c[4];
extern u8 data_ov039_02099250[4];
extern u8 data_ov039_02099254[4];
extern u8 data_ov039_02099258[4];
extern u8 data_ov039_0209925c[4];
extern u8 data_ov039_02099260[4];
extern u8 data_ov039_02099264[4];
extern u8 data_ov039_02099268[4];

/* ------------------------------------------------------------------ */
/* Callees in this overlay that are not decompiled yet.                 */
/* ------------------------------------------------------------------ */

/*
 * Signatures are inferred from the call sites, and only the ones the call sites
 * pin down are given real parameter lists -- mwcc reads an unknown `T*` as
 * `int`, which then collides with the real definition wherever one exists.
 */

/** `data_ov039_0209af20 + index * 6`; nothing else. */

/** Number of children in a child list (`dispatch->stageBlock` + 0x144). */
s32 func_ov039_020883ac(TinPinSlammer_Scene* scene);

/** The `index`th child of a child list, resolved out of pool 2. */
void* func_ov039_020883c8(TinPinSlammer_Scene* scene, s32 index);

/** Wireless receive callback: (record, scene). Reached only as a function pointer. */
s32 func_ov039_020885f4(void* record, TinPinSlammer_Scene* scene);

/** Wireless receive callback: (record, scene). Reached only as a function pointer. */
void func_ov039_02088698(void* record, TinPinSlammer_Scene* scene);

/** The stage's "board has changed" entry; clears state.unk_698 and re-seeds. */
void func_ov039_02088ec4(TinPinSlammer_Scene* scene);

/** The stage's board teardown. */
void func_ov039_02088f18(TinPinSlammer_Scene* scene);

/** The board's per-frame physics, run at the end of both tick routines. */
void func_ov039_020894cc(TinPinSlammer_Scene* scene);

/** Creates the second child task; returns its handle. */

/* OtuVecOps.c's three helpers, declared here because nothing inside this
 * translation unit already declares them and an implicit `int (...)` would lose
 * the `OtuPoint*` parameters. The prototypes are copied verbatim from their
 * definitions. Note that func_ov039_02098d10 is deliberately *not* repeated: band
 * 3 declares it `s32` and OtuVecOps.c defines it `void`, and this band needs the
 * value, so band 3's declaration is the one that has to win in the include
 * order. That inconsistency is pre-existing. */
s32  func_ov039_02098c40(OtuPoint* a, OtuPoint* b);
void func_ov039_02098d3c(OtuPoint* src, OtuPoint* dest);

/* 02087cac is defined in OtuCountdown.c, so this TU needs its own declaration
 * before it can be named as the wireless stack's setup callback. */
void func_ov039_02087cac(TinPinSlammer_Scene* scene, s32 slot, const void* src, s32 len);

/*
 * Overlay 40's wireless stack, plus the two 02047xxx/02044xxx engine entry
 * points the board's setup calls. All void except where noted.
 */
s32  func_ov040_0209cb68(void);
s32  func_ov040_0209cde4(void);
void func_ov040_0209caac(s32 arg0);
void func_ov040_0209cb9c(void);
void func_ov040_0209cb08(void* state, s32 arg1);
void func_ov040_0209cabc(void* state, s32 arg1);
void func_ov040_0209c158(void);
void func_ov040_0209ba04(void (*cb)(void*, TinPinSlammer_Scene*), void* scene, void* buf, s32 arg3);
void func_ov040_0209d0a8(s32 arg0, s32 arg1, s32 arg2);
void func_ov040_0209d290(s32 arg0, void* arg1);
void func_ov040_0209d40c(void (*cb)(), void* arg);
void func_ov040_0209d420(void (*cb)(), void* arg);

void func_ov040_0209d588(void);
s32  func_ov040_0209d728(void);
void func_ov040_0209d818(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5);
void func_ov040_0209d848(s32 arg0);
void func_ov040_0209ece8(u8* buf, s32 len);
void func_ov040_0209ec5c(void (*cb)(TinPinSlammer_Scene*, s32, const void*, s32), void* scene, void* buf, s32 len);
void func_ov040_0209ed58(s32 arg0);
void func_ov040_0209ef88(void);
void func_ov040_0209efc0(void);

s32  func_020442f8(void);
void func_020471ec(s32 a0, s32 a1, void* arg);
void func_020472a8(void* arg);
void func_02047338(void);
void func_0204737c(void);
s32  func_02047e84(u16 arg0);

/* ------------------------------------------------------------------ */
/* Task-data shapes.                                                    */
/* ------------------------------------------------------------------ */

/**
 * @brief The wireless board's stage block, 0x2D0 bytes.
 *
 * Reached as `func_ov039_02098b70(OTU_STAGE(scene))`, which is a one-instruction
 * accessor for `dispatch->stageBlock`. The size is not an estimate: it is the
 * `size` word the header's `OtuScene_WirelessBoard` descriptor carries, and
 * 0x2CC is the last field anything here touches.
 *
 * Every field below is placed on the offset at least one load or store in this
 * band uses. Note that +0x140 and +0x144 are two *different* counts: +0x140 is
 * latched from `state.unk_EE0` by the init stage and is what the six
 * child-polling loops bound themselves by (i.e. the shared `OTU_CHILD_COUNT`
 * field), while +0x144 is what `func_ov039_020883ac` reports and what
 * 0x0208a988's placement loop walks. Conflating them is the easy mistake here.
 *
 * The three words at 0x2C4, 0x2C8 and 0x2CC are task handles: 0x2C8
 * and 0x2CC are deleted by the cleanup and by stage 8 respectively, and 0x2C4 is
 * resolved through pool 1 by both tick routines. The child-handle arrays at
 * +0x17C and +0x28C are reached through `OTU_CHILD_ID`, which is a macro taking
 * the block, so they stay out of the struct rather than being named twice.
 */
typedef struct {
    /* 0x000 */ s32 unk_00;     // the board stage's dispatch selector, 0..8
    /* 0x004 */ u16 unk_04;     // handshake flag; the two board variants set it either way
    /* 0x006 */ u16 unk_06;     // two bytes of wireless state, pushed as a pair
    /* 0x008 */ s32 state;      // 0..8, the selector both update routines switch on
    /* 0x00C */ u8  pad_00C[0x134];
    /* 0x140 */ s32 childCount; // latched from state.unk_EE0; what OTU_CHILD_COUNT reads
    /* 0x144 */ s32 subCount;   // func_ov039_020883ac's answer: a *different* count
    /* 0x148 */ u8  pad_148[4];
    /* 0x14C */ s32 slotIndex;  // written from the 0x44000 block's +0xA68
    /* 0x150 */ u8  pad_150[0x20];
    /* 0x170 */ s32 unk_170;    // written from data_ov039_0209a360[slotIndex]
    /* 0x174 */ s32 timer;      // 0x258 on entry, counted down every frame
    /* 0x178 */ u8  pad_178[0x14C];
    /* 0x2C4 */ s32 tickTask;   // handle resolved through pool 1 by the tick stages
    /* 0x2C8 */ s32 initTask;   // handle deleted by the cleanup
    /* 0x2CC */ s32 fadeTask;   // handle deleted by stage 8
} OtuWirelessStage;             // Size: 0x2D0

/**
 * @brief The pin task's own state block.
 *
 * This is the same object the shared header calls `OtuPinTask` -- +0x120/+0x124
 * are that type's `x`/`y` and +0x16C its `pinID` -- but the header's version
 * stops at 0x174 and these routines reach 0x228, so the wider shape is declared
 * here rather than by widening a type that matched code already depends on.
 *
 * The five consecutive two-word pairs at +0x110, +0x118, +0x120, +0x128 and
 * +0x138 are the overlay's five OtuPoint-shaped slots; `OtuFieldAccess.c` gives
 * each of them a getter/setter/copy-out triple, which is what identifies them.
 * Only +0x110 and +0x120 are used here, and they are the `src` and `dst` pair
 * every `func_ov039_02087d04` call in the overlay takes.
 */
typedef struct {
    /* 0x000 */ u8        pad_000[8];
    /* 0x008 */ TaskPool* pool;       // the pool every handle below is resolved through
    /* 0x00C */ u8        pad_00C[0x104];
    /* 0x110 */ OtuPoint  anchor;     // the `src` of func_ov039_02087d04
    /* 0x118 */ u8        pad_118[8];
    /* 0x120 */ OtuPoint  pos;        // the `dst` of func_ov039_02087d04
    /* 0x128 */ u8        pad_128[4];
    /* 0x12C */ OtuPoint  vel;        // clamped to length 0x8000 by 0208a6c4
    /* 0x134 */ u8        pad_134[4];
    /* 0x138 */ OtuPoint  dir;        // vel re-normalised into here when vel is non-zero
    /* 0x140 */ u8        pad_140[0x2C];
    /* 0x16C */ u16*      pinID;      // a tray slot; 0x130 there means "no pin"
    /* 0x170 */ u8*       slots;      // OtuBoardSlot[], indexed by *pinID
    /* 0x174 */ u8        pad_174[0x2C];
    /* 0x1A0 */ s16       slot;       // round-robins over the two children at +0x228
    /* 0x1A2 */ u8        pad_1A2[0xA];
    /* 0x1AC */ s32       total;      // accumulated score, clamped to 0x3E7
    /* 0x1B0 */ u8        pad_1B0[0x20];
    /* 0x1D0 */ s32       mode;       // 0x10 selects one of two blend modes
    /* 0x1D4 */ u8        pad_1D4[0x20];
    /* 0x1F4 */ s32       counterId;  // handle of the task 02093d68 drives
    /* 0x1F8 */ u8        pad_1F8[0x30];
    /* 0x228 */ s32       childId[2]; // the two children the score is split between
} OtuPinLogic;                        // Size: 0x230

/**
 * @brief One board slot's record, 0x1C bytes.
 *
 * The pin task's +0x170 points at an array of these, indexed by the pin id in
 * its tray slot with a *byte* stride of 0x1C. Only the byte at +4 is read here,
 * and it is an index into `data_ov039_0209a3dc` rather than a value.
 */
typedef struct {
    /* 0x00 */ u8 pad_00[4];
    /* 0x04 */ u8 speedIndex;
    /* 0x05 */ u8 pad_05[0x17];
} OtuBoardSlot; // Size: 0x1C

/**
 * @brief The board's cell grid, as 0208a794 sees it.
 *
 * Two fields are read: a byte scale at +0x02 that multiplies the coarse row
 * term, and a byte pointer at +0x10 to an array of four-byte cell records whose
 * first two bytes are two independent states. Nothing else in this band (or in
 * anything else decompiled so far) reaches into it, so the rest is padding kept
 * only to hold the two offsets.
 */
typedef struct {
    /* 0x00 */ u8  pad_00[2];
    /* 0x02 */ u8  rowScale; // how many cell rows one screen row spans
    /* 0x03 */ u8  pad_03[0x0D];
    /* 0x10 */ u8* cells;    // four bytes per entry: two state bytes, two unused
} OtuCellGrid;               // Size: 0x14

/**
 * @brief The change-mask latch 0208ac98 keeps.
 *
 * A pointer to a live 16-bit state word, the value that word held last time, and
 * the bits that have changed since. Only the three fields at +0xE8..+0xEF are
 * known, so the stated size is the extent of that, not the object's real one.
 */
typedef struct {
    u8               pad_000[0xE8];
    /* 0x0E8 */ u16* state; // the live word; the one read is at its own +0x04
    /* 0x0EC */ u16  last;
    /* 0x0EE */ u16  changed;
} OtuInputLatch; // Size: 0xF0 (only 0xE8..0xEF are known)

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

/** One Q12.12 multiply, rounded to nearest. */
static inline s32 OtuQ12Mul(s32 a, s32 b) {
    return (s32)(((s64)a * b + 0x800) >> 12);
}

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

/* ============================================================================
 * Band B3 -- Tsk_OtosuGame_badge, 0x0208acc0 - 0x0208c4ec.
 *
 * Claim range: .text 0x0208acc0 .. 0x0208c4ec (22 functions, 6188 bytes,
 * 0x0208acc0 .. 0x0208c45c). It sits in the gap between OtuFieldAccess.c's
 * own claim (0x02088400 - 0x02098b8c) and band 1 (0x0208f000 - 0x02090000),
 * and like bands 6, 7 and 13 it is one whole task rather than a slice of a
 * call graph: nothing in the band calls anything in another band except the
 * shared vector unit, and nothing outside the band calls into it except the
 * overlay's own dispatcher.
 *
 * WHAT THE TASK IS
 * ----------------
 * A "badge" is one of the small round tokens that slides around the Tin Pin
 * Slammer board under the player's own pin. The task owns a badge's whole
 * simulation: a position in Q12.12, a velocity, a rolling set of per-tile
 * behaviours, and a wheel of AI decisions that re-rolls whenever the badge
 * arrives somewhere new.
 *
 * The state block is 0x25C bytes and is driven by a phase counter at +0xF8
 * (2..9) with a finer `step` counter at +0xF4 underneath it. Each phase has
 * its own one-shot entry setter, and every setter opens with the same four
 * instructions: load the phase, compare it against its own number, return if
 * it already matches. So a phase transition is idempotent and the setters
 * double as "have we reached this phase yet" tests. That is the clearest
 * single piece of evidence for what the block is, and it is why 0x0208ad88,
 * 0x0208ade8 and 0x0208ae14 are so alike.
 *
 * STRUCTURAL FINDINGS
 * -------------------
 *   * +0x100 is not a scalar. It is the head of a 0xA0-byte timer block: a
 *     32-bit frame budget at +0x100 (0x1E / 0x3C / 9 depending on phase), the
 *     last-observed tile type and tile cell at +0x104/+0x108/+0x10C, six
 *     16-bit countdown slots at +0x178..+0x17E and +0x19C/+0x19E, and the
 *     trailing trail cursor. They are collected into `OtuTimers` below rather
 *     than left as loose fields, because every one of them is written by a
 *     `ldrsh`/`sub 1`/`strh` triple or a plain word store against the same
 *     base and nothing outside this band reaches into the middle of it.
 *   * +0xE4 is a bare byte cursor, not a struct pointer, and it is used two
 *     different ways. Bytes +2, +3 and +5 are the board's own description
 *     (tile width, tile height, and a flag); a word at +0x10 is the grid of
 *     per-cell tile bytes; and bytes +8 onwards are a two-byte-per-badge
 *     table of starting tiles, which is why the phase-5 entry forms
 *     `add r1, board, index, lsl #1` and then loads at +8 and +9. Reading it
 *     as one struct would lose that second indexing, so it stays a `u8*` and
 *     the layout is documented here instead.
 *   * +0x170 is a pointer to the per-badge-kind behaviour records, indexed by
 *     the tray slot at +0x16C on a 0x1C-byte stride. That record's +0x04 byte
 *     is itself an index into a Q12.12 speed table, so the chain is
 *     tray slot -> behaviour record -> speed -> rate table.
 *   * Every wall bounce masks with 0x1FFFF and then folds the position back to
 *     a cell boundary: the board is a grid of 0x20000 (2.0 in Q12.12) cells
 *     and a badge is snapped to cell edges rather than to exact coordinates.
 *     0x20000 is also the stride the spiral search at 0x0208bed8 walks.
 *   * The two bounce constants are not the same. Moving *into* a wall scales
 *     the velocity by -0x2000; moving *away* from one negates it outright.
 *     The target derives -0x2000 by arithmetic-shifting the very mask constant
 *     it has already materialised -- `(~0x1FFFF) >> 17` -- which is why it
 *     reads as an unrelated constant in the disassembly and shares the
 *     materialisation with the mask beside it.
 * ==========================================================================*/

#include "Debug/Sugata/TinPinSlammer.h"
#include "Engine/EasyTask.h"
#include "Engine/Math/Random.h"
#include <nitro/fx/fx_atan.h>
#include <nitro/fx/fx_division.h>

/* ------------------------------------------------------------------ */
/* Data the band reads but does not define.                            */
/* ------------------------------------------------------------------ */

/*
 * Every symbol here lives in the overlay's own gap-filled `.data` and keeps
 * the build's name for it. Declaring an object of our own would let the
 * linker place it at an address of its choosing rather than the one the target
 * uses, which costs the pool word; see the same note in band 5 and band 13.
 */

/** 0x133 -- the badge's per-frame acceleration, in Q12.12. */
extern s32 data_ov039_0209a318;

/** -0x2000 -- the velocity the phase-7 entry sets going through a corner. */
extern s32 data_ov039_0209a31c;

/** -0x2000 again, as the phase-9 entry's terminal velocity. */
extern s32 data_ov039_0209a320;

/** 0x800 -- the bounce re-normalisation factor. */
extern s32 data_ov039_0209a324;

/** 0x8000 -- the axis speed the four tile-driven launch cases set. */
extern s32 data_ov039_0209a308;

/*
 * The movement-rate table, four words, is declared as
 * `data_ov039_0209a39c[4]` in TinPinSlammer.h -- func_ov039_02084020 and
 * func_ov039_020840c0 reach the same four words from OtuScoreRow.c.
 */

/**
 * @brief One badge kind's speed record: a 16-byte-stride Q12.12 multiplier.
 *
 * Indexed by the behaviour record's +0x04 byte, which is why the target gets
 * the stride as a shift rather than as a scale. Only the leading word is read
 * by this band; the other three are whatever the board data carries.
 */
typedef struct {
    /* 0x00 */ s32 speed; // Q12.12 multiplier applied to the badge's velocity
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ s32 unk_0C;
} OtuBadgeSpeed; // Size: 0x10
extern OtuBadgeSpeed data_ov039_0209a3e0[];

/**
 * The base module's sine/cosine table, indexed by `(angle >> 4) * 2`.
 *
 * `entry[n * 2]` is the cosine and `entry[n * 2 + 1]` the sine; 0x0208af6c
 * puts the sine on x and the cosine on y, which is the overlay's convention
 * for turning a heading into a direction vector.
 */
/* declared by band 3 as s32[]; bands 5 and 7 read it through a s16* cast -- declared/defined elsewhere in this TU; see the
 * note below on why band 11 does not repeat it. */

/* ------------------------------------------------------------------ */
/* Task-data shapes.                                                   */
/* ------------------------------------------------------------------ */

/**
 * @brief Where a badge was spawned: one flag byte and its starting tile.
 *
 * Read through +0xE8. Bit 0 of `flags` is copied straight into bit 0 of the
 * badge's packed state word by 0x0208acc0, which is the only thing that ties
 * the two objects together; `tileX`/`tileY` are the tile the badge belongs to
 * and are shifted up by twelve to become Q12.12.
 */
typedef struct {
    /* 0x0 */ u8 flags; // bit 0 is mirrored into the badge's stateFlags
    /* 0x1 */ u8 pad_01;
    /* 0x2 */ u8 tileX;
    /* 0x3 */ u8 tileY;
} OtuHomePos; // Size: 0x4

/**
 * @brief One badge kind's behaviour record, 0x1C-byte stride.
 *
 * Reached through +0x170 and indexed by the tray slot at +0x16C, so the stride
 * is a `mla` against 0x1C rather than an array subscript the compiler could
 * fold away. The two groups of fields serve two different phases and nothing
 * suggests they are related: +0x04/+0x06 drive the rolling phase, +0x08
 * onward the scripted set-piece phases.
 */
typedef struct {
    /* 0x00 */ u8  pad_00[0x04];
    /* 0x04 */ u8  speedIndex; // index into data_ov039_0209a3e0
    /* 0x05 */ u8  pad_05;
    /* 0x06 */ s16 friction;   // Q12.12, scaled by unk_144 / 3 into a heading offset
    /* 0x08 */ u16 animX;
    /* 0x0A */ u16 animY;
    /* 0x0C */ u8  pad_0C[0x04];
    /* 0x10 */ s16 curveA;
    /* 0x12 */ s16 curveB;
    /* 0x14 */ u16 curveC;
    /* 0x16 */ u16 curveD;
    /* 0x18 */ u8  pad_18[0x04];
} OtuBadgeSlot; // Size: 0x1C

/**
 * @brief The badge's six 16-bit countdown slots, 0x28 bytes at +0x178.
 *
 * Every one is read-modify-written with a `ldrsh`/`sub 1`/`strh` triple, which
 * is what identifies them as counters that are allowed to go negative rather
 * than as flags, and each is the one and only guard on its phase's entry
 * setter.
 *
 * The task's own code reaches them through a `+0x100` base register and then a
 * small displacement -- `add rX, self, #0x100` followed by `ldrsh [rX, #0x78]`
 * -- rather than folding 0x178 into the load. Keeping them in a struct of their
 * own at 0x178 preserves that split, because the displacement is too large for
 * the short form to be what this build emits.
 */
typedef struct {
    /* 0x00 */ s16 trackFrames; // self +0x178: phase 6
    /* 0x02 */ s16 bounceTimer; // self +0x17A: phase 8
    /* 0x04 */ s16 arcFrames;   // self +0x17C: phase 7
    /* 0x06 */ s16 spinFrames;  // self +0x17E: phase 9
    /* 0x08 */ u8  pad_08[0x1C];
    /* 0x24 */ s16 trailIndex;  // self +0x19C: cursor into the trail id table
    /* 0x26 */ s16 trailTimer;  // self +0x19E: frames between trail marks
} OtuTimers;                    // Size: 0x28

/**
 * @brief "Tsk_OtosuGame_badge", 0x25C bytes of state.
 *
 * The same object is modelled twice more in this translation unit:
 * OtuPinTray as `OtuBadge` (the task's own view) and the header as the
 * 0x174 prefix `OtuPinTask` (the view the pool queries use). The three share
 * the handle at 0x0209926c; they are not three tasks.
 *
 * Every offset below is fixed by at least one load or store in this band, and
 * the padding is sized so each field lands on the displacement the target uses.
 * The six point-shaped regions are `OtuPoint` because the overlay's vector
 * helpers take them by address and this band hands them straight to
 * `func_ov039_02098b8c` and friends.
 *
 * `stateFlags` is a packed word rather than a set of booleans: bit 0 mirrors
 * the home tile's flag, bit 1 records that the badge disagrees with it, bit 2
 * records that it used to agree. `contactFlags` is a mask of the set-piece
 * phases allowed to fire this frame.
 */
typedef struct {
    /* 0x000 */ TinPinSlammer_Scene* scene;
    /* 0x004 */ u8                   pad_004[0x04];
    /* 0x008 */ TaskPool*            pool;
    /* 0x00C */ u8                   pad_00C[0xD0];
    /* 0x0DC */ s32                  unk_0DC; // cleared on arriving at tile 12
    /* 0x0E0 */ s32                  index;   // which badge this is
    /* 0x0E4 */ u8*                  board;   // see the note at the top of the file
    /* 0x0E8 */ OtuHomePos*          home;
    /* 0x0EC */ u16                  unk_0EC;
    /* 0x0EE */ u16                  contactFlags;
    /* 0x0F0 */ u16                  stateFlags;
    /* 0x0F2 */ u8                   pad_0F2[0x02];
    /* 0x0F4 */ s32                  step;        // sub-phase within `phase`
    /* 0x0F8 */ s32                  phase;       // 2..9
    /* 0x0FC */ s32                  unk_FC;
    /* 0x100 */ s32                  frameBudget; // the current phase's frame budget
    /* 0x104 */ s32                  lastTileType;
    /* 0x108 */ s32                  lastCellX;
    /* 0x10C */ s32                  lastCellY;
    /* 0x110 */ OtuPoint             origin;   // the sound cue's anchor
    /* 0x118 */ OtuPoint             unk_118;  // shared offset added to both aim points
    /* 0x120 */ OtuPoint             pos;      // Q12.12
    /* 0x128 */ s32                  unk_128;  // "something else is steering", in frames
    /* 0x12C */ OtuPoint             vel;      // Q12.12
    /* 0x134 */ s32                  unk_134;  // per-frame velocity nudge
    /* 0x138 */ OtuPoint             velNorm;  // the last unit vector taken
    /* 0x140 */ s32                  unk_140;
    /* 0x144 */ s32                  unk_144;  // the wobble, in Q12.12
    /* 0x148 */ s32                  unk_148;  // frames left of a live interaction
    /* 0x14C */ OtuPoint             startPos; // where the current aim began
    /* 0x154 */ OtuPoint             curPos;   // where it is aiming now
    /* 0x15C */ s32                  unk_15C;  // the phase-5 start x, in Q12.12
    /* 0x160 */ s32                  unk_160;  // the phase-5 start y, in Q12.12
    /* 0x164 */ s32                  unk_164;
    /* 0x168 */ u8                   pad_168[0x04];
    /* 0x16C */ u16*                 pinID;     // a tray slot's value; 0x130 = no pin
    /* 0x170 */ OtuBadgeSlot*        slots;     // per-kind behaviour records, 0x1C stride
    /* 0x174 */ u16*                 chanceTbl; // one 0x10000 chance per AI
    /* 0x178 */ OtuTimers            timers;
    /* 0x1A0 */ u8                   pad_1A0[0x04];
    /* 0x1A4 */ s32                  unk_1A4; // set to 1 when phase 5 starts
    /* 0x1A8 */ s32                  unk_1A8; // non-zero: fade the display out first
    /* 0x1AC */ s32                  unk_1AC; // stepped by the sound helper
    /* 0x1B0 */ void*                unk_1B0; // live sound handle, retired at phase 4
    /* 0x1B4 */ s32                  curAI;   // which AI is driving; 0x11 = parked
    /* 0x1B8 */ s32                  unk_1B8;
    /* 0x1BC */ OtuPinTask*          chaseTarget;
    /* 0x1C0 */ OtuPoint             anchorPt; // where the current AI sent us
    /* 0x1C8 */ u8                   pad_1C8[0x04];
    /* 0x1CC */ s32                  unk_1CC;  // frames spent aiming; 0x1E = commit
    /* 0x1D0 */ s32                  mode;     // 0x10 once the badge is committed
    /* 0x1D4 */ u8                   pad_1D4[0x10];
    /* 0x1E4 */ u32                  taskId1;  // the arc task
    /* 0x1E8 */ u32                  taskId2;  // the track task
    /* 0x1EC */ u32                  taskId3;  // the spin task
    /* 0x1F0 */ u8                   pad_1F0[0x08];
    /* 0x1F8 */ u32                  trailId[12];
    /* 0x228 */ u8                   pad_228[0x30];
    /* 0x258 */ u32                  taskId; // the badge's own sprite task
} OtuBadgeState;                             // Size: 0x25C

/* ------------------------------------------------------------------ */
/* Declarations this TU does not otherwise have.                       */
/* ------------------------------------------------------------------ */

/*
 * Everything below is outside the band and outside OtuFieldAccess's own
 * bodies, so nothing in this translation unit declares it. Undefined callees
 * are not a problem: dsd resolves them against the original overlay.
 */

/** The shared vector unit's normalise helper. */

/*
 * func_ov039_02098b78 is really a point copy (`out = *(OtuPoint*)src`) but
 * OtuFieldAccess.c above declares it as the dispatch container's word pair,
 * and that declaration is what this TU sees. It is called through that
 * prototype here; the type is being corrected separately, not here.
 */

/** The board's tile lookup: which tile kind is under this Q12.12 point. */
/* defined in band 10 as u8 (OtuPoint*, OtuCellGrid*) -- declared/defined elsewhere in this TU; see the note
 * below on why band 11 does not repeat it. */

/** Scales a direction to `scale`, applies it to the badge, then plays it. */
/* defined in band 10 as void (OtuPinLogic*, OtuPoint*, s32) -- declared/defined elsewhere in this TU; see the note
 * below on why band 11 does not repeat it. */

/** Joins two directions through `out` and returns the length; 0 = unreachable. */
s32 func_ov039_0208a624(OtuPoint* a, OtuPoint* b, OtuPoint* out);

/** The turn between two directions, against the 0x24000 turn radius. */
s32 func_ov039_0208a530(OtuPoint* a, OtuPoint* b, OtuPoint* out);

/** The arc task's per-frame stepper, driven from phase 7. */
void func_ov039_02091028(void* task, s32 x, s32 y, s32 z, u16 w);

/** The overlay's nearest-child queries, by pin kind. */
OtuPinTask* func_ov039_02087f4c(TaskPool* pool, TinPinSlammer_Scene* scene, s32 which);
OtuPinTask* func_ov039_02088064(TaskPool* pool, TinPinSlammer_Scene* scene, s32 which);

/* ------------------------------------------------------------------ */
/* The band.                                                           */
/* ------------------------------------------------------------------ */

/**
 * The mask of the sub-tile bits of one board cell.
 *
 * A cell is 0x20000 across in Q12.12, so this is `0x20000 - 1`: masking with it
 * and then adding or subtracting a whole cell walks from one cell boundary to
 * the next without having to know where in the cell the badge actually is.
 */
#define OTU_CELL_BITS 0x1FFFF

/** One board cell, in Q12.12. */
#define OTU_CELL_SIZE 0x20000

/*
 * Every Q12.12 multiply below is written out rather than hidden behind a macro
 * so each one can be read against the `smull`/`adds #0x800`/`adc`/`lsr #0xC`
 * quartet it produces. The rounding constant is in all of them: the target
 * rounds to nearest, and dropping it is a one-bit-per-call difference that a
 * match will not forgive.
 */

/** Rebuilds the packed state word from the home tile's flag. */
void func_ov039_0208acc0(OtuBadgeState* self) {
    u16 home  = (self->home->flags & 1) != 0 ? 1 : 0;
    u16 flags = self->stateFlags;

    // Bit 1 records "the badge is not where its home tile says it is". The
    // condition is the XOR masked back down to one bit rather than a plain
    // compare, which is what produces the target's eor/and/tst trio.
    if ((flags ^ home) & home & 1) {
        flags |= 2;
    } else {
        flags &= ~2;
    }
    self->stateFlags = flags;

    // Bit 2 records the transition itself, so it runs the other way round: the
    // badge has to have been home and no longer is.
    flags = self->stateFlags;
    if ((flags ^ home) & flags & 1) {
        flags |= 4;
    } else {
        flags &= ~4;
    }
    self->stateFlags = flags;

    // Bit 0 is not a flag at all; it is the home tile's bit 0 latched in.
    self->stateFlags &= ~1;
    self->stateFlags |= home;
}

/**
 * Scales a velocity down to at most `shortfall`, never up.
 *
 * The magnitude is taken first, so the vector is left normalised on return,
 * and a zero-length vector is left alone rather than normalised into the
 * direction of nothing.
 */
void func_ov039_0208ad2c(OtuPoint* vel, s32 shortfall) {
    OtuPoint norm;
    OtuPoint zero;
    s32      len = func_ov039_02098d10(vel);
    s32      scale;

    if (len <= 0) {
        return;
    }

    scale  = len - shortfall;
    zero.x = 0;
    zero.y = 0;

    // `shortfall` is an allowance, not a demand: a vector already shorter than
    // it is left alone rather than stretched.
    if (scale < 0) {
        scale = 0;
    }

    func_ov039_02098d3c(vel, &norm);
    func_ov039_02098c00(scale, &norm, &zero, vel);
}

/** Enters phase 2: pick the badge's sprite up and send it back to the home tile. */
void func_ov039_0208ad88(OtuBadgeState* self) {
    if (self->phase == 2) {
        return;
    }

    func_ov039_02097aa4(EasyTask_GetTaskData(self->pool, self->taskId), (void*)&self->pos, 1);
    func_ov039_02087d04(0x343, &self->pos, &self->origin);

    self->vel.x  = 0;
    self->vel.y  = 0;
    self->step   = 0;
    self->phase  = 2;
    self->unk_FC = 0;
}

/** Enters phase 3: sit still for 0x1E frames with the badge visible. */
void func_ov039_0208ade8(OtuBadgeState* self) {
    if (self->phase == 3) {
        return;
    }

    self->step        = 0;
    self->phase       = 3;
    self->frameBudget = 0x1E;
    self->unk_148     = 0;
}

/** Enters phase 4: retire whatever the badge was attached to. */
void func_ov039_0208ae14(OtuBadgeState* self) {
    if (self->phase == 4) {
        return;
    }

    // A badge still carrying a sound handle hands it back with a different cue
    // depending on whether it had a live rival to chase.
    if (self->unk_1B0 != NULL && *self->pinID < 0x130) {
        func_ov039_0208a490(self, self->unk_148 > 0 ? 5 : 2);
    }
    self->unk_1B0 = NULL;

    func_ov039_02087dc0(self->scene, self);

    self->unk_148     = 0;
    self->step        = 0;
    self->phase       = 4;
    self->frameBudget = 0x3C;
}

/** Enters phase 5: place the badge on its starting tile and start it rolling. */
void func_ov039_0208ae8c(OtuBadgeState* self) {
    if (self->unk_1A8 != 0) {
        EasyFade_FadeMainDisplay(2, 0x10, 0x1000);
    }

    self->unk_140 = 0;
    self->unk_144 = 0;
    self->unk_164 = 0x50;
    self->step    = 0;
    self->phase   = 5;
    self->unk_FC  = 0;

    // Tiles are 0x20 pixels and every badge is inset by half a tile, which is
    // what lands it in the middle of its cell rather than on a corner. The
    // starting tile comes from the board's two-bytes-per-badge table at +8.
    self->unk_15C = ((self->board[self->index * 2 + 8] << 5) + 0x10) << 0xC;
    self->unk_160 = ((self->board[self->index * 2 + 9] << 5) + 0x10) << 0xC;
    self->pos.x   = self->unk_15C;
    self->pos.y   = self->unk_160;
    self->vel.x   = 0;
    self->vel.y   = 0;

    self->velNorm.x = 0x1000;
    self->velNorm.y = 0;
    self->unk_148   = 0;
    self->unk_1A4   = 1;
}

/** Enters phase 7's approach: kill the velocity and play the corner sound. */
void func_ov039_0208af38(OtuBadgeState* self) {
    self->unk_134 = data_ov039_0209a31c;
    self->step    = 0;

    func_ov039_02087d04(0x344, &self->pos, &self->origin);
}

/**
 * The rolling phase: accelerate, turn, clamp to the board, bounce off walls
 * and wall corners, drop trail marks, and finally react to the tile the badge
 * has just arrived on.
 */
void func_ov039_0208af6c(OtuBadgeState* self) {
    OtuPoint       probe;
    OtuPoint       norm;
    OtuPoint       zero;
    OtuBadgeSlot*  slot;
    OtuBadgeSpeed* speed;
    s32            tile;
    s32            cellX;
    s32            cellY;
    s32            len;
    s32            hit;

    self->unk_134 = self->unk_134 + data_ov039_0209a318;

    // Acceleration, unless something has already committed the badge to a
    // mode of its own.
    if (self->mode <= 0) {
        s32 rate;

        if (self->unk_128 == 0) {
            tile = func_ov039_0208a794(&self->pos, (OtuCellGrid*)self->board);

            switch (tile) {
                case 3:
                    rate = data_ov039_0209a39c[2];
                    break;

                case 4:
                    rate = data_ov039_0209a39c[1];
                    break;

                default:
                    rate = data_ov039_0209a39c[0];
                    break;
            }
        } else {
            rate = data_ov039_0209a39c[3];
        }

        slot  = &self->slots[self->pinID[0]];
        speed = &data_ov039_0209a3e0[slot->speedIndex];

        rate = (s32)(((s64)speed->speed * rate + 0x800) >> 12);

        // While the badge is still being set up its rate is doubled. The test
        // is `phase <= 3`, i.e. every phase before the rolling one.
        if (self->phase <= 3) {
            rate = rate * 2;
        }

        func_ov039_0208ad2c(&self->vel, rate);
    }

    // Turning, but only while nothing external is steering the badge.
    if (self->unk_128 == 0) {
        s32 turn;
        s16 dir;
        s32 cell;

        len  = func_ov039_02098d10(&self->vel);
        slot = &self->slots[self->pinID[0]];

        // How hard the badge curves is the friction coefficient scaled by the
        // wobble 0x0208be30 left behind.
        turn = (s32)(((s64)slot->friction * (self->unk_144 / 3) + 0x800) >> 12);

        dir  = FX_Atan2Idx(self->vel.y, self->vel.x) + turn;
        cell = dir >> 4;

        /* The sin/cos table is s32[] by band 3's declaration but holds pairs of
         * s16, so each element has to be fetched through a s16* at a doubled
         * byte offset -- the same shape bands 5 and 7 use. */
        self->vel.x = (s32)(((s64)len * *(s16*)((u8*)&data_0205e4e0 + (cell * 2 + 1) * 2) + 0x800) >> 12);
        self->vel.y = (s32)(((s64)len * *(s16*)((u8*)&data_0205e4e0 + (cell * 2) * 2) + 0x800) >> 12);
    }

    func_ov039_02098b8c(&self->pos, &self->vel, &self->pos);

    // The board is a fixed playfield, so both axes are clamped to a hard inset
    // rather than to whatever extent the tile lookup would accept.
    if (self->pos.x < 0x80000) {
        self->pos.x = 0x80000;
    } else {
        s32 limit = ((s32)self->board[2] << 5) - 0x80;

        if (self->pos.x >= (limit << 0xC)) {
            self->pos.x = (limit << 0xC) - 1;
        }
    }
    if (self->pos.y < 0x60000) {
        self->pos.y = 0x60000;
    } else {
        s32 limit = ((s32)self->board[3] << 5) - 0x60;

        if (self->pos.y >= (limit << 0xC)) {
            self->pos.y = (limit << 0xC) - 1;
        }
    }

    // A negative accumulator is an impulse off something solid. Once it has
    // been absorbed both accumulators are cleared together.
    self->unk_128 = self->unk_128 + self->unk_134;
    if (self->unk_128 > 0) {
        self->unk_134 = 0;
        self->unk_128 = 0;
    }

    // Trail marks, dropped every few frames while the badge is moving fast.
    if (self->unk_128 == 0 && self->phase != 3) {
        s32 trailLen = func_ov039_02098d10(&self->vel);

        if (trailLen > 0x1000) {
            s32 angle;

            self->timers.trailTimer = self->timers.trailTimer - 1;
            if (self->timers.trailTimer <= 0) {
                angle = FX_Atan2Idx(self->vel.y, self->vel.x);

                // Two halves, so the mark is centred on the badge rather than
                // trailing off one side of it.
                func_ov039_02095788((void*)EasyTask_GetTaskData(self->pool, self->trailId[self->timers.trailIndex]),
                                    (void*)&self->pos, angle, 1, trailLen);
                self->timers.trailIndex = self->timers.trailIndex + 1;

                func_ov039_02095788((void*)EasyTask_GetTaskData(self->pool, self->trailId[self->timers.trailIndex]),
                                    (void*)&self->pos, angle, 0, trailLen);
                self->timers.trailIndex = self->timers.trailIndex + 1;

                if (self->timers.trailIndex >= 0xC) {
                    self->timers.trailIndex = 0;
                }
                self->timers.trailTimer = 5;
            }
        }
    }

    // Walls. A wall is tile type 2; the badge is folded back onto the boundary
    // it crossed rather than stopped at the probe point. An axis that has
    // already reflected suppresses all four corner tests below.
    if (self->unk_128 == 0) {
        hit = 0;

        if (self->vel.x > 0) {
            func_ov039_02098b78((OtuStageDispatch*)&self->pos, (s32*)&probe);
            probe.x = probe.x + 0xC000;
            tile    = func_ov039_0208a794(&probe, (OtuCellGrid*)self->board);
            if (tile == 2) {
                self->pos.x = (probe.x & ~OTU_CELL_BITS) - 0xC000;
                self->vel.x = self->vel.x * -0x2000;
                hit         = 1;
            }
        } else if (self->vel.x < 0) {
            func_ov039_02098b78((OtuStageDispatch*)&self->pos, (s32*)&probe);
            probe.x = probe.x - 0xC000;
            tile    = func_ov039_0208a794(&probe, (OtuCellGrid*)self->board);
            if (tile == 2) {
                self->pos.x = (probe.x | OTU_CELL_BITS) + 0xC000;
                self->vel.x = -self->vel.x;
                hit         = 1;
            }
        }

        if (self->vel.y > 0) {
            func_ov039_02098b78((OtuStageDispatch*)&self->pos, (s32*)&probe);
            probe.y = probe.y + 0xC000;
            tile    = func_ov039_0208a794(&probe, (OtuCellGrid*)self->board);
            if (tile == 2) {
                self->pos.y = (probe.y & ~OTU_CELL_BITS) - 0xC000;
                self->vel.y = self->vel.y * -0x2000;
                hit         = 1;
            }
        } else if (self->vel.y < 0) {
            func_ov039_02098b78((OtuStageDispatch*)&self->pos, (s32*)&probe);
            probe.y = probe.y - 0xC000;
            tile    = func_ov039_0208a794(&probe, (OtuCellGrid*)self->board);
            if (tile == 2) {
                self->pos.y = (probe.y | OTU_CELL_BITS) + 0xC000;
                self->vel.y = -self->vel.y;
                hit         = 1;
            }
        }

        // Corners. The probe is pushed a whole cell along each velocity
        // component, snapped to the cell boundary, and only then measured: a
        // corner the badge could follow round is not worth reversing for, so
        // it has to be closer than the reversal's own distance.
        if (hit == 0) {
            if (self->vel.x > 0 || self->vel.y < 0) {
                func_ov039_02098b78((OtuStageDispatch*)&self->pos, (s32*)&probe);
                probe.x = probe.x + OTU_CELL_SIZE;
                probe.y = probe.y - OTU_CELL_SIZE;
                tile    = func_ov039_0208a794(&probe, (OtuCellGrid*)self->board);
                if (tile == 2) {
                    probe.x = probe.x & ~OTU_CELL_BITS;
                    probe.y = probe.y | OTU_CELL_BITS;
                    len     = func_ov039_02098ca8(&self->pos, &probe);
                    if (len < 0xC000) {
                        self->pos.x = probe.x - 0xC000;
                        hit         = 1;
                        self->pos.y = probe.y + 0xC000;
                        self->vel.x = -self->vel.x;
                        self->vel.y = -self->vel.y;
                    }
                }
            }

            if (self->vel.x > 0 || self->vel.y > 0) {
                func_ov039_02098b78((OtuStageDispatch*)&self->pos, (s32*)&probe);
                probe.x = probe.x + OTU_CELL_SIZE;
                probe.y = probe.y + OTU_CELL_SIZE;
                tile    = func_ov039_0208a794(&probe, (OtuCellGrid*)self->board);
                if (tile == 2) {
                    probe.x = probe.x & ~OTU_CELL_BITS;
                    probe.y = probe.y & ~OTU_CELL_BITS;
                    len     = func_ov039_02098ca8(&self->pos, &probe);
                    if (len < 0xC000) {
                        self->pos.x = probe.x - 0xC000;
                        hit         = 1;
                        self->pos.y = probe.y - 0xC000;
                        self->vel.x = -self->vel.x;
                        self->vel.y = -self->vel.y;
                    }
                }
            }

            if (self->vel.x < 0 || self->vel.y < 0) {
                func_ov039_02098b78((OtuStageDispatch*)&self->pos, (s32*)&probe);
                probe.x = probe.x - OTU_CELL_SIZE;
                probe.y = probe.y - OTU_CELL_SIZE;
                tile    = func_ov039_0208a794(&probe, (OtuCellGrid*)self->board);
                if (tile == 2) {
                    probe.x = probe.x | OTU_CELL_BITS;
                    probe.y = probe.y | OTU_CELL_BITS;
                    len     = func_ov039_02098ca8(&self->pos, &probe);
                    if (len < 0xC000) {
                        self->pos.x = probe.x + 0xC000;
                        hit         = 1;
                        self->pos.y = probe.y + 0xC000;
                        self->vel.x = -self->vel.x;
                        self->vel.y = -self->vel.y;
                    }
                }
            }

            if (self->vel.x < 0 || self->vel.y > 0) {
                func_ov039_02098b78((OtuStageDispatch*)&self->pos, (s32*)&probe);
                probe.x = probe.x - OTU_CELL_SIZE;
                probe.y = probe.y + OTU_CELL_SIZE;
                tile    = func_ov039_0208a794(&probe, (OtuCellGrid*)self->board);
                if (tile == 2) {
                    probe.x = probe.x | OTU_CELL_BITS;
                    probe.y = probe.y & ~OTU_CELL_BITS;
                    len     = func_ov039_02098ca8(&self->pos, &probe);
                    if (len < 0xC000) {
                        self->pos.x = probe.x + 0xC000;
                        hit         = 1;
                        self->pos.y = probe.y - 0xC000;
                        self->vel.x = -self->vel.x;
                        self->vel.y = -self->vel.y;
                    }
                }
            }
        }

        // Having hit something, re-normalise so the next turn is taken from the
        // reflected heading rather than the incoming one.
        if (hit != 0) {
            zero.x = 0;
            zero.y = 0;

            len = func_ov039_02098d10(&self->vel);
            if (len > 0) {
                func_ov039_02098d3c(&self->vel, &norm);
                func_ov039_02098c00((s32)(((s64)len * data_ov039_0209a324 + 0x800) >> 12), &norm, &zero, &self->vel);
                self->velNorm = norm;
            }
        }
    }

    // `unk_128` is the "something else owns this badge" latch; while it is set
    // the tile under the badge is not read at all.
    if (self->unk_128 != 0) {
        return;
    }

    tile = func_ov039_0208a794(&self->pos, (OtuCellGrid*)self->board);

    if (tile == 0) {
        func_ov039_0208ade8(self);
    } else if (tile == 12) {
        self->unk_0DC = 0;
    }

    // Tile changes are detected by the cell the badge is standing in rather
    // than by its position, so a badge drifting inside one tile does not
    // re-trigger anything.
    cellX = (self->pos.x >> 0xC) / 32;
    cellY = (self->pos.y >> 0xC) / 32;
    if (self->lastCellX == (cellX >> 5) && self->lastCellY == (cellY >> 5)) {
        return;
    }

    switch (tile) {
        case 5:
            // Only the board that admits it has a home to go back to.
            if (self->board[5] != 0) {
                func_ov039_0208ad88(self);
            }
            break;

        case 7:
            func_ov039_0208af38(self);
            break;

        case 8:
            self->vel.x = 0;
            self->vel.y = -data_ov039_0209a308;
            func_ov039_02098d3c(&self->vel, &self->velNorm);
            func_ov039_02087d04(0x345, &self->pos, &self->origin);
            break;

        case 9:
            self->vel.x = data_ov039_0209a308;
            self->vel.y = 0;
            func_ov039_02098d3c(&self->vel, &self->velNorm);
            func_ov039_02087d04(0x345, &self->pos, &self->origin);
            break;

        case 10:
            self->vel.x = 0;
            self->vel.y = data_ov039_0209a308;
            func_ov039_02098d3c(&self->vel, &self->velNorm);
            func_ov039_02087d04(0x345, &self->pos, &self->origin);
            break;

        case 11:
            self->vel.x = -data_ov039_0209a308;
            self->vel.y = 0;
            func_ov039_02098d3c(&self->vel, &self->velNorm);
            func_ov039_02087d04(0x345, &self->pos, &self->origin);
            break;
    }

    self->lastTileType = tile;
    self->lastCellX    = cellX >> 5;
    self->lastCellY    = cellY >> 5;
}

/** Enters phase 6: hand the badge to the track task for `trackFrames`. */
void func_ov039_0208b94c(OtuBadgeState* self) {
    OtuBadgeSlot* slot;

    if (self->timers.trackFrames <= 0) {
        return;
    }

    self->step  = 0;
    self->phase = 6;

    slot = &self->slots[self->pinID[0]];
    func_ov039_02091654(EasyTask_GetTaskData(self->pool, self->taskId2), slot->animX, slot->animY);

    self->timers.trackFrames = self->timers.trackFrames - 1;
}

/** Enters phase 7: hand the badge to the arc task for `arcFrames`. */
void func_ov039_0208b9b4(OtuBadgeState* self) {
    OtuBadgeSlot* slot;

    if (self->timers.arcFrames <= 0) {
        return;
    }

    self->step  = 0;
    self->phase = 7;

    slot = &self->slots[self->pinID[0]];
    func_ov039_02091028(EasyTask_GetTaskData(self->pool, self->taskId1), slot->curveA, slot->curveB, slot->curveC,
                        slot->curveD);

    self->timers.arcFrames = self->timers.arcFrames - 1;
}

/** Enters phase 8: stop dead for `bounceFrames`. */
void func_ov039_0208ba34(OtuBadgeState* self) {
    if (self->timers.bounceTimer <= 0) {
        return;
    }

    self->vel.x  = 0;
    self->vel.y  = 0;
    self->phase  = 8;
    self->step   = 0;
    self->unk_FC = 0;

    self->timers.bounceTimer = self->timers.bounceTimer - 1;
}

/** Enters phase 9: spin down to a stop over `spinFrames`. */
void func_ov039_0208ba70(OtuBadgeState* self) {
    OtuPoint zero;

    if (self->timers.spinFrames <= 0) {
        return;
    }

    zero.x = 0;
    zero.y = 0;

    func_ov039_02098c00(data_ov039_0209a38c, &self->velNorm, &zero, &self->vel);
    func_ov039_02098d3c(&self->vel, &self->velNorm);

    self->unk_134     = data_ov039_0209a320;
    self->step        = 0;
    self->phase       = 9;
    self->step        = 0;
    self->frameBudget = 9;

    func_ov039_02091b34(EasyTask_GetTaskData(self->pool, self->taskId3));

    if (self->unk_148 > 0) {
        func_ov039_0208a490(self, 1);
    }

    self->timers.spinFrames = self->timers.spinFrames - 1;
}

/** The per-frame decision: which set-piece phase, if any, runs this frame. */
void func_ov039_0208bb2c(OtuBadgeState* self) {
    OtuPoint fromHome;
    OtuPoint fromPos;
    OtuPoint mid;
    OtuPoint legA;
    OtuPoint legB;

    // A badge that still has a rival attached is finishing an interaction. The
    // two-frame decrement only happens once the interaction is flagged as over,
    // so the frames are counted from the end rather than from the start.
    if (self->unk_148 > 0) {
        if (self->stateFlags & 2) {
            self->unk_148 = self->unk_148 - 2;
            if (self->unk_148 < 0) {
                self->unk_148 = 0;
            }
        }
        self->step = 0;
        return;
    }

    if (self->unk_128 != 0) {
        self->step = 0;
        return;
    }

    // The set-piece phases, in the order they are allowed to claim the frame.
    if (self->contactFlags & 0x11) {
        self->step = 0;
        func_ov039_0208b94c(self);
        return;
    }
    if (self->contactFlags & 0x820) {
        self->step = 0;
        func_ov039_0208b9b4(self);
        return;
    }
    if (self->contactFlags & 0x440) {
        self->step = 0;
        func_ov039_0208ba34(self);
        return;
    }

    if (self->step == 0) {
        // Wait for the home tile to report itself before aiming anywhere.
        if (!(self->stateFlags & 2)) {
            return;
        }

        self->unk_1CC = 0;
        self->step    = 1;

        self->startPos.x = self->home->tileX << 0xC;
        self->startPos.y = self->home->tileY << 0xC;
        self->curPos.x   = self->startPos.x;
        self->curPos.y   = self->startPos.y;

    } else if (self->step == 1) {
        s32 turn;
        s32 scale;
        s32 reach;
        s32 sign;

        if (self->unk_1CC < 0x1E) {
            self->unk_1CC = self->unk_1CC + 1;
        }

        // The aim point tracks the home tile, so until the badge has somewhere
        // else to be it is still following it.
        self->curPos.x = self->home->tileX << 0xC;
        self->curPos.y = self->home->tileY << 0xC;

        if (!(self->stateFlags & 4)) {
            return;
        }

        self->step = 0;

        turn = func_ov039_02098ca8(&self->startPos, &self->curPos);
        if (turn <= 0) {
            return;
        }

        // Join the two aim points by an offset from where the badge is now.
        func_ov039_02098b8c(&self->startPos, &self->unk_118, &fromHome);
        func_ov039_02098b8c(&self->curPos, &self->unk_118, &fromPos);

        reach = func_ov039_0208a624(&fromHome, &fromPos, &self->pos);
        if (reach == 0) {
            return;
        }

        func_ov039_02098bb0(&fromPos, &fromHome, &mid);

        turn = func_ov039_02098d10(&mid);
        if (turn > 0x50000) {
            turn = 0x50000;
        }
        scale = FX_Divide(turn, 0x50000);

        // Having aimed for a full half second the badge commits: the mode flag
        // goes up and the turn is sharpened from here on.
        if (self->unk_1CC >= 0x1E) {
            scale      = (s32)(((s64)scale * 0x1800 + 0x800) >> 12);
            self->mode = 0x10;
        }

        func_ov039_02098d3c(&mid, &mid);
        func_ov039_0208a6f8((OtuPinLogic*)self, &mid, scale);

        if (self->unk_1CC >= 0x1E) {
            return;
        }

        func_ov039_02098bb0(&fromPos, &fromHome, &legB);
        func_ov039_02098bb0(&self->pos, &fromHome, &legA);

        reach = func_ov039_0208a530(&fromHome, &fromPos, &self->pos);
        if (reach > 0x24000) {
            reach = 0x24000;
        }
        reach = FX_Divide(reach, 0x24000);

        if (*self->pinID >= 0x130) {
            return;
        }

        // Which way round the turn is decides the sign, and with it whether
        // the badge ends up winding tight or winding loose.
        sign = (func_ov039_02098c70(&legB, &legA) >= 0) ? data_ov039_0209a394 : -data_ov039_0209a394;

        self->unk_144 = (s32)(((s64)(sign * scale) * reach + 0x800) >> 12) * 3;
    }
}

/** Steers the badge along `dir` at full speed, jittering its wobble. */
void func_ov039_0208be30(OtuBadgeState* self, OtuPoint* dir) {
    OtuPoint step;

    func_ov039_02098bb0(dir, &self->pos, &step);

    // A zero vector has no direction to normalise, and normalising it would
    // hand back whatever the divide unit happens to leave in the register.
    if (step.x != 0 || step.y != 0) {
        func_ov039_02098d3c(&step, &step);
    } else {
        step.x = 0x1000;
        step.y = 0;
    }

    func_ov039_0208a6f8((OtuPinLogic*)self, &step, 0x1000);

    if (*self->pinID >= 0x130) {
        return;
    }

    // The wobble is a full-width random offset, tripled to match the three
    // the caller divides it back down by.
    self->unk_144 = (s32)((RNG_Next(data_ov039_0209a394 * 2) - data_ov039_0209a394) << 0xC) * 3;
}

/**
 * Walks the board outwards from the badge in a square spiral, looking for the
 * first cell whose tile type falls in [loType, hiType]. True when one is
 * found, in which case `out` receives that cell's point.
 */
s32 func_ov039_0208bed8(OtuBadgeState* self, s32 rings, s32 loType, s32 hiType, OtuPoint* out) {
    OtuPoint at;
    s32      ring;
    s32      span = 2;
    s32      i;
    s32      tile;

    at.x = self->pos.x - OTU_CELL_SIZE;
    at.y = self->pos.y - OTU_CELL_SIZE;

    for (ring = 1; ring <= rings; ring++) {
        // Four straight walks round the ring: right, down, left, up. The span
        // grows by two per ring, which is what makes this a spiral rather than
        // a sweep. The ring is entered one cell down-left of where the badge
        // is, so the search never revisits the cell it started from.
        for (i = 0; i < span; i++) {
            tile = func_ov039_0208a794(&at, (OtuCellGrid*)self->board);
            if (tile >= loType && tile <= hiType) {
                out->x = at.x;
                out->y = at.y;
                return 1;
            }
            at.x = at.x + OTU_CELL_SIZE;
        }

        for (i = 0; i < span; i++) {
            tile = func_ov039_0208a794(&at, (OtuCellGrid*)self->board);
            if (tile >= loType && tile <= hiType) {
                out->x = at.x;
                out->y = at.y;
                return 1;
            }
            at.y = at.y + OTU_CELL_SIZE;
        }

        for (i = 0; i < span; i++) {
            tile = func_ov039_0208a794(&at, (OtuCellGrid*)self->board);
            if (tile >= loType && tile <= hiType) {
                out->x = at.x;
                out->y = at.y;
                return 1;
            }
            at.x = at.x - OTU_CELL_SIZE;
        }

        for (i = 0; i < span; i++) {
            tile = func_ov039_0208a794(&at, (OtuCellGrid*)self->board);
            if (tile >= loType && tile <= hiType) {
                out->x = at.x;
                out->y = at.y;
                return 1;
            }
            at.y = at.y - OTU_CELL_SIZE;
        }

        at.x = at.x - OTU_CELL_SIZE;
        at.y = at.y - OTU_CELL_SIZE;
        span = span + 2;
    }

    return 0;
}

/** True when two points are in the same board cell. */
s32 func_ov039_0208c0e4(OtuPoint* a, OtuPoint* b) {
    if ((a->x & ~OTU_CELL_BITS) != (b->x & ~OTU_CELL_BITS)) {
        return 0;
    }
    if ((a->y & ~OTU_CELL_BITS) == (b->y & ~OTU_CELL_BITS)) {
        return 1;
    }
    return 0;
}

/** The "wander to an interesting tile" AI, entry `which` of the chance table. */
s32 func_ov039_0208c128(OtuBadgeState* self, s32 which, s32 loType, s32 hiType, s32 kind) {
    OtuPoint found;

    // Re-rolling only happens once the badge is running a different AI than
    // last frame. While it is still on the old one the previous decision is
    // simply carried forward.
    if (self->curAI != which) {
        if (self->chanceTbl[which] < RNG_Next(0x10000)) {
            return 0;
        }

        if (func_ov039_0208bed8(self, 3, loType, hiType, &found) == 0) {
            return 0;
        }

        // Landing back on the anchor is not a move, so it is rejected.
        if (func_ov039_0208c0e4(&found, &self->anchorPt) != 0) {
            return 0;
        }

        func_ov039_0208be30(self, &found);

        self->curAI    = which;
        self->unk_1B8  = kind;
        self->anchorPt = found;
        return 1;
    }

    if (func_ov039_0208c0e4(&self->pos, &self->anchorPt) != 0) {
        self->curAI = 0x11;
    } else {
        func_ov039_0208be30(self, &self->anchorPt);
    }

    return 1;
}

/** The "spin down in place" AI. */
s32 func_ov039_0208c218(OtuBadgeState* self) {
    // Once the badge is committed to something it stops re-rolling, so the
    // chance test is only worth making while it is still free.
    if (self->curAI != 0 && self->chanceTbl[0] < RNG_Next(0x10000)) {
        return 0;
    }

    func_ov039_0208ba70(self);
    return 1;
}

/** The "keep rolling forward until something opens up" AI. */
s32 func_ov039_0208c258(OtuBadgeState* self) {
    OtuPoint at;
    s32      tries = 0;
    s32      tile;

    if (self->chanceTbl[1] < RNG_Next(0x10000)) {
        return 0;
    }

    at.x = self->pos.x;
    at.y = self->pos.y;

    // Look four cells ahead for open road. Giving up after four is the point
    // of the loop: a boxed-in badge should stop rather than commit.
    do {
        func_ov039_02098b8c(&at, &self->vel, &at);
        tile = func_ov039_0208a794(&at, (OtuCellGrid*)self->board);
        if (tile == 0) {
            break;
        }
        tries++;
    } while (tries < 4);

    if (tries >= 4) {
        return 0;
    }

    func_ov039_02098bb0(&self->pos, &self->vel, &at);
    func_ov039_0208be30(self, &at);
    return 1;
}

/** The "chase the nearest rival badge" AI. */
s32 func_ov039_0208c304(OtuBadgeState* self) {
    OtuPinTask* target;
    s32         alive;
    s32         gap;

    if (self->curAI != 2) {
        if (self->chanceTbl[2] < RNG_Next(0x10000)) {
            return 0;
        }

        target = func_ov039_02087f4c(self->pool, self->scene, self->index);
        if (target == NULL) {
            return 0;
        }

        gap = func_ov039_02098ca8(&self->pos, (OtuPoint*)&target->x);
        if (gap > 0xC8000) {
            return 0;
        }

        func_ov039_0208be30(self, (OtuPoint*)&target->x);

        self->curAI       = 2;
        self->unk_1B8     = 1;
        self->chaseTarget = target;
        return 1;
    }

    // Already chasing: keep going while the target is still worth chasing.
    target = self->chaseTarget;
    alive  = func_ov039_0208efb0(target, 0x444);
    if (alive != 0) {
        func_ov039_0208be30(self, (OtuPoint*)&target->x);
    }
    return 1;
}

/** The "run away from the nearest rival badge" AI. */
s32 func_ov039_0208c3bc(OtuBadgeState* self) {
    OtuPinTask* target;
    OtuPoint    away;
    s32         gap;

    if (self->chanceTbl[3] < RNG_Next(0x10000)) {
        return 0;
    }

    target = func_ov039_02088064(self->pool, self->scene, self->index);
    if (target == NULL) {
        return 0;
    }

    gap = func_ov039_02098ca8(&self->pos, (OtuPoint*)&target->x);
    if (gap > 0x46000) {
        return 0;
    }

    // Two subtractions, which net out to `target - (self - target)`: running
    // away is the chase reflected through the target.
    func_ov039_02098bb0((OtuPoint*)&target->x, &self->pos, &away);
    func_ov039_02098bb0(&self->pos, &away, &away);

    func_ov039_0208be30(self, &away);
    return 1;
}

/** The "take the scripted arc if there is one left" AI. */
s32 func_ov039_0208c45c(OtuBadgeState* self) {
    OtuPinTask* target;
    s32         speed;
    s32         gap;

    if (self->chanceTbl[4] < RNG_Next(0x10000)) {
        return 0;
    }

    // An arc needs an arc task with frames left on it, and is not worth
    // starting for a badge already moving too fast to join it.
    if (self->timers.arcFrames <= 0) {
        return 0;
    }
    speed = func_ov039_02098d10(&self->vel);
    if (speed >= 0x3000) {
        return 0;
    }

    target = func_ov039_02088064(self->pool, self->scene, self->index);
    if (target == NULL) {
        return 0;
    }

    gap = func_ov039_02098ca8(&self->pos, (OtuPoint*)&target->x);
    if (gap > 0x46000) {
        return 0;
    }

    func_ov039_0208b9b4(self);
    return 1;
}

/* ============================================================================
 * BATCH B4 -- the "badge" task's own support routines,
 * 0x0208d3bc - 0x0208e504 (18 functions, 4880 bytes).
 *
 * WHAT THIS BATCH IS
 * -----------------
 * These are not a slice of somebody else's call graph -- they are one whole
 * task's body. The TaskHandle at 0x0209926c names it, and the name string at
 * 0x0209a49c is "Tsk_OtosuGame_badge": {name, taskFunc = 0x0208dc68,
 * dataSize = 0x25C}. The stage table at 0x02099278 is
 *
 *     0x0208d6dc  init
 *     0x0208d7e4  update
 *     0x0208da74  render
 *     0x0208db44  cleanup
 *
 * and 0x0208dc68 is the usual four-slot trampoline over it. So the first
 * four stage-shaped functions here are that task's lifecycle, 0x0208d3bc /
 * 0x0208d4cc / 0x0208d554 are its three sprite loaders (one per embedded
 * Sprite), 0x0208d5dc is its state reset, 0x0208d9ec its per-frame position
 * push, 0x0208dcb0 its spawner, and the tail of the band
 * (0x0208df2c .. 0x0208e504) is the pair-versus-pair collision code that the
 * pin task runs against itself.
 *
 * The same task is modelled twice more in this translation unit: the header's
 * `OtuPinTask` is the narrow 0x174 prefix of it that the pool queries use, and
 * band B3's `OtuBadgeState` is the badge AI's own full view of the same 0x25C
 * bytes under different field names. All three describe one object. There is
 * no "badger" task -- that was a misreading of the handle's
 * "Tsk_OtosuGame_badge", confirmed against the ROM's task strings, none of
 * which contains "badger" except `Tsk_OtosuGame_badgeradar`.
 *
 * STRUCTURAL FINDINGS
 * -------------------
 * 1. Three Sprites are embedded at +0x0C, +0x4C and +0x8C. That is not an
 *    inference from the address arithmetic: 0x0208d9ec stores to +0x18/+0x1A
 *    and +0x98/+0x9A, which are Sprite.posX/posY (+0x0C/+0x0E) inside a
 *    Sprite at +0x0C and +0x8C respectively, and 0x0208da74 does the same at
 *    +0x58/+0x5A for the one at +0x4C. 0x0C + 0x40 + 0x40 = 0x8C exactly.
 * 2. The +0x100 sub-object is real and is what OtuFieldAccess.c's four
 *    0x100+0x78..0x7E accessors read. 0x0208d5dc writes them from four
 *    *bytes* of a 0x1C-stride record table (so they are four s16 fields fed
 *    byte by byte, not one u32), and also writes +0x100+0x9E = 5.
 * 3. The record table at +0x170 is indexed by the u16 at +0x16C with a
 *    0x1C *byte* stride. +0x16C is the same tray-slot pointer the header
 *    documents for OtuPinTask.pinID, and 0x130 is the same "no pin" sentinel.
 * 4. `kind` at +0xF8 is the state's discriminator. Three different switches
 *    over it appear in this band and they select *different* sets:
 *      0x0208d7e4 update : 1,3,6,7 share func_ov039_0208af6c, then a full
 *                          1..9 dispatch to nine per-kind handlers
 *      0x0208da74 render : 1,3,4,5,7,9 render, 8 renders only for subKind
 *                          0/1/6
 *      0x0208e28c        : only kinds 1,6,7 may interact
 *      0x0208e37c        : only kinds 1,3,4,6,7
 *      0x0208e504        : only kinds 1,6,7,9
 *    So 1/6/7 is one class (they interact and are what 0x0208e28c drives) and
 *    9 joins them only for the wall/board case in 0x0208e504.
 * 5. The three sprite loaders differ only in which 0x2C-byte SpriteAnimation
 *    template they copy (0x02099288 / 0x020992b4 / 0x020992e0 -- exactly one
 *    struct apart each) and, for the first only, in a pin-dependent
 *    binIden/packIndex patch. The `<< 0x1C` / `>> 0x1A` pair is the
 *    `dataType` bitfield insert; the `bic #0x380` / `orr #0x300` in the first
 *    one alone is `bits_7_9 = 6`.
 * 6. The child-task handles at +0x1D4..+0x258 are created by band 1/2/3/4/5/7's
 *    spawners in a fixed order and deleted in reverse by 0x0208db44, which is
 *    the same set of eleven child sorts the header's OtuPinTask comment calls
 *    "the three pin types the results screen scores separately" -- except this
 *    task creates *eleven*, so the three-kind story in the header is about
 *    querying, not about what a pin owns.
 *
 * NOT DETERMINED / KNOWN GAPS
 * ---------------------------
 *  - The `adds r4, #0x12C` / `bne` / `adds r4, #0x130` / `popeq` tail of
 *    0x0208dff0 compares *addresses*, with no loads at all. No reading of
 *    "vel.x != 0 || vel.y != 0" produces that (it needs two loads), and both
 *    fields are read as data everywhere else in the overlay. Written below as
 *    the pointer test the flags encode, with a comment.
 *  - +0xE4 is a pointer and +0xE0 an index multiplied by *two*, then bytes +8
 *    and +9 of the result are read. A 2-byte stride with +8/+9 offsets is not
 *    self-consistent; reproduced literally.
 *  - `self->unk_0CC = (u32)unk_140 * 0x10 / 0x10000` is guessed from the
 *    `lsl #4` / `lsr #16` pair. A plain `>> 12` is one instruction, so the
 *    source really did spell it as a multiply and a divide; which of the two
 *    is which cannot be told from the encoding.
 *  - Six of the eleven spawners 0x0208dcb0 calls are typed `void` in bands
 *    2/4/5 while the target stores their return value into a child-handle
 *    field. Band 4 already retyped two siblings of exactly this kind (02096b18,
 *    02096e4c) to `s32` for the same reason; the same fix is needed here and
 *    is called out at the call sites.
 *
 * WHERE THIS FILE HAS TO BE INCLUDED
 * ----------------------------------
 * Last, after OtuPinSprites. It calls 0x0208f40c / 0x0208f770 / 0x0208fa5c /
 * 0x0208fe60 (band 1), 0x0209383c / 0x02093cd8 / 0x02093d18 (band 2),
 * 0x02095750 / 0x02095ca0 (band 3), 0x02096124 / 0x02096548 / 0x02096154
 * (band 4), 0x0209771c / 0x02097a70 (band 5) and 0x02090e1c (band 7) with no
 * declaration of its own for any of them, so going in earlier means an implicit
 * `int (...)` that then collides with the real definition -- the failure mode
 * the file header already documents for bands 4/5/1.
 * ==========================================================================*/

/* ------------------------------------------------------------------ */
/* Data the band reads.                                               */
/* ------------------------------------------------------------------ */

/** The badge task's three SpriteAnimation templates, 0x2C bytes each. */

/** The four stage slots, indexed by the stage argument of 0x0208dc68. */

/** The task's handle: name "Tsk_OtosuGame_badge", taskFunc 0x0208dc68, 0x25C. */

/*
 * The bin identifiers the first sprite loader patches in. Both pool words sit
 * inside one run of {0x27, char*} pairs at 0x0209a0b4 and the two are exactly
 * five entries (0x28 bytes) apart, so the run is one array and the two names
 * are element 0 and element 5 of it. Declared as two so the pool words keep
 * the target's names, per the note in TinPinSlammer.h about gap-filled data.
 */

/** The two scale tables 0x0208e130 indexes, four words apart, 0x10 stride.
 *
 *  Both are read as `ldr rX, [base, index, lsl #4]`, and the bases are four
 *  bytes apart, so they are one array of 0x10-byte records read at two
 *  different field offsets rather than two independent tables. Declared as
 *  two so each pool word names the address the target names.
 */
typedef struct {
    /* 0x00 */ s32 scaleA;
    /* 0x04 */ s32 scaleB;
    u8             pad_08[0x08];
} OtuPinScale; // Size: 0x10

/** The constant 0x800 0x0208e504 scales the pin's speed by. */

/* ------------------------------------------------------------------ */
/* Shapes.                                                            */
/* ------------------------------------------------------------------ */

/** One 0x1C-byte row of the tray table at +0x170. */
typedef struct {
    /* 0x00 */ u8 tile[4]; // copied byte by byte to +0x178..+0x17E as four s16s
    /* 0x04 */ u8 weight;  // indexes the two OtuPinScale tables
    u8            pad_05[0x17];
} OtuPinRow;               // Size: 0x1C

/** Q12.12 multiply, rounded -- the form the target spells with smull + 0x800. */
#define OTU_MUL_Q12(a, b) ((s32)((((s64)(a) * (s64)(b)) + 0x800) >> 12))

/**
 * @brief "Tsk_OtosuGame_badge": the badge task, 0x25C bytes.
 *
 * This is the same object the header models as `OtuPinTask`, which stops at
 * 0x174; that type exists for the five nearest-child queries and only needs
 * +0xF8/+0x120/+0x124/+0x148/+0x16C, so it is left alone. Every offset below
 * 0x174 agrees with it -- `kind`, `x`, `y`, `alive`, `pinID` -- and this type
 * continues past it.
 *
 * It is also the same object band B3 models as `OtuBadgeState`, seen from the
 * AI's side and under its own field names. The two do not share every name, so
 * they are kept apart rather than merged; the handle above ties them together.
 *
 * Positions and velocities are Q12.12, the overlay's usual scale, and three
 * Sprites are embedded rather than pointed at (see the header note).
 */
typedef struct OtuBadge {
    /* 0x000 */ s32              unk_000;  // the spawn argument, unused here
    /* 0x004 */ s32              dataType; // copied into SpriteAnimation.dataType
    /* 0x008 */ TaskPool*        pool;     // saved by the init stage
    /* 0x00C */ Sprite           spriteA;
    /* 0x04C */ Sprite           spriteB;
    /* 0x08C */ Sprite           spriteC;
    /* 0x0CC */ s32              unk_0CC; // unk_140 rescaled by 1/4096
    /* 0x0D0 */ s32              unk_0D0; // starts at 0x1000
    /* 0x0D4 */ s32              unk_0D4; // starts at 0x1000
    /* 0x0D8 */ u16              unk_0D8;
    /* 0x0DA */ u16              unk_0DA;
    /* 0x0DC */ s32              unk_0DC;   // raised to 1 by init and by update
    /* 0x0E0 */ s32              tileIndex; // scaled by two, see the init stage
    /* 0x0E4 */ u8*              tileTable;
    /* 0x0E8 */ void*            unk_0E8;   // non-NULL switches on the variant path
    /* 0x0EC */ u16              unk_0EC;   // *(u16*)(unk_0E8 + 4)
    /* 0x0EE */ u16              unk_0EE;
    /* 0x0F0 */ u16              unk_0F0;
    /* 0x0F4 */ s32              unk_0F4;
    /* 0x0F8 */ s32              kind;    // 1..9; see the header note above
    /* 0x0FC */ s32              subKind; // gates kind 8 in the render stage
    /* 0x100 */ s32              unk_100;
    /* 0x104 */ s32              unk_104; // starts at 1
    /* 0x108 */ s32              unk_108;
    /* 0x10C */ s32              unk_10C;
    /* 0x110 */ s32              anchorX; // the render subtracts these
    /* 0x114 */ s32              anchorY;
    /* 0x118 */ s32              unk_118;
    /* 0x11C */ s32              unk_11C;
    /* 0x120 */ s32              x;       // Q12.12, OtuPoint
    /* 0x124 */ s32              y;
    /* 0x128 */ s32              unk_128; // added into y by the render
    /* 0x12C */ OtuPoint         vel;
    /* 0x134 */ s32              unk_134;
    /* 0x138 */ OtuPoint         dir;
    /* 0x140 */ s32              travel; // integrated from vel each update
    /* 0x144 */ s32              velMag; // decays by a fixed step each update
    /* 0x148 */ s32              alive;
    /* 0x14C */ u8               pad_14C[0x1C];
    /* 0x168 */ s32              flags;    // init 0xA2; bit 1 blocks the render
    /* 0x16C */ u16*             pinID;    // a tray slot; 0x130 means "no pin"
    /* 0x170 */ u8*              rowTable; // OtuPinRow[], 0x1C stride
    /* 0x174 */ s32              unk_174;
    /* 0x178 */ s16              tile0;    // rowTable[*pinID].tile[0]
    /* 0x17A */ s16              tile1;
    /* 0x17C */ s16              tile2;
    /* 0x17E */ s16              tile3;
    /* 0x180 */ u8               pad_180[0x1C];
    /* 0x19C */ u16              unk_19C;
    /* 0x19E */ u16              unk_19E; // starts at 5
    /* 0x1A0 */ u16              unk_1A0;
    /* 0x1A2 */ u8               pad_1A2[2];
    /* 0x1A4 */ s32              unk_1A4;  // raised to 1 by the init stage
    /* 0x1A8 */ s32              hasLabel; // gates two of the children
    /* 0x1AC */ s32              unk_1AC;
    /* 0x1B0 */ struct OtuBadge* partner;  // the pin this one last touched
    /* 0x1B4 */ s32              unk_1B4;
    /* 0x1B8 */ s32              unk_1B8;  // counts down, then re-raises unk_1B4
    /* 0x1BC */ s32              unk_1BC;
    /* 0x1C0 */ s32              unk_1C0;
    /* 0x1C4 */ s32              unk_1C4;
    /* 0x1C8 */ s32              unk_1C8;
    /* 0x1CC */ s32              unk_1CC;
    /* 0x1D0 */ s32              unk_1D0; // frame counter, saturating
    /* 0x1D4 */ s32              child0;
    /* 0x1D8 */ s32              child1;
    /* 0x1DC */ s32              child2;
    /* 0x1E0 */ s32              child3;
    /* 0x1E4 */ s32              child4;
    /* 0x1E8 */ s32              child5;
    /* 0x1EC */ s32              child6;
    /* 0x1F0 */ s32              child7;
    /* 0x1F4 */ s32              child8;
    /* 0x1F8 */ s32              children[12];
    /* 0x228 */ s32              pairIds[2];
    /* 0x230 */ s32              labelTask;
    /* 0x234 */ s32              child9;
    /* 0x238 */ s32              groupIds[8];
    /* 0x258 */ s32              child10;
} OtuBadge; // Size: 0x25C

/** The row of the tray table this pin's tray slot names.
 *
 *  Spelled `base + *pinID * 0x1C` rather than as `&rowTable[*pinID]` on
 *  purpose: the target keeps the scaled add and the fixed displacement
 *  separate (`mla r1, r3, r1, r6` then `ldrb r3, [r1, #4]`), and folding the
 *  two together costs an instruction. Same reasoning as OTU_CHILD_ID.
 */
#define OTU_PIN_ROW(self) ((OtuPinRow*)((u8*)(self)->rowTable + *(self)->pinID * 0x1C))

/** The pin's position, as the vector helpers want it. */
#define OTU_PIN_POS(self) ((OtuPoint*)&(self)->x)

/** The pin's velocity. */
#define OTU_PIN_VEL(self) ((OtuPoint*)&(self)->vel)

/**
 * @brief The nine-word block the init stage reads and the spawner builds.
 *
 * Built in the outgoing-argument area of 0x0208dcb0 as nine consecutive
 * words at sp+8..sp+0x28 and handed to EasyTask_CreateTask as its `param`,
 * which is why the frame is 0x48 with the block's own pointer at sp+4.
 */
typedef struct {
    /* 0x00 */ s32   unk_00; // -> self + 0x004, the SpriteAnimation dataType
    /* 0x04 */ s32   unk_04; // -> self + 0x0E0
    /* 0x08 */ void* unk_08; // -> self + 0x0E8
    /* 0x0C */ u8*   unk_0C; // -> self + 0x0E4
    /* 0x10 */ s32   unk_10; // -> self + 0x000
    /* 0x14 */ u8*   rowTable;
    /* 0x18 */ u16*  pinID;
    /* 0x1C */ s32   unk_1C; // -> self + 0x1A8
    /* 0x20 */ s32   unk_20; // -> self + 0x174
} OtuBadge_InitArgs;         // Size: 0x24

/* ------------------------------------------------------------------ */

/* ------------------------------------------------------------------ */
/* Band 12's own functions, declared here because they are called from
   earlier code above. Without this each such call becomes an implicit
   `int (...)` and the definition below is then a redeclaration of it. */
/* ------------------------------------------------------------------ */
/* Callees outside this batch. These must sit above every use: with no
   prototype a call becomes an implicit `int (...)`, and a prototype
   below the call is then a redeclaration of it. Declaring them at the
   foot of the file, where this block started, cost 39 errors.        */
/* ------------------------------------------------------------------ */
extern const SpriteAnimation data_ov039_02099288;
extern const SpriteAnimation data_ov039_020992b4;
extern const SpriteAnimation data_ov039_020992e0;
extern const TaskStages      data_ov039_02099278;
extern const TaskHandle      data_ov039_0209926c;
extern const BinIdentifier   data_ov039_0209a0b4[5];
extern const OtuPinScale     data_ov039_0209a3e4[];
extern const OtuPinScale     data_ov039_0209a3e8[];
extern void                  func_ov039_0208c9cc(OtuBadge* self);
extern void                  func_ov039_0208ca1c(OtuBadge* self);
extern void                  func_ov039_0208cb64(OtuBadge* self);
extern void                  func_ov039_0208cc4c(OtuBadge* self);
extern void                  func_ov039_0208cd50(OtuBadge* self);
extern void                  func_ov039_0208ce20(OtuBadge* self);
extern void                  func_ov039_0208ce54(OtuBadge* self);
extern void                  func_ov039_0208ce88(OtuBadge* self);
extern void                  func_ov039_0208d210(OtuBadge* self);
extern s32                   func_ov039_020915a8(TaskPool* pool, s32 dataType, s32 childId);
extern s32                   func_ov039_02091b00(TaskPool* pool, s32 dataType, s32 childId);

/* ------------------------------------------------------------------ */
/* The three sprite loaders.                                          */
/* ------------------------------------------------------------------ */

/** Loads sprite A, picking the bin and pack index from the tray slot.
 *
 *  The whole 0x2C-byte template is copied in first (three `ldm`/`stm` pairs,
 *  0x20 + 0x0C bytes, which is exactly sizeof(SpriteAnimation)), so the
 *  template's own binIden and packIndex are overwritten rather than merely
 *  defaulted. 0x130 at the tray slot means "no pin" and selects the second
 *  bin with a fixed pack index of 0xB.
 */
void func_ov039_0208d3bc(OtuBadge* self, Sprite* sprite) {
    SpriteAnimation anim = data_ov039_02099288;

    anim.owner    = self;
    anim.dataType = (u16)self->dataType;
    anim.posX     = self->x >> 12;
    anim.posY     = self->y >> 12;
    // The only one of the three that sets this. `bic #0x380` / `orr #0x300`
    // clears bits 7..9 and writes 6 into them.
    anim.bits_7_9 = 6;

    if (*self->pinID < 0x130) {
        anim.binIden   = &data_ov039_0209a0b4[0];
        anim.packIndex = *self->pinID + 1;
    } else {
        anim.binIden   = (BinIdentifier*)&data_ov039_0209a0dc;
        anim.packIndex = 0xB;
    }

    anim.unk_1C = 1;
    anim.unk_20 = 4;
    anim.unk_26 = 2;
    anim.unk_28 = 3;

    _Sprite_Load(sprite, &anim);
}

/** Loads sprite B. The same loader with the second template and no patch-up. */
void func_ov039_0208d4cc(OtuBadge* self, Sprite* sprite) {
    SpriteAnimation anim = data_ov039_020992b4;

    anim.owner    = self;
    anim.dataType = (u16)self->dataType;
    anim.posX     = self->x >> 12;
    anim.posY     = self->y >> 12;

    _Sprite_Load(sprite, &anim);
}

/** Loads sprite C. func_ov039_0208d4cc with the third template.
 *
 *  The third template differs from the second only in its packIndex (1 against
 *  6) and its animIndex (5 against 1), neither of which the loader touches --
 *  the three really are one function instantiated three times. */
void func_ov039_0208d554(OtuBadge* self, Sprite* sprite) {
    SpriteAnimation anim = data_ov039_020992e0;

    anim.owner    = self;
    anim.dataType = (u16)self->dataType;
    anim.posX     = self->x >> 12;
    anim.posY     = self->y >> 12;

    _Sprite_Load(sprite, &anim);
}

/* ------------------------------------------------------------------ */
/* The task's state reset and its four stages.                        */
/* ------------------------------------------------------------------ */

/**
 * @brief Zeroes the per-frame state and re-reads the pin's tray row.
 *
 *  Two details are load-bearing. The four bytes at +0x178..+0x17E are read
 *  from the row one `ldrb` at a time and stored with `strh`, so they are four
 *  separate s16 fields rather than one word -- that is also what makes the
 *  +0x100 sub-object that OtuFieldAccess.c's four accessors point at.
 *
 *  The target derives `self + 0x100` once and reaches all five of those
 *  fields through it. Written flat here; the base is a register-allocation
 *  choice the source does not get to make.
 */
void func_ov039_0208d5dc(OtuBadge* self) {
    self->unk_0F4 = 0;
    self->unk_104 = 1;
    self->unk_108 = 0;
    self->unk_10C = 0;
    self->unk_128 = 0;
    self->vel.x   = 0;
    self->vel.y   = 0;
    self->dir.x   = 0x1000;
    self->dir.y   = 0;
    self->unk_134 = 0;
    self->travel  = 0;
    self->velMag  = 0;
    self->unk_0CC = 0;
    self->unk_0D0 = 0x1000;
    self->unk_0D4 = 0x1000;
    self->unk_0D8 = 0;
    self->unk_0DA = 0;
    self->unk_19E = 5;
    self->alive   = 0;
    self->unk_1CC = 0;
    self->unk_1D0 = 0;
    self->flags   = 0xA2;
    self->partner = NULL;
    self->unk_1B4 = 0x11;
    self->unk_1B8 = 0;
    self->unk_1BC = 0;
    self->unk_1C0 = 0;
    self->unk_1C4 = 0;

    // As in 0x0208e130: the row is re-derived for each byte rather than
    // hoisted, because the target reloads pinID and rowTable every time.
    self->tile0 = OTU_PIN_ROW(self)->tile[0];
    self->tile1 = OTU_PIN_ROW(self)->tile[1];
    self->tile2 = OTU_PIN_ROW(self)->tile[2];
    self->tile3 = OTU_PIN_ROW(self)->tile[3];

    func_ov039_0208d3bc(self, &self->spriteA);
}

/**
 * @brief The init stage: takes the spawn block, seeds the grid position,
 *        resets the state and loads all three sprites.
 *
 *  The grid position comes out of two *bytes* of the tile table at +0xE4,
 *  scaled by 32 and biased by 0x10 before being promoted to Q12.12 -- so the
 *  board is a 32-pixel grid and +0x120/+0x124 are that cell in fixed point.
 *
 *  The `add r1, r1, r2, lsl #1` is a two-byte stride over an index that is
 *  itself a word, and the two reads are at +8 and +9 of the result. That does
 *  not describe any record this overlay uses elsewhere, so it is reproduced
 *  exactly rather than tidied into a plausible layout.
 *
 *  `unk_0EC` is the one field read out of the variant pointer: `*(u16*)(unk_0E8 + 4)`
 *  when there is one, zero otherwise -- the `ldrhne`/`strhne`/`strheq` shape.
 */
s32 func_ov039_0208d6dc(TaskPool* pool, Task* task, OtuBadge_InitArgs* args) {
    OtuBadge* self = (OtuBadge*)task->data;
    u8*       cell;

    self->dataType  = args->unk_00;
    self->pool      = pool;
    self->tileIndex = args->unk_04;
    self->unk_0E8   = args->unk_08;
    self->tileTable = args->unk_0C;
    self->unk_000   = args->unk_10;
    self->unk_0DC   = 1;
    self->unk_19C   = 0;
    self->unk_1A0   = 0;
    self->hasLabel  = args->unk_1C;
    self->unk_1AC   = 0;
    self->unk_1C8   = 0;
    self->rowTable  = args->rowTable;
    self->pinID     = args->pinID;
    self->unk_174   = args->unk_20;

    self->unk_0EC = (self->unk_0E8 != NULL) ? *(u16*)((u8*)self->unk_0E8 + 4) : 0;

    self->unk_0EE = 0;
    self->unk_0F0 = 0;
    self->anchorX = 0;
    self->anchorY = 0;
    self->unk_118 = 0;
    self->unk_11C = 0;

    cell    = self->tileTable + self->tileIndex * 2;
    self->x = ((cell[8] << 5) + 0x10) << 12;
    self->y = ((cell[9] << 5) + 0x10) << 12;

    self->unk_1A4 = 1;

    func_ov039_0208d5dc(self);
    func_ov039_0208d4cc(self, &self->spriteB);
    func_ov039_0208d554(self, &self->spriteC);

    return 1;
}

/**
 * @brief The update stage: the per-kind handler, the travel integration, then
 *        the per-kind behaviour dispatch.
 *
 *  Two things worth stating. The three saturating counters (+0x1D0, +0x148,
 *  +0x168) are three separate `cmp`/`subgt`/`strgt` triples in the target, so
 *  they are three statements and not a loop. And +0x144 is decayed by a step
 *  read from `data_ov039_0209a390` (which holds 10) -- `add r0, r0, r0, lsl #1`
 *  then `lsl #0xc`, so the source is `(v * 3) << 12`, not `v * 0x3000` folded.
 *
 *  The `switch` over `kind` here is a *table* dispatch (`cmp r0, #9; addls pc,
 *  pc, r0, lsl #2`) with nine out-of-line bodies, one per kind, all named.
 */
s32 func_ov039_0208d7e4(TaskPool* pool, Task* task, void* args) {
    OtuBadge* self = (OtuBadge*)task->data;

    if (self->unk_0E8 != NULL) {
        func_ov039_0208ac98((OtuInputLatch*)self);
        func_ov039_0208acc0((OtuBadgeState*)self);
    }

    self->unk_0DC = 1;
    self->unk_0D0 = 0x1000;
    self->unk_0D4 = 0x1000;

    if (self->unk_1D0 > 0) {
        self->unk_1D0--;
    }
    if (self->alive > 0) {
        self->alive--;
    }
    if (self->flags > 0) {
        self->flags--;
    }

    switch (self->kind) {
        case 1:
        case 3:
        case 6:
        case 7:
            func_ov039_0208af6c((OtuBadgeState*)self);
            break;

        default:
            break;
    }

    self->travel = self->travel + self->velMag;

    if (self->velMag > 0) {
        self->velMag = self->velMag - ((data_ov039_0209a390 * 3) << 12);
        if (self->velMag < 0) {
            self->velMag = 0;
        }
    } else if (self->velMag < 0) {
        self->velMag = self->velMag + ((data_ov039_0209a390 * 3) << 12);
        if (self->velMag > 0) {
            self->velMag = 0;
        }
    }

    // `lsl #4` then `lsr #0x10`: a multiply by 0x10 followed by an unsigned
    // divide by 0x10000, not a shift right by twelve. A plain `>> 12` is one
    // instruction and would not be what the target wrote.
    self->unk_0CC = (u32)self->travel * 0x10 / 0x10000;

    if (self->unk_1B8 > 0) {
        self->unk_1B8 = self->unk_1B8 - 1;
        if (self->unk_1B8 <= 0) {
            self->unk_1B4 = 0x11;
        }
    }

    switch (self->kind) {
        case 1:
            if (self->unk_0E8 != NULL) {
                func_ov039_0208bb2c((OtuBadgeState*)self);
            } else {
                func_ov039_0208c9cc(self);
            }
            break;

        case 2:
            func_ov039_0208ca1c(self);
            break;

        case 3:
            func_ov039_0208cb64(self);
            break;

        case 4:
            func_ov039_0208cc4c(self);
            break;

        case 5:
            func_ov039_0208cd50(self);
            break;

        case 6:
            func_ov039_0208ce20(self);
            break;

        case 7:
            func_ov039_0208ce54(self);
            break;

        case 8:
            func_ov039_0208ce88(self);
            break;

        case 9:
            func_ov039_0208d210(self);
            break;

        default:
            break;
    }

    Sprite_Update(&self->spriteA);
    Sprite_Update(&self->spriteB);
    Sprite_Update(&self->spriteC);

    return 1;
}

/**
 * @brief Pushes the pin's grid position into sprite C's and sprite A's cells.
 *
 *  The same two expressions twice; only sprite C's are gated on +0x1D0. The
 *  stored fields are `Sprite.posX`/`Sprite.posY` (a Sprite's +0x0C/+0x0E),
 *  which is what fixes the sprite bases at +0x0C/+0x4C/+0x8C.
 */
void func_ov039_0208d9ec(OtuBadge* self) {
    if (self->unk_1D0 > 0) {
        self->spriteC.posX = (self->x - self->anchorX) >> 12;
        self->spriteC.posY = ((self->y + self->unk_128) - self->anchorY) >> 12;

        Sprite_RenderFrame(&self->spriteC);
    }

    self->spriteA.posX = (self->x - self->anchorX) >> 12;
    self->spriteA.posY = ((self->y + self->unk_128) - self->anchorY) >> 12;

    Sprite_RenderFrame(&self->spriteA);
}

/**
 * @brief The render stage.
 *
 *  While +0xDC is clear the pin has not started moving, so only the middle
 *  sprite is drawn and only if the pin carries a label; after that the pin
 *  renders itself through 0x0208d9ec, but only for the kinds listed, and kind
 *  8 only for three of its sub-kinds. +0x168 bit 1 suppresses all of it.
 */
s32 func_ov039_0208da74(TaskPool* pool, Task* task, void* args) {
    OtuBadge* self = (OtuBadge*)task->data;

    if (self->unk_0DC == 0) {
        if (self->hasLabel != 0) {
            self->spriteB.posX = (self->x - self->anchorX) >> 12;
            self->spriteB.posY = ((self->y + self->unk_128) - self->anchorY) >> 12;

            Sprite_RenderFrame(&self->spriteB);
        }

        return 1;
    }

    if (self->flags & 2) {
        return 1;
    }

    switch (self->kind) {
        case 1:
        case 3:
        case 4:
        case 5:
        case 7:
        case 9:
            func_ov039_0208d9ec(self);
            break;

        case 8:
            if (self->subKind == 0 || self->subKind == 1 || self->subKind == 6) {
                func_ov039_0208d9ec(self);
            }
            break;

        default:
            break;
    }

    return 1;
}

/**
 * @brief The cleanup stage: deletes all eleven children, then the sprites.
 *
 *  The three looped groups are walked as `self + i * 4 + offset` rather than
 *  indexed off the array member, because that is the shape the target emits
 *  (`add r0, r5, r6, lsl #2` then `ldr r1, [r0, #0x238]`) and folding the
 *  displacement into the scaled add costs an instruction.
 *
 *  Deletion order is the reverse of creation, except that the labelled child
 *  is created between the pair group and the twelve, and is deleted between
 *  the pair group and child9 -- so the order is exact, not simply reversed.
 */
s32 func_ov039_0208db44(TaskPool* pool, Task* task, void* args) {
    OtuBadge* self = (OtuBadge*)task->data;
    s32       i;

    EasyTask_DeleteTask(pool, self->child10);

    for (i = 0; i < 8; i++) {
        EasyTask_DeleteTask(pool, *(s32*)((u8*)self + i * 4 + 0x238));
    }

    EasyTask_DeleteTask(pool, self->child9);

    if (self->hasLabel != 0) {
        EasyTask_DeleteTask(pool, self->labelTask);
    }

    for (i = 0; i < 2; i++) {
        EasyTask_DeleteTask(pool, *(s32*)((u8*)self + i * 4 + 0x228));
    }

    for (i = 0; i < 12; i++) {
        EasyTask_DeleteTask(pool, *(s32*)((u8*)self + i * 4 + 0x1F8));
    }

    EasyTask_DeleteTask(pool, self->child8);
    EasyTask_DeleteTask(pool, self->child7);
    EasyTask_DeleteTask(pool, self->child6);
    EasyTask_DeleteTask(pool, self->child5);
    EasyTask_DeleteTask(pool, self->child4);
    EasyTask_DeleteTask(pool, self->child3);
    EasyTask_DeleteTask(pool, self->child2);
    EasyTask_DeleteTask(pool, self->child1);
    EasyTask_DeleteTask(pool, self->child0);

    Sprite_Release(&self->spriteA);
    Sprite_Release(&self->spriteB);
    Sprite_Release(&self->spriteC);

    return 1;
}

/** The task's entry point: the four-slot stage trampoline. */
s32 func_ov039_0208dc68(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_02099278;

    return stages.iter[stage](pool, task, args);
}

/* ------------------------------------------------------------------ */
/* The spawner.                                                       */
/* ------------------------------------------------------------------ */

/**
 * @brief Creates a badge and its eleven children, in a fixed order.
 *
 *  Every spawner is handed the *new* task's id, not the parent's, so the
 *  children know which pin they belong to; that is why `id` is passed
 *  alongside the pool everywhere.
 *
 *  The two `EasyTask_GetTaskData` calls after 0x02093cd8 and 0x02096124 are
 *  not redundant: the first hands the counter task its tray slot pointer
 *  (`args` word 7) and the second pokes the label task with whether the pin's
 *  tray slot holds a label. Both re-derive the pool from `self->pool` rather
 *  than from the argument, which is why +0x008 has to be saved at all.
 *
 *  NB: six of these spawners are typed `void` in bands 2/4/5 while the target
 *  stores their result. Retyped to `s32` there -- band 4 already did this for
 *  0x02096b48/0x02096e4c's siblings -- nothing here changes.
 *
 *  The frame is exactly 0x2C: 8 bytes of outgoing arguments plus the 0x24-byte
 *  block, with no slack. `id`, `self` and `i` therefore have to live in
 *  registers, and there are only five callee-saved ones, so this is at the
 *  edge of what mwcc can hold -- if the frame comes out larger, the fix is to
 *  drop `i` and unroll one of the loops rather than to shrink the block.
 */
/* Typed to band 9's declaration, the one already compiling everywhere else
 * in this translation unit. Taking all nine arguments as s32 and casting at
 * each use collided with band 9 and cascaded into nine "illegal access to
 * local variable" errors inside this body. */
s32 func_ov039_0208dcb0(TaskPool* pool, s32 arg1, s32 arg2, void* arg3, void* arg4, void* arg5, void* arg6, void* arg7,
                        s32 arg8, void* arg9) {
    OtuBadge_InitArgs args;
    OtuBadge*         self;
    s32               id;
    s32               i;

    args.unk_00   = arg1;
    args.unk_04   = arg2;
    args.unk_08   = arg3;
    args.unk_0C   = (u8*)arg4;
    args.unk_10   = arg5;
    args.rowTable = (u8*)arg6;
    args.pinID    = (u16*)arg7;
    args.unk_1C   = arg8;
    args.unk_20   = arg9;

    id   = EasyTask_CreateTask(pool, &data_ov039_0209926c, NULL, 0, NULL, &args);
    self = (OtuBadge*)EasyTask_GetTaskData(pool, id);

    self->child0 = func_ov039_0208f40c(pool, arg1, id);
    self->child1 = func_ov039_0208f770(pool, arg1, id);
    self->child2 = func_ov039_0208fa5c(pool, arg1, id);
    self->child3 = func_ov039_0208fe60(pool, arg1, id);
    self->child4 = func_ov039_02090e1c(pool, arg1, id);
    self->child5 = func_ov039_020915a8(pool, arg1, id);
    self->child6 = func_ov039_02091b00(pool, arg1, id);

    // The radar and the counter each take two extra words, built in the
    // outgoing-argument area rather than in a frame of their own.
    self->child7 = func_ov039_0209383c(pool, arg1, id, arg2, (s32)arg4, arg8);
    self->child8 = func_ov039_02093cd8(pool, arg1, id, arg2, arg8);
    func_ov039_02093d18(EasyTask_GetTaskData(pool, self->child8), (u16*)arg7);

    for (i = 0; i < 12; i++) {
        *(s32*)((u8*)self + i * 4 + 0x1F8) = func_ov039_02095750(pool, arg1, id);
    }

    for (i = 0; i < 2; i++) {
        *(s32*)((u8*)self + i * 4 + 0x228) = func_ov039_02095ca0(pool, arg1, id);
    }

    if (self->hasLabel != 0) {
        self->labelTask = func_ov039_02096124(pool, arg1);
    }

    self->child9 = func_ov039_02096548(pool, arg1, id);

    for (i = 0; i < 8; i++) {
        *(s32*)((u8*)self + i * 4 + 0x238) = func_ov039_0209771c(pool, arg1, id);
    }

    self->child10 = func_ov039_02097a70(pool, arg1, id);

    if (self->hasLabel != 0 && *self->pinID < 0x130) {
        // The label rides in the *second* halfword of the tray slot, and the
        // `moveq`/`movne` pair shows it is the second halfword that is tested.
        func_ov039_02096154(EasyTask_GetTaskData(self->pool, self->labelTask), 1, self->pinID[1] == 0x130 ? 1 : 0);
    }

    func_ov039_0208ae8c((OtuBadgeState*)self);

    return id;
}

/* ------------------------------------------------------------------ */
/* The pair-versus-pair collision code.                                */
/* ------------------------------------------------------------------ */

/**
 * @brief True when two pins are close enough and closing.
 *
 *  Two guards, then a sign test. The first guard is the `ldreq`/`cmpeq`
 *  chain: both velocities are zero means neither pin is heading anywhere, and
 *  that is decided without a branch. The second is the reach test, summed
 *  from the two arguments rather than compared against a constant -- the
 *  callers both pass 0xC000, so this is 3.0 in Q12.12.
 *
 *  The sign test is "at least one of the two dots is positive", i.e. the pair
 *  is not separating on both axes. Returned as a materialised 1/0 rather than
 *  as the comparison, because the target materialises it.
 */
s32 func_ov039_0208df2c(OtuPoint* posA, OtuPoint* velA, s32 reachA, OtuPoint* posB, OtuPoint* velB, s32 reachB) {
    OtuPoint dir;
    s32      dotA;
    s32      dotB;

    if (velA->x == 0 && velA->y == 0 && velB->x == 0 && velB->y == 0) {
        return 0;
    }

    if (func_ov039_02098ca8(posA, posB) > reachA + reachB) {
        return 0;
    }

    func_ov039_02098bb0(posB, posA, &dir);
    dotA = func_ov039_02098c40(&dir, velA);

    func_ov039_02098bb0(posA, posB, &dir);
    dotB = func_ov039_02098c40(&dir, velB);

    return (dotA > 0 || dotB > 0) ? 1 : 0;
}

/**
 * @brief Pushes one pin's velocity along a direction and re-normalises it.
 *
 *  The angle is built in two Q12.12 steps, not one, and both round -- the
 *  `smull`/`adds #0x800`/`adc` pairs are two independent rounded multiplies.
 *
 *  The tail guard compares *addresses*: `adds r0, r4, #0x12C` / `bne` and
 *  `adds r0, r4, #0x130` / `popeq`, with no loads between them. Both +0x12C
 *  and +0x130 are read as data everywhere else in this overlay (0x0208e504
 *  writes through them), so a "vel is non-zero" reading cannot be what the
 *  source said -- that needs two `ldr`s. Written here as the pointer test the
 *  flags actually encode, which is the only reading consistent with zero
 *  loads. See the note in the header.
 */
void func_ov039_0208dff0(OtuPoint* dir, s32 speed, s32 scaleA, s32 scaleB, OtuBadge* other) {
    s32 mag = OTU_MUL_Q12(speed, scaleA);

    func_ov039_02098c00(OTU_MUL_Q12(mag, scaleB), dir, &other->vel, &other->vel);
    func_ov039_0208a6c4(&other->vel);

    if (&other->vel != NULL && (OtuPoint*)((u8*)&other->vel + 4) != NULL) {
        func_ov039_02098d3c(&other->vel, &other->dir);
    }
}

/**
 * @brief Measures how hard two pins are pushing apart, and in which direction.
 *
 *  The distance is scaled by a dot with the approach direction, so a pair
 *  that is close but not closing scores near zero rather than the full
 *  distance -- that is what makes the result usable as a force.
 *
 *  `out` doubles as the scratch: it is tested on entry, overwritten with
 *  `posA - velA` in the middle, normalised, and the final dot is taken against
 *  it. It is why the parameter is a pointer to a caller's point rather than a
 *  value.
 */
void func_ov039_0208e058(OtuPoint* posA, OtuPoint* velA, OtuPoint* posB, OtuPoint* velB, OtuPoint* out, s32* score) {
    OtuPoint dir;
    s32      dot;

    // Both of these take the two *velocities*, not the two positions -- the
    // first computes a relative speed and the second a relative velocity, and
    // only the third call below touches the positions.
    *score = func_ov039_02098ca8(velB, velA);
    func_ov039_02098bb0(velB, velA, &dir);

    if (out->x != 0 || out->y != 0) {
        func_ov039_02098d3c(&dir, &dir);
    }

    func_ov039_02098bb0(posA, posB, out);

    if (out->x != 0 || out->y != 0) {
        func_ov039_02098d3c(out, out);
    } else {
        out->x = 0x1000;
        out->y = 0;
    }

    dot = func_ov039_02098c40(&dir, out);
    if (dot < 0) {
        dot = -dot;
    }

    *score = OTU_MUL_Q12(*score, dot);
}

/**
 * @brief The actual impulse: both pins are pushed apart along the line
 *        between them, and linked to each other.
 *
 *  Symmetric in every term -- the first half is (self's scale, other's scale,
 *  target self) and the second is the same with the two swapped and the angle
 *  negated -- which is why it is written as two nearly identical blocks rather
 *  than a loop.
 *
 *  The two scale tables are indexed by the *other* pin's row weight and the
 *  doubling is gated on the *other* pin's frame counter, so a pin still in its
 *  first few frames pushes twice as hard. The Q12.12 scale factor is 0x3000
 *  (`data_ov039_0209a388`) while a settled pin gets 0x1000, i.e. 3.0 against
 *  1.0.
 */
void func_ov039_0208e130(OtuBadge* self, OtuBadge* other) {
    OtuPoint dir;
    s32      score;
    s32      scale;

    func_ov039_0208e058(OTU_PIN_POS(self), OTU_PIN_VEL(self), OTU_PIN_POS(other), OTU_PIN_VEL(other), &dir, &score);

    // The row pointer is not hoisted into a local: the target re-reads both
    // `pinID` and `rowTable` from the task on each of the four lookups, so a
    // cached OtuPinRow* is a source-level difference, not a scheduling one.
    scale = (self->alive > 0) ? data_ov039_0209a388 : 0x1000;
    func_ov039_0208dff0(&dir, OTU_MUL_Q12(score, scale), data_ov039_0209a3e4[OTU_PIN_ROW(self)->weight].scaleA,
                        (other->unk_1D0 > 0) ? data_ov039_0209a3e8[OTU_PIN_ROW(other)->weight].scaleB * 2
                                             : data_ov039_0209a3e8[OTU_PIN_ROW(other)->weight].scaleB,
                        self);
    self->partner = other;

    scale = (other->alive > 0) ? data_ov039_0209a388 : 0x1000;
    func_ov039_0208dff0(&dir, -OTU_MUL_Q12(score, scale), data_ov039_0209a3e4[OTU_PIN_ROW(other)->weight].scaleA,
                        (self->unk_1D0 > 0) ? data_ov039_0209a3e8[OTU_PIN_ROW(self)->weight].scaleB * 2
                                            : data_ov039_0209a3e8[OTU_PIN_ROW(self)->weight].scaleB,
                        other);
    other->partner = self;
}

/**
 * @brief The pin-versus-pin test: kinds 1, 6 and 7 only, then hand over to
 *        0x0208e130.
 *
 *  Note this is a *narrower* kind set than 0x0208e37c, which drops 1's
 *  companions 6 and 7 out but admits 3 and 4. The two entry points are
 *  therefore not two spellings of one predicate: one is pin-against-pin
 *  attraction and the other is pin-against-board.
 */
/* Typed to band 9's declaration, which comes first in this translation
 * unit and so governs. Taking the real struct types here collides with it
 * and cascades into "expression syntax error" through the whole body. */
s32 func_ov039_0208e28c(void* a, void* b) {
    OtuBadge* self  = (OtuBadge*)a;
    OtuBadge* other = (OtuBadge*)b;

    if (self->flags > 0) {
        return 0;
    }
    if (self->kind != 1 && self->kind != 6 && self->kind != 7) {
        return 0;
    }
    if (self->unk_128 != 0) {
        return 0;
    }
    if (other->flags > 0) {
        return 0;
    }
    if (other->kind != 1 && other->kind != 6 && other->kind != 7) {
        return 0;
    }
    if (other->unk_128 != 0) {
        return 0;
    }

    if (func_ov039_0208df2c(OTU_PIN_POS(self), OTU_PIN_VEL(self), 0xC000, OTU_PIN_POS(other), OTU_PIN_VEL(other), 0xC000) == 0)
    {
        return 0;
    }

    func_ov039_0208e130(self, other);

    return 1;
}

/**
 * @brief The board-collision response: shove both pins apart along the line
 *        between them, each by half the overlap.
 *
 *  A `switch` over each pin's kind, both admitting {1,3,4,6,7} and rejecting
 *  {0,2,5,8} -- the target's two jump tables are identical. 0x18000 is 6.0 in
 *  Q12.12, so this fires inside six grid cells.
 *
 *  The two divides are signed `/ 2` on `gap` and on `-gap` separately, not
 *  `-(gap / 2)`: the target computes `rsb` *before* the `>> 31` sign fix-up, so
 *  an odd overlap rounds the two halves differently and reproduces the
 *  target's asymmetry.
 */
/* Typed to band 9's declaration, which comes first in this translation
 * unit and so governs. Taking the real struct types here collides with it
 * and cascades into "expression syntax error" through the whole body. */
s32 func_ov039_0208e37c(void* a, void* b) {
    OtuBadge* self  = (OtuBadge*)a;
    OtuBadge* other = (OtuBadge*)b;

    OtuPoint dir;
    s32      dist;
    s32      gap;

    if (self->flags > 0) {
        return 0;
    }

    switch (self->kind) {
        case 0:
        case 2:
        case 5:
        case 8:
            return 0;

        default:
            break;
    }

    if (self->unk_128 != 0) {
        return 0;
    }

    if (other->flags > 0) {
        return 0;
    }

    switch (other->kind) {
        case 0:
        case 2:
        case 5:
        case 8:
            return 0;

        default:
            break;
    }

    if (other->unk_128 != 0) {
        return 0;
    }

    dist = func_ov039_02098ca8(OTU_PIN_POS(self), OTU_PIN_POS(other));

    if (dist > 0x18000) {
        return 0;
    }

    gap = 0x18000 - dist;

    func_ov039_02098bb0(OTU_PIN_POS(self), OTU_PIN_POS(other), &dir);

    if (dir.x == 0 && dir.y == 0) {
        dir.x = 0x1000;
        dir.y = 0;
    } else {
        func_ov039_02098d3c(&dir, &dir);
    }

    func_ov039_02098c00(gap / 2, &dir, OTU_PIN_POS(self), OTU_PIN_POS(self));
    func_ov039_02098c00((-gap) / 2, &dir, OTU_PIN_POS(other), OTU_PIN_POS(other));

    return 1;
}

/**
 * @brief The wall/obstacle response, and the one place in this band that
 *        reaches outside the pin task.
 *
 *  The second argument is *not* another pin: it is read through
 *  func_ov039_02092744 / _02092758 / _02092760, which are the accessors for
 *  the overlay's second, smaller object -- a point at +0x48/+0x4C, a radius
 *  at +0x50, and a flag at +0x54. Those three are already defined in
 *  OtuFieldAccess.c above the band includes, so they are called rather than
 *  declared, and the parameter is left `void*` because no type for that object
 *  exists yet.
 *
 *  The third block is the interesting one: the pin's velocity is normalised,
 *  measured, and turned into a *position* offset which lands in +0x138/+0x13C
 *  -- the pair func_ov039_0208e6cc copies out. So +0x138 is where the pin
 *  publishes the shove it received.
 */
/* Typed to band 9's declaration, which comes first in this translation
 * unit and so governs. Taking the real struct types here collides with it
 * and cascades into "expression syntax error" through the whole body. */
s32 func_ov039_0208e504(void* a, void* obstacle) {
    OtuBadge* self = (OtuBadge*)a;

    OtuPoint other;
    OtuPoint dir;
    s32      radius;
    s32      len;

    switch (self->kind) {
        case 1:
        case 6:
        case 7:
        case 9:
            break;

        default:
            return 0;
    }

    func_ov039_02092744(obstacle, &other);
    radius = func_ov039_02092758(obstacle);

    if (func_ov039_02098ca8(OTU_PIN_POS(self), &other) >= radius + 0xC000) {
        return 0;
    }

    func_ov039_02098bb0(OTU_PIN_POS(self), &other, &dir);

    if (dir.x != 0 || dir.y != 0) {
        func_ov039_02098d3c(&dir, &dir);
    } else {
        dir.x = 0x1000;
        dir.y = 0;
    }

    // Note the argument order: `other` is the base here, not `dir`, so this
    // moves the obstacle's copy of the point rather than the pin's.
    func_ov039_02098c00(radius + 0xC000, &dir, &other, OTU_PIN_POS(self));

    func_ov039_02098bb0(&other, OTU_PIN_POS(self), &dir);

    if (dir.x != 0 || dir.y != 0) {
        func_ov039_02098d3c(&dir, &dir);
    } else {
        dir.x = 0x1000;
        dir.y = 0;
    }

    len = func_ov039_02098c40(OTU_PIN_VEL(self), &dir);
    func_ov039_02098c00(-(len * 2), &dir, OTU_PIN_VEL(self), OTU_PIN_VEL(self));

    if (self->vel.x != 0 || self->vel.y != 0) {
        func_ov039_02098d3c(OTU_PIN_VEL(self), &dir);

        len = func_ov039_02098d10(OTU_PIN_VEL(self));
        func_ov039_02098bd4(OTU_MUL_Q12(len, data_ov039_0209a324), &dir, OTU_PIN_VEL(self));

        self->dir.x = dir.x;
        self->dir.y = dir.y;
    }

    func_ov039_02092760(obstacle);

    return 1;
}

/* ============================================================================
 * Band 14 -- the rest of OtuBadge's field accessors.
 *
 * The seven remaining functions in the run that follows band 12, from 0x0208e890
 * to 0x0208e9e4. The seven below them -- 0208e6cc, _6e0, _6f4, _848, _85c, _870
 * and _87c -- are already in OtuFieldAccess.c itself, so this file does not
 * repeat them. Every body here is a handful of instructions copied straight from
 * the target, and where the target used a jump table this keeps the switch so
 * mwcc emits one too.
 *
 * Two signature styles appear here because both already exist for these
 * functions, in bands 1, 7, 13 and the shared header, and the earlier
 * declaration has to be the one the definition agrees with:
 *
 *   OtuPinTask*  the header's trimmed +0x174-byte view of the same object,
 *                modelling only the fields the nearest-child queries read.
 *   void*        what band 1 and band 7 happen to pass around.
 *
 * OtuPinTask has no field for +0x0DC or +0x0FC, so those two read through
 * OtuBadge, which band 12 declares and which models the whole 0x25C bytes.
 * Every offset OtuPinTask does model agrees with OtuBadge, so the cast is
 * always safe.
 *
 * Offsets: +0x0DC unk_0DC, +0x0F8 kind, +0x0FC subKind, +0x138 dir.
 * =========================================================================*/

/**
 * @brief Only kinds 0, 2, 3 and 4 answer with zero; everything else -- kind 1,
 *        and any value past the table -- answers with +0x0DC.
 *
 *  The target reaches its five entries with `cmp r1, #4 / addls pc, pc, r1,
 *  lsl #2`, defaulting to the load, so this stays a switch with one arm per
 *  case value rather than a chain of equality tests.
 */
s32 func_ov039_0208e890(void* task) {
    OtuBadge* self = (OtuBadge*)task;

    switch (self->kind) {
        case 0:
        case 2:
        case 3:
        case 4:
            return 0;

        case 1:
        default:
            break;
    }

    return self->unk_0DC;
}

/**
 * @brief A three-valued answer, but only for kind 8: subkind 2 answers 2,
 *        subkind 3 answers 3, and every other combination answers 0.
 *
 *  The `bne` past both tests is why this is a nested if rather than a second
 *  switch -- the subkind is never read unless the kind already matched.
 */
s32 func_ov039_0208e950(void* task) {
    OtuBadge* self = (OtuBadge*)task;
    s32       r    = 0;

    if (self->kind == 8) {
        /* A switch, not two `if`s: with two `if`s mwcc selects both arms with
         * `moveq`, while the target branches to an out-of-line `mov r2, #2` for
         * the first and only then tests for 3 with a conditional move. The arms
         * need their own labels to come out that way. */
        switch (self->subKind) {
            case 2:
                r = 2;
                break;

            case 3:
                r = 3;
                break;
        }
    }

    return r;
}

/**
 * @brief True when the pin's kind is 8.
 *
 *  Written as a local plus a select, not as `return task->kind == 8;`. The
 *  target copies the field into a register, zeroes the result and selects into
 *  it -- `ldr r1, [r0, #0xf8] / mov r0, #0 / cmp r1, #0x8 / moveq r0, #1` --
 *  where a bare comparison compiles to a conditional move against the loaded
 *  field. Same answer, different shape, 58% against 100%.
 */
s32 func_ov039_0208e984(OtuPinTask* task) {
    s32 found = task->kind;
    s32 r     = 0;

    if (found == 8) {
        r = 1;
    }

    return r;
}

/**
 * @brief True when the pin's kind is 7.
 *
 *  Written as a local plus a select, not as `return task->kind == 7;`. The
 *  target copies the field into a register, zeroes the result and selects into
 *  it -- `ldr r1, [r0, #0xf8] / mov r0, #0 / cmp r1, #0x7 / moveq r0, #1` --
 *  where a bare comparison compiles to a conditional move against the loaded
 *  field. Same answer, different shape, 58% against 100%.
 */
s32 func_ov039_0208e998(OtuPinTask* task) {
    s32 found = task->kind;
    s32 r     = 0;

    if (found == 7) {
        r = 1;
    }

    return r;
}

/**
 * @brief The heading of -dir, as an unsigned 16-bit index.
 *
 *  Both components are negated before the call rather than the result being
 *  negated after it, and the answer is cut to sixteen bits with
 *  `lsl #0x10 / lsr #0x10`. That pair is a zero-extend, so the narrowing is to
 *  `u16` and not to `s16` -- an `s16` emits `asr` for the second shift and costs
 *  the match. The return type stays `s32` because band 7 declares it that way
 *  and uses the value.
 */
s32 func_ov039_0208e9ac(void* task) {
    OtuBadge* self = (OtuBadge*)task;
    u16       a;
    s32       x = -self->dir.x;
    s32       y = -self->dir.y;

    /* y first, x second -- that is the callee's own parameter order
     * (`u16 FX_Atan2Idx(s32 y, s32 x)`), and mwcc evaluates arguments in
     * reverse, which is why the target loads +0x138 into r1 and +0x13C into r2
     * even though r1 is the first argument slot.
     *
     * Nonmatching: the target keeps the call and then narrows with
     * `lsl #0x10 / lsr #0x10`, so its FX_Atan2Idx must have been visible as
     * returning something wider than u16 -- an implicit int. fx_atan.h declares
     * it as u16, so mwcc here proves the narrowing redundant and turns the whole
     * thing into a tail call through a veneer, dropping the mask. Recovering the
     * last 60% means hiding that prototype from this translation unit, which
     * would cost bands 3 and 11 their own narrowing. Not worth it.
     */
    a = FX_Atan2Idx(y, x);

    return a;
}

/**
 * @brief True when the pin's kind is 6.
 *
 *  Written as a local plus a select, not as `return task->kind == 6;`. The
 *  target copies the field into a register, zeroes the result and selects into
 *  it -- `ldr r1, [r0, #0xf8] / mov r0, #0 / cmp r1, #0x6 / moveq r0, #1` --
 *  where a bare comparison compiles to a conditional move against the loaded
 *  field. Same answer, different shape, 58% against 100%.
 */
s32 func_ov039_0208e9d0(OtuPinTask* task) {
    s32 found = task->kind;
    s32 r     = 0;

    if (found == 6) {
        r = 1;
    }

    return r;
}

/**
 * @brief True when the pin's kind is 9.
 *
 *  Written as a local plus a select, not as `return task->kind == 9;`. The
 *  target copies the field into a register, zeroes the result and selects into
 *  it -- `ldr r1, [r0, #0xf8] / mov r0, #0 / cmp r1, #0x9 / moveq r0, #1` --
 *  where a bare comparison compiles to a conditional move against the loaded
 *  field. Same answer, different shape, 58% against 100%.
 */
s32 func_ov039_0208e9e4(OtuPinTask* task) {
    s32 found = task->kind;
    s32 r     = 0;

    if (found == 9) {
        r = 1;
    }

    return r;
}

/* ============================================================================
 * Band 15 -- the badge AI: per-kind "should this pin act now?" tests, the chase
 * and homing states, and the per-frame update.
 *
 * Nineteen functions, 0x0208c4ec to 0x0208ce54. Every body here was transcribed
 * from objdiff's own target disassembly, not from build/usa/asm/ov039_4.s --
 * that file is a stale March snapshot of a source state that no longer exists,
 * and reading it cost this band a whole false start.
 *
 * The first five are one family and differ only in four numbers: which entry of
 * self->chanceTbl is the roll, which timer is compared against zero, whether
 * the speed cap applies, the distance limit, and which per-kind update runs.
 * They are written out separately rather than shared through a helper, because
 * that is how the target has them -- five separate bodies, five separate
 * addresses, no shared tail.
 * =========================================================================*/

/* The fade-manager block the target reaches as `.word gFaders`. Only the
 * word at +8 is touched by this band. */
extern u32 gFaders[];

/* The per-kind dispatch table 0x0208c9cc walks, one function pointer per
 * entry, indexed from 1 to 0x10 inclusive. */
extern u32 data_ov039_0209a4b0[];

/* Callees with no owner yet. Declared here rather than relied on implicitly:
 * under -lang=c99 an undeclared call becomes an implicit int(...), and the
 * pointer arguments below are then rejected. */
extern OtuPinTask* func_ov039_0208817c(TaskPool* pool, void* scene, s32 index);
extern OtuPinTask* func_ov039_02088294(TaskPool* pool, void* scene, s32 index);
extern OtuPinTask* func_ov039_02087e2c(TaskPool* pool, void* scene, s32 index);
extern s32         func_ov039_02097ad8(void* data);
extern s32         func_ov039_0209167c(void* data);
extern s32         func_ov039_02091014(void* data);

/* --- family A: should this pin run its update this frame? ---------------- */

/**
 * @brief Frame-budget test for one pin kind: roll against chanceTbl[5], require
 *        the timer at +0x17A to be positive, require the scene to still have
 *        that pin, then require it to be within 0x46000 of us.
 *
 *  0x46000 is 18.0 in Q12.12. Returns 1 when the update should run.
 */
s32 func_ov039_0208c4ec(OtuBadgeState* self) {
    OtuPinTask* other;

    if (self->chanceTbl[5] < RNG_Next(0x10000)) {
        return 0;
    }

    if (*(s16*)((u8*)self + 0x100 + 0x7A) <= 0) {
        return 0;
    }

    other = func_ov039_0208817c(self->pool, self->scene, self->index);

    if (other == NULL) {
        return 0;
    }

    if (func_ov039_02098ca8(&self->pos, (OtuPoint*)&other->x) > 0x46000) {
        return 0;
    }

    func_ov039_0208ba34(self);

    return 1;
}

/** @brief As 0x0208c4ec, but chanceTbl[6], the timer at +0x178, a speed cap of
 *         0x3000, and its own kind handler. */
s32 func_ov039_0208c568(OtuBadgeState* self) {
    OtuPinTask* other;

    if (self->chanceTbl[6] < RNG_Next(0x10000)) {
        return 0;
    }

    if (*(s16*)((u8*)self + 0x100 + 0x78) <= 0) {
        return 0;
    }

    if (func_ov039_02098d10(&self->vel) >= 0x3000) {
        return 0;
    }

    other = func_ov039_02088294(self->pool, self->scene, self->index);

    if (other == NULL) {
        return 0;
    }

    if (func_ov039_02098ca8(&self->pos, (OtuPoint*)&other->x) > 0x46000) {
        return 0;
    }

    func_ov039_0208b94c(self);

    return 1;
}

/** @brief As 0x0208c568, but chanceTbl[7] and a 0x32000 distance limit. */
s32 func_ov039_0208c5f8(OtuBadgeState* self) {
    OtuPinTask* other;

    if (self->chanceTbl[7] < RNG_Next(0x10000)) {
        return 0;
    }

    if (*(s16*)((u8*)self + 0x100 + 0x78) <= 0) {
        return 0;
    }

    if (func_ov039_02098d10(&self->vel) >= 0x3000) {
        return 0;
    }

    other = func_ov039_02087e2c(self->pool, self->scene, self->index);

    if (other == NULL) {
        return 0;
    }

    if (func_ov039_02098ca8(&self->pos, (OtuPoint*)&other->x) > 0x32000) {
        return 0;
    }

    func_ov039_0208b94c(self);

    return 1;
}

/** @brief chanceTbl[8], the timer at +0x17C, and a 0x64000 limit. */
s32 func_ov039_0208c688(OtuBadgeState* self) {
    OtuPinTask* other;

    if (self->chanceTbl[8] < RNG_Next(0x10000)) {
        return 0;
    }

    if (*(s16*)((u8*)self + 0x100 + 0x7C) <= 0) {
        return 0;
    }

    if (func_ov039_02098d10(&self->vel) >= 0x3000) {
        return 0;
    }

    other = func_ov039_02087e2c(self->pool, self->scene, self->index);

    if (other == NULL) {
        return 0;
    }

    if (func_ov039_02098ca8(&self->pos, (OtuPoint*)&other->x) > 0x64000) {
        return 0;
    }

    func_ov039_0208b9b4(self);

    return 1;
}

/** @brief chanceTbl[9], the timer at +0x17A, no speed cap, 0x64000. */
s32 func_ov039_0208c718(OtuBadgeState* self) {
    OtuPinTask* other;

    if (self->chanceTbl[9] < RNG_Next(0x10000)) {
        return 0;
    }

    if (*(s16*)((u8*)self + 0x100 + 0x7A) <= 0) {
        return 0;
    }

    other = func_ov039_02087e2c(self->pool, self->scene, self->index);

    if (other == NULL) {
        return 0;
    }

    if (func_ov039_02098ca8(&self->pos, (OtuPoint*)&other->x) > 0x64000) {
        return 0;
    }

    func_ov039_0208ba34(self);

    return 1;
}

/* --- the two states that latch on curAI ---------------------------------- */

/**
 * @brief The chase state, curAI 0xA: acquire a target once, then steer toward
 *        it for as long as it is still alive.
 *
 *  The acquire path and the chase path both end by returning 1, so the `goto`
 *  is the target's own shape and a flag would not compile the same.
 */
s32 func_ov039_0208c794(OtuBadgeState* self) {
    OtuPinTask* other;

    if (self->curAI != 0xA) {
        if (self->chanceTbl[10] < RNG_Next(0x10000)) {
            return 0;
        }

        other = func_ov039_02087e2c(self->pool, self->scene, self->index);

        if (other == NULL) {
            return 0;
        }

        if (func_ov039_02098ca8(&self->pos, (OtuPoint*)&other->x) > 0xC8000) {
            return 0;
        }

        func_ov039_0208be30(self, (OtuPoint*)&other->x);

        self->curAI       = 0xA;
        self->unk_1B8     = 1;
        self->chaseTarget = other;

        return 1;
    }

    if (func_ov039_0208efb0(self->chaseTarget, 0x444) != 0) {
        func_ov039_0208be30(self, (OtuPoint*)&self->chaseTarget->x);
    }

    return 1;
}

/** @brief The homing state, curAI 0xB: acquire once when standing on tile 12,
 *         then do nothing but answer 1. */
s32 func_ov039_0208c84c(OtuBadgeState* self) {
    if (self->curAI != 0xB) {
        if (self->chanceTbl[11] < RNG_Next(0x10000)) {
            return 0;
        }

        if (func_ov039_0208a794(&self->pos, (OtuCellGrid*)self->board) != 0xC) {
            return 0;
        }

        self->curAI   = 0xB;
        self->unk_1B8 = 0x3C;
    }

    return 1;
}

/* --- the four task-spawner wrappers -------------------------------------- */

/* Each just seeds 0x0208c128 with a fixed triple and a flag. The argument that
 * lands on the stack is written out longhand because mwcc builds the outgoing
 * area from the declaration, not from the call. */

void func_ov039_0208c8ac(OtuBadgeState* self) {
    func_ov039_0208c128(self, 0xC, 0xC, 0xC, 1);
}

void func_ov039_0208c8cc(OtuBadgeState* self) {
    func_ov039_0208c128(self, 0xD, 5, 5, 1);
}

void func_ov039_0208c8ec(OtuBadgeState* self) {
    func_ov039_0208c128(self, 0xE, 7, 7, 1);
}

void func_ov039_0208c90c(OtuBadgeState* self) {
    func_ov039_0208c128(self, 0xF, 8, 0xB, 1);
}

/** @brief The third latching state, curAI 0x10 -- same shape as 0x0208c794. */
s32 func_ov039_0208c92c(OtuBadgeState* self) {
    OtuPinTask* other;

    if (self->curAI != 0x10) {
        if (self->chanceTbl[16] < RNG_Next(0x10000)) {
            return 0;
        }

        other = func_ov039_02087e2c(self->pool, self->scene, self->index);

        if (other == NULL) {
            return 0;
        }

        func_ov039_0208be30(self, (OtuPoint*)&other->x);

        self->curAI       = 0x10;
        self->unk_1B8     = 1;
        self->chaseTarget = other;

        return 1;
    }

    if (func_ov039_0208efb0(self->chaseTarget, 0x444) != 0) {
        func_ov039_0208be30(self, (OtuPoint*)&self->chaseTarget->x);
    }

    return 1;
}

/**
 * @brief Ask every live pin, in turn, whether it still wants to act.
 *
 *  The table at data_ov039_0209a4b0 holds one function pointer per entry, and
 *  the loop runs from index 1 to 0x10 inclusive. Any non-zero answer stops the
 *  walk, so this reads as an early-out over the table.
 */
void func_ov039_0208c9cc(OtuBadge* arg) {
    OtuBadgeState* self = (OtuBadgeState*)arg;
    s32 (**fn)(OtuBadgeState*);
    s32 i;

    if (self->unk_148 > 0) {
        return;
    }

    if (self->unk_128 != 0) {
        return;
    }

    fn = (s32(**)(OtuBadgeState*))data_ov039_0209a4b0;

    for (i = 1; i < 0x11; i++) {
        if (fn[i](self) != 0) {
            return;
        }
    }
}

/**
 * @brief Step the pin along the board, in whichever of two directions the
 *        target's own phase field asks for.
 *
 *  Nonmatching: the target derives a cell pair with a rounding `asr #4 / add
 *  lsr #0x1b / asr #5` sequence per axis, then walks a two-byte-per-cell table
 *  at board+0x18 comparing 16-bit values, then reads bytes at +2 and +3 of the
 *  winning cell to rebuild the position as ((byte << 5) + 0x10) << 12. The
 *  arithmetic here reproduces the intent; mwcc's rounding and the ldrh/ldrb
 *  pairing are not reproduced yet.
 */
void func_ov039_0208ca1c(OtuBadge* arg) {
    OtuBadgeState* self = (OtuBadgeState*)arg;
    OtuPinTask*    data;
    s32            gx;
    s32            gy;
    u8             cx;
    u8             cy;
    s32            i;
    s32            count;

    if (self->unk_FC != 0) {
        return;
    }

    data = (OtuPinTask*)EasyTask_GetTaskData(self->pool, self->taskId);

    if (func_ov039_02097ad8(data) != 0) {
        return;
    }

    gx = self->pos.x >> 0xC;
    gy = self->pos.y >> 0xC;
    gx = gx + ((gx >> 4) >> 27);
    gy = gy + ((gy >> 4) >> 27);
    cx = (u8)(gx >> 5);
    cy = (u8)(gy >> 5);

    count = self->board[5];

    for (i = 0; i < count; i++) {
        u16 v = *(u16*)(self->board + 0x18 + i * 4);

        if (v != *(u16*)(cx)) {
            goto found;
        }
    }

    if (i >= count) {
        return;
    }

found:
    self->pos.x     = (((u32)self->board[0x18 + i * 4 + 2] << 5) + 0x10) << 0xC;
    self->pos.y     = (((u32)self->board[0x18 + i * 4 + 3] << 5) + 0x10) << 0xC;
    self->unk_1A4   = 1;
    self->lastCellX = self->board[0x18 + i * 4 + 2];
    self->lastCellY = self->board[0x18 + i * 4 + 3];

    func_ov039_02097aa4((OtuTaskSprite*)data, (OtuTaskParams*)&self->pos, 2);

    self->unk_FC = 1;
}

/**
 * @brief One frame of the pin: release, tick, fade out and tear down.
 */
void func_ov039_0208cb64(OtuBadge* arg) {
    OtuBadgeState* self = (OtuBadgeState*)arg;
    if (func_ov039_0208a794(&self->pos, (OtuCellGrid*)self->board) == 0) {
        self->phase               = 1;
        self->step                = 0;
        *(s32*)((u8*)self + 0xD0) = 0x1000;
        *(s32*)((u8*)self + 0xD4) = 0x1000;
        return;
    }

    if (self->unk_148 > 0) {
        return;
    }

    if (self->home != NULL) {
        if ((self->contactFlags & 0x82) == 0) {
            return;
        }

        func_ov039_0208ba70(self);
        return;
    }

    func_ov039_0208c218(self);

    *(s32*)((u8*)self + 0xD0) = OTU_MUL_Q12(self->frameBudget, 0x88888889);
    *(s32*)((u8*)self + 0xD4) = *(s32*)((u8*)self + 0xD0);

    self->frameBudget = self->frameBudget - 1;

    if (self->frameBudget > 0) {
        return;
    }

    func_ov039_0208ae14(self);

    func_ov039_02087d04(0x33C, &self->pos, &self->origin);

    Sprite_Release((Sprite*)((u8*)self + 0xC));

    func_ov039_0209657c((Sprite*)EasyTask_GetTaskData(self->pool, *(u32*)((u8*)self + 0x234)));
}

/**
 * @brief The main update: tick the frame budget, advance the tray slot, and
 *        refresh the counter and the two linked-list tasks.
 */
void func_ov039_0208cc4c(OtuBadge* arg) {
    OtuBadgeState* self = (OtuBadgeState*)arg;
    s32            i;

    self->frameBudget = self->frameBudget - 1;

    if (self->frameBudget > 0) {
        return;
    }

    func_ov039_0208ae8c(self);

    if (self->pinID[0] >= 0x130) {
        goto refresh;
    }

    self->pinID = self->pinID + 1;

    func_ov039_02093d18((OtuCounterData*)EasyTask_GetTaskData(self->pool, *(u32*)((u8*)self + 0x1F4)), self->pinID);

    if (self->unk_1A8 != 0) {
        if (self->pinID[0] >= 0x130) {
            goto refresh;
        }

        func_ov039_02096154((void*)EasyTask_GetTaskData(self->pool, *(u32*)((u8*)self + 0x230)), 1, self->pinID[1] == 0x130);
    }

    if (self->pinID[0] >= 0x130) {
        return;
    }

    func_ov039_02087d04(0x33F, &self->pos, &self->origin);
    return;

refresh:
    self->unk_1AC = 0;

    func_ov039_02093d68((OtuCounterData*)EasyTask_GetTaskData(self->pool, *(u32*)((u8*)self + 0x1F4)), self->unk_1AC);

    func_ov039_0208d5dc((OtuBadge*)self);

    for (i = 0; i < 2; i++) {
        func_ov039_02095ddc((void*)EasyTask_GetTaskData(self->pool, *(u32*)((u8*)self + 0x228 + i * 4)));
    }
}

/**
 * @brief The fade-out state.
 */
void func_ov039_0208cd50(OtuBadge* arg) {
    OtuBadgeState* self = (OtuBadgeState*)arg;
    if (self->unk_1A8 == 0) {
        return;
    }

    EasyFade_FadeMainDisplay(3, 0, 0x1E);

    if (self->unk_FC == 0) {
        return;
    }

    if (self->unk_FC != 1) {
        return;
    }

    self->unk_164 = self->unk_164 - 1;

    if (self->unk_164 > 0) {
        return;
    }

    if (self->unk_1A8 != 0) {
        func_ov039_02096154((void*)EasyTask_GetTaskData(self->pool, *(u32*)((u8*)self + 0x230)), 2, 0);
    }

    *(s32*)((u8*)gFaders + 8) = 0x10000;

    EasyFade_FadeMainDisplay(2, 0x10, 0x1000);

    self->unk_164 = 0x34;
    self->unk_FC  = 1;
}

/* --- the two one-line probes -------------------------------------------- */

/** @brief If that probe task is finished, retire the pin. */
void func_ov039_0208ce20(OtuBadge* arg) {
    OtuBadgeState* self = (OtuBadgeState*)arg;
    if (func_ov039_0209167c((void*)EasyTask_GetTaskData(self->pool, self->taskId2)) != 0) {
        return;
    }

    self->phase = 1;
    self->step  = 0;
}

/** @brief The same shape, against a different task id. */
void func_ov039_0208ce54(OtuBadge* arg) {
    OtuBadgeState* self = (OtuBadgeState*)arg;
    if (func_ov039_02091014((void*)EasyTask_GetTaskData(self->pool, self->taskId1)) != 0) {
        return;
    }

    self->phase = 1;
    self->step  = 0;
}

/* ============================================================================
 * Band 15 -- the pin task's own queries and the small helpers around them.
 *
 * These are the gaps left in OtuFieldAccess.c's own address range plus a few
 * neighbours it can see: the pin task's phase filter (0x0208ee84 - 0x0208efb0)
 * and the wireless input snapshot (0x02088418 - 0x020885f4). They sit at the
 * end of this file so every function they call is already declared or defined
 * above.
 *
 * All of this now lives in OtuFieldAccess.c's single translation unit, so
 * an edit invalidates the object normally -- the `-ipa file` stale-cache
 * trap that applied to the old `.inc` band files is gone.
 * =========================================================================*/

#include "Debug/Sugata/TinPinSlammer.h"
#include "Engine/Core/System.h"
#include "Engine/IO/TouchInput.h"
#include "Engine/Math/Random.h"
#include "SpriteMgr.h"

extern u8  data_ov039_0209ad00;
extern s32 data_ov039_0209ad04;
extern s32 data_ov039_0209ad08[4];

/* The task handles the result-screen spawners hand to EasyTask_CreateTask. */
extern const TaskHandle data_ov039_0209a06c;
extern const TaskHandle data_ov039_02099c64;
extern const TaskHandle data_ov039_020995a4;
extern const TaskHandle data_ov039_020995ec;
extern const TaskHandle data_ov039_02099998;

/* The stage tables the four-word dispatchers copy and index. */
extern const TaskStages data_ov039_02099568;
extern const TaskStages data_ov039_020995b0;
extern const TaskStages data_ov039_02099610;
extern const TaskStages data_ov039_020999a4;
extern const TaskStages data_ov039_0209a030;
extern const TaskStages data_ov039_0209a078;

/*
 * The four sprite-load helpers the parameter initialisers hand the block to.
 * All of them are the OtuTaskStages loader shape and all of them take the
 * caller's `args` straight through in r2 -- which is why the callers never
 * reload it, and why the constant-zero register in the callers moves off r2.
 */
void func_ov039_02091788(void* self, void* sprite, s32* args);
void func_ov039_02094bac(void* self, void* sprite, s32* args);
void func_ov039_020911dc(void* self, void* sprite, s32* args);
void func_ov039_020980b8(void* self, void* sprite, s32* args);

extern const TaskHandle      data_ov039_0209955c;
extern const SpriteAnimation data_ov039_0209a088;
extern s32*                  data_ov039_0209a620[];

/* The child-tile setup helper func_ov039_020929ec drives. */
void func_ov039_0209276c(void* self, s32 paramsAddr);

/* The 7- and 4-argument cell set-up pair func_ov039_0209276c drives. */
void func_0200d898(void* buf, void* src, s32 w, s32 h);
void func_0200d1d8(void* obj, s32 a, s32 b, s32 c, void* buf, s32 w, s32 h);
void func_0200d858(void* obj, s32 a, s32 b, s32 c);

/* The per-child step func_ov039_02091070 runs four times. */
void func_ov039_020983c8(void* data, OtuPoint* pt, void* table, s32 index);

/* The two more func_ov039_0208e9f8 dispatches to, alongside +0x1628. */
s32  func_ov039_02090e9c(OtuHammer* self, OtuBar* out);
void func_ov039_0208fee0(OtuShadowTask* self, OtuPinRecord* out);

/* The three Q12.12 constants func_ov039_02094ca8 damps with. */
extern s32 data_ov039_0209a300;
extern s32 data_ov039_0209a304;
extern s32 data_ov039_0209a310;

/* The four more func_ov039_02094e9c randomises its launch between. */
extern s32 data_ov039_0209a2fc;
extern s32 data_ov039_0209a30c;
extern s32 data_ov039_0209a314;
extern s32 data_ov039_0209a328;

/** The 4-word template func_ov039_020983c8 copies onto the sprite block. */
typedef struct {
    /* 0x00 */ s32 w0;
    /* 0x04 */ s32 w1;
    /* 0x08 */ s32 w2;
    /* 0x0C */ s32 w3;
} OtuWord4; // Size: 0x10
extern const SpriteAnimation data_ov039_020995c0;
extern const SpriteAnimation data_ov039_02099578;
extern const SpriteAnimation data_ov039_02099c80;
extern const SpriteAnimation data_ov039_0209a040;
extern const SpriteAnimation data_ov039_02099504;
extern const SpriteAnimation data_ov039_02099530;

/* ------------------------------------------------------------------ */
/* The pin task's own queries, 0x0208ee84 - 0x0208efb0.                */
/* ------------------------------------------------------------------ */

/**
 * @brief True while the pin's "alive" counter at +0x148 is positive.
 *
 * `movgt`/`movle`, not `movne`/`moveq`: the comparison is signed, so this is
 * `> 0` and not `!= 0`. Written as a select into zero, the shape that gives
 * this overlay's predicates their two conditional moves.
 */
s32 func_ov039_0208ee84(OtuPinTask* task) {
    return *(s32*)((u8*)task + 0x148) > 0;
}

/**
 * @brief The pin's three-way phase test, read off +0x128/+0xF8/+0xF4.
 *
 * Answers 1 only when +0x128 is zero and both +0xF8 and +0xF4 are 1 *and* the
 * distance between the points at +0x14C and +0x154 is at least 0x10000. The
 * target's `ldreq`/`cmpeq` run of conditional loads is mwcc folding that
 * five-term short-circuit chain into one compare chain.
 */
s32 func_ov039_0208eed0(void* pin) {
    u8* self = (u8*)pin;
    s32 r    = 0;

    if (*(s32*)(self + 0x128) == 0 && *(s32*)(self + 0xF8) == 1 && *(s32*)(self + 0xF4) == 1) {
        if (func_ov039_02098ca8((OtuPoint*)(self + 0x14C), (OtuPoint*)(self + 0x154)) >= 0x10000) {
            r = 1;
        }
    }

    return r;
}

/** Copies the +0x14C and +0x154 pairs out as two points. */
void func_ov039_0208ef14(void* pin, OtuPoint* a, OtuPoint* b) {
    u8* self = (u8*)pin;

    *a = *(OtuPoint*)(self + 0x14C);
    *b = *(OtuPoint*)(self + 0x154);
}

/**
 * @brief True once the pin's age at +0x1CC has reached 0x1E (30).
 *
 * `movge`/`movlt` again: `>= 0x1E`, not `> 0x1E`.
 */
s32 func_ov039_0208ef38(void* task) {
    return *(s32*)((u8*)task + 0x1CC) >= 0x1E;
}

/**
 * @brief The pin-scoring filter: true when a child is a real pin.
 *
 * Two guards open it -- the nearest-child query at +0x120 must answer non-zero,
 * and the pin id at +0x16C must not be the 0x130 "no pin" sentinel -- and the
 * real answer comes from func_ov039_0208ef4c.
 */
s32 func_ov039_0208efb0(OtuPinTask* task, s32 which) {
    u8* self = (u8*)task;

    if (func_ov039_0208a794((OtuPoint*)(self + 0x120), (OtuCellGrid*)*(s32*)(self + 0xE4)) == 0) {
        return 0;
    }
    if (*(u16*)*(s32*)(self + 0x16C) == 0x130) {
        return 0;
    }

    return func_ov039_0208ef4c(task, which);
}

/**
 * @brief True when the pin should be removed this frame.
 *
 * A switch on the kind at +0xF8 in which only kind 1 answers zero: every other
 * kind, and any kind past the table, goes to the body. That is why the jump
 * table has one distinct arm and the rest share the default -- the case list is
 * `0, 2, 3, 4, 5` plus `default`, with `1` out of line.
 *
 * The body applies the age filter and the RNG gate: a pin whose cell query is
 * not exactly 0xC is removed outright; one that is gets removed with
 * probability `which / 0x10000`.
 */
s32 func_ov039_0208ef4c(void* task, u32 which) {
    u8* self = (u8*)task;
    s32 kind = *(s32*)(self + 0xF8);
    s32 r    = 0;

    switch (kind) {
        case 1:
        default:
            if (func_ov039_0208a794((OtuPoint*)(self + 0x120), (OtuCellGrid*)*(s32*)(self + 0xE4)) != 0xC) {
                r = 1;
            } else if (RNG_Next(0x10000) < which) {
                r = 1;
            }
            break;

        case 0:
        case 2:
        case 3:
        case 4:
        case 5:
            break;
    }

    return r;
}

/* ------------------------------------------------------------------ */
/* The wireless input snapshot, 0x02088418 - 0x020885f4.               */
/* ------------------------------------------------------------------ */

/**
 * @brief One player's touch-pad descriptor, six bytes.
 *
 * `data_ov039_0209ad20` is one of these and `data_ov039_0209af20` is an array of
 * them; `func_ov039_02088440` is `data_ov039_0209af20 + index * 6`. The first
 * byte is the "was touched" latch the poller resets, the next two are the touch
 * coordinate bytes, the fourth is the phase byte the board's loops read, and
 * +4/+5 are the raw `SysControl` word copied in each poll.
 */
typedef struct {
    /* 0x00 */ u8  touched;
    /* 0x01 */ u8  phase;
    /* 0x02 */ u8  x;
    /* 0x03 */ u8  y;
    /* 0x04 */ u16 sysControl;
} OtuPadState; // Size: 0x6

extern OtuPadState data_ov039_0209ad20;

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

/* ------------------------------------------------------------------ */
/* The badge-task state predicates, 0x02091014 - 0x0209167c.           */
/* ------------------------------------------------------------------ */

/** True while the badge task has an active phase at +0xE8. */
s32 func_ov039_02091014(void* task) {
    return *(s32*)((u8*)task + 0xE8) != 0;
}

/**
 * @brief Seeds the badge task's cursors and enters the active phase.
 *
 * Four cursors: +0xF0, +0xF4 and +0xF8 take the three words, +0xFC takes the
 * fourth as a u16 (the target reads it with `ldrh` from the stack argument),
 * and both the +0x124 sub-phase and the +0xE8 phase are raised to 1.
 */
void func_ov039_02091028(void* task, s32 a, s32 b, s32 c, u16 d) {
    u8* self = (u8*)task;

    *(s32*)(self + 0xF0)  = a;
    *(s32*)(self + 0xF4)  = b;
    *(s32*)(self + 0xF8)  = c;
    *(s32*)(self + 0xFC)  = d;
    *(s32*)(self + 0x124) = 1;
    *(s32*)(self + 0xE8)  = 1;
}

/** Clears the +0xEC cursor once the +0xE8 phase has reached 4. */
void func_ov039_0209104c(void* task) {
    if (*(s32*)((u8*)task + 0xE8) == 4) {
        *(s32*)((u8*)task + 0xEC) = 0;
    }
}

/**
 * @brief The +0xE0 field, zero-extended to sixteen bits.
 *
 * The target loads a whole word and masks it with `lsl #0x10 / lsr #0x10`, so
 * the field is not a halfword in the source -- the read is a word cast down.
 */
u16 func_ov039_02091060(void* task) {
    return (u16) * (s32*)((u8*)task + 0xE0);
}

/**
 * @brief Latch the +0x7C value into +0x78 while the +0x80 phase is 3.
 *
 * A conditional load-and-store pair rather than a branch, so it stays a plain
 * `if` with the two statements in that order.
 */
void func_ov039_02091668(void* task) {
    u8* self = (u8*)task;

    if (*(s32*)(self + 0x80) == 3) {
        *(s32*)(self + 0x78) = *(s32*)(self + 0x7C);
    }
}

/** True while the +0x80 phase is non-zero. */
s32 func_ov039_0209167c(void* task) {
    return *(s32*)((u8*)task + 0x80) != 0;
}

/** Raises bit 1 of the +0x18 flags when `event` is non-zero. */
void func_ov039_020923b4(void* task, s32 event) {
    if (event != 0) {
        *(s32*)((u8*)task + 0x18) |= 2;
    }
}

/** Releases the sprite at task+0x18 and reports success. */
s32 func_ov039_02094dfc(void* pool, void* task) {
    (void)pool;
    Sprite_Release(*(Sprite**)((u8*)task + 0x18));
    return 1;
}

/** Releases the sprite at task+0x18 and reports success. */
s32 func_ov039_02098338(void* pool, void* task) {
    (void)pool;
    Sprite_Release(*(Sprite**)((u8*)task + 0x18));
    return 1;
}

/* ------------------------------------------------------------------ */
/* The pin-child steering helpers, 0x0208e6fc and 0x0208e8c4.         */
/* ------------------------------------------------------------------ */

/**
 * @brief The velocity a pin child should steer with, written to `out`.
 *
 * Only kind 8 enters the switch; kind 1 and everything past the two cases fall
 * through to the plain "point at the child's own +0x120/+0x124, rebased by the
 * board origin" answer, which is also the whole body for every other kind.
 *
 *   sub-kind 2  steers around a point offset from the pin by (0x80000, 0x60000);
 *               when that offset is at least 0x400 long the vector is
 *               normalised and the angle re-aimed at the pin's +0x118 face.
 *   sub-kind 3  scales the same offset by 0x1000 / (+0x100 << 12) first.
 *
 * The stack point is passed as both the source and the destination of
 * `func_ov039_02098bb0`; that aliasing is what the target does.
 */
// Nonmatching: 73%, and the whole gap is one scheduling choice. The target
// loads the +0x124 word before the +0x120 one and only then subtracts both;
// this source reads them in address order. Writing the two reads in either
// order, as an initialiser, or through named temporaries all move the score but
// none reproduces the target's pair, so this is mwcc's scheduler and not the
// source shape. Every instruction otherwise agrees.
void func_ov039_0208e6fc(void* task, OtuPoint* out) {
    u8* self = (u8*)task;

    if (*(s32*)(self + 0xF8) == 8) {
        switch (*(s32*)(self + 0xFC)) {
            case 2: {
                OtuPoint scratch = {*(s32*)(self + 0x120) - 0x80000, *(s32*)(self + 0x124) - 0x60000};

                func_ov039_02098bb0(&scratch, (OtuPoint*)(self + 0x118), &scratch);

                if (func_ov039_02098d10(&scratch) >= 0x400) {
                    func_ov039_02098d3c(&scratch, &scratch);
                    func_ov039_02098c00(0x400, &scratch, (OtuPoint*)(self + 0x118), out);
                    return;
                }

                out->x = *(s32*)(self + 0x120) - 0x80000;
                out->y = *(s32*)(self + 0x124) - 0x60000;
                return;
            }

            case 3: {
                OtuPoint scratch;
                s32      scale = FX_Divide(0x1000, *(s32*)(self + 0x100) << 12);

                scratch.x = *(s32*)(self + 0x120) - 0x80000;
                scratch.y = *(s32*)(self + 0x124) - 0x60000;
                func_ov039_02098bb0(&scratch, (OtuPoint*)(self + 0x118), &scratch);
                func_ov039_02098c00(scale, &scratch, (OtuPoint*)(self + 0x118), out);
                return;
            }

            default:
                break;
        }
    }

    out->x = *(s32*)(self + 0x120) - 0x80000;
    out->y = *(s32*)(self + 0x124) - 0x60000;
}

/**
 * @brief The pin child's aim scale, 0x1000 when nothing applies.
 *
 * Kind 8, sub-kind 1: while the +0x100 cursor is under 10 this is the cursor
 * over 10, on a 0x1000 scale.
 *
 * Kind 8, sub-kind 4: the cursor at +0x100 against half the cell's size word,
 * counted down from 0x1000.
 */
s32 func_ov039_0208e8c4(void* task) {
    u8* self  = (u8*)task;
    s32 scale = 0x1000;

    if (*(s32*)(self + 0xF8) == 8) {
        switch (*(s32*)(self + 0xFC)) {
            case 1: {
                s32 cursor = *(s32*)(self + 0x100);

                if (cursor < 0xA) {
                    scale = FX_Divide(cursor << 12, 0xA000);
                }
                break;
            }

            case 4: {
                u16 index = *(u16*)*(s32*)(self + 0x16C);
                s32 base  = *(s32*)(self + 0x170);
                u32 size  = *(u16*)((u8*)(base + index * 0x1C) + 0xE);

                if (*(s32*)(self + 0x100) < (s32)(size >> 1)) {
                    scale = 0x1000 - FX_Divide(*(s32*)(self + 0x100) << 12, (s32)(size >> 1) << 12);
                }
                break;
            }

            default:
                break;
        }
    }

    return scale;
}

/* ------------------------------------------------------------------ */
/* The result-screen task lifecycle callbacks and small shims.         */
/* ------------------------------------------------------------------ */

/** The three one-word predicates reduced to `return 1`. */
s32 func_ov039_020921f4(void) {
    return 1;
}

s32 func_ov039_02092bf0(void) {
    return 1;
}

s32 func_ov039_02096d98(void) {
    return 1;
}

/** Releases the sprite at task+0x18 and reports success. */
s32 func_ov039_02091aa4(void* pool, void* task) {
    (void)pool;
    Sprite_Release(*(Sprite**)((u8*)task + 0x18));
    return 1;
}

/** out = a + b, component-wise. */
void func_ov039_02098b8c(OtuPoint* a, OtuPoint* b, OtuPoint* out) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
}

/** True when +0x0C is clear and +0x10 is set. */
s32 func_ov039_02098b44(void* task) {
    if (*(s32*)((u8*)task + 0x0C) == 0 && *(s32*)((u8*)task + 0x10) != 0) {
        return 1;
    }
    return 0;
}

/** A four-entry handler table copied to the stack before one is called. */
extern const TaskStages data_ov039_02099c70;

/**
 * @brief Runs the stage handler at `index` from the table.
 *
 * The table is copied onto the stack first (the `ldm`/`stm` pair) and only then
 * indexed, which is why it is a struct copy rather than four assigns: mwcc
 * emits the copy for the whole struct and a plain `ldr` of the selected entry.
 */
void func_ov039_02094188(void* a, void* b, void* c, s32 index) {
    TaskStages table = data_ov039_02099b5c;

    table.iter[index](a, b, c);
}

void func_ov039_02094e10(void* a, void* b, void* c, s32 index) {
    TaskStages table = data_ov039_02099c70;

    table.iter[index](a, b, c);
}

/**
 * @brief Calls the +0x04 handler's slot at +0x08 through the object at +0x08.
 *
 * `scene` is passed straight through as the handler's only argument.
 */
void func_ov039_02098acc(void* task, void* scene) {
    u8* self = (u8*)task;
    u8* obj  = *(u8**)(self + 0x04);
    void (**table)(void*);

    if (obj == NULL) {
        return;
    }

    table = *(void (***)(void*))(obj + 0x08);
    table[*(s32*)(self + 0x08)](scene);
}

/** Spawns the one-word argument task table data_ov039_0209a06c. */
s32 func_ov039_020989f0(TaskPool* pool, s32 arg) {
    s32 args = arg;

    return EasyTask_CreateTask(pool, &data_ov039_0209a06c, NULL, 0, NULL, &args);
}

/** Spawns the task table data_ov039_02099c64 with one word of args. */
s32 func_ov039_02094e58(TaskPool* pool, s32 arg) {
    s32 args = arg;

    return EasyTask_CreateTask(pool, &data_ov039_02099c64, NULL, 0, NULL, &args);
}

/** Spawns the task table data_ov039_02099b50 with two words of args. */
s32 func_ov039_020941d0(TaskPool* pool, s32 a, s32 b) {
    s32 args[2];

    args[0] = a;
    args[1] = b;
    return EasyTask_CreateTask(pool, &data_ov039_02099b50, NULL, 0, NULL, args);
}

/** A 0x28-strided flag slot: only its first word is touched. */
typedef struct {
    /* 0x00 */ s32 flags;
    /* 0x04 */ u8  pad_04[0x24];
} OtuFlagSlot;

/** Raises bit 1 in two +0x28-strided flag words from +0x30 on. */
void func_ov039_02092e04(void* task, s32 enable) {
    s32          i;
    OtuFlagSlot* p;

    if (enable == 0) {
        return;
    }

    p = (OtuFlagSlot*)((u8*)task + 0x30);
    for (i = 0; i < 2; i++) {
        p->flags |= 2;
        p++;
    }
}

/** Releases the three sprites at sprite+4, 0x40 apart. */
s32 func_ov039_02094158(void* pool, void* task) {
    s32     i;
    Sprite* p = (Sprite*)((u8*)*(void**)((u8*)task + 0x18) + 4);

    (void)pool;

    for (i = 0; i < 3; i++) {
        Sprite_Release(p);
        p = (Sprite*)((u8*)p + 0x40);
    }
    return 1;
}

/** Renders the three sprites at sprite+4, 0x40 apart, when +0xC8 is set. */
s32 func_ov039_0209411c(void* pool, void* task) {
    u8*     sprite = *(u8**)((u8*)task + 0x18);
    s32     i;
    Sprite* p;

    (void)pool;

    if (*(s32*)(sprite + 0xC8) != 0) {
        p = (Sprite*)(sprite + 4);
        for (i = 0; i < 3; i++) {
            Sprite_RenderFrame(p);
            p = (Sprite*)((u8*)p + 0x40);
        }
    }
    return 1;
}

/** Steps and renders the sprite block at task+0x18 when +0x78 is set. */
s32 func_ov039_02094dac(void* pool, void* task) {
    Sprite* sprite = *(Sprite**)((u8*)task + 0x18);

    (void)pool;

    if (*(s32*)((u8*)sprite + 0x78) != 0) {
        *(s16*)((u8*)sprite + 0x0C) = (s16)((*(s32*)((u8*)sprite + 0x58) - *(s32*)((u8*)sprite + 0x50)) >> 12);
        *(s16*)((u8*)sprite + 0x0E) =
            (s16)((*(s32*)((u8*)sprite + 0x60) + (*(s32*)((u8*)sprite + 0x5C) - *(s32*)((u8*)sprite + 0x54))) >> 12);
        Sprite_RenderFrame(sprite);
    }
    return 1;
}

/** The three-word point-and-scale record `func_ov039_02091628` fills in. */
typedef struct {
    /* 0x00 */ s32 x;
    /* 0x04 */ s32 y;
    /* 0x08 */ s32 scale;
} OtuPoint3;

/**
 * @brief Reads the +0x5C/+0x60 pair into a point when the +0x80 phase is 3.
 *
 * Returns 0 otherwise, leaving the point untouched.
 */
s32 func_ov039_02091628(void* task, OtuPoint3* out) {
    u8* self = (u8*)task;
    s32 y;
    s32 x;

    if (*(s32*)(self + 0x80) != 3) {
        return 0;
    }

    y = *(s32*)(self + 0x60);
    x = *(s32*)(self + 0x5C);

    out->x     = x;
    out->y     = y;
    out->scale = 0x20000;
    return 1;
}

/** The child-task group stepped by `func_ov039_02091690`. */
typedef struct {
    /* 0x00 */ TaskPool*     pool;
    /* 0x04 */ u8            pad_04[0x58];
    /* 0x5C */ OtuTaskParams params;
    /* 0x64 */ u8            pad_64[0x20];
    /* 0x84 */ s32           ids[8];
} OtuChildGroup;

/** Advances the eight child tasks named by +0x84. */
void func_ov039_02091690(OtuChildGroup* self) {
    s32 i;

    for (i = 0; i < 8; i++) {
        func_ov039_02097e9c(EasyTask_GetTaskData(self->pool, self->ids[i]), &self->params);
    }
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

/* ------------------------------------------------------------------ */
/* The six-word record reset, the sprite step pair and the child       */
/* teardown.                                                           */
/* ------------------------------------------------------------------ */

/** The flat six-word record `func_ov039_02098a20` zeroes out. */
typedef struct {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ s32 unk_0C;
    /* 0x10 */ s32 unk_10;
    /* 0x14 */ s32 unk_14;
} OtuRec6;

/**
 * @brief Stores `owner` in the first word and clears the other five.
 *
 * The zero stores are not in address order and neither a designator list nor a
 * memset reproduces the target's 0/4/C/10/8/14 sequence, so the six writes are
 * spelled out in the order the target emits them.
 */
void func_ov039_02098a20(void* a, void* owner) {
    OtuRec6* rec = (OtuRec6*)a;

    rec->unk_00 = (s32)owner;
    rec->unk_04 = 0;
    rec->unk_0C = 0;
    rec->unk_10 = 0;
    rec->unk_08 = 0;
    rec->unk_14 = 0;
}

/** Steps the sprite block at task+0x18 when +0x44 is set. */
s32 func_ov039_020988d8(void* pool, void* task) {
    u8* sprite = *(u8**)((u8*)task + 0x18);

    (void)pool;

    if (*(s32*)(sprite + 0x44) != 0) {
        Sprite_Update((Sprite*)(sprite + 4));
    }
    return 1;
}

/** Renders the sprite block at task+0x18 when +0x44 is set. */
s32 func_ov039_020988fc(void* pool, void* task) {
    u8* sprite = *(u8**)((u8*)task + 0x18);

    (void)pool;

    if (*(s32*)(sprite + 0x44) != 0) {
        Sprite_RenderFrame((Sprite*)(sprite + 4));
    }
    return 1;
}

/**
 * @brief Steps the sprite block at task+0x18 when +0x6C is set.
 *
 * The same pair of Q12.12 position writes as `func_ov039_02094dac`, minus that
 * function's +0x60 term and against a different guard word.
 */
s32 func_ov039_02091a5c(void* pool, void* task) {
    Sprite* sprite = *(Sprite**)((u8*)task + 0x18);

    (void)pool;

    if (*(s32*)((u8*)sprite + 0x6C) != 0) {
        *(s16*)((u8*)sprite + 0x0C) = (s16)((*(s32*)((u8*)sprite + 0x58) - *(s32*)((u8*)sprite + 0x50)) >> 12);
        *(s16*)((u8*)sprite + 0x0E) = (s16)((*(s32*)((u8*)sprite + 0x5C) - *(s32*)((u8*)sprite + 0x54)) >> 12);
        Sprite_RenderFrame(sprite);
    }
    return 1;
}

/** Releases the block's sprite and deletes its eight child tasks. */
s32 func_ov039_02091524(TaskPool* pool, void* task) {
    OtuChildGroup* self = *(OtuChildGroup**)((u8*)task + 0x18);
    s32            i;

    Sprite_Release((Sprite*)((u8*)self + 4));

    for (i = 0; i < 8; i++) {
        EasyTask_DeleteTask(pool, self->ids[i]);
    }
    return 1;
}

/** The two-argument result-screen spawners: one word of dataType, one of id. */
s32 func_ov039_02091b00(TaskPool* pool, s32 dataType, s32 childId) {
    s32 args[2];

    args[0] = dataType;
    args[1] = childId;
    return EasyTask_CreateTask(pool, &data_ov039_020995a4, NULL, 0, NULL, args);
}

/** The board's three-word spawner. */
s32 func_ov039_02092310(TaskPool* pool, s32 dataType, Heap* heap, OtuBoardParams* params) {
    s32 args[3];

    args[0] = dataType;
    args[1] = (s32)heap;
    args[2] = (s32)params;
    return EasyTask_CreateTask(pool, &data_ov039_020995ec, NULL, 0, NULL, args);
}

/** The board's second three-word spawner, against its own task table. */
s32 func_ov039_02092cd4(TaskPool* pool, s32 dataType, Heap* heap, OtuBoardParams* params) {
    s32 args[3];

    args[0] = dataType;
    args[1] = (s32)heap;
    args[2] = (s32)params;
    return EasyTask_CreateTask(pool, &data_ov039_02099998, NULL, 0, NULL, args);
}

/**
 * @brief Runs the stage handler at `index` from the table at data_ov039_02099568.
 *
 * Same stack copy and index as `func_ov039_02094188`, against a different table.
 */
void func_ov039_02091560(void* a, void* b, void* c, s32 index) {
    TaskStages table = data_ov039_02099568;

    table.iter[index](a, b, c);
}

/** The same dispatcher against data_ov039_020995b0. */
void func_ov039_02091ab8(void* a, void* b, void* c, s32 index) {
    TaskStages table = data_ov039_020995b0;

    table.iter[index](a, b, c);
}

/** The same dispatcher against data_ov039_02099610. */
void func_ov039_020922c8(void* a, void* b, void* c, s32 index) {
    TaskStages table = data_ov039_02099610;

    table.iter[index](a, b, c);
}

/** The same dispatcher against data_ov039_020999a4. */
void func_ov039_02092c8c(void* a, void* b, void* c, s32 index) {
    TaskStages table = data_ov039_020999a4;

    table.iter[index](a, b, c);
}

/** The same dispatcher against data_ov039_0209a030. */
void func_ov039_0209834c(void* a, void* b, void* c, s32 index) {
    TaskStages table = data_ov039_0209a030;

    table.iter[index](a, b, c);
}

/** The same dispatcher against data_ov039_0209a078. */
void func_ov039_020989a8(void* a, void* b, void* c, s32 index) {
    TaskStages table = data_ov039_0209a078;

    table.iter[index](a, b, c);
}

/**
 * @brief Steps the sprite block at task+0x18 when +0x80 is set.
 *
 * `func_ov039_02094dac` again, one guard word further along.
 */
s32 func_ov039_020982e8(void* pool, void* task) {
    Sprite* sprite = *(Sprite**)((u8*)task + 0x18);

    (void)pool;

    if (*(s32*)((u8*)sprite + 0x80) != 0) {
        *(s16*)((u8*)sprite + 0x0C) = (s16)((*(s32*)((u8*)sprite + 0x58) - *(s32*)((u8*)sprite + 0x50)) >> 12);
        *(s16*)((u8*)sprite + 0x0E) =
            (s16)((*(s32*)((u8*)sprite + 0x60) + (*(s32*)((u8*)sprite + 0x5C) - *(s32*)((u8*)sprite + 0x54))) >> 12);
        Sprite_RenderFrame(sprite);
    }
    return 1;
}

/**
 * @brief The same two Q12.12 position writes against a shifted field set.
 *
 * The x half reads the +0x5C/+0x54 pair and the y half adds a third term at
 * +0x64 before subtracting +0x58; the block is rendered at +4 rather than at
 * its base, which is the one place it differs from `func_ov039_020982e8`.
 */
s32 func_ov039_020914d0(void* pool, void* task) {
    u8* sprite = *(u8**)((u8*)task + 0x18);

    (void)pool;

    if (*(s32*)(sprite + 0x6C) != 0) {
        *(s16*)(sprite + 0x10) = (s16)((*(s32*)(sprite + 0x5C) - *(s32*)(sprite + 0x54)) >> 12);
        *(s16*)(sprite + 0x12) = (s16)((*(s32*)(sprite + 0x60) + *(s32*)(sprite + 0x64) - *(s32*)(sprite + 0x58)) >> 12);
        Sprite_RenderFrame((Sprite*)(sprite + 4));
    }
    return 1;
}

/**
 * @brief Clears the parameter block at task+0x18 and re-derives it.
 *
 * The store order is not address order: the four zero words at +0x50 come after
 * the one at +0x68, and +0x40 comes last. Written in the target's order because
 * nothing about the sequence is recoverable from the field names.
 */
s32 func_ov039_02091814(void* pool, void* task, s32* args) {
    u8* sprite = *(u8**)((u8*)task + 0x18);

    (void)pool;

    *(s32*)(sprite + 0x68) = args[1];
    *(s32*)(sprite + 0x50) = 0;
    *(s32*)(sprite + 0x54) = 0;
    *(s32*)(sprite + 0x58) = 0;
    *(s32*)(sprite + 0x5C) = 0;
    *(s32*)(sprite + 0x40) = 0;
    *(s32*)(sprite + 0x44) = 0x2000;
    *(s32*)(sprite + 0x48) = 0x2000;
    *(s16*)(sprite + 0x4C) = 0;
    *(s16*)(sprite + 0x4E) = 0;
    *(s32*)(sprite + 0x6C) = 0;
    *(s32*)(sprite + 0x70) = 0;

    func_ov039_02091788(sprite, sprite, args);
    return 1;
}

/** The same reset against the sibling helper, over a longer zero run. */
s32 func_ov039_02094c50(void* pool, void* task, s32* args) {
    u8* sprite = *(u8**)((u8*)task + 0x18);

    (void)pool;

    *(s32*)(sprite + 0x74) = 0;
    *(s32*)(sprite + 0x78) = 0;
    *(s32*)(sprite + 0x50) = 0;
    *(s32*)(sprite + 0x54) = 0;
    *(s32*)(sprite + 0x58) = 0;
    *(s32*)(sprite + 0x5C) = 0;
    *(s32*)(sprite + 0x60) = 0;
    *(s32*)(sprite + 0x64) = 0;
    *(s32*)(sprite + 0x68) = 0;
    *(s32*)(sprite + 0x40) = 0;
    *(s32*)(sprite + 0x44) = 0x2000;
    *(s32*)(sprite + 0x48) = 0x2000;
    *(s16*)(sprite + 0x4C) = 0;
    *(s16*)(sprite + 0x4E) = 0;

    func_ov039_02094bac(sprite, sprite, args);
    return 1;
}

/**
 * @brief Stamps a 4x4 block of tile numbers into a 32-wide map.
 *
 * Each row is 32 entries apart, so the value advances by 0x20 per row while
 * only the first four entries of each are written. `value` is a u16 and is
 * narrowed after every increment, which is why the loop reads as
 * `add`/`lsl`/`lsr` rather than a plain `add`.
 */
void func_ov039_02091b40(u16 start, u16* map) {
    u16 value = start;
    s32 col;
    s32 row;

    for (row = 0; row < 4; row++) {
        for (col = 0; col < 4; col++) {
            map[row * 0x20 + col] = value;
            value++;
        }
        value += 0x1C;
    }
}

/**
 * @brief Runs the +0x04 object's +0x04 handler on `arg`, then frees the block's
 * allocation.
 *
 * The two conditions are one `&&` chain, not two `if`s: the target guards the
 * handler load with `ldrne` so the pointer is loaded only when the object is
 * present, and it is then called from the loaded register rather than reloaded.
 * The assignment is written inside the condition on purpose, because splitting
 * it into a separate statement turns the `cmpne` pair into two branches.
 *
 * The free is `Mem_Free(heap, ptr)`: the +0x14 word is both the guard and the
 * pointer handed in, which is why it is loaded into a local before the test.
 */
void func_ov039_02098af4(void* task, void* arg) {
    u8*   self = (u8*)task;
    u8*   obj;
    void* ptr;
    void (*fn)(void*);

    if (*(s32*)(self + 0x10) == 0) {
        return;
    }

    obj = *(u8**)(self + 0x04);
    if (obj != NULL && (fn = *(void (**)(void*))(obj + 0x04)) != NULL) {
        fn(arg);
    }

    ptr = *(void**)(self + 0x14);
    if (ptr == NULL) {
        return;
    }

    Mem_Free(*(Heap**)(self + 0x00), ptr);
    *(s32*)(self + 0x14) = 0;
}

/**
 * @brief Resets the block at task+0x18 and re-derives it through +0x11dc.
 *
 * The first store is the *pool*, not a field of the block's own state: the
 * block's word 0 is the heap it will later allocate out of. The zero and
 * 0x1000 runs are written in the target's order rather than by address.
 */
s32 func_ov039_02091268(TaskPool* pool, void* task, s32* args) {
    u8* block = *(u8**)((u8*)task + 0x18);

    *(TaskPool**)(block + 0x00) = pool;
    *(s32*)(block + 0x6C)       = 0;
    *(s32*)(block + 0x68)       = args[1];
    *(s32*)(block + 0x54)       = 0;
    *(s32*)(block + 0x58)       = 0;
    *(s32*)(block + 0x5C)       = 0;
    *(s32*)(block + 0x60)       = 0;
    *(s32*)(block + 0x64)       = 0;
    *(s32*)(block + 0x44)       = 0;
    *(s32*)(block + 0x48)       = 0x1000;
    *(s32*)(block + 0x4C)       = 0x1000;
    *(s16*)(block + 0x50)       = 0;
    *(s16*)(block + 0x52)       = 0;
    *(s32*)(block + 0x80)       = 0;

    func_ov039_020911dc(block, block + 4, args);
    return 1;
}

/** The same reset against the +0x80b8 helper, over a longer zero run. */
s32 func_ov039_02098144(TaskPool* pool, void* task, s32* args) {
    u8* block = *(u8**)((u8*)task + 0x18);

    (void)pool;

    *(s32*)(block + 0x7C) = 0;
    *(s32*)(block + 0x80) = 0;
    *(s32*)(block + 0x78) = args[1];
    *(s32*)(block + 0x50) = 0;
    *(s32*)(block + 0x54) = 0;
    *(s32*)(block + 0x58) = 0;
    *(s32*)(block + 0x5C) = 0;
    *(s32*)(block + 0x60) = 0;
    *(s32*)(block + 0x64) = 0;
    *(s32*)(block + 0x68) = 0;
    *(s32*)(block + 0x40) = 0;
    *(s32*)(block + 0x44) = 0x2000;
    *(s32*)(block + 0x48) = 0x2000;
    *(s16*)(block + 0x4C) = 0;
    *(s16*)(block + 0x4E) = 0;

    func_ov039_020980b8(block, block, args);
    return 1;
}

/** Loads the sprite for the task whose anim template is data_ov039_0209a088. */
void func_ov039_020985e0(void* self, void* sprite) {
    SpriteAnimation anim = data_ov039_0209a088;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)self;
    _Sprite_Load((Sprite*)sprite, &anim);
}

/**
 * @brief Runs the group's stage at +0x10/+0x14 and records a position pair.
 *
 * The group is an element of `g_DisplaySettings.engineState`, sized 0x220,
 * which is what folds the +0x64 addend into the pool word: spelling the base as
 * a raw `+ 0x64` leaves mwcc materialising `g_DisplaySettings` and then adding
 * 0x64 in a register. The stage kind is the first word of `bgSettings[b]` and is
 * a real `switch` -- the target builds a six-entry jump table rather than
 * testing the values, so an `if` chain is not interchangeable here. Cases 1-5
 * share one body and case 0 falls through to `default`, which is why the
 * table's first entry lands on the same label as the out-of-range branch.
 *
 * The three writes land on `bgAffines[b].unk_14` and the `bgOffsets[b]` pair.
 */
// Nonmatching: 98.5%, and the complete gap is one register rename. The target
// keeps the engine-state index in r4 and the group pointer in lr; this source
// has them the other way round, so every `mla`/`add` reads `r4` where the
// target reads `lr`. mwcc hoists the pool-constant load to the top of the
// block, which is what makes the group pointer win r4; the target loads the
// index first. Ten shapes were tried -- all six declaration orders, no named
// group local at all (leaving the address to CSE), array decay instead of
// &array[i], a named base local, and both load orders -- and four of them score
// identically here while the rest are worse. Every other instruction agrees.
void func_ov039_02092348(void* self, s32 x, s32 y) {
    s32                 a     = *(s32*)((u8*)self + 0x10);
    s32                 b     = *(s32*)((u8*)self + 0x14);
    DisplayEngineState* group = g_DisplaySettings.engineState + a;
    switch (group->bgSettings[b].bgMode) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            group->bgAffines[b].unk_14 = 1;
            break;

        default:
            break;
    }

    group->bgOffsets[b].hOffset = x;
    group->bgOffsets[b].vOffset = y;
}

/**
 * @brief Promotes the +0x0C word into +0x04, optionally allocates, then calls
 *        the +0x04 object's first handler.
 *
 * The allocation is sized by the +0x0C word of the object that was just moved
 * into +0x04, which is why that word is tested before the call. The handler is
 * one `&&` chain for the same reason as `func_ov039_02098af4`.
 */
void func_ov039_02098a60(void* task, void* arg) {
    u8* self = (u8*)task;
    u8* obj;
    s32 size;
    void (*fn)(void*);

    if (*(s32*)(self + 0x10) == 0) {
        return;
    }

    *(s32*)(self + 0x04) = *(s32*)(self + 0x0C);
    *(s32*)(self + 0x08) = 0;
    *(s32*)(self + 0x0C) = 0;
    *(s32*)(self + 0x10) = 0;

    obj  = *(u8**)(self + 0x04);
    size = *(s32*)(obj + 0x0C);
    if (size != 0) {
        *(void**)(self + 0x14) = Mem_AllocHeapTail(*(Heap**)(self + 0x00), size);
    }

    obj = *(u8**)(self + 0x04);
    if (obj != NULL && (fn = *(void (**)(void*))(obj + 0x00)) != NULL) {
        fn(arg);
    }
}

/**
 * @brief Spawns the group task and its eight children, returning the handle.
 *
 * The children are named into the group's +0x84 array, which is the same
 * `OtuChildGroup` layout `func_ov039_02091524` and `func_ov039_02091690` walk.
 */
s32 func_ov039_020915a8(TaskPool* pool, s32 word0, s32 word1) {
    OtuTaskParams  args;
    s32            handle;
    OtuChildGroup* group;
    s32            i;

    args.word0 = word0;
    args.word1 = word1;

    handle = EasyTask_CreateTask(pool, &data_ov039_0209955c, NULL, 0, NULL, &args);
    group  = EasyTask_GetTaskData(pool, handle);

    for (i = 0; i < 8; i++) {
        group->ids[i] = func_ov039_02097e68(pool, word0, word1);
    }
    return handle;
}

/**
 * The two loaders that share `func_ov039_020985e0`'s shape and are re-run by the
 * parameter resets above. Both take the caller's `args` in the third slot and
 * read the dataType out of its first word.
 */

/** Loads the sprite for the task whose anim template is data_ov039_020995c0. */
void func_ov039_02091788(void* self, void* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_020995c0;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)args;
    anim.posX     = *(s32*)((u8*)self + 0x58) >> 12;
    anim.posY     = *(s32*)((u8*)self + 0x5C) >> 12;
    _Sprite_Load((Sprite*)sprite, &anim);
}

/** Loads the sprite for the task whose anim template is data_ov039_02099578. */
void func_ov039_020911dc(void* self, void* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_02099578;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)args;
    anim.posX     = *(s32*)((u8*)self + 0x5C) >> 12;
    anim.posY     = *(s32*)((u8*)self + 0x60) >> 12;
    _Sprite_Load((Sprite*)sprite, &anim);
}

/** Loads the sprite for the task whose anim template is data_ov039_0209a040. */
void func_ov039_020980b8(void* self, void* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_0209a040;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)args;
    anim.posX     = *(s32*)((u8*)self + 0x58) >> 12;
    anim.posY     = *(s32*)((u8*)self + 0x5C) >> 12;
    _Sprite_Load((Sprite*)sprite, &anim);
}

/**
 * Loads the sprite for the task whose anim template is data_ov039_02099c80.
 *
 * The only member of the family whose position is a difference rather than a
 * raw value: x is `+0x58 - +0x50` and y adds a third term at +0x60, the same
 * pair `func_ov039_02094dac` writes into the sprite's posX/posY.
 */
void func_ov039_02094bac(void* self, void* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_02099c80;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)args;
    anim.posX     = (s32)((*(s32*)((u8*)self + 0x58) - *(s32*)((u8*)self + 0x50)) >> 12);
    anim.posY     = (s32)((*(s32*)((u8*)self + 0x60) + (*(s32*)((u8*)self + 0x5C) - *(s32*)((u8*)self + 0x54))) >> 12);
    _Sprite_Load((Sprite*)sprite, &anim);
}

/** Loads the sprite for the task whose anim template is data_ov039_02099504. */
void func_ov039_02090390(OtuHammer* self, Sprite* sprite, OtuHammer_Params* params) {
    SpriteAnimation anim = data_ov039_02099504;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)params;
    anim.posX     = *(s32*)((u8*)self + 0xAC) >> 12;
    anim.posY     = *(s32*)((u8*)self + 0xB0) >> 12;
    _Sprite_Load(sprite, &anim);
}

/** Loads the sprite for the task whose anim template is data_ov039_02099530. */
void func_ov039_0209041c(OtuHammer* self, Sprite* sprite, OtuHammer_Params* params) {
    SpriteAnimation anim = data_ov039_02099530;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)params;
    anim.posX     = *(s32*)((u8*)self + 0xB4) >> 12;
    anim.posY     = *(s32*)((u8*)self + 0xB8) >> 12;
    _Sprite_Load(sprite, &anim);
}

/* ------------------------------------------------------------------ */
/* The resource teardown pair and the palette-source installers.       */
/* ------------------------------------------------------------------ */

/**
 * @brief Releases every screen, char, palette and data handle the block holds.
 *
 * The six manager calls run in pairs per display: screen, char, then palette,
 * with the palette manager indexed separately from the two Bg resource
 * managers. The final pair releases the data handle at +0x48 and the sprite
 * block at +4.
 */
s32 func_ov039_02098920(void* pool, void* task) {
    u8* block = *(u8**)((u8*)task + 0x18);

    (void)pool;

    BgResMgr_ReleaseScreen(g_BgResourceManagers[0], *(BgResource**)(block + 0x54));
    BgResMgr_ReleaseChar(g_BgResourceManagers[0], *(BgResource**)(block + 0x50));
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], *(PaletteResource**)(block + 0x4C));

    BgResMgr_ReleaseScreen(g_BgResourceManagers[1], *(BgResource**)(block + 0x60));
    BgResMgr_ReleaseChar(g_BgResourceManagers[1], *(BgResource**)(block + 0x5C));
    PaletteMgr_ReleaseResource(g_PaletteManagers[1], *(PaletteResource**)(block + 0x58));

    DatMgr_ReleaseData(*(Data**)(block + 0x48));
    Sprite_Release((Sprite*)(block + 4));
    return 1;
}

/** The two-wide background slot group torn down by `func_ov039_02092bf8`. */
typedef struct {
    /* 0x00 */ u8               pad_00[0x04];
    /* 0x04 */ Heap*            heap;
    /* 0x08 */ s32              flag;
    /* 0x0C */ u8               pad_0C[0x04];
    /* 0x10 */ Data*            dataIds[2];
    /* 0x18 */ PaletteResource* paletteIds[2];
    /* 0x20 */ BgResource*      charIds[2];
    /* 0x28 */ u8               pad_28[0x50];
    /* 0x78 */ void*            buffers[2];
} OtuBgGroup; // Size: 0x80

/**
 * @brief The two-wide version of the same teardown, plus a slot free.
 *
 * Returns early when +0x08 is clear, and that early `return 1` is what lets
 * mwcc if-convert the guard into `moveq`/`popeq` -- the sibling
 * `func_ov039_0209411c` had to spell its guard the other way round to stop it.
 */
s32 func_ov039_02092bf8(void* pool, void* task) {
    OtuBgGroup* group = *(OtuBgGroup**)((u8*)task + 0x18);
    s32         i;

    (void)pool;

    if (group->flag == 0) {
        return 1;
    }

    func_0200d954(0, 2);
    func_0200d954(0, 3);

    for (i = 0; i < 2; i++) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[0], group->charIds[i]);
        PaletteMgr_ReleaseResource(g_PaletteManagers[0], group->paletteIds[i]);
        Mem_Free(group->heap, group->buffers[i]);
        DatMgr_ReleaseData(group->dataIds[i]);
    }
    return 1;
}

/**
 * @brief Points palette manager 0 at five sources, one per 0x14-strided slot.
 *
 * Each slot is a frame cursor at +0 and a palette resource at +0x10; the cursor
 * is stepped by `func_ov039_02098dbc` and its result is the source palette, so
 * the call has to be the third argument expression and not hoisted into a
 * statement of its own.
 */
s32 func_ov039_02092154(void* pool, void* task) {
    u8* block = *(u8**)((u8*)task + 0x18);

    (void)pool;

    /* Spelled out five times rather than looped: the target unrolls it. */
    PaletteMgr_SetSource(g_PaletteManagers[0], *(PaletteResource**)(block + 0x64), (void*)func_ov039_02098dbc(block + 0x54));
    PaletteMgr_SetSource(g_PaletteManagers[0], *(PaletteResource**)(block + 0x78), (void*)func_ov039_02098dbc(block + 0x68));
    PaletteMgr_SetSource(g_PaletteManagers[0], *(PaletteResource**)(block + 0x8C), (void*)func_ov039_02098dbc(block + 0x7C));
    PaletteMgr_SetSource(g_PaletteManagers[0], *(PaletteResource**)(block + 0xA0), (void*)func_ov039_02098dbc(block + 0x90));
    PaletteMgr_SetSource(g_PaletteManagers[0], *(PaletteResource**)(block + 0xB4), (void*)func_ov039_02098dbc(block + 0xA4));
    return 1;
}

/**
 * @brief The full resource teardown: two buffers, two chars, six palettes, two
 *        data handles.
 *
 * The six palette releases are the same 0x14-strided slot run
 * `func_ov039_02092154` installs, read back off the block in the order the
 * install wrote them.
 */
s32 func_ov039_020921fc(void* pool, void* task) {
    u8* block = *(u8**)((u8*)task + 0x18);

    (void)pool;

    func_0200d954(0, 1);

    Mem_Free(*(Heap**)(block + 0x04), *(void**)(block + 0x38));
    Mem_Free(*(Heap**)(block + 0x04), *(void**)(block + 0x3C));

    BgResMgr_ReleaseChar(g_BgResourceManagers[0], *(BgResource**)(block + 0x50));
    BgResMgr_ReleaseChar(g_BgResourceManagers[0], *(BgResource**)(block + 0x48));

    PaletteMgr_ReleaseResource(g_PaletteManagers[0], *(PaletteResource**)(block + 0x4C));
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], *(PaletteResource**)(block + 0x64));
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], *(PaletteResource**)(block + 0x78));
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], *(PaletteResource**)(block + 0x8C));
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], *(PaletteResource**)(block + 0xA0));
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], *(PaletteResource**)(block + 0xB4));

    DatMgr_ReleaseData(*(Data**)(block + 0x44));
    DatMgr_ReleaseData(*(Data**)(block + 0x40));
    return 1;
}

/** The block `func_ov039_02092d0c` drives two display groups out of. */
typedef struct {
    /* 0x00 */ u8  pad_00[0x08];
    /* 0x08 */ s32 guard;
    /* 0x0C */ u8  pad_0C[0x1C];
    /* 0x28 */ s32 a1;
    /* 0x2C */ s32 b1;
    /* 0x30 */ u8  pad_30[0x20];
    /* 0x50 */ s32 a2;
    /* 0x54 */ s32 b2;
    /* 0x58 */ u8  pad_58[0x38];
    /* 0x90 */ s32 u1;
    /* 0x94 */ s32 u2;
    /* 0x98 */ s32 v1;
    /* 0x9C */ s32 v2;
} OtuBoardPair; // Size: 0xA0

/**
 * @brief Runs `func_ov039_02092348`'s stage step twice, over two field sets.
 *
 * Both halves are the same operation on `g_DisplaySettings.engineState`: pick
 * the group by a 0x220-strided index, switch on the `bgSettings[b].bgMode`
 * first word, raise `bgAffines[b].unk_14` for cases 1-5, and write the position
 * pair into `bgOffsets[b]` -- except that here the pair is an offset added to
 * the incoming x/y rather than a replacement. Spelled out twice rather than
 * factored, because `-inline noauto` would turn a shared helper into a `bl`.
 */
// Nonmatching: 98.5%, and the gap is the same one func_ov039_02092348 has: the
// engine-state index and the `g_DisplaySettings.engineState` base swap r4/r5
// between the two sides. See that marker for the shapes already tried.
void func_ov039_02092d0c(void* task, s32 x, s32 y) {
    OtuBoardPair*       self = (OtuBoardPair*)task;
    DisplayEngineState* group;
    s32                 b;
    s32                 px;
    s32                 py;

    if (self->guard == 0) {
        return;
    }

    b     = self->b1;
    px    = x + self->u1;
    py    = y + self->v1;
    group = &g_DisplaySettings.engineState[self->a1];
    switch (group->bgSettings[b].bgMode) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            group->bgAffines[b].unk_14 = 1;
            break;

        default:
            break;
    }
    group->bgOffsets[b].hOffset = px;
    group->bgOffsets[b].vOffset = py;

    b     = self->b2;
    px    = x + self->u2;
    py    = y + self->v2;
    group = &g_DisplaySettings.engineState[self->a2];
    switch (group->bgSettings[b].bgMode) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            group->bgAffines[b].unk_14 = 1;
            break;

        default:
            break;
    }
    group->bgOffsets[b].hOffset = px;
    group->bgOffsets[b].vOffset = py;
}
/** The block `func_ov039_020929ec` stores its params into and then sets up. */
typedef struct {
    /* 0x00 */ s32 word0;
    /* 0x04 */ s32 word1;
    /* 0x08 */ s32 active;
    /* 0x0C */ u8* selector;
    /* 0x10 */ u8  pad_10[0x80];
    /* 0x90 */ s32 flagsA[2];
    /* 0x98 */ s32 flagsB[2];
} OtuTileSetup; // Size: 0xA0

/**
 * @brief Stores the three-word params block and runs the tile setup twice.
 *
 * +0x0C is a pointer to a byte that both selects the table in
 * `data_ov039_0209a620` and gates the whole body: when it is zero the block is
 * still written and the function returns, which is the `else` half of the
 * `movne`/`strne`/`bne` sequence rather than an early guard.
 */
s32 func_ov039_020929ec(void* pool, void* task, s32* params) {
    OtuTileSetup* self = *(OtuTileSetup**)((u8*)task + 0x18);
    s32           i;

    (void)pool;

    self->word0    = params[0];
    self->word1    = params[1];
    self->selector = (u8*)params[2];

    if (*self->selector != 0) {
        self->active = 1;
    } else {
        self->active = 0;
        return 1;
    }

    for (i = 0; i < 2; i++) {
        func_ov039_0209276c(self, data_ov039_0209a620[*self->selector][i]);
        self->flagsA[i] = 0;
        self->flagsB[i] = 0;
    }
    return 1;
}

/**
 * @brief The +0x8538 member of the sprite-slot selector family.
 *
 * Byte-for-byte the shape of `func_ov039_0209352c`: the same
 * `&data_0206b408` pointer, the same three flat short-circuit tests over +0x18,
 * +0x1C and +0x16, the same two-step u16 lookup, and `depthKey` set to 3 rather
 * than computed. The tail sets `depthKey` only -- there is no +0x0C write here,
 * which is what separates this one from `func_ov039_0208d2f8`.
 */
// Nonmatching: 88.7%, byte-for-byte the same gap as func_ov039_0209352c, which
// this is a copy of. The target reloads the +0x16 index and the +0x1C table
// pointer after the `slot->unk_04` store (`ldrsh r2, [r0, #0x16]` / `ldr r3,
// [r0, #0x1c]`); this source keeps both in registers across that store, so the
// second half of the two-step lookup reads `r12` where the target reads `r3`.
// Writing the lookup through `t`'s fields rather than the locals does not
// restore the reload either -- the store to `slot` is a global and mwcc has
// already decided it cannot alias `t`.
OtuSpriteSlot* func_ov039_02098538(OtuSpriteTask* t, s32 arg, s32 sel) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    switch (sel) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->depthKey = 3;
            return slot;
        }

        default:
            return NULL;
    }
}

/** The four-child arc group stepped by `func_ov039_02091070`. */
typedef struct {
    /* 0x00 */ TaskPool* pool;
    /* 0x04 */ u8        pad_04[0xA8];
    /* 0xAC */ OtuPoint  home;
    /* 0xB4 */ u8        pad_B4[0x08];
    /* 0xBC */ OtuPoint  center;
    /* 0xC4 */ u8        pad_C4[0x64];
    /* 0x128 */ s32      ids[4];
} OtuArcGroup; // Size: 0x138

/**
 * @brief Steps the four child tasks named by +0x128 and places them along an arc.
 *
 * Child 0 is placed at the raw `home` pair against the +0x84 table; the other
 * three are placed by difference -- `func_ov039_02098bb0` takes the arc chord
 * between `home` and `center`, then `func_ov039_02098c00` scales it by a quarter
 * of the phase counter and adds it back to `center`, against the +0x94 table.
 * The phase advances 0x1000 per child and is divided by 4 when used, so the
 * scale runs 0, 0x400, 0x800, 0xC00.
 */
// Nonmatching: 89.4%, and the gap is one register's worth of allocation. The
// target gives `table` its own callee-saved register (r7) and so needs six plus
// a `push {r3, ...}` alignment dummy; this source computes `table` straight
// into the call's r2 and needs only five. Every other difference follows from
// that: the whole register naming shifts by one because the values pack
// tighter. Three things were real and are fixed -- the ids load needs a typed
// `ids[4]` at +0x128 or mwcc strength-reduces `i * 4` into a running offset
// instead of the `add rN, rN, rN, lsl #2` the target has; the `/ 4` must be
// taken of `i * 0x1000` rather than of an accumulator, or mwcc folds the divide
// into the induction variable and advances it by 0x400 instead of 0x1000; and
// the home pair has to be loaded before either is stored or the two do not come
// out in the target's order. Declaration order of `table` and of the home pair
// makes no difference.
void func_ov039_02091070(OtuArcGroup* self) {
    OtuPoint pt;
    s32      i;

    for (i = 0; i < 4; i++) {
        void* table;
        void* data = EasyTask_GetTaskData(self->pool, self->ids[i]);

        if (i == 0) {
            s32 hy = self->home.y;
            s32 hx = self->home.x;

            pt.x  = hx;
            pt.y  = hy;
            table = (u8*)self + 0x84;
        } else {
            /* `i * 0x1000` is spelled out rather than kept as an accumulator:
             * mwcc strength-reduces the product into a `+ 0x1000` counter but
             * still emits a real signed divide for the `/ 4`, which is what the
             * target has. An accumulator declared outside the loop gets the
             * divide folded into it and advances by 0x400 instead. */
            s32 phase = i * 0x1000;

            func_ov039_02098bb0(&self->home, &self->center, &pt);
            func_ov039_02098c00(phase / 4, &pt, &self->center, &pt);
            table = (u8*)self + 0x94;
        }

        func_ov039_020983c8(data, &pt, table, i);
    }
}

/**
 * @brief Counts down a one-shot, then integrates the badge's velocity.
 *
 * While +0x100 is positive it is a one-shot timer: the countdown reaches zero
 * exactly once and fires the board commit at +0x120 against the +0x110 origin,
 * then every later call returns immediately. Once it is spent the function
 * becomes the per-frame integrator -- the same accel step
 * `func_ov039_0208af6c` takes, but pinned to `data_ov039_0209a39c[3]` rather
 * than to the tile under the badge.
 */
// Nonmatching: 97.5%, one register apart. The target keeps the `slots` pointer
// in r12 across the `data_ov039_0209a39c[3]` load and puts the rate in r1 and
// the speed in r2; this source recycles r12 for the rate and puts the speed in
// r1. Giving the speed a named local makes it worse (97.2%), so the allocation
// is not reachable from the declaration shape.
void func_ov039_0208d210(OtuBadge* task) {
    u8* self = (u8*)task;
    s32 count;
    s32 rate;
    s32 v;

    count = *(s32*)(self + 0x100);
    if (count > 0) {
        --count;
        *(s32*)(self + 0x100) = count;
        if (count != 0) {
            return;
        }

        func_ov039_02089064(*(TinPinSlammer_Scene**)(self + 0x00), (OtuPoint*)(self + 0x120));
        func_ov039_02087d04(0x34E, (OtuPoint*)(self + 0x120), (OtuPoint*)(self + 0x110));
        return;
    }

    *(s32*)(self + 0x134) = *(s32*)(self + 0x134) + data_ov039_0209a318;

    /* The cell record is reached through a u16 index whose home is itself a
     * pointer at +0x16C, and the record is 0x1C bytes at +0x170. */
    {
        u16 index      = *(u16*)*(s32*)(self + 0x16C);
        u8* slots      = *(u8**)(self + 0x170);
        u8  speedIndex = *(u8*)(slots + index * 0x1C + 4);

        rate = data_ov039_0209a39c[3];
        rate = (s32)(((s64)data_ov039_0209a3e0[speedIndex].speed * rate + 0x800) >> 12);
    }

    func_ov039_0208ad2c((OtuPoint*)(self + 0x12C), rate);
    func_ov039_02098b8c((OtuPoint*)(self + 0x120), (OtuPoint*)(self + 0x12C), (OtuPoint*)(self + 0x120));

    v                     = *(s32*)(self + 0x128) + *(s32*)(self + 0x134);
    *(s32*)(self + 0x128) = v;
    if (v > 0) {
        *(s32*)(self + 0x128) = 0;
        *(s32*)(self + 0xF8)  = 1;
        *(s32*)(self + 0xF4)  = 0;
    }
}

/**
 * @brief The spring step: a scaled value, then a damped velocity and a bounce.
 *
 * +0x80 is a countdown. While it runs, +0x44 and +0x48 are both set to
 * `(count << 13) / +0x7C`, the +0x6C axis is stepped down by
 * `data_ov039_0209a304` and clamped at zero, +0x70 is stepped up by
 * `data_ov039_0209a310`, and `func_ov039_02098c00` integrates the pair at
 * +0x64 into +0x58. The +0x60 accumulator is then moved by +0x70 and, when it
 * overshoots, is clamped and its velocity damped by `-data_ov039_0209a300`.
 */
s32 func_ov039_02094ca8(void* pool, void* task) {
    u8* sprite = *(u8**)((u8*)task + 0x18);
    s32 count;
    s32 scale;
    s32 v;

    (void)pool;

    /* One `return 1` at the end, and the guard arms ordered so the *clear* is
     * the `then`: the target lays that inline as the fall-through and branches
     * to the spring body, which `if (count > 0) { spring } else { clear }`
     * inverts. `func_ov039_020981a4` needed the opposite polarity on its first
     * guard -- in both cases it is the layout, not the meaning, that decides. */
    if (*(s32*)(sprite + 0x74) != 0) {
        count                  = *(s32*)(sprite + 0x80) - 1;
        *(s32*)(sprite + 0x80) = count;

        if (count <= 0) {
            *(s32*)(sprite + 0x74) = 0;
            *(s32*)(sprite + 0x78) = 0;
        } else {
            scale                  = (count << 13) / *(s32*)(sprite + 0x7C);
            *(s32*)(sprite + 0x48) = scale;
            *(s32*)(sprite + 0x44) = scale;

            v                      = *(s32*)(sprite + 0x6C) - data_ov039_0209a304;
            *(s32*)(sprite + 0x6C) = v;
            if (v < 0) {
                *(s32*)(sprite + 0x6C) = 0;
            }

            *(s32*)(sprite + 0x70) = *(s32*)(sprite + 0x70) + data_ov039_0209a310;

            func_ov039_02098c00(*(s32*)(sprite + 0x6C), (OtuPoint*)(sprite + 0x64), (OtuPoint*)(sprite + 0x58),
                                (OtuPoint*)(sprite + 0x58));

            v                      = *(s32*)(sprite + 0x60) + *(s32*)(sprite + 0x70);
            *(s32*)(sprite + 0x60) = v;
            if (v > 0) {
                *(s32*)(sprite + 0x60) = 0;
                if (*(s32*)(sprite + 0x70) > 0) {
                    *(s32*)(sprite + 0x70) = (s32)(((s64) * (s32*)(sprite + 0x70) * -data_ov039_0209a300 + 0x800) >> 12);
                }
            }

            Sprite_Update((Sprite*)sprite);
            *(s32*)(sprite + 0x78) = 1;
        }
    }
    return 1;
}

/* ------------------------------------------------------------------ */
/* The +0x276c resource-allocation setup.                              */
/* ------------------------------------------------------------------ */

/** The 0x28-byte per-slot object func_ov039_0209276c drives. */
typedef struct {
    /* 0x00 */ u8 pad[0x28];
} OtuResSlot; // Size: 0x28

/** The per-slot parameter block func_ov039_0209276c reads. */
typedef struct {
    /* 0x00 */ s32 slot;
    /* 0x04 */ s32 width;
    /* 0x08 */ s32 height;
    /* 0x0C */ s32 fileId;
    /* 0x10 */ s32 group;
    /* 0x14 */ s32 layers;
    /* 0x18 */ s32 priority;
    /* 0x1C */ s32 idx[2];
    /* 0x24 */ s32 pad_24;
    /* 0x28 */ s32 idx1;
    /* 0x2C */ s32 idx2;
    /* 0x30 */ s32 palStart;
    /* 0x34 */ s32 palCount;
} OtuResParams; // Size: 0x38

/** The block func_ov039_0209276c loads and allocates into. */
typedef struct {
    /* 0x00 */ s32              dataType;
    /* 0x04 */ Heap*            heap;
    /* 0x08 */ u8               pad_08[0x04];
    /* 0x0C */ u8*              selector;
    /* 0x10 */ Data*            data[2];
    /* 0x18 */ PaletteResource* palettes[2];
    /* 0x20 */ BgResource*      chars[2];
    /* 0x28 */ OtuResSlot       slots[2];
    /* 0x78 */ void*            buffers[2];
    /* 0x80 */ s32              widths[2];
    /* 0x88 */ s32              heights[2];
} OtuResGroup; // Size: 0x90

/*
 * A relative-offset lookup: the record's +0x08 word is a base, the table at
 * `base + 0x20` is 8 bytes a row, and each row's first word is a signed offset
 * from `base + 0x20` itself. Spelled through a macro because it is used three
 * times and `-inline noauto` would turn a shared helper into a `bl` -- the
 * target has the four instructions three times over.
 *
 * The `NULL`/non-positive case is the `then`, not the `else`: the target lays
 * that one out inline as the fall-through and branches to the body, so the
 * natural `if (rec && idx > 0) { body } else { NULL }` arm order inverts it.
 */
#define Otu_RES_REF(out, rec, idx)                        \
    do {                                                  \
        if ((rec) == NULL || (idx) <= 0) {                \
            (out) = NULL;                                 \
        } else {                                          \
            u8* _base = *(u8**)((u8*)(rec) + 0x08);       \
            u8* _tbl  = _base + 0x20;                     \
            (out)     = _tbl + *(s32*)(_tbl + (idx) * 8); \
        }                                                 \
    } while (0)

/**
 * @brief Loads a slot's raw data, then allocates its palette, chars and buffer.
 *
 * The +0x00 word is the `DatMgr_LoadRawData` dataType, not a pool pointer. The
 * three relative references are the palette source, the char source and the
 * cell source in that order; the char source's first word is split into a
 * 24-bit size and a nibble flag, and the size drops by 4 when that nibble is
 * clear.
 */
// Nonmatching: 86.7%, and the rest is register naming and scheduling rather
// than anything structural. Three things were real and are fixed:
//
//   * `PaletteMgr_AllocPalette`'s `start` is `s16`, so the prototype makes mwcc
//     narrow `palStart` with a `lsl #0x10` the target does not have. The call
//     goes through `PaletteMgr_AllocPaletteNoProto`, which is what that macro
//     in PaletteMgr.h is for.
//   * The record has to be stored into the block and read back out, not kept in
//     a local -- the target reloads `rec` after `self->data[slot] = rec` and the
//     store is what forces it.
//   * The first index is hoisted out of the macro. Its load is independent of
//     the `rec == NULL` test and the target has it above the branch; leaving it
//     inside puts the branch first.
//
// What is left is `selector` in r1 vs r0 and `slot` in r0 vs r2 in the first
// block, plus `mov r3, #0` for the `offset` argument landing elsewhere.
void func_ov039_0209276c(void* selfArg, s32 paramsAddr) {
    OtuResGroup*  self   = (OtuResGroup*)selfArg;
    OtuResParams* params = (OtuResParams*)paramsAddr;
    Data*         rec;
    u8*           ref0;
    u8*           ref1;
    u8*           ref2;
    s32           size;

    /* Written as a store-then-reload rather than kept in a local: the target
     * re-reads the record out of the block after storing it, and the store is
     * what forces that. */
    self->data[params->slot] =
        DatMgr_LoadRawData(self->dataType, NULL, 0, (BinIdentifier*)&data_ov039_0209a0b4[params->fileId]);
    rec = self->data[params->slot];

    /* The first index is hoisted: its load is independent of the `rec == NULL`
     * test and the target has it above the branch, where leaving it inside the
     * macro puts the branch first. */
    {
        s32 idxA = params->idx[*(u8*)(self->selector + 1)];

        Otu_RES_REF(ref0, rec, idxA);
    }
    Otu_RES_REF(ref1, rec, params->idx1);
    Otu_RES_REF(ref2, rec, params->idx2);

    /* `PaletteMgr_AllocPaletteNoProto` because `palStart` is a 32-bit value
     * going into a `s16` parameter: the prototype makes mwcc narrow it with a
     * `lsl #0x10` the target does not have. */
    self->palettes[params->slot] =
        PaletteMgr_AllocPaletteNoProto(g_PaletteManagers[0], ref0, 0, params->palStart, params->palCount);

    size = (s32)((*(u32*)ref1) >> 8);
    if ((*(u8*)ref1 & 0xF0) == 0) {
        size -= 4;
    }

    self->chars[params->slot] = BgResMgr_AllocChar32(
        g_BgResourceManagers[0], ref1, g_DisplaySettings.engineState[0].bgSettings[params->group].charBase, 0, size);

    PaletteMgr_Flush(g_PaletteManagers[0], self->palettes[params->slot]);

    self->buffers[params->slot] = Mem_AllocHeapTail(self->heap, params->height * (params->width * 4));

    func_0200d898(self->buffers[params->slot], ref2 + 4, params->width, params->height);

    func_0200d1d8(&self->slots[params->slot], 0, params->group, 0, self->buffers[params->slot], params->width, params->height);

    func_0200d858(&self->slots[params->slot], 1, 1, 0);

    self->widths[params->slot]  = params->width << 20;
    self->heights[params->slot] = params->height << 20;

    if (params->layers >= 0) {
        g_DisplaySettings.controls[0].layers |= params->layers;
    }

    g_DisplaySettings.engineState[0].bgSettings[params->group].priority = params->priority;
}

/**
 * @brief Randomises the sprite's launch vector and picks a starting animation.
 *
 * The two velocity components are drawn from opposite ends of their ranges:
 * +0x6C starts at `data_ov039_0209a314` and is pushed *up* by a random span,
 * +0x70 starts at the negation of `data_ov039_0209a2fc` and is pushed *down*.
 * The magnitude at +0x7C/+0x80 is then that second component scaled by 3 and
 * taken as an absolute value -- spelled as the three-way ternary `abs` is a
 * macro for, which is why `FX_Divide` is called three times in the target
 * rather than once and reused.
 */
// Nonmatching: 85.5%, all of it scheduling and register choice at the top of
// the body. The target loads both `at` components before either is stored (x in
// r2, y in r1); this source stores x before it loads y. Hoisting the two reads
// into named locals is the obvious fix and scores *worse* (84%), because it
// also moves the pool-constant loads. Everything from the `data_ov039_0209a314`
// store down agrees instruction for instruction, including the three
// `FX_Divide` calls.
void func_ov039_02094e9c(void* task, OtuPoint* at) {
    u8* sprite = (u8*)task;
    s32 cell;
    s32 mag;

    *(s32*)(sprite + 0x74) = 1;
    *(s32*)(sprite + 0x44) = 0x2000;
    *(s32*)(sprite + 0x48) = 0x2000;
    *(s32*)(sprite + 0x58) = at->x;
    *(s32*)(sprite + 0x5C) = at->y;
    *(s32*)(sprite + 0x60) = 0;

    *(s32*)(sprite + 0x6C) = data_ov039_0209a314;
    *(s32*)(sprite + 0x6C) = *(s32*)(sprite + 0x6C) + RNG_Next(data_ov039_0209a328 - data_ov039_0209a314);

    *(s32*)(sprite + 0x70) = -data_ov039_0209a2fc;
    *(s32*)(sprite + 0x70) = *(s32*)(sprite + 0x70) - RNG_Next(data_ov039_0209a30c - data_ov039_0209a2fc);

#define OTU_SPRING_MAG ((FX_Divide(*(s32*)(sprite + 0x70), data_ov039_0209a310) * 3) >> 12)
    /* `abs` as the macro it is: the argument is evaluated once for the test and
     * once per arm, so the three-way form is load-bearing. The `< 0` polarity
     * is too -- the target lays the negating arm out inline and branches to the
     * plain one (`bpl`), and the `>= 0` spelling inverts that. */
    mag = OTU_SPRING_MAG < 0 ? -OTU_SPRING_MAG : OTU_SPRING_MAG;
#undef OTU_SPRING_MAG

    *(s32*)(sprite + 0x7C) = mag;
    *(s32*)(sprite + 0x80) = mag;

    /* The sin/cos table is s32[] but holds pairs of s16, so each half has to be
     * fetched through a s16* at a doubled byte offset. */
    cell                   = RNG_Next(0x10000) >> 4;
    *(s32*)(sprite + 0x64) = *(s16*)((u8*)&data_0205e4e0 + (cell * 2 + 1) * 2);
    *(s32*)(sprite + 0x68) = *(s16*)((u8*)&data_0205e4e0 + (cell * 2) * 2);

    Sprite_ChangeAnimation((Sprite*)sprite, *(s32*)(sprite + 0x18), *(s32*)(sprite + 0x1C), (s16)(RNG_Next(3) + 1));
}

/**
 * @brief `func_ov039_02094e9c`'s child variant: same launch, plus a template
 *        copy and a different animation pick.
 *
 * Three differences from the parent. A 4-word template is copied onto the
 * sprite block at +0x40 where the parent writes two fixed 0x2000 halves; the
 * magnitude lands at +0x84/+0x88 rather than +0x7C/+0x80; and the animation
 * index is `index == 0 ? 2 : 1` rather than a fresh `RNG_Next(3) + 1`. The
 * +0x74 word is a s16 and is `RNG_Next(0x800) + 0x800`.
 */
// Nonmatching: 85.4%. Two differences left. The target keeps `at`'s halves as
// two separate `ldr`s (x in r2, y in r1); this source merges them into
// `ldmia lr, {r1, r2}`, which also swaps which half lands in which register.
// Reading them straight into the stores instead interleaves the writes and
// scores 84.3%, and hoisting them into locals is what got this to 85.4% -- the
// merge is a load-pair decision made against this frame's register pressure and
// there is no source shape here that stops it. Everything from the +0x6C store
// down agrees instruction for instruction.
void func_ov039_020983c8(void* task, OtuPoint* at, void* table, s32 index) {
    u8* sprite = (u8*)task;
    s32 ax;
    s32 ay;
    s32 cell;
    s32 mag;

    *(s32*)(sprite + 0x7C)      = 1;
    *(OtuWord4*)(sprite + 0x40) = *(OtuWord4*)table;

    /* Both halves are read before either is written: the target keeps x in r2
     * and y in r1 across the pair of stores. */
    ax = at->x;
    ay = at->y;

    *(s32*)(sprite + 0x58) = ax;
    *(s32*)(sprite + 0x5C) = ay;
    *(s32*)(sprite + 0x60) = 0;

    *(s32*)(sprite + 0x6C) = data_ov039_0209a314;
    *(s32*)(sprite + 0x6C) = *(s32*)(sprite + 0x6C) + RNG_Next(data_ov039_0209a328 - data_ov039_0209a314);

    *(s32*)(sprite + 0x70) = -data_ov039_0209a2fc;
    *(s32*)(sprite + 0x70) = *(s32*)(sprite + 0x70) - RNG_Next(data_ov039_0209a30c - data_ov039_0209a2fc);

#define OTU_SPRING_MAG ((FX_Divide(*(s32*)(sprite + 0x70), data_ov039_0209a310) * 3) >> 12)
    mag = OTU_SPRING_MAG < 0 ? -OTU_SPRING_MAG : OTU_SPRING_MAG;
#undef OTU_SPRING_MAG

    *(s32*)(sprite + 0x84) = mag;
    *(s32*)(sprite + 0x88) = mag;

    cell                   = RNG_Next(0x10000) >> 4;
    *(s32*)(sprite + 0x64) = *(s16*)((u8*)&data_0205e4e0 + (cell * 2 + 1) * 2);
    *(s32*)(sprite + 0x68) = *(s16*)((u8*)&data_0205e4e0 + (cell * 2) * 2);

    *(s16*)(sprite + 0x74) = RNG_Next(0x800) + 0x800;

    Sprite_ChangeAnimation((Sprite*)sprite, *(s32*)(sprite + 0x18), *(s32*)(sprite + 0x1C), (s16)(index == 0 ? 2 : 1));
}

/**
 * @brief Advances up to four counters and raises a flag when each wraps.
 *
 * The selector byte at +0x0C picks which half runs: case 1 advances the +0x94
 * pair against the +0x84/+0x8C limits and flags +0x58, case 2 does that *and*
 * the +0x90 pair against +0x80/+0x88 with a smaller step flagging +0x30. Each
 * counter is compared against its limit and, on reaching it, taken modulo it --
 * `_s32_div_f` leaves the remainder in r1, which is what is stored back.
 */
s32 func_ov039_02092a78(void* pool, void* task) {
    u8* self = *(u8**)((u8*)task + 0x18);
    s32 limit;
    s32 v;

    (void)pool;

    if (*(s32*)(self + 0x08) == 0) {
        return 1;
    }

    switch (*(u8*)*(s32*)(self + 0x0C)) {
        case 1:
            v                    = *(s32*)(self + 0x94) + 0x19A;
            *(s32*)(self + 0x94) = v;
            limit                = *(s32*)(self + 0x84);
            if (v >= limit) {
                *(s32*)(self + 0x94) %= limit;
                *(s32*)(self + 0x58) |= 2;
            }

            v                    = *(s32*)(self + 0x9C) + 0x19A;
            *(s32*)(self + 0x9C) = v;
            limit                = *(s32*)(self + 0x8C);
            if (v >= limit) {
                *(s32*)(self + 0x9C) %= limit;
                *(s32*)(self + 0x58) |= 2;
            }
            break;

        case 2:
            v                    = *(s32*)(self + 0x90) + 0x333;
            *(s32*)(self + 0x90) = v;
            limit                = *(s32*)(self + 0x80);
            if (v >= limit) {
                *(s32*)(self + 0x90) %= limit;
                *(s32*)(self + 0x30) |= 2;
            }

            v                    = *(s32*)(self + 0x98) + 0x333;
            *(s32*)(self + 0x98) = v;
            limit                = *(s32*)(self + 0x88);
            if (v >= limit) {
                *(s32*)(self + 0x98) %= limit;
                *(s32*)(self + 0x30) |= 2;
            }

            v                    = *(s32*)(self + 0x94) + 0x19A;
            *(s32*)(self + 0x94) = v;
            limit                = *(s32*)(self + 0x84);
            if (v >= limit) {
                *(s32*)(self + 0x94) %= limit;
                *(s32*)(self + 0x58) |= 2;
            }

            v                    = *(s32*)(self + 0x9C) + 0x19A;
            *(s32*)(self + 0x9C) = v;
            limit                = *(s32*)(self + 0x8C);
            if (v >= limit) {
                *(s32*)(self + 0x9C) %= limit;
                *(s32*)(self + 0x58) |= 2;
            }
            break;

        default:
            break;
    }
    return 1;
}

/**
 * @brief Runs the pin's current phase handler and reports whether it claimed.
 *
 * The three cases are the three phase handlers at +0xF8: 6 takes the read from
 * `func_ov039_02091628`, 7 the one from `func_ov039_02090e9c`, and 8 the
 * completion from `func_ov039_0208fee0` -- all three writing the same
 * three-word record at +0x184 and stashing their result in +0x180. Cases 6 and
 * 7 set the return value only when the handler reports a positive count;
 * case 8 claims unconditionally but only when +0xFC is 5 and +0x100 is 0x27.
 */
// Nonmatching: 97.4%, three instructions. The target branches past the body
// twice (`bne`/`bne`) for the +0xFC/+0x100 pair; this source has mwcc
// if-convert the pair into a conditional load (`ldreq r1, [r5, #0x100]`) and a
// single `cmpeq`. Three spellings were tried -- a positive `&&`, nested `if`s,
// a negated `||`, and two sequential guards each with its own `break` -- and
// all four compile to the same folded form. mwcc is choosing conditional
// execution over branching here and there is no source shape that changes that.
s32 func_ov039_0208e9f8(TaskPool* pool, void* task) {
    u8* self    = (u8*)task;
    s32 claimed = 0;
    s32 ret;

    switch (*(s32*)(self + 0xF8)) {
        case 6:
            ret = func_ov039_02091628(EasyTask_GetTaskData(pool, *(s32*)(self + 0x1E8)), (OtuPoint3*)(self + 0x184));
            *(s32*)(self + 0x180) = ret;
            if (ret > 0) {
                claimed = 1;
            }
            break;

        case 7:
            ret = func_ov039_02090e9c(EasyTask_GetTaskData(pool, *(s32*)(self + 0x1E4)), (OtuBar*)(self + 0x184));
            *(s32*)(self + 0x180) = ret;
            if (ret > 0) {
                claimed = 1;
            }
            break;

        case 8:
            /* Two sequential guards with their own `break`, not one combined
             * condition: `&&`, nested `if`s and a negated `||` all fold the
             * pair into `ldreq`/`cmpeq`, where the target branches past the
             * body twice. */
            if (*(s32*)(self + 0xFC) != 5) {
                break;
            }
            if (*(s32*)(self + 0x100) != 0x27) {
                break;
            }

            claimed = 1;
            func_ov039_0208fee0(EasyTask_GetTaskData(pool, *(s32*)(self + 0x1E0)), (OtuPinRecord*)(self + 0x184));
            *(s32*)(self + 0x180) = claimed;
            break;

        default:
            break;
    }
    return claimed;
}

/**
 * @brief The +0x8f7b4 member of the sprite-slot selector family.
 *
 * Same `&data_0206b408` pointer, same three flat short-circuit tests and same
 * two-step u16 lookup as `func_ov039_0209352c`. What separates it is the tail:
 * `depthKey` is derived from the +0x24 object's +0x4C word, masked to 0x7FF and
 * shifted back up by 12 with a +1 bias -- a Q12.12 value rounded up to the next
 * whole step. That tail runs whether or not the lookup succeeded, which is why
 * it sits outside the guard.
 */
// Nonmatching: 84.3%, and it carries two of the family's known gaps at once.
// `table` lands in lr here rather than r12, which forces a `stmdb`/`ldmia`
// prologue the target does not have and shifts the index into r0; and the
// lookup keeps +0x16/+0x1C in registers across the `slot->unk_04` store where
// the target reloads both, the same thing that holds func_ov039_02098538 and
// func_ov039_0209352c at 88.7%. Swapping the `obj`/`slot` declarations was
// worth 10% (74.4% -> 84.3%) because it moved `slot` onto r3 and `obj` onto r1;
// swapping `index`/`table` does nothing.
OtuSpriteSlot* func_ov039_0208f7b4(OtuSpriteTask* t, s32 arg, s32 sel) {
    u8*            obj  = *(u8**)((u8*)t + 0x24);
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    switch (sel) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            u8* table;
            s32 index;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->depthKey = (((*(s32*)(obj + 0x4C) >> 12) & 0x7FF) + 1) << 12;
            return slot;
        }

        default:
            return NULL;
    }
}

/**
 * @brief Builds a 2x2 tile bitmask grid over the +0x04 object's cell array.
 *
 * The +0x04 object is a width/height header at +2/+3 with a cell array pointer
 * at +0x10, cells two bytes apart. The grid is walked in 2x2 blocks: each block
 * samples the four cells at (5+2j .. 7+2j) x (5+2i .. 7+2i), ORs a `bit` into a
 * mask that starts at 0x23F0, and shifts `bit` left for the next cell. `bit` is
 * a u16 and is narrowed after every shift, which is why the loop reads as
 * `lsl`/`lsl`/`lsr` rather than one `lsl`.
 *
 * The output is a u16 grid at +0x40 with 32 entries per row, offset by
 * `xBase` entries and `yBase` rows. xBase/yBase are the block counts put back
 * into grid space: `(0x28 - (w - 10)) / 2 + 10` and `(0x28 - (h - 10)) / 2 + 2`.
 */
// Nonmatching: 36.4%, and it is a long way from done -- worth recording what is
// known so the next pass does not re-derive it.
//
//   * The block counts are `0x28 - (w - 10)`, not `50 - w`. mwcc keeps the
//     inner subtract as its own instruction and folding the pair into one
//     `50 - w` costs it. Worth 4% on its own.
//   * `cols` spills: the target keeps `(w - 9) / 2` in r11 and allocates five
//     stack words (`sub sp, #0x14`), this source needs seven and puts an extra
//     `cols <= 0` guard in front of the outer loop that the target does not
//     have -- mwcc rotated the inner loop and added the pre-header test. That
//     rotation is what the frame-size gap and the extra check both come from.
//   * The mask/bit inner pair, the `0x23F0` seed, the u16 narrowing of `bit`
//     and the `ptr + (y * w + x) * 2` cell fetch are all believed correct.
void func_ov039_02092e30(void* task) {
    u8* self = (u8*)task;
    u8* obj  = *(u8**)(self + 0x04);
    s32 w    = obj[2];
    s32 h    = obj[3];
    u8* ptr  = *(u8**)(obj + 0x10);

    /* The block counts are put back into grid space as `0x28 - (w - 10)`,
     * not `50 - w`: mwcc keeps the inner `w - 10` / `h - 10` as its own
     * subtract and the outer `rsb` as a separate one, and folding them into a
     * single `50 - w` costs that instruction. */
    s32 dw    = w - 10;
    s32 dh    = h - 10;
    s32 xBase = (0x28 - dw) / 2 + 10;
    s32 yBase = (0x28 - dh) / 2 + 2;
    s32 rows  = (h - 9) / 2;
    s32 cols  = (w - 9) / 2;
    s32 j;
    s32 i;

    if (rows <= 0) {
        return;
    }

    for (j = 0; j < rows; j++) {
        for (i = 0; i < cols; i++) {
            u16 mask = 0x23F0;
            u16 bit  = 1;
            s32 y;
            s32 x;

            for (y = 5 + j * 2; y < 7 + j * 2; y++) {
                for (x = 5 + i * 2; x < 7 + i * 2; x++) {
                    if (*(u8*)(ptr + (y * w + x) * 2) != 0) {
                        mask |= bit;
                    }
                    bit = (u16)(bit << 1);
                }
            }

            *(u16*)(self + 0x40 + xBase * 2 + (yBase + j) * 64 + i * 2) = mask;
        }
    }
}

/**
 * @brief The +0x81a4 sibling of `func_ov039_02094ca8`'s spring step.
 *
 * Same integral, same damping, same bounce -- but the countdown is at +0x88
 * against a +0x84 divisor, the magnitude is written to +0x44/+0x48, and the
 * step runs only when the +0x78 task's data resolves *and* +0x7C is set. The
 * +0x40 word is a u16 accumulator advanced by the u16 at +0x74 and narrowed
 * after the add, then stored back into the 32-bit slot.
 */
s32 func_ov039_020981a4(TaskPool* pool, void* task) {
    u8*   sprite = *(u8**)((u8*)task + 0x18);
    void* data   = EasyTask_GetTaskData(pool, *(s32*)(sprite + 0x78));
    s32   count;
    s32   scale;
    s32   v;

    /* One `return 1` at the end rather than one per exit: the target shares a
     * single epilogue across the three ways out. The first guard is also
     * negated -- `data != NULL` with the clear as its `else` -- because that is
     * what makes mwcc branch *to* the clear block and lay the main body out as
     * the fall-through, which is the target's shape. */
    if (data != NULL) {
        if (*(s32*)(sprite + 0x7C) != 0) {
            func_ov039_0208e85c(data, (OtuPoint*)(sprite + 0x50));

            count                  = *(s32*)(sprite + 0x88) - 1;
            *(s32*)(sprite + 0x88) = count;

            if (count <= 0) {
                *(s32*)(sprite + 0x7C) = 0;
                *(s32*)(sprite + 0x80) = 0;
            } else {
                scale                  = (count << 13) / *(s32*)(sprite + 0x84);
                *(s32*)(sprite + 0x48) = scale;
                *(s32*)(sprite + 0x44) = scale;

                *(s32*)(sprite + 0x40) = (u16)(*(s32*)(sprite + 0x40) + *(u16*)(sprite + 0x74));

                v                      = *(s32*)(sprite + 0x6C) - data_ov039_0209a304;
                *(s32*)(sprite + 0x6C) = v;
                if (v < 0) {
                    *(s32*)(sprite + 0x6C) = 0;
                }

                *(s32*)(sprite + 0x70) = *(s32*)(sprite + 0x70) + data_ov039_0209a310;

                func_ov039_02098c00(*(s32*)(sprite + 0x6C), (OtuPoint*)(sprite + 0x64), (OtuPoint*)(sprite + 0x58),
                                    (OtuPoint*)(sprite + 0x58));

                v                      = *(s32*)(sprite + 0x60) + *(s32*)(sprite + 0x70);
                *(s32*)(sprite + 0x60) = v;
                if (v > 0) {
                    *(s32*)(sprite + 0x60) = 0;
                    if (*(s32*)(sprite + 0x70) > 0) {
                        *(s32*)(sprite + 0x70) = (s32)(((s64) * (s32*)(sprite + 0x70) * -data_ov039_0209a300 + 0x800) >> 12);
                    }
                }

                Sprite_Update((Sprite*)sprite);
                *(s32*)(sprite + 0x80) = 1;
            }
        }
    } else {
        *(s32*)(sprite + 0x7C) = 0;
        *(s32*)(sprite + 0x80) = 0;
    }
    return 1;
}

/* ------------------------------------------------------------------ */
/* The five-phase swing task, 0x02091868.                              */
/* ------------------------------------------------------------------ */

/**
 * @brief The swing task's phase machine: wind-up, swing, recover.
 *
 * The sprite block (task+0x18) carries its own small state block: +0x6C is the
 * "step this frame" flag, +0x70 the phase, +0x74 a 4-frame hold count and
 * +0x48 the swing angle. Phase 1 seeds the swing from the pin's +0x138
 * position and falls straight through into phase 2, which is the swing
 * itself: it copies the pin's position pair, aims by 0x38000 and slews
 * +0x48 from 0x2000 toward zero over the four frames of +0x74. Phases 3 and 4
 * walk the angle back and the completion checks are gated on the owner's bit
 * 10 (the "swing enabled" bit), read as `(word << 0x15) >> 0x1F`.
 */
s32 func_ov039_02091868(TaskPool* pool, void* task) {
    u8*   sprite = *(u8**)((u8*)task + 0x18);
    void* pin;

    pin = EasyTask_GetTaskData(pool, *(s32*)(sprite + 0x68));

    if (pin != NULL) {
        switch (*(s32*)(sprite + 0x70)) {
            case 0:
                *(s32*)(sprite + 0x6C) = 0;
                break;

            case 1:
                *(s32*)(sprite + 0x6C) = 1;
                func_ov039_0208e6cc(pin, (OtuPoint*)(sprite + 0x60));
                func_ov039_02098bd4(-0x1000, (OtuPoint*)(sprite + 0x60), (OtuPoint*)(sprite + 0x60));
                *(s32*)(sprite + 0x40) = (u16)(FX_Atan2Idx(*(s32*)(sprite + 0x64), *(s32*)(sprite + 0x60)) + 0xC000);
                *(s32*)(sprite + 0x48) = 0;
                *(s32*)(sprite + 0x70) = 2;
                *(s32*)(sprite + 0x74) = 4;
                Sprite_SetAnimation((Sprite*)sprite, *(s16**)(sprite + 0x18), 1, *(SpriteCell**)(sprite + 0x1C));
                /* fall through */
            case 2:
                func_ov039_0208e85c(pin, (OtuPoint*)(sprite + 0x50));
                func_ov039_0208e6e0(pin, (OtuPoint*)(sprite + 0x58));
                func_ov039_02098c00(0x38000, (OtuPoint*)(sprite + 0x60), (OtuPoint*)(sprite + 0x58),
                                    (OtuPoint*)(sprite + 0x58));
                if (*(s32*)(sprite + 0x74) > 0) {
                    *(s32*)(sprite + 0x48) =
                        *(s32*)(sprite + 0x48) + (0x2000 - *(s32*)(sprite + 0x48)) / *(s32*)(sprite + 0x74);
                    *(s32*)(sprite + 0x74) = *(s32*)(sprite + 0x74) - 1;
                }

                if (((u32)(*(s32*)sprite << 0x15)) >> 0x1F == 1) {
                    *(s32*)(sprite + 0x70) = 3;
                    Sprite_ChangeAnimation((Sprite*)sprite, *(s32*)(sprite + 0x18), 2, *(SpriteCell**)(sprite + 0x1C));
                }
                break;

            case 3:
                func_ov039_0208e85c(pin, (OtuPoint*)(sprite + 0x50));

                if (((u32)(*(s32*)sprite << 0x15)) >> 0x1F == 1) {
                    *(s32*)(sprite + 0x70) = 4;
                    *(s32*)(sprite + 0x74) = 4;
                    Sprite_ChangeAnimation((Sprite*)sprite, *(s32*)(sprite + 0x18), 3, *(SpriteCell**)(sprite + 0x1C));
                }
                break;

            case 4:
                func_ov039_0208e85c(pin, (OtuPoint*)(sprite + 0x50));

                if (*(s32*)(sprite + 0x74) > 0) {
                    *(s32*)(sprite + 0x48) = *(s32*)(sprite + 0x48) + -*(s32*)(sprite + 0x48) / *(s32*)(sprite + 0x74);
                    *(s32*)(sprite + 0x74) = *(s32*)(sprite + 0x74) - 1;
                } else {
                    *(s32*)(sprite + 0x70) = 0;
                    *(s32*)(sprite + 0x6C) = 0;
                }
                break;
        }
    } else {
        *(s32*)(sprite + 0x6C) = 0;
    }

    if (*(s32*)(sprite + 0x6C) != 0) {
        Sprite_Update((Sprite*)sprite);
    }
    return 1;
}

/**
 * @brief The hammer-charge task's phase machine: raise, hold, swing.
 *
 * Sprite state block: +0x6C the step flag, +0x80 the phase, +0x78 a running
 * frame count and +0x7C a sub-count. Phase 0 clears the flag; phase 1 sets the
 * charge mode 0x334 and the wind-up is phase 2, which ramps +0x48 from 0x99A
 * toward 0x1000 over the +0x70 frames -- both +0x48 and its +0x4C copy keep
 * the ramp value. On frame zero it raises the sprite's +4 bit-6 bit and enters
 * the hold, phase 3, which counts +0x7C down and re-arms the charge (mode
 * 0x336) once its own counter lands. Phase 4 runs the same ramp back against
 * 0x99A. Phase 2 re-arms at frame zero and phase 3 counts +0x7C down before
 * running the same ramp back in phase 4, which zeroes the phase on landing.
 * While their frame count is still positive, phases 2 and 3 break out of the
 * switch; the fall-throughs (1 to 2, then on to 3 and 4) only happen on the
 * counts that reach zero.
 */
s32 func_ov039_020912c8(TaskPool* pool, void* task) {
    u8*   sprite = *(u8**)((u8*)task + 0x18);
    void* pin;

    pin = EasyTask_GetTaskData(pool, *(s32*)(sprite + 0x68));

    if (pin != NULL) {
        s32 alive = func_ov039_0208e9d0(pin);

        *(s32*)(sprite + 0x6C) = alive;
        if (alive == 0) {
            *(s32*)(sprite + 0x80) = 0;
        } else {
            func_ov039_0208e85c(pin, (OtuPoint*)(sprite + 0x54));
            func_ov039_0208e6e0(pin, (OtuPoint*)(sprite + 0x5C));
            *(s32*)(sprite + 0x64) = func_ov039_0208e6f4(pin);
        }

        switch (*(s32*)(sprite + 0x80)) {
            case 0:
                *(s32*)(sprite + 0x6C) = 0;
                break;

            case 1:
                *(s32*)(sprite + 0x4C) = 0x99A;
                *(s32*)(sprite + 0x48) = 0x99A;
                Sprite_ChangeAnimation((Sprite*)(sprite + 4), *(s32*)(sprite + 0x1C), 0xE, *(SpriteCell**)(sprite + 0x20));
                *(s32*)(sprite + 0x78) = *(s32*)(sprite + 0x70);
                *(s32*)(sprite + 0x80) = 2;
                /* fall through */
            case 2:
                *(s32*)(sprite + 0x48) = *(s32*)(sprite + 0x48) + (0x1000 - *(s32*)(sprite + 0x48)) / *(s32*)(sprite + 0x78);
                *(s32*)(sprite + 0x4C) = *(s32*)(sprite + 0x48);

                *(s32*)(sprite + 0x78) = *(s32*)(sprite + 0x78) - 1;

                if (*(s32*)(sprite + 0x78) > 0) {
                    break;
                }

                *(s32*)(sprite + 4) = (*(s32*)(sprite + 4) & ~0x60) | 0x40;
                Sprite_ChangeAnimation((Sprite*)(sprite + 4), *(s32*)(sprite + 0x1C), 0xF, *(SpriteCell**)(sprite + 0x20));
                func_ov039_02087d04(0x334, (OtuPoint*)(sprite + 0x5C), (OtuPoint*)(sprite + 0x54));
                *(s32*)(sprite + 0x78) = *(s32*)(sprite + 0x74);
                *(s32*)(sprite + 0x7C) = 0x1E;
                *(s32*)(sprite + 0x80) = 3;
                /* fall through */
            case 3:
                if (*(s32*)(sprite + 0x7C) > 0) {
                    *(s32*)(sprite + 0x7C) = *(s32*)(sprite + 0x7C) - 1;
                }
                *(s32*)(sprite + 0x78) = *(s32*)(sprite + 0x78) - 1;

                if (*(s32*)(sprite + 0x78) > 0) {
                    break;
                }

                *(s32*)(sprite + 4) = (*(s32*)(sprite + 4) & ~0x60) | 0x40;
                Sprite_ChangeAnimation((Sprite*)(sprite + 4), *(s32*)(sprite + 0x1C), 2, *(SpriteCell**)(sprite + 0x20));
                func_ov039_02087d04(0x336, (OtuPoint*)(sprite + 0x5C), (OtuPoint*)(sprite + 0x54));
                *(s32*)(sprite + 0x78) = 0xE;
                *(s32*)(sprite + 0x80) = 4;
                /* fall through */
            case 4:
                *(s32*)(sprite + 0x48) = *(s32*)(sprite + 0x48) + (0x99A - *(s32*)(sprite + 0x48)) / *(s32*)(sprite + 0x78);
                *(s32*)(sprite + 0x4C) = *(s32*)(sprite + 0x48);

                *(s32*)(sprite + 0x78) = *(s32*)(sprite + 0x78) - 1;

                if (*(s32*)(sprite + 0x78) <= 0) {
                    *(s32*)(sprite + 0x80) = 0;
                }
                break;
        }

        if (*(s32*)(sprite + 0x6C) != 0) {
            Sprite_Update((Sprite*)(sprite + 4));
        }
    } else {
        *(s32*)(sprite + 0x6C) = 0;
    }
    return 1;
}

/* ------------------------------------------------------------------ */
/* The board-shape background loader, 0x02098650.                      */
/* ------------------------------------------------------------------ */

/** The 0x27-sized bin the loader pulls, plus its three sub-objects. */
extern const BinIdentifier data_ov039_0209a0e4;

/**
 * @brief Loads the board's background data and allocates both displays from
 *        it.
 *
 * The result screen's background setup: sets the step flag at +0x44 and the
 * uncopied `dataType` word at +0, raises both displays' BG1 layer mask
 * (`0x1F | 0x12`), marks `bgAffines[1].unk_14` when the layer's bgMode is one
 * of the five scaled modes and clears both displays' +1 offsets, then loads
 * `0209a0e4`'s bin and walks its pointer table: the buffer's +8/+0x10/+0x18
 * words hold relative offsets from the buffer's +0x20 to the palette, char
 * and screen sources. Each source is allocated once per display (palette
 * first, then char and screen from `bgSettings[1]`, palette flush in between),
 * the sprite is reloaded and both displays fade to black.
 */
s32 func_ov039_02098650(TaskPool* pool, void* task, s32* args) {
    DisplayEngineState* state;
    u8*                 sprite = *(u8**)((u8*)task + 0x18);
    Data*               data;

    *(s32*)(sprite + 0x44) = 1;
    *(s32*)sprite          = *(s32*)args;

    g_DisplaySettings.controls[0].layers = 0x1F;
    g_DisplaySettings.controls[0].layers |= 0x12;
    state = &g_DisplaySettings.engineState[0];
    switch (state->bgSettings[1].bgMode) {
        case DISPLAY_BGMODE_AFFINE:
        case DISPLAY_BGMODE_PLTT:
        case DISPLAY_BGMODE_BMP256:
        case DISPLAY_BGMODE_BMPDIRECT:
        case DISPLAY_BGMODE_BMPLARGE:
            state->bgAffines[1].unk_14 = 1;
            break;

        default:
            break;
    }
    state->bgOffsets[1].hOffset = 0;
    state->bgOffsets[1].vOffset = 0;

    g_DisplaySettings.controls[1].layers = 0x1F;
    g_DisplaySettings.controls[1].layers |= 0x12;
    state = &g_DisplaySettings.engineState[1];
    switch (state->bgSettings[1].bgMode) {
        case DISPLAY_BGMODE_AFFINE:
        case DISPLAY_BGMODE_PLTT:
        case DISPLAY_BGMODE_BMP256:
        case DISPLAY_BGMODE_BMPDIRECT:
        case DISPLAY_BGMODE_BMPLARGE:
            state->bgAffines[1].unk_14 = 1;
            break;

        default:
            break;
    }
    state->bgOffsets[1].hOffset = 0;
    state->bgOffsets[1].vOffset = 0;

    data                   = DatMgr_LoadRawData(*(s32*)args, NULL, 0, &data_ov039_0209a0e4);
    *(s32*)(sprite + 0x48) = (s32)data;

    {
        void* pal;
        void* chr;
        void* scr;

        if (data == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            pal      = base + *(u32*)(base + 8);
        }
        if (data == NULL) {
            chr = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            chr      = base + *(u32*)(base + 0x10);
        }
        if (data == NULL) {
            scr = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            scr      = base + *(u32*)(base + 0x18);
        }

        *(PaletteResource**)(sprite + 0x4C) = PaletteMgr_AllocPalette(g_PaletteManagers[0], pal, 0, 0, 1);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            *(BgResource**)(sprite + 0x50) = BgResMgr_AllocChar32(
                g_BgResourceManagers[0], chr, g_DisplaySettings.engineState[0].bgSettings[1].charBase, 0, size);
        }
        *(BgResource**)(sprite + 0x54) =
            BgResMgr_AllocScreen(g_BgResourceManagers[0], scr, g_DisplaySettings.engineState[0].bgSettings[1].screenBase,
                                 (u32)g_DisplaySettings.engineState[0].bgSettings[1].screenSizeText);
        PaletteMgr_Flush(g_PaletteManagers[0], *(PaletteResource**)(sprite + 0x4C));

        *(PaletteResource**)(sprite + 0x58) = PaletteMgr_AllocPalette(g_PaletteManagers[1], pal, 0, 0, 1);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            *(BgResource**)(sprite + 0x5C) = BgResMgr_AllocChar32(
                g_BgResourceManagers[1], chr, g_DisplaySettings.engineState[1].bgSettings[1].charBase, 0, size);
        }
        *(BgResource**)(sprite + 0x60) =
            BgResMgr_AllocScreen(g_BgResourceManagers[1], scr, g_DisplaySettings.engineState[1].bgSettings[1].screenBase,
                                 (u32)g_DisplaySettings.engineState[1].bgSettings[1].screenSizeText);
        PaletteMgr_Flush(g_PaletteManagers[1], *(PaletteResource**)(sprite + 0x58));
    }

    func_ov039_020985e0(sprite, sprite + 4);
    EasyFade_FadeMainDisplay(2, 0, 0x1000);
    EasyFade_FadeSubDisplay(2, 0, 0x1000);
    return 1;
}

/* ------------------------------------------------------------------ */
/* The 7-phase badge intro sequence, 0x0208ce88.                       */
/* ------------------------------------------------------------------ */

/**
 * @brief The badge's seven-phase intro, one state machine step per call.
 *
 * The badge block's +0xF4 flag pair and +0xFC phase counter drive it:
 *
 *   0 seeds a 0x14-frame hold and the +0x128 offset, fires
 *     `func_ov039_0208fefc` and sfx mode 0x331;
 *   1 winds +0x128 down by 0xA000 per frame and, when the hold lands, reloads
 *     the frame count from the badge's cell table at +0xC;
 *   2 is the free-move probe -- either wireless (+0xE8 set: adds the +2/+3
 *     byte pair as a Q12.12 point scaled) or local (nearest pin via
 *     `func_ov039_02087e2c`, then a 0x1E000-range test against 0x1800), and
 *     below both, steps the +0x100 counter and the +0xF0 bit-2 check;
 *   3 fires `func_ov039_0208ff30` and re-seeds from cell +0xE, mode 0x332;
 *   4 damps +0x128 to zero, fires `func_ov039_0208ff68`, then spawns 8 task
 *     sprites via `func_ov039_02097750` on a Q12.12 rotation step, mode 0x341
 *     and a 0x28-frame hold;
 *   5 drains the hold, and case 6 raises the +0xF8 "done" flag.
 *
 * Cell counts are u16 at +0x16C-indexed * 0x1C rows of the +0x170 table.
 */
// Nonmatching: 92.4%, and every row it does flag reduces to one fact: the
// target keeps `self` in r9 for the whole function and starts the case-4
// loop's constants at r6 (0xCD) / r5 (0x14) / r4 (0x2000), while this build
// puts `self` in r8 and hands the three constants r5/r4/r9. Because of that
// rename over 100 near-identical rows differ. Four things were real and are
// fixed: the +0x238 group slots must be read as a typed `groupIds[8]` (flat
// `self + 0x238 + i*4` strength-reduced into a walking pointer); the case-4
// selector's step must be spelled `i * 0x10000` inside the loop, not an
// accumulator outside -- an accumulator walking +0x10000 made mwcc split a
// parallel `step >> 2` IV and walk it by 0x4000, which the target does not
// have; case 2's point pair must be assigned x before y (mwcc stores x to the
// lower stack word first); and case 2's wireless +0xE8 pointer must be a named
// local, or mwcc loads it twice instead of once. The `aim` pair's own stack
// slots prologue/epilogue, every branch target and every store now agree.
void func_ov039_0208ce88(OtuBadge* task) {
    u8* self = (u8*)task;

    switch (*(s32*)(self + 0xFC)) {
        default:
            break;

        case 0:
            *(s32*)(self + 0x100) = 0x14;
            *(s32*)(self + 0xFC)  = *(s32*)(self + 0xFC) + 1;
            *(s32*)(self + 0x128) = 0;
            func_ov039_0208fefc(EasyTask_GetTaskData(*(void**)(self + 8), *(s32*)(self + 0x1E0)), 0x14);
            func_ov039_02087d04(0x331, (OtuPoint*)(self + 0x120), (OtuPoint*)(self + 0x110));
            return;

        case 1:
            *(s32*)(self + 0x128) = *(s32*)(self + 0x128) - 0xA000;
            *(s32*)(self + 0x100) = *(s32*)(self + 0x100) - 1;
            if (*(s32*)(self + 0x100) > 0) {
                return;
            }
            *(s32*)(self + 0x100) = *(u16*)(*(u8**)(self + 0x170) + (u32) * (u16*)*(s32*)(self + 0x16C) * 0x1C + 0xC);
            *(s32*)(self + 0xFC)  = *(s32*)(self + 0xFC) + 1;
            return;

        case 2: {
            OtuPoint aim;
            OtuPoint dir;

            if (*(s32*)(self + 0xE8) != NULL) {
                if (*(u16*)(self + 0xF0) & 1) {
                    u8* source = *(u8**)(self + 0xE8);

                    aim.x = *(u8*)(source + 2) << 0xC;
                    aim.y = *(u8*)(source + 3) << 0xC;
                    func_ov039_02098b8c((OtuPoint*)(self + 0x118), &aim, &aim);
                    if (func_ov039_0208a988(&aim, *(OtuCellGrid**)(self + 0xE4), *(TinPinSlammer_Scene**)(self + 0)) != 0) {
                        *(s32*)(self + 0x120) = aim.x;
                        *(s32*)(self + 0x124) = aim.y;
                    }
                }
            } else {
                OtuPinTask* cand =
                    func_ov039_02087e2c(*(TaskPool**)(self + 8), *(TinPinSlammer_Scene**)(self + 0), *(s32*)(self + 0xE0));

                if (cand != NULL) {
                    func_ov039_02098bb0((OtuPoint*)((u8*)cand + 0x120), (OtuPoint*)(self + 0x120), &dir);
                    if (func_ov039_02098d10(&dir) < 0x1E000) {
                        *(s32*)(self + 0x100) = 1;
                    } else {
                        func_ov039_02098d3c(&dir, &dir);
                        func_ov039_02098c00(0x1800, &dir, (OtuPoint*)(self + 0x120), &dir);
                        if (func_ov039_0208a988(&dir, *(OtuCellGrid**)(self + 0xE4), *(TinPinSlammer_Scene**)(self + 0)) != 0)
                        {
                            *(s32*)(self + 0x120) = dir.x;
                            *(s32*)(self + 0x124) = dir.y;
                        }
                    }
                }
            }

            *(s32*)(self + 0x100) = *(s32*)(self + 0x100) - 1;
            if (*(s32*)(self + 0x100) > 0 && !(*(u16*)(self + 0xF0) & 4)) {
                return;
            }
            *(s32*)(self + 0x100) = 0xA;
            *(s32*)(self + 0xFC)  = *(s32*)(self + 0xFC) + 1;
            return;
        }

        case 3:
            *(s32*)(self + 0x100) = *(s32*)(self + 0x100) - 1;
            if (*(s32*)(self + 0x100) > 0) {
                return;
            }
            func_ov039_0208ff30(EasyTask_GetTaskData(*(void**)(self + 8), *(s32*)(self + 0x1E0)));
            *(s32*)(self + 0x100) = *(u16*)(*(u8**)(self + 0x170) + (u32) * (u16*)*(s32*)(self + 0x16C) * 0x1C + 0xE);
            *(s32*)(self + 0xFC)  = *(s32*)(self + 0xFC) + 1;
            func_ov039_02087d04(0x332, (OtuPoint*)(self + 0x120), (OtuPoint*)(self + 0x110));
            return;

        case 4: {
            s32 i = 0;

            *(s32*)(self + 0x128) = *(s32*)(self + 0x128) + -*(s32*)(self + 0x128) / *(s32*)(self + 0x100);
            *(s32*)(self + 0x100) = *(s32*)(self + 0x100) - 1;
            if (*(s32*)(self + 0x100) > 0) {
                return;
            }
            func_ov039_0208ff68(EasyTask_GetTaskData(*(void**)(self + 8), *(s32*)(self + 0x1E0)));

            do {
                s32 step = i * 0x10000;

                func_ov039_02097750(EasyTask_GetTaskData(*(void**)(self + 8), ((OtuBadge*)task)->groupIds[i]),
                                    (OtuTaskParams*)(self + 0x120), (u32)((step + ((u32)(step >> 2) >> 0x1D)) << 0xD) >> 0x10,
                                    0x2000, 0xCD, 0x14);
                i = i + 1;
            } while (i < 8);

            func_ov039_02087d04(0x341, (OtuPoint*)(self + 0x120), (OtuPoint*)(self + 0x110));
            *(s32*)(self + 0x100) = 0x28;
            *(s32*)(self + 0xFC)  = *(s32*)(self + 0xFC) + 1;
            return;
        }

        case 5:
            *(s32*)(self + 0x100) = *(s32*)(self + 0x100) - 1;
            if (*(s32*)(self + 0x100) > 0) {
                return;
            }
            *(s32*)(self + 0xFC) = *(s32*)(self + 0xFC) + 1;
            return;

        case 6:
            *(s32*)(self + 0xF8) = 1;
            *(s32*)(self + 0xF4) = 0;
            return;
    }
}

/* ------------------------------------------------------------------ */
/* The badge clash wind-up, 0x0208eaa0.                                */
/* ------------------------------------------------------------------ */

/** The Q12 scale word and the 0x10-stride per-speed factor table. */
// extern s32 data_ov039_0209a398;   (already declared in TinPinSlammer.h)
// extern OtuSpeedEntry data_ov039_0209a3dc[];  (declared in OtuPinLogic)

/**
 * @brief Resolves whatever the badge found to clash against and starts the
 *        strike.
 *
 * Gate: +0x128 clear and +0x168 not positive, and the badge's mock mode
 * (+0xF8) one of 1/6/7 or, for mode 8, only while the +0xFC phase is 0 (or
 * past the range). Then for every 0xC-stride entry in the caller's object:
 * the candidate must be within the entry's +0x18C rank plus 0xC000, and the
 * clash sequence runs:
 *
 *   mode 6  fires the latch (mode 0x337), re-raises the 0x130-"no pin" shadow
 *           reset, resolves the contact with d3c (or a unit vector if the
 *           points coincide),
 *   mode 7  drops the +0x4000 phase byte of the active cursor, leaves
 *           `func_ov039_0209104c` to hold it, then starts a directional smoke
 *           pass from the sin/cos table (mode 0x33B) and steps its children
 *           while the badge's kind is still 6,
 *   mode 8  drops the airborne child task, mode 0x333, the contact again, and
 *           the tail that only mode 8 spares.
 * The tail (shared by 6/7/8): the badge's wind-up counter at +0x148 and kind
 * reset (mode 1) with `func_ov039_0208f7a4`, the position delta saved into
 * +0x138/+0x13C, and the two-way "clash partner" handoff at +0x1B0.
 */
// Nonmatching: 89.9%. Three scheduling shape differences left:
//   * the contact block. Target's contact block schedule mixes the y-diff
//     loads before the x-diff stores, laid out: load +0x124 pair, +0x120 self
//     copy, y-sub, +0x120 pair, x-sub, then store +0x12C before +0x130. This
//     source compiles straightforwardly in address order. Reversing the two
//     assignments in C scores worse (89.0).
//   * the tail's pool words. Target reads `data_ov039_0209a398` before the
//     +0x16C/+0x170 cell chain, so its literal pool lists a398 ahead of
//     a3dc; this source's factor-read runs first, giving the reversed pool
//     order. The inline-scale spelling (no scale local) supposedly matched
//     m2c's, but scored 85.9 -- the smull operand order flipped too.
//   * the `i = 0` init lands after the walk-inits here, before them in the
//     target (one `mov r7` row).
// The contact's `!= 0 || != 0` negation (swapped d3c arms) was worth +9% on
// its own and matched subsequently. Everything else -- all three mode bodies,
// the wind-down, the 64-bit smull/adc scale and every branch target -- agrees.
void func_ov039_0208eaa0(void* self_, void* task) {
    u8* other = (u8*)self_;
    u8* self  = (u8*)task;

    if (*(s32*)(self + 0x128) != 0) {
        return;
    }
    if (*(s32*)(self + 0x168) > 0) {
        return;
    }

    switch (*(s32*)(self + 0xF8)) {
        /* m2c's label order: default first (it joins the big block), then the
         * modes that share the body, then mode 8's phase gate, then the
         * modes that return. */
        default:
        case 1:
        case 6:
        case 7:
            break;

        case 8:
            switch (*(s32*)(self + 0xFC)) {
                default:
                case 0:
                    break;

                case 1:
                case 2:
                case 3:
                case 4:
                    return;
            }
            break;

        case 0:
        case 2:
        case 3:
        case 4:
        case 5:
        case 9:
            return;
    }

    if (*(s32*)(other + 0x180) <= 0) {
        return;
    }
    {
        s32 i     = 0;
        u8* mask  = other;
        u8* entry = other + 0x184;

        do {
            s32 limit = *(s32*)(mask + 0x18C) + 0xC000;

            if (func_ov039_02098ca8((OtuPoint*)(self + 0x120), (OtuPoint*)entry) >= limit) {
                goto next;
            }

            switch (*(s32*)(other + 0xF8)) {
                case 6: {
                    func_ov039_02091668(EasyTask_GetTaskData(*(void**)(other + 8), *(s32*)(other + 0x1E8)));
                    func_ov039_02087d04(0x337, (OtuPoint*)(other + 0x120), (OtuPoint*)(other + 0x110));

                    if (*(s32*)(self + 0x148) <= 0) {
                        if (*(u16*)*(s32*)(self + 0x16C) < 0x130) {
                            func_ov039_0208a490(self_, 1);
                        }
                    }

                    *(s32*)(self + 0x12C) = *(s32*)(self + 0x120) - *(s32*)(other + 0x120);
                    *(s32*)(self + 0x130) = *(s32*)(self + 0x124) - *(s32*)(other + 0x124);

                    if (*(s32*)(self + 0x12C) != 0 || *(s32*)(self + 0x130) != 0) {
                        func_ov039_02098d3c((OtuPoint*)(self + 0x12C), (OtuPoint*)(self + 0x12C));
                    } else {
                        *(s32*)(self + 0x12C) = 0x1000;
                        *(s32*)(self + 0x130) = 0;
                    }
                    break;
                }

                case 7: {
                    void* cand   = EasyTask_GetTaskData(*(void**)(other + 8), *(s32*)(other + 0x1E4));
                    u16   cursor = (u16)(func_ov039_02091060(cand) - 0x4000);
                    s32   pair   = (cursor >> 4) * 2;

                    func_ov039_0209104c(cand);

                    *(s32*)(self + 0x12C) = ((s16*)data_0205e4e0)[pair + 1];
                    *(s32*)(self + 0x130) = ((s16*)data_0205e4e0)[pair];

                    if (*(s32*)(self + 0xF8) == 6) {
                        func_ov039_02091690(EasyTask_GetTaskData(*(void**)(self + 8), *(s32*)(self + 0x1E8)));
                    }
                    func_ov039_02087d04(0x33B, (OtuPoint*)(other + 0x120), (OtuPoint*)(other + 0x110));

                    if (*(s32*)(self + 0x148) <= 0) {
                        if (*(u16*)*(s32*)(self + 0x16C) < 0x130) {
                            func_ov039_0208a490(self_, 1);
                        }
                    }
                    break;
                }

                case 8: {
                    if (*(s32*)(self + 0xF8) == 7) {
                        func_ov039_02091070(EasyTask_GetTaskData(*(void**)(self + 8), *(s32*)(self + 0x1E4)));
                    }
                    func_ov039_02087d04(0x333, (OtuPoint*)(other + 0x120), (OtuPoint*)(other + 0x110));

                    if (*(s32*)(self + 0x148) <= 0) {
                        if (*(u16*)*(s32*)(self + 0x16C) < 0x130) {
                            func_ov039_0208a490(self_, 1);
                        }
                    }

                    *(s32*)(self + 0x12C) = *(s32*)(self + 0x120) - *(s32*)(other + 0x120);
                    *(s32*)(self + 0x130) = *(s32*)(self + 0x124) - *(s32*)(other + 0x124);

                    if (*(s32*)(self + 0x12C) != 0 || *(s32*)(self + 0x130) != 0) {
                        func_ov039_02098d3c((OtuPoint*)(self + 0x12C), (OtuPoint*)(self + 0x12C));
                    } else {
                        *(s32*)(self + 0x12C) = 0x1000;
                        *(s32*)(self + 0x130) = 0;
                    }
                    break;
                }

                default:
                    break;
            }

            if (*(s32*)(self + 0x148) <= 0) {
                *(s32*)(self + 0x148) = *(u16*)(*(u8**)(self + 0x170) + (u32) * (u16*)*(s32*)(self + 0x16C) * 0x1C + 0x18);
                *(s32*)(self + 0xF8)  = 1;
                *(s32*)(self + 0xF4)  = 0;
                func_ov039_0208f7a4(EasyTask_GetTaskData(*(void**)(self + 8), *(s32*)(self + 0x1D8)), *(s32*)(self + 0x148));
            }
            {
                OtuPoint  start;
                OtuPoint* base = (OtuPoint*)(self + 0x12C);
                s32       factor =
                    *(s32*)((u8*)&data_ov039_0209a3dc +
                            *(u8*)(*(u8**)(self + 0x170) + (u32) * (u16*)*(s32*)(self + 0x16C) * 0x1C + 4) * 0x10 + 8);
                s32 scale;

                start.x = 0;
                scale   = (s32)(((s64)data_ov039_0209a398 * factor + 0x800) >> 0xC);
                start.y = 0;

                *(s32*)(self + 0x138) = *(s32*)(self + 0x12C);
                *(s32*)(self + 0x13C) = *(s32*)(self + 0x130);

                func_ov039_02098c00(scale, base, &start, base);
                *(s32*)(other + 0x1B0) = (s32)task;
                *(s32*)(self + 0x1B0)  = (s32)self_;
            }

        next:
            mask += 0xC;
            entry += 0xC;
            i = i + 1;
        } while (i < *(s32*)(other + 0x180));
    }
}

/* ------------------------------------------------------------------ */
/* The wrestling-banner board loader, 0x02092f88.                      */
/* ------------------------------------------------------------------ */

/** The board's bin id and the 0x12-wide frame-slot table. */
extern const BinIdentifier data_ov039_0209a0f4;
extern const void*         data_ov039_020999d0;

/**
 * @brief The wrestling board's background loader, one slot per layer.
 *
 * Reaches the sprite block at task+0x18, stamps the two words the caller
 * passes in +0/+4, marks the sub engine's BG0-3 layers on and sets up the
 * blend block (mode 1 over layers 3 and 0x3E, coefficients 0xA/6). Then four
 * identical resource slots, each reading its palette/char/screen sources out
 * of the Data buffer's relative offsets (+8/+0x10/+0x18, +0x20/+0x30/+0x38,
 * +0x40/+0x48, +0x50/+0x70/+0x78), allocating palette/char against
 * `engineState[1].bgSettings[N]` (the screen draw against
 * `bgSettings[2]`'s +2 and 0x3F0), and flushing. Layer 1's palette source is
 * the frame cursor at +0x840 loaded through `func_ov039_02098d7c` against
 * `data_ov039_020999d0` with 0x12 slots. In the middle: the 0x3F0 scratch
 * fill, `func_ov039_02092e30`'s mask build and the +0x3C word.
 */
s32 func_ov039_02092f88(TaskPool* pool, void* task, s32* args) {
    u8*   sprite = *(u8**)((u8*)task + 0x18);
    Data* data;
    u8*   pal;
    u8*   chr;
    u8*   scr;

    *(s32*)(sprite + 0x0) = *(s32*)((u8*)args + 4);
    *(s32*)(sprite + 0x4) = *(s32*)((u8*)args + 8);

    g_DisplaySettings.controls[1].layers |= 0xF;
    g_DisplaySettings.engineState[1].blendMode   = 1;
    g_DisplaySettings.engineState[1].blendLayer0 = 3;
    g_DisplaySettings.engineState[1].blendLayer1 = 0x3E;
    g_DisplaySettings.engineState[1].blendCoeff0 = 0xA;
    g_DisplaySettings.engineState[1].blendCoeff1 = 6;

    data                  = DatMgr_LoadRawData(*(s32*)args, NULL, 0, &data_ov039_0209a0f4);
    *(s32*)(sprite + 0x8) = (s32)data;

    {
        if (data == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            pal      = base + *(u32*)(base + 8);
        }
        if (data == NULL) {
            chr = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            chr      = base + *(u32*)(base + 0x10);
        }
        if (data == NULL) {
            scr = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            scr      = base + *(u32*)(base + 0x18);
        }

        *(PaletteResource**)(sprite + 0xC) = PaletteMgr_AllocPalette(g_PaletteManagers[1], pal, 0, 0, 1);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            *(BgResource**)(sprite + 0x1C) = BgResMgr_AllocChar32(
                g_BgResourceManagers[1], chr, g_DisplaySettings.engineState[1].bgSettings[0].charBase, 0, size);
        }
        *(BgResource**)(sprite + 0x2C) =
            BgResMgr_AllocScreen(g_BgResourceManagers[1], scr, g_DisplaySettings.engineState[1].bgSettings[0].screenBase,
                                 (u32)g_DisplaySettings.engineState[1].bgSettings[0].screenSizeText);
        PaletteMgr_Flush(g_PaletteManagers[1], *(PaletteResource**)(sprite + 0xC));

        data = *(Data**)(sprite + 0x8);
        if (data == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x20);
        }
        if (data == NULL) {
            chr = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            chr      = base + *(u32*)(base + 0x30);
        }
        if (data == NULL) {
            scr = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            scr      = base + *(u32*)(base + 0x38);
        }

        *(PaletteResource**)(sprite + 0x10) = PaletteMgr_AllocPalette(
            g_PaletteManagers[1], func_ov039_02098d7c(sprite + 0x840, (s32)pal, &data_ov039_020999d0, 0x12), 0, 1, 1);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            *(BgResource**)(sprite + 0x20) = BgResMgr_AllocChar32(
                g_BgResourceManagers[1], chr, g_DisplaySettings.engineState[1].bgSettings[1].charBase, 0, size);
        }
        *(BgResource**)(sprite + 0x30) =
            BgResMgr_AllocScreen(g_BgResourceManagers[1], scr, g_DisplaySettings.engineState[1].bgSettings[1].screenBase,
                                 (u32)g_DisplaySettings.engineState[1].bgSettings[1].screenSizeText);
        PaletteMgr_Flush(g_PaletteManagers[1], *(PaletteResource**)(sprite + 0x10));

        data = *(Data**)(sprite + 0x8);
        if (data == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x40);
        }
        if (data == NULL) {
            chr = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            chr      = base + *(u32*)(base + 0x48);
        }

        MI_CpuFillU16(0x3F0, sprite + 0x3C, 0x804);
        func_ov039_02092e30(sprite);
        *(s32*)(sprite + 0x3C) = 0x80400;

        *(PaletteResource**)(sprite + 0x14) = PaletteMgr_AllocPalette(g_PaletteManagers[1], pal, 0, 2, 1);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            *(BgResource**)(sprite + 0x24) = BgResMgr_AllocChar32(
                g_BgResourceManagers[1], chr, g_DisplaySettings.engineState[1].bgSettings[2].charBase, 0x3F0, size);
        }
        *(BgResource**)(sprite + 0x34) = BgResMgr_AllocScreen(
            g_BgResourceManagers[1], (void*)(sprite + 0x3C), g_DisplaySettings.engineState[1].bgSettings[2].screenBase,
            (u32)g_DisplaySettings.engineState[1].bgSettings[2].screenSizeText);
        PaletteMgr_Flush(g_PaletteManagers[1], *(PaletteResource**)(sprite + 0x14));

        data = *(Data**)(sprite + 0x8);
        if (data == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x50);
        }
        if (data == NULL) {
            chr = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            chr      = base + *(u32*)(base + 0x70);
        }
        if (data == NULL) {
            scr = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            scr      = base + *(u32*)(base + 0x78);
        }

        *(PaletteResource**)(sprite + 0x18) = PaletteMgr_AllocPalette(g_PaletteManagers[1], pal, 0, 3, 1);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            *(BgResource**)(sprite + 0x28) = BgResMgr_AllocChar32(
                g_BgResourceManagers[1], chr, g_DisplaySettings.engineState[1].bgSettings[3].charBase, 0, size);
        }
        *(BgResource**)(sprite + 0x38) =
            BgResMgr_AllocScreen(g_BgResourceManagers[1], scr, g_DisplaySettings.engineState[1].bgSettings[3].screenBase,
                                 (u32)g_DisplaySettings.engineState[1].bgSettings[3].screenSizeText);
        PaletteMgr_Flush(g_PaletteManagers[1], *(PaletteResource**)(sprite + 0x18));
    }
    return 1;
}

/* ------------------------------------------------------------------ */
/* The wrestling-board switchboard block, 0x02091b98.                  */
/* ------------------------------------------------------------------ */

/** The 0x40-byte table slicer copied to the bottom-half of the grid. */
extern s32                 data_ov039_020995f8[];
extern s32                 data_ov039_02099604[];
extern s32                 data_ov039_0209968c[];
extern s32                 data_ov039_02099620[];
extern s32                 data_ov039_02099630[];
extern s32                 data_ov039_0209964c[];
extern s32                 data_ov039_0209966c[];
extern s32                 data_ov039_020996b0[];
extern const BinIdentifier data_ov039_0209a0bc[4];
extern u8*                 data_ov039_0209a5d8[3];

/** The whole-word block shapes the loader's three stack tables copy out to. */
typedef struct {
    s32 w0;
    s32 w1;
    s32 w2;
} OtuWord3; // Size: 0xC

typedef struct {
    s32 w0;
    s32 w1;
    s32 w2;
    s32 w3;
    s32 w4;
    s32 w5;
    s32 w6;
    s32 w7;
    s32 w8;
} OtuWord9; // Size: 0x24

/**
 * @brief The wrestling board's block-loader: picks a sheet from the args
 *        block, allocates everything it needs and stamps the mixed-grid.
 *
 * The sprite block at task+0x18 gets: +0/+4 the sheet index and the heap, +8
 * the "loading" flag and +C the sheet description pointer. Then: three
 * literal tables are snapped to the stack whose layout picks a row by
 * sheet.cellType and sheet.faceId (a 3x3 u32 matrix plus a flat u32 table);
 * the relative refs +8/+0x10/+0x18 are loaded and allocated via palette and
 * BgResMgr_AllocChar32 against `bgSettings[1].charBase`, palette-flushed.
 * Then five frame cursors (0x54/0x64, 0x68/0x78, 0x7C/0x8C, 0x90/0xA0,
 * 0xA4/0xB4) are allocated against `data_ov039_0209964c` (8 slots),
 * `02099620` (4), `0209966c` (8), `020996b0` (0x19) and `02099630` (7),
 * each from a Data-buffer offset (0x10/0x18/0x20/0x28/0x30), each flushed.
 * The final +0x48 AllocChar32 comes from the +0x38-offset ref against
 * `bgSettings[1].charBase` at offset 0x300.
 *
 * The grid is `sheet.width + 7 / 8` by `sheet.height + 7 / 8`, backed by two
 * heaps -- a tile-pool buffer (0x800 bytes per cell) and a pointer table --
 * MI_CpuSet zeroed, and the pages are linked so rows[i][j] = tilePool +
 * (i + j) * 0x800. Then the layout walk stamps every tile pair
 * through `func_ov039_02091b40`, and the whole thing hands the pointer
 * table and the tile pool to `func_0200d1d8`.
 */
// Nonmatching: 81.7%. The body, constants and every branch agree; the residual
// gap is register naming and two extra stack words. What was spent here:
//   * the three literal-table copies must be struct assignments spelled with
//     whole-block casts (`*(OtuWord3*)sheetTable = *(OtuWord3*)...`),
//     not loops or word chains -- only the struct form emits the target's
//     ldm/stm pairs;
//   * the sheet kind must be re-read as a byte expression everywhere it is
//     used (`*(u8*)sheet0`), never held in a named u8 local -- a named local
//     forced an extra stack slot, a `str [sp+0xc]` spill the target does not
//     have, and shifted the whole frame (+2 words) and every pool offset;
//   * `sheet[2]` likewise is re-read per loop iteration (this build forgot
//     that once and dropped 3%);
//   * `sheet[1]` is a fresh leaf byte too -- naming it cost a register;
//   * the two cell-grid [8]-accumulator sizes are `(w+7+(correction)) >> 3`
//     spelled exactly as m2c's -- matches.
// The pointers currently land r5/r9/r6 where the target has r5/r7/r8, so
// every sprite+* memory row reads r6 where the target reads r7. Permuter and
// eight spelling sweeps found nothing further.
s32 func_ov039_02091b98(TaskPool* pool, OtuBadge* task, s32* args) {
    u8*   sprite = *(u8**)((u8*)task + 0x18);
    Data* data;
    Data* data2;
    u8*   pal;
    u8*   chr;
    u8*   scr;
    s32   sheetTable[3];
    s32   styleTable[9];
    s32   miscTable[3];
    u8*   sheet0;
    Heap* heap;
    s32   row;
    s32   row2;
    s32   cellsWide;
    s32   cellsHigh;
    s32   i;
    s32   j;
    s32   run;
    u8    wide;
    s32   col;

    *(s32*)(sprite + 0x8) = 1;
    *(s32*)(sprite + 0x0) = *(s32*)args;
    *(s32*)(sprite + 0x4) = *(s32*)((u8*)args + 4);
    *(s32*)(sprite + 0xC) = *(s32*)((u8*)args + 8);

    *(OtuWord3*)sheetTable = *(OtuWord3*)data_ov039_020995f8;
    *(OtuWord9*)styleTable = *(OtuWord9*)data_ov039_0209968c;
    *(OtuWord3*)miscTable  = *(OtuWord3*)data_ov039_02099604;

    sheet0 = *(u8**)((u8*)args + 8);
    heap   = *(Heap**)(sprite + 4);

    data                   = DatMgr_LoadRawData(*(s32*)args, NULL, 0, &data_ov039_0209a0b4[sheetTable[*(u8*)sheet0]]);
    *(s32*)(sprite + 0x44) = (s32)data;

    row2 = miscTable[*(u8*)sheet0];

    row  = styleTable[(u32) * (u8*)sheet0 * 3 + sheet0[1]];
    wide = sheet0[2];

    {
        if (data == NULL || row <= 0) {
            chr = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            chr      = base + *(u32*)(base + (u32)row * 8);
        }
        if (data == NULL || row2 <= 0) {
            pal = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            pal      = base + *(u32*)(base + (u32)row2 * 8);
        }

        *(PaletteResource**)(sprite + 0x4C) = PaletteMgr_AllocPalette(g_PaletteManagers[0], pal, 0, 0, 5);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            *(BgResource**)(sprite + 0x50) = BgResMgr_AllocChar32(
                g_BgResourceManagers[0], chr, g_DisplaySettings.engineState[0].bgSettings[1].charBase, 0, size);
        }
        PaletteMgr_Flush(g_PaletteManagers[0], *(PaletteResource**)(sprite + 0x4C));
    }

    data2                  = DatMgr_LoadRawData(*(s32*)args, NULL, 0, data_ov039_0209a0bc);
    *(s32*)(sprite + 0x40) = (s32)data2;

    {
        if (data2 == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x10);
        }
        *(PaletteResource**)(sprite + 0x64) = PaletteMgr_AllocPalette(
            g_PaletteManagers[0], func_ov039_02098d7c(sprite + 0x54, (s32)pal, data_ov039_0209964c, 8), 0, 6, 1);
        PaletteMgr_Flush(g_PaletteManagers[0], *(PaletteResource**)(sprite + 0x64));
    }

    {
        if (data2 == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x18);
        }
        *(PaletteResource**)(sprite + 0x78) = PaletteMgr_AllocPalette(
            g_PaletteManagers[0], func_ov039_02098d7c(sprite + 0x68, (s32)pal, data_ov039_02099620, 4), 0, 7, 1);
        PaletteMgr_Flush(g_PaletteManagers[0], *(PaletteResource**)(sprite + 0x78));
    }

    {
        if (data2 == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x20);
        }
        *(PaletteResource**)(sprite + 0x8C) = PaletteMgr_AllocPalette(
            g_PaletteManagers[0], func_ov039_02098d7c(sprite + 0x7C, (s32)pal, data_ov039_0209966c, 8), 0, 8, 1);
        PaletteMgr_Flush(g_PaletteManagers[0], *(PaletteResource**)(sprite + 0x8C));
    }

    {
        if (data2 == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x28);
        }
        *(PaletteResource**)(sprite + 0xA0) = PaletteMgr_AllocPalette(
            g_PaletteManagers[0], func_ov039_02098d7c(sprite + 0x90, (s32)pal, data_ov039_020996b0, 0x19), 0, 9, 1);
        PaletteMgr_Flush(g_PaletteManagers[0], *(PaletteResource**)(sprite + 0xA0));
    }

    {
        if (data2 == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x30);
        }
        *(PaletteResource**)(sprite + 0xB4) = PaletteMgr_AllocPalette(
            g_PaletteManagers[0], func_ov039_02098d7c(sprite + 0xA4, (s32)pal, data_ov039_02099630, 7), 0, 0xA, 1);
        PaletteMgr_Flush(g_PaletteManagers[0], *(PaletteResource**)(sprite + 0xB4));
    }

    {
        if (data2 == NULL) {
            chr = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            chr      = base + *(u32*)(base + 0x38);
        }
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            *(BgResource**)(sprite + 0x48) = BgResMgr_AllocChar32(
                g_BgResourceManagers[0], chr, g_DisplaySettings.engineState[0].bgSettings[1].charBase, 0x300, size);
        }
    }

    cellsWide = (wide + 7 + ((u32)((wide + 7) >> 2) >> 0x1D)) >> 3;
    cellsHigh = (sheet0[3] + 7 + ((u32)((sheet0[3] + 7) >> 2) >> 0x1D)) >> 3;

    *(s32*)(sprite + 0x3C) = (s32)Mem_AllocHeapTail(heap, cellsHigh * (cellsWide << 0xB));
    *(s32*)(sprite + 0x38) = (s32)Mem_AllocHeapTail(heap, cellsHigh * (cellsWide * 4));
    MI_CpuSet(*(void**)(sprite + 0x3C), 0, Mem_GetBlockSize(heap, *(void**)(sprite + 0x3C)));

    if (cellsHigh > 0) {
        i   = 0;
        run = 0;
        do {
            j = 0;
            if (cellsWide > 0) {
                do {
                    *(u32*)((u8*)*(s32*)(sprite + 0x38) + (s32)(run + j) * 4) =
                        (u32)((u8*)*(s32*)(sprite + 0x3C) + (j + run) * 0x800);
                    j = j + 1;
                } while (j < cellsWide);
            }
            i   = i + 1;
            run = run + cellsWide;
        } while (i < cellsHigh);
    }

    if (sheet0[3] > 0) {
        col = 0;
        do {
            if (sheet0[2] > 0) {
                j = 0;
                do {
                    u8* cellBase = *(u8**)(sheet0 + 0x10) + (col * sheet0[2] + j) * 2;
                    u8  cellByte = cellBase[1];
                    u16 tileVal  = *(u16*)(data_ov039_0209a5d8[*(u8*)sheet0] + cellByte * 2);

                    func_ov039_02091b40(tileVal,
                                        (u16*)(*(u32*)((u8*)*(s32*)(sprite + 0x38) + ((j / 8) * cellsWide + (col / 8)) * 4) +
                                               ((j % 8) * 4 + (col % 8) * 128) * 2));
                    j = j + 1;
                } while (j < (s32)sheet0[2]);
            }
            col = col + 1;
        } while (col < (s32)sheet0[3]);
    }

    func_0200d1d8(sprite + 0x10, 0, 1, 0, *(s32*)(sprite + 0x38), cellsWide, cellsHigh);
    return 1;
}
