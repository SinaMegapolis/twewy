/**
 * @file OtuVecOps.c
 * @brief The overlay's Q12.12 vector helpers.
 *
 * All of them are the same pair of words at a scale of 0x1000, the overlay's
 * Q12.12 fixed point, the same convention the obstacle and hammer sprites
 * position with. The divides are written as `/ 0x1000` rather than as shifts
 * because the target spells them as the multiply-high plus shift pair mwcc emits
 * for a 32-bit divide by a power of two.
 */

#include "Debug/Sugata/TinPinSlammer.h"
#include <nitro/fx/fx_vector.h>

/** out = a - b. */
void func_ov039_02098bb0(OtuPoint* a, OtuPoint* b, OtuPoint* out) {
    out->x = a->x - b->x;
    out->y = a->y - b->y;
}

/** out = (angle * a) / 0x1000 -- a rotation, which here is only a scale.
 *
 * The multiply is done at 64 bits and shifted, not divided in 32. The target's
 * multiply-high pair is how mwcc spells the high word of a 64-bit product, so a
 * plain `angle * a->x / 0x1000` overflows where the target does not and scores
 * 31.8% against 58.3% for this form.
 */
void func_ov039_02098bd4(s32 angle, OtuPoint* a, OtuPoint* out) {
    out->x = (s32)(((s64)angle * a->x) >> 12);
    out->y = (s32)(((s64)angle * a->y) >> 12);
}

/** out = b + (angle * a) / 0x1000 -- a scaled vector offset.
 *
 * The sibling every caller actually wants: this one keeps `b`, which is what
 * makes it a translate-by rather than a replace.
 */
void func_ov039_02098c00(s32 angle, OtuPoint* a, OtuPoint* b, OtuPoint* out) {
    out->x = b->x + (s32)(((s64)angle * a->x) >> 12);
    out->y = b->y + (s32)(((s64)angle * a->y) >> 12);
}

/** The dot product, at the same scale. Returns a scalar, not a point.
 *
 * 64-bit products again, with a +0x800 rounding constant before the shift -- the
 * target rounds to nearest rather than truncating, and dropping that term is a
 * one-bit-per-call difference that a match will not forgive.
 */
s32 func_ov039_02098c40(OtuPoint* a, OtuPoint* b) {
    return (s32)(((s64)a->x * b->x + (s64)a->y * b->y + 0x800) >> 12);
}

/** The 2D cross product's z component, same scale and rounding.
 *
 * The second product is written `b->x * a->y` rather than `a->y * b->x`. Same
 * arithmetic, and the difference is the load order: the target reads a.x, b.y,
 * b.x, a.y, which only comes out of the operands in that order.
 */
s32 func_ov039_02098c70(OtuPoint* a, OtuPoint* b) {
    return (s32)(((s64)a->x * b->y - (s64)b->x * a->y + 0x800) >> 12);
}

/** One entry of a frame table: a screen offset, halved on use, and how long to
 *  hold it. The table's stride is 4 bytes. */
typedef struct {
    /* 0x00 */ s16 offset;   // doubled into the returned value
    /* 0x02 */ s16 duration; // frames to hold before advancing
} OtuFrameSlot;              // Size: 0x4

/** A cursor walking a frame table, returning where it currently points. */
typedef struct {
    /* 0x00 */ OtuFrameSlot* slots;
    /* 0x04 */ s16           count;
    /* 0x06 */ s16           index;
    /* 0x08 */ s16           remaining;
    /* 0x0C */ s32           base;
} OtuFrameCursor; // Size: 0x10

/** The DS square root unit, driven by hand.
 *
 * 0x040002B0 is the control/scratch port: write 1 to start, poll bit 15 for
 * busy, read the result at 0x040002B4. The input is a Q12.12 value promoted to
 * the Q2.30 the unit wants, which is the low word shifted up by 2 with the high
 * word's bottom bits folded in below it.
 */
#define SQRT_CONTROL  ((volatile s16*)0x040002B0)
#define SQRT_RESULT   (*(volatile s32*)0x040002B4)
#define SQRT_INPUT_LO (*(volatile s32*)0x040002B8)
#define SQRT_INPUT_HI (*(volatile s32*)0x040002BC)

/** The distance between two points, rounded to nearest.
 *
 * The squared distance is taken at 64 bits, which is what the unit's input width
 * needs, then rounded by adding 1 before the halving shift.
 */
s32 func_ov039_02098ca8(OtuPoint* a, OtuPoint* b) {
    s64 sum = (s64)(a->y - b->y) * (a->y - b->y) + (s64)(a->x - b->x) * (a->x - b->x);
    u32 lo  = (u32)sum;
    u32 hi  = (u32)(sum >> 32);

    *SQRT_CONTROL = 1;
    SQRT_INPUT_HI = hi << 2;
    SQRT_INPUT_LO = (lo << 2) | (hi >> 30);

    while (*SQRT_CONTROL & 0x8000) {
    }

    return (SQRT_RESULT + 1) >> 1;
}

/** Vec_Magnitude over a 2D point: the third component is always zero.
 *
 * 66.8%, and the remaining gap is that the target loads both words in one
 * `ldmia` where this does two scalar `ldr`s in a different order. Writing the
 * copy as a struct assignment through the shared prefix was the obvious way to
 * ask for the block load and scored *worse* (57.2%), so whatever produces it is
 * not this. Left as the better of the two rather than guessed at further.
 */
s32 func_ov039_02098d10(OtuPoint* v) {
    Vec u;

    u.x = v->x;
    u.y = v->y;
    u.z = 0;

    return Vec_Magnitude(&u);
}

/** Vec_Normalize over a 2D point, likewise zero in z. Same 66.8%, same cause. */
void func_ov039_02098d3c(OtuPoint* src, OtuPoint* dest) {
    Vec u;
    Vec out;

    u.x = src->x;
    u.y = src->y;
    u.z = 0;

    Vec_Normalize(&u, &out);

    dest->x = out.x;
    dest->y = out.y;
}

/** Points the cursor at slot 0 and returns base + slots[0].offset * 2.
 *
 * The offset is doubled on the way out and the base is added, so a table stores
 * half-offsets -- which is the same reason the other helpers here work in a
 * scale of 0x1000.
 */
s32 func_ov039_02098d7c(OtuFrameCursor* cursor, s32 base, OtuFrameSlot* slots, s16 count) {
    cursor->slots     = slots;
    cursor->count     = count;
    cursor->base      = base;
    cursor->index     = 0;
    cursor->remaining = slots->duration;

    return cursor->base + cursor->slots[cursor->index].offset * 2;
}

/** Holds for one more frame, advancing and wrapping only when the hold expires. */
s32 func_ov039_02098dbc(OtuFrameCursor* cursor) {
    if (cursor->remaining <= 0) {
        cursor->index++;
        if (cursor->index >= cursor->count) {
            cursor->index = 0;
        }
        cursor->remaining = cursor->slots[cursor->index].duration;
    }
    cursor->remaining--;

    return cursor->base + cursor->slots[cursor->index].offset * 2;
}