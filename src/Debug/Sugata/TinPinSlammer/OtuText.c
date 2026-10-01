#include "Debug/Sugata/TinPinSlammer.h"

/**
 * @file OtuText.c
 * @brief Number and string helpers for the Otosu result screens.
 *
 * The result rows are built by writing decimal digits straight into a small
 * stack buffer, then handing the buffer to the text renderer. These are the
 * primitives that do the formatting: digit extraction, a fixed-point clamp, and
 * the separators the rows need.
 */

/**
 * @brief Write `value` into `buf` as decimal digits, least significant first.
 *
 * The rows are built right-to-left, so the digits land at `buf[count-1]` down to
 * `buf[0]` and a space terminator goes at `buf[count]`.  The sign is taken off
 * before dividing, so a negative value writes its digits without a minus sign;
 * the caller has already accounted for the sign.
 */
void func_ov039_02083974(char* buf, s32 value, s32 count) {
    s32 i;

    if (value < 0) {
        value = -value;
    }

    for (i = count - 1; i >= 0; i--) {
        buf[i] = '0' + (value % 10);
        value /= 10;
    }
    buf[count] = ' ';
}

/**
 * @brief Clamp `base + offset * 10^digits` into [lo, hi].
 *
 * Positions a field `offset` digits wide starting `digits` places in.  Returns
 * `lo` when the result falls under the low bound, and `hi` when it overruns.
 */
s32 func_ov039_020839d4(s32 base, s32 offset, s32 digits, s32 lo, s32 hi) {
    s32 scale = 1;
    s32 i;

    for (i = 0; i < digits; i++) {
        scale *= 10;
    }

    base += offset * scale;
    if (base < lo) {
        return lo;
    }
    if (base > hi) {
        base = hi;
    }
    return base;
}

/**
 * @brief Format a fixed-point value as `d.ddd`, with the decimal point at index 1.
 *
 * `value` arrives as a Q12.12 fixed-point number and is scaled by 1000 to get
 * the four digits, so the buffer reads thousands, point, hundreds, tens, ones.
 * Zero-padded rather than space-padded: the result screen wants every column
 * aligned, so a short value still occupies all four digits.
 */
void func_ov039_02083a20(OtuTextDigits* out, s32 value) {
    s32 scaled = value * 1000;

    scaled = (s32)scaled / 4096;
    if (scaled < 0) {
        scaled = -scaled;
    }

    out->thousands = '0' + (scaled / 1000) % 10;
    out->point     = '.';
    out->hundreds  = '0' + (scaled / 100) % 10;
    out->tens      = '0' + (scaled / 10) % 10;
    out->ones      = '0' + (scaled % 10);
    out->space     = ' ';
}
