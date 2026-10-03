#include "Debug/Sugata/TinPinSlammer.h"

/*
 * The band files. See OtuBands.h for why they are #included rather than being
 * separate translation units -- briefly, a delinks entry may claim a `.text`
 * range only once, ranges may not overlap, and every remaining function in this
 * overlay is already inside this file's claim. They are preprocessed into this
 * object, so they need no delinks entry and no configure.py run.
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
 * The band files, #included at the *bottom* of this file rather than the top.
 *
 * That ordering is load-bearing. At the top, every band was compiled before
 * this file's own 55 definitions, so each one had to forward-declare them --
 * and band 1's forward declarations, written without sight of the
 * definitions, guessed signatures that collided (int where the definition
 * returns void, six arguments where there are four). At the bottom, every
 * band sees every definition that precedes it, and no forward declarations
 * are needed.
 *
 * The order within the block matters too: band 1 is last because it calls
 * func_ov039_02097240, which band 5 defines, and func_ov039_02087ba0, which
 * band 4 declares. A call with no declaration in between is implicitly
 * `int (...)` and then collides with the real definition.
 *
 * See OtuBands.h for why these are includes and not separate translation
 * units, and for the `-ipa file` cache trap that means an edit to a band file
 * does not by itself invalidate this object.
 */
/* Order matters, and not for the reason it looks.
 *
 * OtuPinSprites.c calls func_ov039_02097240, which band 5 defines, and
 * func_ov039_02087ba0, which sits outside every band and which band 4
 * declares. Band 1's own address range is disjoint from all the others, so
 * including it last costs nothing.
 *
 * A call to a function defined later in the same translation unit, with no
 * declaration in between, is implicitly declared `int (...)` by C and then
 * collides with the real definition. Not hypothetical: that is exactly what
 * happened when these were concatenated in address order, and it is why band
 * 5 was told to declare its shared callees unprototyped.
 */
/* The clang-format off/on pair below is load-bearing. The pre-commit hook runs
 * clang-format with SortIncludes, which sorts this block back into address order
 * and silently reintroduces exactly the implicit `int (...)` declarations the note
 * above warns about. It has already done so once.
 */
// ============================================================================
// The include order below is load-bearing.
//
// OtuFieldAccess.c is one translation unit built from the fifteen .inc files
// listed here, and each of them calls functions that files further down define.
// Under -lang=c99 a call with no declaration ahead of it becomes an implicit
// `int (...)`; the explicit declaration then turns up later and the compiler
// reports it as a redeclaration, with the real error surfacing dozens of lines
// away inside an unrelated function.
//
// Nothing in C enforces that, so the order is only a convention -- and getting
// it wrong does not fail where the mistake is. It surfaced as 138 compile
// errors from one drafted batch and 67 from another.
//
//     python tools/check_band_order.py
//
// walks the translation unit in this order and fails if any symbol is used
// before it is declared. It currently reports none.
//
// When adding a file, put it where its dependencies allow, and run the check.
// ============================================================================

// clang-format off
#include "OtuCounters.inc"
#include "OtuEntryTasks.inc"
#include "OtuTaskStages.inc"
#include "OtuSpriteTasks.inc"
// Band 6 is self-contained -- it calls nothing the other bands define -- so it
// only has to come before band 1, which is the one include with a hard ordering
// requirement.
#include "OtuObstacles.inc"
// Band 7 is the hammer task's cursor family. It calls nothing, so like band 6 it
// only has to precede band 1.
#include "OtuMeters.inc"
#include "OtuHammerSpawn.inc"
// Band 13: Tsk_OtosuGame_specialgauge, drafted as a contiguous batch.
// Included here only because band 1 is the one include with a hard ordering
// requirement.
// band 1 is the one include with a hard ordering requirement.
#include "OtuGauge.inc"
// Band 9: a drafted batch, contiguous run, included here only because
// band 1 is the one include with a hard ordering requirement.
#include "OtuBoard.inc"
#include "OtuPinSprites.inc"
// Batch B2: the wireless-board stage block, 0x02089950-0x0208acc0.
// Placed AFTER band 1: it calls band-defined functions with no local
// forward declarations, so it must follow every band whose functions it uses.
#include "OtuPinLogic.inc"
// Batch B3: the badge/pin state machine. Last, because band 10 defines
// OtuPinLogic and OtuCellGrid and band 11 has to name both of them.
#include "OtuBadgeState.inc"
// Batch B4: OtuBadge, the pin tray. Last: it calls band 10's OtuPinLogic
// and band 11's OtuBadgeState, both of which are declared in those bands.
#include "OtuPinTray.inc"
// Band 14: OtuBadge's field accessors. After band 12, which declares the
// type they all read and write.
#include "OtuPinAccessors.inc"
// Batch: the badge AI and update. After band 14, which it calls and whose
// OtuBadgeState field names it shares with band 11.
#include "OtuBadgeAi.inc"
// Band 15: the pin task's own queries (0x0208ee84 - 0x0208efb0) and the
// wireless input snapshot. Last, because it calls band 10's cell query and
// bands 1/3's accessors with no forward declarations of its own.
#include "OtuGaps.inc"
// clang-format on
