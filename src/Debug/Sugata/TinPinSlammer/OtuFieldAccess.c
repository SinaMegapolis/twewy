#include "Debug/Sugata/TinPinSlammer.h"

/**
 * @file OtuFieldAccess.c
 *
 * The overlay's field accessors: short routines that read, write or clear one
 * or two words of a structure and return.
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
/* The stage task's 0x110 - 0x1B4 block.                              */
/* ------------------------------------------------------------------ */

/** Reads the +0x110/+0x114 pair out to a point. */
// Nonmatching: 60% (64% for 02092730). The target emits four instructions --
// load x, load y, store x, store y. mwcc will not produce that from any of the
// three forms tried here:
//
//   reading straight into out->x / out->y   60%  right, but it stores the first
//                                                 word between the two loads;
//   two scalar locals                       68%  mwcc merges the stores into
//                                                 `stmia r1, {r0, r2}` and
//                                                 reverses the load order;
//   one named OtuPoint local                68%  same stmia.
//
// The plain form is kept because it is the only one that is genuinely two
// separate stores, so the gap is mwcc's scheduling rather than a misread of
// the source. Nothing about the overlay is in doubt here.
void func_ov039_0208e85c(void* task, OtuPoint* out) {
    out->x = *(s32*)((u8*)task + 0x110);
    out->y = *(s32*)((u8*)task + 0x114);
}

/** Writes the +0x110/+0x114 pair from a point. */
void func_ov039_0208e848(void* task, OtuPoint* in) {
    s32 x = in->x;
    s32 y = in->y;

    *(s32*)((u8*)task + 0x110) = x;
    *(s32*)((u8*)task + 0x114) = y;
}

/** Reads the +0x118/+0x11C pair out to a point. */
// Nonmatching: 60% (64% for 02092730). The target emits four instructions --
// load x, load y, store x, store y. mwcc will not produce that from any of the
// three forms tried here:
//
//   reading straight into out->x / out->y   60%  right, but it stores the first
//                                                 word between the two loads;
//   two scalar locals                       68%  mwcc merges the stores into
//                                                 `stmia r1, {r0, r2}` and
//                                                 reverses the load order;
//   one named OtuPoint local                68%  same stmia.
//
// The plain form is kept because it is the only one that is genuinely two
// separate stores, so the gap is mwcc's scheduling rather than a misread of
// the source. Nothing about the overlay is in doubt here.
void func_ov039_0208e87c(void* task, OtuPoint* out) {
    out->x = *(s32*)((u8*)task + 0x118);
    out->y = *(s32*)((u8*)task + 0x11C);
}

/** Writes the +0x118/+0x11C pair from two values. */
void func_ov039_0208e870(void* task, s32 x, s32 y) {
    *(s32*)((u8*)task + 0x118) = x;
    *(s32*)((u8*)task + 0x11C) = y;
}

/** The +0x120/+0x124 pair, copied out. Used by the nearest-child queries. */
// Nonmatching: 60% (64% for 02092730). The target emits four instructions --
// load x, load y, store x, store y. mwcc will not produce that from any of the
// three forms tried here:
//
//   reading straight into out->x / out->y   60%  right, but it stores the first
//                                                 word between the two loads;
//   two scalar locals                       68%  mwcc merges the stores into
//                                                 `stmia r1, {r0, r2}` and
//                                                 reverses the load order;
//   one named OtuPoint local                68%  same stmia.
//
// The plain form is kept because it is the only one that is genuinely two
// separate stores, so the gap is mwcc's scheduling rather than a misread of
// the source. Nothing about the overlay is in doubt here.
void func_ov039_0208e6e0(OtuPinTask* task, OtuPoint* out) {
    out->x = *(s32*)((u8*)task + 0x120);
    out->y = *(s32*)((u8*)task + 0x124);
}

/** The +0x138/+0x13C pair, copied out -- the fourth such pair on this object. */
// Nonmatching: 60% (64% for 02092730). The target emits four instructions --
// load x, load y, store x, store y. mwcc will not produce that from any of the
// three forms tried here:
//
//   reading straight into out->x / out->y   60%  right, but it stores the first
//                                                 word between the two loads;
//   two scalar locals                       68%  mwcc merges the stores into
//                                                 `stmia r1, {r0, r2}` and
//                                                 reverses the load order;
//   one named OtuPoint local                68%  same stmia.
//
// The plain form is kept because it is the only one that is genuinely two
// separate stores, so the gap is mwcc's scheduling rather than a misread of
// the source. Nothing about the overlay is in doubt here.
void func_ov039_0208e6cc(void* task, OtuPoint* out) {
    out->x = *(s32*)((u8*)task + 0x138);
    out->y = *(s32*)((u8*)task + 0x13C);
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

/** Reads the +0x40/+0x44 pair out to a point. */
// Nonmatching: 60% (64% for 02092730). The target emits four instructions --
// load x, load y, store x, store y. mwcc will not produce that from any of the
// three forms tried here:
//
//   reading straight into out->x / out->y   60%  right, but it stores the first
//                                                 word between the two loads;
//   two scalar locals                       68%  mwcc merges the stores into
//                                                 `stmia r1, {r0, r2}` and
//                                                 reverses the load order;
//   one named OtuPoint local                68%  same stmia.
//
// The plain form is kept because it is the only one that is genuinely two
// separate stores, so the gap is mwcc's scheduling rather than a misread of
// the source. Nothing about the overlay is in doubt here.
void func_ov039_02092730(void* task, OtuPoint* out) {
    out->x = *(s32*)((u8*)task + 0x40);
    out->y = *(s32*)((u8*)task + 0x44);
}

/** Reads the +0x48/+0x4C pair out to a point. */
// Nonmatching: 60% (64% for 02092730). The target emits four instructions --
// load x, load y, store x, store y. mwcc will not produce that from any of the
// three forms tried here:
//
//   reading straight into out->x / out->y   60%  right, but it stores the first
//                                                 word between the two loads;
//   two scalar locals                       68%  mwcc merges the stores into
//                                                 `stmia r1, {r0, r2}` and
//                                                 reverses the load order;
//   one named OtuPoint local                68%  same stmia.
//
// The plain form is kept because it is the only one that is genuinely two
// separate stores, so the gap is mwcc's scheduling rather than a misread of
// the source. Nothing about the overlay is in doubt here.
void func_ov039_02092744(void* task, OtuPoint* out) {
    out->x = *(s32*)((u8*)task + 0x48);
    out->y = *(s32*)((u8*)task + 0x4C);
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
    s32 x = in->x;
    s32 y = in->y;

    *(s32*)((u8*)task + 0x50) = x;
    *(s32*)((u8*)task + 0x54) = y;
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

/** Raises the scene's +0x41EE4 flag. */
void func_ov039_02088688(TinPinSlammer_Scene* scene) {
    *(s32*)((u8*)scene + 0x41000 + 0xEE4) = 1;
}