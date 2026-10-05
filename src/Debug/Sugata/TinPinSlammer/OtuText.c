#include "Debug/Sugata/TinPinSlammer.h"
#include "Engine/Text.h"

/**
 * @file OtuText.c
 * @brief The debug editor's number formatting and stepping helpers, its cursor,
 *        and SINGLE MENU 1's round row.
 */

/** Writes |value|'s low `count` decimal digits into `buf`, then a space. */
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
 * Adds `offset * 10^digits` to `base`, wrapping within [lo, hi]: below `lo`
 * gives `hi` and above `hi` gives `lo`.
 */
s32 func_ov039_020839d4(s32 base, s32 offset, s32 digits, s32 lo, s32 hi) {
    s32 i;
    s32 scale = 1;

    for (i = 0; i < digits; i++) {
        scale *= 10;
    }

    base += offset * scale;
    if (base < lo) {
        return hi;
    }
    if (base > hi) {
        base = lo;
    }
    return base;
}

/** Formats Q12.12 `value` as `d.ddd` and a space; the sign is dropped. */
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
 * Steps Q12.12 `*value` by one decimal place of `d.ddd`: place 0 is the units,
 * 3 the thousandths. The magnitude wraps within [0, 9.999]; the sign is kept.
 */
void func_ov039_02083af4(s32* value, s32 direction, s32 place) {
    s32 step;
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
        scaled = 9999;
    } else if (scaled >= 10000) {
        scaled = 0;
    }
    if (negative != 0) {
        scaled = -scaled;
    }

    *value = scaled * 4096 / 1000;
}

/**
 * Redraws the cursor where it has moved: the marker beside the selected row,
 * which changes shape while a digit is selected.
 */
void func_ov039_02083bb0(TinPinSlammer_Scene* scene) {
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));

    if (table->cursor[table->page].row != table->cursor[table->page].drawnRow) {
        if (table->cursor[table->page].drawnRow >= 0) {
            Text_RenderToScreen(OTU_TEXT(scene), 0, table->cursor[table->page].drawnRow * 8 + 8, data_ov039_0209a16c);
        }
        Text_RenderToScreen(OTU_TEXT(scene), 0, table->cursor[table->page].row * 8 + 8, data_ov039_0209a170);
        table->cursor[table->page].drawnRow = table->cursor[table->page].row;
    }

    if (table->cursor[table->page].digit == table->cursor[table->page].drawnDigit) {
        return;
    }

    if (table->cursor[table->page].digit >= 0) {
        Text_RenderToScreen(OTU_TEXT(scene), 0, table->cursor[table->page].row * 8 + 8, data_ov039_0209a174);
    } else {
        Text_RenderToScreen(OTU_TEXT(scene), 0, table->cursor[table->page].row * 8 + 8, data_ov039_0209a170);
    }
    table->cursor[table->page].drawnDigit = table->cursor[table->page].digit;
}

/**
 * Steps SINGLE MENU 1's round row: the match index, then the match's board,
 * time limit and the opponents' AI records.
 */
void func_ov039_02083cbc(TinPinSlammer_Scene* scene, s32 step) {
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    s32        value;

    table->blink = 0;

    value = table->cursor[table->page].digit;
    switch (value) {
        case 0:
        case 1:
            scene->matchIndex = func_ov039_020839d4(scene->matchIndex, step, 1 - value, 0, 0x2B);
            func_ov039_020842bc(scene, 1);
            func_ov039_020842bc(scene, 2);
            func_ov039_020842bc(scene, 3);
            break;
        case 2:
            scene->matches[scene->matchIndex].board =
                (s16)func_ov039_020839d4(scene->matches[scene->matchIndex].board, step, 2 - value, 0, 9);
            break;
        case 3:
        case 4:
        case 5:
            scene->matches[scene->matchIndex].timeLimit =
                (s16)func_ov039_020839d4(scene->matches[scene->matchIndex].timeLimit, step, 5 - value, 0, 0x3E7);
            break;
        case 6:
            scene->matches[scene->matchIndex].opponents[0].ai =
                (s16)func_ov039_020839d4(scene->matches[scene->matchIndex].opponents[0].ai, step, 6 - value, 0, 6);
            break;
        case 7:
            scene->matches[scene->matchIndex].opponents[1].ai =
                (s16)func_ov039_020839d4(scene->matches[scene->matchIndex].opponents[1].ai, step, 6 - value, 0, 6);
            break;
        case 8:
            scene->matches[scene->matchIndex].opponents[2].ai =
                (s16)func_ov039_020839d4(scene->matches[scene->matchIndex].opponents[2].ai, step, 6 - value, 0, 6);
            break;
        default:
            break;
    }
}

/** Draws the round row. */
void func_ov039_02083ed4(TinPinSlammer_Scene* scene) {
    char       row[0x20];
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));

    func_ov039_02083974(&row[0x0], scene->matchIndex, 2);
    func_ov039_02083974(&row[0x3], scene->matches[scene->matchIndex].board, 1);
    func_ov039_02083974(&row[0x5], scene->matches[scene->matchIndex].timeLimit, 3);
    func_ov039_02083974(&row[0x9], scene->matches[scene->matchIndex].opponents[0].ai, 1);
    func_ov039_02083974(&row[0xB], scene->matches[scene->matchIndex].opponents[1].ai, 1);
    func_ov039_02083974(&row[0xD], scene->matches[scene->matchIndex].opponents[2].ai, 1);

    if (table->cursor[table->page].digit >= 0 && (table->blink & 0x20)) {
        row[data_ov039_02098f18[table->cursor[table->page].digit]] = ' ';
    }
    row[0xE] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x10, row);
}
