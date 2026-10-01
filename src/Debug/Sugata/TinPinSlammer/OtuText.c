#include "Debug/Sugata/TinPinSlammer.h"
#include "Engine/Text.h"

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

/**
 * @brief Step a Q12.12 value by one decimal place, wrapping at 10000.
 *
 * `place` is a small integer selecting the step: 0 is thousands, 1 hundreds,
 * 2 tens, 3 ones.  Values outside [0, 10000) wrap, and the sign is preserved
 * across the step so negative scores count down the same way positive ones count
 * up.  The scale is the usual divide-by-1000 for the Q12.12 to integer
 * conversion, then back again on the way out.
 */
// Nonmatching: 91.5%. The switch on `place`, the wrap at 10000 and the Q12.12
// scale in and out are all right, and the size matches. The remainder is mwcc
// normalising the `place` argument as an unsigned remainder (shift, subtract,
// rotate) where this build takes it as already in range.
void func_ov039_02083af4(s32* value, s32 direction, u32 place) {
    s32 step = 1;
    s32 scaled;
    s32 negative;

    scaled   = *value * 1000 / 4096;
    negative = (scaled < 0) ? 1 : 0;
    if (negative != 0) {
        scaled = -scaled;
    }

    switch (place) {
        case 0:
            step = 1000;
            break;
        case 1:
            step = 100;
            break;
        case 2:
            step = 10;
            break;
        case 3:
            step = 1;
            break;
    }

    scaled += step * direction;

    if (scaled < 0) {
        scaled = 10000;
    } else if (scaled >= 10000) {
        scaled = 0;
    }
    if (negative != 0) {
        scaled = -scaled;
    }

    *value = scaled * 4096 / 1000;
}

/**
 * @brief Draw the result screens' scrollbar.
 *
 * Two markers in the same column. The first tracks the value's row: a run of
 * fill glyphs from the previous row up to the current one, then a head glyph on
 * the current row. The second tracks the selection, and shows a dot rather than
 * a head when the selection is off the top of the list.
 *
 * Both are drawn into the same text object at x=0, spaced 8 pixels per row
 * starting one row in. Each marker is only redrawn when its row has actually
 * moved, which is what the `drawnValue`/`drawnRow` comparisons are for -- the
 * scene calls this every frame and the result screens are static most of the
 * time.
 */
// Nonmatching: 87.1%. Every call, field access and branch agrees with the
// target and the size is exact; what remains is register choice (the target
// holds the table in r4 and re-derives r0 per access, this build keeps r5).
void func_ov039_02083bb0(TinPinSlammer_Scene* scene) {
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuTextRow* row   = OTU_TEXT_ROW(table, table->index);

    if (row->row != row->drawnRow) {
        if (row->drawnRow >= 0) {
            Text_RenderToScreen(OTU_TEXT(scene), 0, row->drawnRow * 8 + 8, OtuGlyph_ScrollFill);
        }
        Text_RenderToScreen(OTU_TEXT(scene), 0, row->row * 8 + 8, OtuGlyph_ScrollHead);
        row->drawnRow = row->row;
    }

    // Re-derive rather than reuse the pointer: the target reloads the index
    // word before each field access.
    row = OTU_TEXT_ROW(table, table->index);
    if (row->value == row->drawnValue) {
        return;
    }

    if (row->value >= 0) {
        Text_RenderToScreen(OTU_TEXT(scene), 0, row->row * 8 + 8, OtuGlyph_ScrollHead);
    } else {
        Text_RenderToScreen(OTU_TEXT(scene), 0, row->row * 8 + 8, OtuGlyph_ScrollDot);
    }
    row->drawnValue = row->value;
}
