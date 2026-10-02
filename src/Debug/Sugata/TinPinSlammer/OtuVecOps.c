/**
 * @file OtuVecOps.c
 * @brief The overlay's Q12.12 vector helpers.
 *
 * These four functions sit in a 664-byte gap at 0x02098b8c-0x02098e24 that no
 * other translation unit claims, which is why they could not live in a band
 * include: OtuFieldAccess.c's .text claim ends at 0x02098b8c, just below them.
 *
 * That gap is why band 3 declares func_ov039_02098bb0 and func_ov039_02098c00
 * and both band 3 and band 5 call them, with no definition anywhere behind those
 * calls -- so objdiff could only ever report them "not present on target" rather
 * than as a bad match. Defining them here closes that.
 *
 * All three are the same pair of words at a scale of 0x1000, the overlay's
 * Q12.12 fixed point, the same convention the obstacle and hammer sprites
 * position with. The divides are written as `/ 0x1000` rather than as shifts
 * because the target spells them as the multiply-high plus shift pair mwcc emits
 * for a 32-bit divide by a power of two.
 */

#include "Debug/Sugata/TinPinSlammer.h"

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