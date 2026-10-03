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
 *
 * `hi` is the fifth argument and so travels on the stack; the target reads it
 * back from `[sp, #0x10]` after the loop rather than keeping it in a register.
 */
s32 func_ov039_020839d4(s32 base, s32 offset, s32 digits, s32 lo, s32 hi) {
    s32 i;
    s32 scale = 1;

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
// scale in and out are all right, and the size matches. The target reduces
// `place` modulo 4 as an *unsigned* value -- `rsb r2, r3, r2, lsl #0x1e` is the
// standard "negate and add 2^32" idiom, and the `0x10624dd3` pool word is
// mwcc's reciprocal for the following divide by 4. This build takes `place` as
// already in range and so skips both.
void func_ov039_02083af4(s32* value, s32 direction, u32 place) {
    s32 step = 1;
    s32 scaled;
    s32 negative;

    scaled   = *value * 1000 / 4096;
    negative = (scaled < 0) ? 1 : 0;
    if (negative != 0) {
        scaled = -scaled;
    }

    switch (place % 4) {
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
// Nonmatching: 99.97% in objdiff, which is its ceiling here -- the residual is
// three constant-pool `.word` symbol names, not code. Every instruction, field
// access and branch matches, and the size is exact.
//
// The fix that closed this was dropping the cached `row` pointer. The target
// re-derives it (`ldr [r4,#0]` then `add r1, r4, r0, lsl #4`) at every field
// access, which is six instructions mwcc will not re-emit once a local has
// swallowed the pointer. The same applies throughout this overlay: its source
// does not cache computed addresses.
void func_ov039_02083bb0(TinPinSlammer_Scene* scene) {
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));

    if (OTU_TEXT_ROW(table, table->index)->row != OTU_TEXT_ROW(table, table->index)->drawnRow) {
        if (OTU_TEXT_ROW(table, table->index)->drawnRow >= 0) {
            Text_RenderToScreen(OTU_TEXT(scene), 0, OTU_TEXT_ROW(table, table->index)->drawnRow * 8 + 8, data_ov039_0209a16c);
        }
        Text_RenderToScreen(OTU_TEXT(scene), 0, OTU_TEXT_ROW(table, table->index)->row * 8 + 8, data_ov039_0209a170);
        OTU_TEXT_ROW(table, table->index)->drawnRow = OTU_TEXT_ROW(table, table->index)->row;
    }

    // Re-derive rather than reuse the pointer: the target reloads the index
    // word before each field access.
    if (OTU_TEXT_ROW(table, table->index)->value == OTU_TEXT_ROW(table, table->index)->drawnValue) {
        return;
    }

    if (OTU_TEXT_ROW(table, table->index)->value >= 0) {
        Text_RenderToScreen(OTU_TEXT(scene), 0, OTU_TEXT_ROW(table, table->index)->row * 8 + 8, data_ov039_0209a174);
    } else {
        Text_RenderToScreen(OTU_TEXT(scene), 0, OTU_TEXT_ROW(table, table->index)->row * 8 + 8, data_ov039_0209a170);
    }
    OTU_TEXT_ROW(table, table->index)->drawnValue = OTU_TEXT_ROW(table, table->index)->value;
}

/**
 * @brief Advance the menu countdown one decimal place.
 *
 * The result screens count their score columns down one place per frame rather
 * than snapping to the final value. `step` is +1 or -1, and the row's `value`
 * says which column is currently counting -- cases 0 and 1 move the whole
 * header, 2 through 8 move one column each, and anything past 8 does nothing.
 *
 * Each column is stepped by a different power of ten, taken from the case number
 * itself (`1 - value`, `2 - value`, `5 - value`, `6 - value`), so the field's
 * magnitude rather than a per-case constant drives the arithmetic.
 */
// Nonmatching: 76.0%. The switch, its nine-way jump table, the bounds check and
// case 0/1 all match the target exactly, as does the size. Dropping the cached
// countdown pointer took this from 52.4%: the target re-derives
// `scene + 0x44000 + menuIndex * 0x34` before every field access, and caching it
// costs the `mla` mwcc then never repeats. Two things are still open: the later
// arms pass a fifth argument to the clamp that the target's callers never set
// (see func_ov039_020839d4), and the diff view under-reports here because nine
// near-identical arms slide out of alignment past case 1.
void func_ov039_02083cbc(TinPinSlammer_Scene* scene, s32 step) {
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    s32         value;

    // Cleared each call, so the countdown restarts if the row changes.
    table->flags = 0;

    value = OTU_TEXT_ROW(table, table->index)->value;
    switch (value) {
        case 0:
        case 1:
            scene->menuIndex = func_ov039_020839d4(scene->menuIndex, step, 1 - value, 0, 0x2B);
            func_ov039_020842bc(scene, 1);
            func_ov039_020842bc(scene, 2);
            func_ov039_020842bc(scene, 3);
            break;
        case 2:
            OTU_COUNTDOWN(scene)->thousands = (s16)func_ov039_020839d4(OTU_COUNTDOWN(scene)->thousands, step, 2 - value, 9, 0);
            break;
        case 3:
        case 4:
        case 5:
            OTU_COUNTDOWN(scene)->hundreds =
                (s16)func_ov039_020839d4(OTU_COUNTDOWN(scene)->hundreds, step, 5 - value, 0, 0x3E7);
            break;
        case 6:
            OTU_COUNTDOWN(scene)->tens = (s16)func_ov039_020839d4(OTU_COUNTDOWN(scene)->tens, step, 6 - value, 0, 6);
            break;
        case 7:
            OTU_COUNTDOWN(scene)->unk_98 = (s16)func_ov039_020839d4(OTU_COUNTDOWN(scene)->unk_98, step, 6 - value, 0, 6);
            break;
        case 8:
            OTU_COUNTDOWN(scene)->unk_A8 = (s16)func_ov039_020839d4(OTU_COUNTDOWN(scene)->unk_A8, step, 6 - value, 0, 6);
            break;
        default:
            break;
    }
}

/**
 * @brief Draw the pin-count row.
 *
 * Assembles six digit groups into one string on the stack -- the pin count, then
 * each of the five countdown columns -- and hands the whole thing to the text
 * renderer as a single row.  Building it in a buffer rather than calling the
 * renderer six times is what keeps the row on one line.
 *
 * `OtuTextFieldOffset` gives the character each countdown column starts at. When
 * the stage reports its own column hidden (the 0x20 flag) the character at that
 * offset is blanked to a space, so the row keeps its shape with the column empty.
 */
// Nonmatching: 83.0%. The six digit-group offsets, the digit counts, the blank
// offset table, the terminator position and the render call all match, and the
// row layout was recovered from the target's own `add r0, sp, #N` sequence --
// the groups tile sp+0x0..0xE, not the contiguous 2/1/3/1/1/1 widths a naive
// layout gives. The size is now exact.
//
// Removing the cached countdown and row pointers took this from 55.8%; the
// target re-derives `0x44000 + menuIndex * 0x34` before every field read, and
// re-derives the row pointer a second time for the blanking check. What is left
// is the tail: the target re-derives the row pointer once more after the render
// call's arguments are set up.
void func_ov039_02083ed4(TinPinSlammer_Scene* scene) {
    // One buffer, six digit groups tiled into it at fixed offsets.  Note there
    // is deliberately no local for the countdown base or the row pointer: the
    // target re-derives both at every use, and caching either costs the
    // multiply-and-add that mwcc then never emits again.
    char        row[0x10];
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));

    func_ov039_02083974(&row[0x0], scene->menuIndex, 2);
    func_ov039_02083974(&row[0x3], OTU_COUNTDOWN(scene)->thousands, 1);
    func_ov039_02083974(&row[0x5], OTU_COUNTDOWN(scene)->hundreds, 3);
    func_ov039_02083974(&row[0x9], OTU_COUNTDOWN(scene)->tens, 1);
    func_ov039_02083974(&row[0xB], OTU_COUNTDOWN(scene)->unk_98, 1);
    func_ov039_02083974(&row[0xD], OTU_COUNTDOWN(scene)->unk_A8, 1);

    if (OTU_TEXT_ROW(table, table->index)->value >= 0 && (table->flags & 0x20)) {
        row[data_ov039_02098f18[OTU_TEXT_ROW(table, table->index)->value]] = ' ';
    }
    row[0xE] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x10, row);
}
