/**
 * @file OtuEditor.c
 * @brief The debug editor (SINGLE MENU 1 to 3, OtuEditor): its number
 *        formatting and stepping helpers, its cursor, a draw function and a
 *        stepper for each row, and the editor stage's enter, layout, input and
 *        exit (OtuScene_FirstStage).
 *
 * A row is drawn as fixed-width digit groups, and the selected digit blinks:
 * when `blink & 0x20` the character at the cursor's column is blanked. Steppers
 * move the selected digit's place by `step` (+1 or -1), wrap the value within
 * the row's range, and restart the blink so the edited digit shows.
 *
 * Unless noted, each value is Q12.12 and drawn as `d.ddd`; SINGLE MENU 1 and 2
 * edit the overlay's tuning constants (listed in TinPinSlammer.h), SINGLE
 * MENU 3 the OtuBadgeParam record of one pin.
 *
 * One TU in the original: the cursor's glyphs (func_ov039_02083bb0) and the
 * row labels (func_ov039_02086174) are function-body string literals, which
 * mwcc appends to a TU's .data in source order, and the target has them as one
 * run right after the editor's stage descriptor (0209a158, 0209a15c).
 */

#include "Debug/Sugata/TinPinSlammer.h"
#include "Engine/Core/System.h"
#include "Engine/Text.h"
#include "OtuFieldAccessShared.h"

extern const char Otu_str_0209a178[16]; // "SINGLE MENU 1"
extern const char Otu_str_0209a188[12]; // start
extern const char Otu_str_0209a194[12]; // round
extern const char Otu_str_0209a1a0[8];  // friction
extern const char Otu_str_0209a1a8[8];  // badge 1
extern const char Otu_str_0209a1b0[8];  // badge 2
extern const char Otu_str_0209a1b8[8];  // badge 3
extern const char Otu_str_0209a1c0[8];  // badge 4
extern const char Otu_str_0209a1c8[8];  // weight 1
extern const char Otu_str_0209a1d0[8];  // weight 2
extern const char Otu_str_0209a1d8[8];  // weight 3
extern const char Otu_str_0209a1e0[8];  // weight 4
extern const char Otu_str_0209a1e8[8];  // weight 5
extern const char Otu_str_0209a1f0[8];  // weight 6
extern const char Otu_str_0209a1f8[8];  // weight 7
extern const char Otu_str_0209a200[4];  // weight 8 (with 0209a204, "8")
extern const char Otu_str_0209a208[8];  // weight 9
extern const char Otu_str_0209a210[8];  // weight 10
extern const char Otu_str_0209a218[12]; // gravity
extern const char Otu_str_0209a224[8];  // turn
extern const char Otu_str_0209a22c[8];  // panel
extern const char Otu_str_0209a234[8];  // stun
extern const char Otu_str_0209a23c[16]; // "SINGLE MENU 2"
extern const char Otu_str_0209a24c[12]; // collision effect 1
extern const char Otu_str_0209a258[12]; // collision effect 2
extern const char Otu_str_0209a264[8];  // special move
extern const char Otu_str_0209a26c[12]; // badge rotation
extern const char Otu_str_0209a278[16]; // "SINGLE MENU 3"
extern const char Otu_str_0209a288[12]; // badge ID
extern const char Otu_str_0209a294[12]; // has special move
extern const char Otu_str_0209a2a0[8];  // weight
extern const char Otu_str_0209a2a8[12]; // turning power
extern const char Otu_str_0209a2b4[12]; // needle
extern const char Otu_str_0209a2c0[8];  // meteo
extern const char Otu_str_0209a2c8[12]; // hammer
extern const char Otu_str_0209a2d4[12]; // stun time

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

/** Steps the friction row: one of the four per-tile-type rates. */
void func_ov039_02084020(TinPinSlammer_Scene* scene, s32 step) {
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    s32*       anchor;
    s32        value = table->cursor[table->page].digit;

    switch (value / 4) {
        case 0:
            anchor = &data_ov039_0209a39c[0];
            break;
        case 1:
            anchor = &data_ov039_0209a3a0;
            break;
        case 2:
            anchor = &data_ov039_0209a3a4;
            break;
        case 3:
            anchor = &data_ov039_0209a3a8;
            break;
    }

    func_ov039_02083af4(anchor, step, table->cursor[table->page].digit % 4);

    table->blink = 0;
}

/** Draws the friction row. */
void func_ov039_020840c0(TinPinSlammer_Scene* scene) {
    char       row[0x20];
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    s32*       src   = &data_ov039_0209a39c[0];
    s32        value;

    func_ov039_02083a20((OtuTextDigits*)&row[0x0], src[0]);
    row[0x5] = ' ';
    func_ov039_02083a20((OtuTextDigits*)&row[0x6], src[1]);
    row[0xB] = ' ';
    func_ov039_02083a20((OtuTextDigits*)&row[0xC], src[2]);
    row[0x11] = ' ';
    func_ov039_02083a20((OtuTextDigits*)&row[0x12], src[3]);
    // No store for the fourth separator: func_ov039_02083a20's own trailing
    // space lands on 0x17, and the target relies on that rather than writing it
    // a second time the way func_ov039_020844f8 does.

    value = table->cursor[table->page].digit;
    if (value >= 0 && (table->blink & 0x20)) {
        row[data_ov039_02099034[value]] = ' ';
    }
    row[0x17] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x18, row);
}

/**
 * Steps one pin id of a deck row (0 is the player's deck, 1 to 3 the match's
 * opponents), one decimal place at a time.
 */
void func_ov039_020841a0(TinPinSlammer_Scene* scene, s32 which, s32 step) {
    u16*       row;
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    s32        value;
    s32        place;
    s32        v;

    // Same source selection, and the same missing `default`, as the row draw.
    switch (which) {
        case 0:
            row = scene->decks[which];
            break;
        case 1:
        case 2:
        case 3:
            row = scene->matches[scene->matchIndex].opponents[which - 1].deck;
            break;
    }

    value = table->cursor[table->page].digit;
    v     = row[value / 3];

    if (v == OTU_NO_PIN) {
        v = 0;
    } else {
        v = v + 1;
    }

    switch (value % 3) {
        case 0:
            place = 100;
            break;
        case 1:
            place = 10;
            break;
        case 2:
            place = 1;
            break;
    }

    v = v + place * step;

    if (v < 0) {
        v = OTU_NO_PIN + 1;
    } else {
        if (v >= OTU_NO_PIN + 2) {
            v = 0;
        }
        v = (v == 0) ? OTU_NO_PIN : v - 1;
    }

    row[value / 3] = v;

    table->blink = 0;
}

/** Draws a deck row: seven three-digit pin ids, OTU_NO_PIN as 000. */
void func_ov039_020842bc(TinPinSlammer_Scene* scene, s32 which) {
    char       row[0x1D];
    u16*       src;
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    s32        value;
    s32        i;

    // No `default`: the target's bounds check sends anything past 3 straight to
    // the formatting loop with the source pointer left as case 0 left it, so the
    // out-of-range case is not a separate assignment.
    switch (which) {
        case 0:
            src = scene->decks[which];
            break;
        case 1:
        case 2:
        case 3:
            src = scene->matches[scene->matchIndex].opponents[which - 1].deck;
            break;
    }

    for (i = 0; i < 6; i++) {
        value = src[0];

        if (value == 0x130) {
            value = 0;
        } else {
            value = value + 1;
        }

        row[i * 4 + 0] = '0' + (value / 100) % 10;
        row[i * 4 + 1] = '0' + (value / 10) % 10;
        row[i * 4 + 2] = '0' + value % 10;
        row[i * 4 + 3] = ' ';

        // Advanced at the *end* of the body, not fused into the read above:
        // the target emits a plain `ldrh` plus a separate `add`, and mwcc only
        // folds the two into a post-indexed load when they are adjacent.
        src = src + 1;
    }

    value = table->cursor[table->page].digit;
    if (value >= 0 && (table->blink & 0x20)) {
        row[data_ov039_020990b4[value]] = ' ';
    }
    row[0x18] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, which * 8 + 0x20, row);
}

/** Steps one field of weight class `slot`. */
void func_ov039_02084458(TinPinSlammer_Scene* scene, s32 slot, s32 step) {
    OtuEditor*  table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuPinTune* tune  = &data_ov039_0209a3dc[slot];
    s32*        column;
    s32         value = table->cursor[table->page].digit;

    switch (value / 4) {
        case 0:
            column = &tune->launch;
            break;
        case 1:
            column = &tune->accel;
            break;
        case 2:
            column = &tune->power;
            break;
        case 3:
            column = &tune->weight;
            break;
    }

    func_ov039_02083af4(column, step, table->cursor[table->page].digit % 4);

    table->blink = 0;
}

/** Draws weight class `slot`. */
void func_ov039_020844f8(TinPinSlammer_Scene* scene, s32 slot) {
    char        row[0x20];
    OtuEditor*  table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuPinTune* tune  = &data_ov039_0209a3dc[slot];
    s32         value;

    func_ov039_02083a20((OtuTextDigits*)&row[0x0], tune->launch);
    row[0x5] = ' ';
    func_ov039_02083a20((OtuTextDigits*)&row[0x6], tune->accel);
    row[0xB] = ' ';
    func_ov039_02083a20((OtuTextDigits*)&row[0xC], tune->power);
    row[0x11] = ' ';
    func_ov039_02083a20((OtuTextDigits*)&row[0x12], tune->weight);
    row[0x17] = ' ';

    value = table->cursor[table->page].digit;
    if (value >= 0 && (table->blink & 0x20)) {
        row[data_ov039_02099074[value]] = ' ';
    }
    row[0x17] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, slot * 8 + 0x40, row);
}

/** Steps the gravity row. */
void func_ov039_020845dc(TinPinSlammer_Scene* scene, s32 step) {
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));

    func_ov039_02083af4(&data_ov039_0209a304, step, table->cursor[table->page].digit % 4);

    table->blink = 0;
}

/** Draws the gravity row. */
void func_ov039_0208462c(TinPinSlammer_Scene* scene) {
    char       row[0x20];
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    s32        value;

    func_ov039_02083a20((OtuTextDigits*)&row[0x0], data_ov039_0209a304);

    value = table->cursor[table->page].digit;
    if (value >= 0 && (table->blink & 0x20)) {
        row[data_ov039_02098e88[value]] = ' ';
    }
    row[0x5] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x90, row);
}

/** Steps the turn row. */
void func_ov039_020846c4(TinPinSlammer_Scene* scene, s32 step) {
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    s32*       column;
    s32        value = table->cursor[table->page].digit;

    switch (value / 4) {
        case 0:
            column = &data_ov039_0209a320;
            break;
        case 1:
            column = &data_ov039_0209a38c;
            break;
    }

    func_ov039_02083af4(column, step, table->cursor[table->page].digit % 4);

    table->blink = 0;
}

/** Draws the turn row. */
void func_ov039_02084738(TinPinSlammer_Scene* scene) {
    char       row[0x20];
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    s32        value;

    func_ov039_02083a20((OtuTextDigits*)&row[0x0], data_ov039_0209a38c);
    row[0x5] = ' ';
    func_ov039_02083a20((OtuTextDigits*)&row[0x6], data_ov039_0209a320);

    value = table->cursor[table->page].digit;
    if (value >= 0 && (table->blink & 0x20)) {
        row[data_ov039_02098ef8[value]] = ' ';
    }
    row[0xB] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x98, row);
}

/** Steps the panel row. */
void func_ov039_020847ec(TinPinSlammer_Scene* scene, s32 step) {
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    s32*       column;
    s32        value = table->cursor[table->page].digit;

    switch (value / 4) {
        case 0:
            column = &data_ov039_0209a31c;
            break;
        case 1:
            column = &data_ov039_0209a308;
            break;
        case 2:
            column = &data_ov039_0209a324;
            break;
    }

    func_ov039_02083af4(column, step, table->cursor[table->page].digit % 4);

    table->blink = 0;
}

/** Draws the panel row. */
void func_ov039_02084874(TinPinSlammer_Scene* scene) {
    char       row[0x20];
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    s32        value;

    func_ov039_02083a20((OtuTextDigits*)&row[0x0], data_ov039_0209a31c);
    row[0x5] = ' ';
    func_ov039_02083a20((OtuTextDigits*)&row[0x6], data_ov039_0209a308);
    row[0xB] = ' ';
    func_ov039_02083a20((OtuTextDigits*)&row[0xC], data_ov039_0209a324);
    value = table->cursor[table->page].digit;
    if (value >= 0 && (table->blink & 0x20)) {
        row[data_ov039_02098fc4[value]] = ' ';
    }
    row[0x11] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0xA0, row);
}

/** Steps the stun row. */
void func_ov039_02084944(TinPinSlammer_Scene* scene, s32 step) {
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    s32*       column;

    // The row has one value, so only quotient 0 assigns the pointer.
    if (table->cursor[table->page].digit / 4 == 0) {
        column = &data_ov039_0209a388;
    }

    func_ov039_02083af4(column, step, table->cursor[table->page].digit % 4);

    table->blink = 0;
}

/** Draws the stun row. */
void func_ov039_020849a4(TinPinSlammer_Scene* scene) {
    char       row[0x20];
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    s32        value;

    func_ov039_02083a20((OtuTextDigits*)&row[0x0], data_ov039_0209a388);

    value = table->cursor[table->page].digit;
    if (value >= 0 && (table->blink & 0x20)) {
        row[data_ov039_02098e98[value]] = ' ';
    }
    row[0x5] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0xA8, row);
}

/** Steps the first collision-effect row. */
void func_ov039_02084a3c(TinPinSlammer_Scene* scene, s32 step) {
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    s32*       column;
    s32        value = table->cursor[table->page].digit;

    switch (value / 4) {
        case 0:
            column = &data_ov039_0209a314;
            break;
        case 1:
            column = &data_ov039_0209a328;
            break;
        case 2:
            column = &data_ov039_0209a318;
            break;
    }

    func_ov039_02083af4(column, step, table->cursor[table->page].digit % 4);

    table->blink = 0;
}

/** Draws the first collision-effect row. */
void func_ov039_02084ac4(TinPinSlammer_Scene* scene) {
    char       row[0x20];
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    s32        value;

    func_ov039_02083a20((OtuTextDigits*)&row[0x0], data_ov039_0209a314);
    row[0x5] = ' ';
    func_ov039_02083a20((OtuTextDigits*)&row[0x6], data_ov039_0209a328);
    row[0xB] = ' ';
    func_ov039_02083a20((OtuTextDigits*)&row[0xC], data_ov039_0209a318);
    value = table->cursor[table->page].digit;
    if (value >= 0 && (table->blink & 0x20)) {
        row[data_ov039_02098f64[value]] = ' ';
    }
    row[0x11] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x8, row);
}

/** Steps the second collision-effect row. */
void func_ov039_02084b94(TinPinSlammer_Scene* scene, s32 step) {
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    s32*       column;
    s32        value = table->cursor[table->page].digit;

    switch (value / 4) {
        case 0:
            column = &data_ov039_0209a2fc;
            break;
        case 1:
            column = &data_ov039_0209a30c;
            break;
        case 2:
            column = &data_ov039_0209a310;
            break;
        case 3:
            column = &data_ov039_0209a300;
            break;
    }

    func_ov039_02083af4(column, step, table->cursor[table->page].digit % 4);

    table->blink = 0;
}

/** Draws the second collision-effect row. */
void func_ov039_02084c34(TinPinSlammer_Scene* scene) {
    char       row[0x20];
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    s32        value;

    func_ov039_02083a20((OtuTextDigits*)&row[0x0], data_ov039_0209a2fc);
    row[0x5] = ' ';
    func_ov039_02083a20((OtuTextDigits*)&row[0x6], data_ov039_0209a30c);
    row[0xB] = ' ';
    func_ov039_02083a20((OtuTextDigits*)&row[0xC], data_ov039_0209a310);
    row[0x11] = ' ';
    func_ov039_02083a20((OtuTextDigits*)&row[0x12], data_ov039_0209a300);
    value = table->cursor[table->page].digit;
    if (value >= 0 && (table->blink & 0x20)) {
        row[data_ov039_02098ff4[value]] = ' ';
    }
    row[0x17] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x10, row);
}

/** Steps the special-move row. */
void func_ov039_02084d20(TinPinSlammer_Scene* scene, s32 step) {
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    s32*       column;

    if (table->cursor[table->page].digit / 4 == 0) {
        column = &data_ov039_0209a398;
    }

    func_ov039_02083af4(column, step, table->cursor[table->page].digit % 4);

    table->blink = 0;
}

/** Draws the special-move row. */
void func_ov039_02084d80(TinPinSlammer_Scene* scene) {
    char       row[0x20];
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    s32        value;

    func_ov039_02083a20((OtuTextDigits*)&row[0x0], data_ov039_0209a398);
    row[0x5] = ' ';

    value = table->cursor[table->page].digit;
    if (value >= 0 && (table->blink & 0x20)) {
        row[data_ov039_02098e68[value]] = ' ';
    }
    row[0x5] = 0;
    row[0x5] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x18, row);
}

/** Steps the badge-rotation row: two five-digit integers. */
void func_ov039_02084e1c(TinPinSlammer_Scene* scene, s32 step) {
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    s32*       counter;
    s32        place;
    s32        value;

    // Two five-digit values.
    switch (table->cursor[table->page].digit / 5) {
        case 0:
            counter = &data_ov039_0209a394;
            break;
        case 1:
            counter = &data_ov039_0209a390;
            break;
    }

    value = *counter;

    switch (table->cursor[table->page].digit % 5) {
        case 0:
            place = 10000;
            break;
        case 1:
            place = 1000;
            break;
        case 2:
            place = 100;
            break;
        case 3:
            place = 10;
            break;
        case 4:
            place = 1;
            break;
    }

    value += place * step;

    if (value >= 100000) {
        value = 0;
    } else if (value < 0) {
        value = 99999;
    }

    *counter = value;

    table->blink = 0;
}

/** Draws the badge-rotation row. */
void func_ov039_02084f08(TinPinSlammer_Scene* scene) {
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    char       row[0x20];
    s32        value;

    row[5]  = ' ';
    row[11] = ' ';

    value  = data_ov039_0209a394;
    row[0] = '0' + (value / 10000) % 10;
    row[1] = '0' + (value / 1000) % 10;
    row[2] = '0' + (value / 100) % 10;
    row[3] = '0' + (value / 10) % 10;
    row[4] = '0' + value % 10;

    value   = data_ov039_0209a390;
    row[6]  = '0' + (value / 10000) % 10;
    row[7]  = '0' + (value / 1000) % 10;
    row[8]  = '0' + (value / 100) % 10;
    row[9]  = '0' + (value / 10) % 10;
    row[10] = '0' + value % 10;

    value = table->cursor[table->page].digit;
    if (value >= 0 && (table->blink & 0x20)) {
        row[data_ov039_02098f3c[value]] = ' ';
    }
    row[11] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x20, row);
}

/** Steps the pin id SINGLE MENU 3 edits. */
void func_ov039_02085124(TinPinSlammer_Scene* scene, s32 step) {
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    s32        place;
    s32        count;

    count = table->pinId;

    switch (table->cursor[table->page].digit % 3) {
        case 0:
            place = 100;
            break;
        case 1:
            place = 10;
            break;
        case 2:
            place = 1;
            break;
    }

    count = count + place * step;

    if (count < 0) {
        count = 0x130;
    } else if (count >= 0x131) {
        count = 0;
    }
    table->pinId = count;

    table->blink = 0;
}

/** Draws the pin id row. */
void func_ov039_020851bc(TinPinSlammer_Scene* scene) {
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    char       row[0x20];
    s32        value = table->pinId + 1;

    row[0] = '0' + (value / 100) % 10;
    row[1] = '0' + (value / 10) % 10;
    row[2] = '0' + value % 10;
    row[3] = ' ';

    value = table->cursor[table->page].digit;
    if (value >= 0 && (table->blink & 0x20)) {
        row[data_ov039_02098e5c[value]] = ' ';
    }
    row[3] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x8, row);
}

/** Steps one of the pin's four special-move counts (OtuBadgeParam.tile). */
void func_ov039_020852c0(TinPinSlammer_Scene* scene, s32 slot, s32 step) {
    OtuEditor*     table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuBadgeParam* param = &scene->badgeParams[slot];
    u8*            counter;
    s32            place;
    s32            value;

    switch (table->cursor[table->page].digit / 2) {
        case 0:
            counter = &param->tile[0];
            break;
        case 1:
            counter = &param->tile[1];
            break;
        case 2:
            counter = &param->tile[2];
            break;
        case 3:
            counter = &param->tile[3];
            break;
    }

    value = *counter;

    switch (table->cursor[table->page].digit % 2) {
        case 0:
            place = 10;
            break;
        case 1:
            place = 1;
            break;
    }

    value += place * step;

    if (value < 0) {
        value = 99;
    } else if (value >= 100) {
        value = 0;
    }

    *counter = value;

    table->blink = 0;
}

/** Draws the special-move counts. */
void func_ov039_02085388(TinPinSlammer_Scene* scene, s32 slot) {
    OtuEditor*     table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuBadgeParam* param = &scene->badgeParams[slot];
    char           row[0x20];
    s32            value;

    value  = param->tile[0];
    row[0] = '0' + (value / 10) % 10;
    row[1] = '0' + value % 10;
    row[2] = ' ';

    value  = param->tile[1];
    row[3] = '0' + (value / 10) % 10;
    row[4] = '0' + value % 10;
    row[5] = ' ';

    value  = param->tile[2];
    row[6] = '0' + (value / 10) % 10;
    row[7] = '0' + value % 10;
    row[8] = ' ';

    value   = param->tile[3];
    row[9]  = '0' + (value / 10) % 10;
    row[10] = '0' + value % 10;
    row[11] = ' ';

    value = table->cursor[table->page].digit;
    if (value >= 0 && (table->blink & 0x20)) {
        row[data_ov039_02098ed8[value]] = ' ';
    }
    row[11] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x10, row);
}

/** Steps the pin's weight class. */
void func_ov039_0208554c(TinPinSlammer_Scene* scene, s32 slot, s32 step) {
    OtuEditor*     table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuBadgeParam* param = &scene->badgeParams[slot];
    u8*            counter;
    s32            place;
    s32            value;

    // The row has one field, so only quotient 0 assigns the pointer.
    switch (table->cursor[table->page].digit / 2) {
        case 0:
            counter = &param->tuneIndex;
            break;
    }

    value = *counter;

    switch (table->cursor[table->page].digit % 2) {
        case 0:
            place = 10;
            break;
        case 1:
            place = 1;
            break;
    }

    value += place * step;

    if (value < 0) {
        value = 9;
    } else if (value >= 10) {
        value = 0;
    }

    *counter = value;

    table->blink = 0;
}

/** Draws the pin's weight class, one-based. */
void func_ov039_020855e0(TinPinSlammer_Scene* scene, s32 slot) {
    OtuEditor*     table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuBadgeParam* param = &scene->badgeParams[slot];
    char           row[0x20];
    s32            value = param->tuneIndex + 1;

    row[0] = '0' + (value / 10) % 10;
    row[1] = '0' + value % 10;
    row[2] = ' ';

    value = table->cursor[table->page].digit;
    if (value >= 0 && (table->blink & 0x20)) {
        row[data_ov039_02098e48[value]] = ' ';
    }
    row[2] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x18, row);
}

/** Steps the pin's bending force (OtuBadgeParam.friction). */
void func_ov039_020856c8(TinPinSlammer_Scene* scene, s32 slot, s32 step) {
    OtuEditor*     table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuBadgeParam* param = &scene->badgeParams[slot];
    s16*           counter;
    s32            temp;

    switch (table->cursor[table->page].digit / 4) {
        case 0:
            counter = &param->friction;
            break;
    }

    temp = *counter;
    func_ov039_02083af4(&temp, step, table->cursor[table->page].digit % 4);

    if (temp >= 0x8000) {
        temp = 0;
    }

    *counter = temp;

    table->blink = 0;
}

/** Draws the pin's bending force. */
void func_ov039_02085770(TinPinSlammer_Scene* scene, s32 slot) {
    OtuEditor* table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    // Deliberately over-declared: only six bytes are written, but the target's
    // stack frame is 0x20 and sizing this to 6 shrinks the frame to 8 bytes.
    char row[0x20];
    s32  value;

    func_ov039_02083a20((OtuTextDigits*)row, scene->badgeParams[slot].friction);

    row[5] = ' ';

    value = table->cursor[table->page].digit;
    if (value >= 0 && (table->blink & 0x20)) {
        row[data_ov039_02098e78[value]] = ' ';
    }
    row[5] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x20, row);
}

/** Steps the pin's needle timings. */
void func_ov039_02085818(TinPinSlammer_Scene* scene, s32 slot, s32 step) {
    OtuEditor*     table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuBadgeParam* param = &scene->badgeParams[slot];
    u16*           counter;
    s32            place;
    s32            value;

    switch (table->cursor[table->page].digit / 3) {
        case 0:
            counter = &param->needleCharge;
            break;
        case 1:
            counter = &param->needleHold;
            break;
    }

    value = *counter;

    switch (table->cursor[table->page].digit % 3) {
        case 0:
            place = 100;
            break;
        case 1:
            place = 10;
            break;
        case 2:
            place = 1;
            break;
    }

    value += place * step;

    if (value < 0) {
        value = 999;
    } else if (value >= 1000) {
        value = 0;
    }

    *counter = value;

    table->blink = 0;
}

/** Draws the pin's needle timings. */
void func_ov039_020858e4(TinPinSlammer_Scene* scene, s32 slot) {
    OtuEditor*     table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuBadgeParam* param = &scene->badgeParams[slot];
    char           row[0x20];
    s32            value;

    value  = param->needleCharge;
    row[0] = '0' + (value / 100) % 10;
    row[1] = '0' + (value / 10) % 10;
    row[2] = '0' + value % 10;
    row[3] = ' ';

    value  = param->needleHold;
    row[4] = '0' + (value / 100) % 10;
    row[5] = '0' + (value / 10) % 10;
    row[6] = '0' + value % 10;
    row[7] = ' ';

    value = table->cursor[table->page].digit;
    if (value >= 0 && (table->blink & 0x20)) {
        row[data_ov039_02098ea8[value]] = ' ';
    }
    row[7] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x28, row);
}

/** Steps the pin's meteo timings. */
void func_ov039_02085a64(TinPinSlammer_Scene* scene, s32 slot, s32 step) {
    OtuEditor*     table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuBadgeParam* param = &scene->badgeParams[slot];
    u16*           counter;
    s32            place;
    s32            value;

    switch (table->cursor[table->page].digit / 3) {
        case 0:
            counter = &param->meteoHold;
            break;
        case 1:
            counter = &param->meteoSquash;
            break;
    }

    value = *counter;

    switch (table->cursor[table->page].digit % 3) {
        case 0:
            place = 100;
            break;
        case 1:
            place = 10;
            break;
        case 2:
            place = 1;
            break;
    }

    value += place * step;

    if (value < 0) {
        value = 999;
    } else if (value >= 1000) {
        value = 0;
    }

    *counter = value;

    table->blink = 0;
}

/** Draws the pin's meteo timings. */
void func_ov039_02085b30(TinPinSlammer_Scene* scene, s32 slot) {
    OtuEditor*     table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuBadgeParam* param = &scene->badgeParams[slot];
    char           row[0x20];
    s32            value;

    value  = param->meteoHold;
    row[0] = '0' + (value / 100) % 10;
    row[1] = '0' + (value / 10) % 10;
    row[2] = '0' + value % 10;
    row[3] = ' ';

    value  = param->meteoSquash;
    row[4] = '0' + (value / 100) % 10;
    row[5] = '0' + (value / 10) % 10;
    row[6] = '0' + value % 10;
    row[7] = ' ';

    value = table->cursor[table->page].digit;
    if (value >= 0 && (table->blink & 0x20)) {
        row[data_ov039_02098ec0[value]] = ' ';
    }
    row[7] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x30, row);
}

/**
 * Steps the pin's hammer parameters: two Q12.12 values, a three-digit frame
 * count and a one-digit arc.
 */
void func_ov039_02085cb0(TinPinSlammer_Scene* scene, s32 slot, s32 step) {
    OtuEditor*     table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuBadgeParam* param = &scene->badgeParams[slot];
    u16*           counter;
    s32            value = table->cursor[table->page].digit;

    // Four counters, four cases each. Written as one switch with the cases
    // falling through, because that is what produces the target's paired
    // jump tables -- one choosing the counter, one choosing the body.
    switch (value) {
        case 0:
        case 1:
        case 2:
        case 3:
            counter = &param->hammerRate;
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            counter = &param->hammerLength;
            break;
        case 8:
        case 9:
        case 10:
            counter = &param->hammerFrames;
            break;
        case 11:
            counter = &param->hammerArc;
            break;
    }

    switch (value) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7: {
            // Stepped through a temporary, as in func_ov039_020856c8.
            s32 temp = *(s16*)counter;

            func_ov039_02083af4(&temp, step, table->cursor[table->page].digit % 4);

            *counter = temp;
            break;
        }
        case 8:
        case 9:
        case 10: {
            // Three places, wrapping at 999, as in func_ov039_02085818.
            s32 place;
            s32 v = *counter;

            switch ((value - 8) % 3) {
                case 0:
                    place = 100;
                    break;
                case 1:
                    place = 10;
                    break;
                case 2:
                    place = 1;
                    break;
            }

            v += place * step;

            if (v < 0) {
                v = 999;
            } else if (v >= 1000) {
                v = 0;
            }

            *counter = v;
            break;
        }
        case 11:
            // The 1-based column: raw step, no weighting, wraps within [1, 9].
            {
                s32 v = *counter + step;

                if (v < 1) {
                    v = 9;
                } else if (v >= 10) {
                    v = 1;
                }

                *counter = v;
                break;
            }
    }

    table->blink = 0;
}

/** Draws the pin's hammer parameters. */
void func_ov039_02085e54(TinPinSlammer_Scene* scene, s32 slot) {
    OtuEditor*     table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuBadgeParam* param = &scene->badgeParams[slot];
    char           row[0x20];
    s32            value;

    func_ov039_02083a20((OtuTextDigits*)(row + 0x00), param->hammerRate);
    row[5] = ' ';
    func_ov039_02083a20((OtuTextDigits*)(row + 0x06), param->hammerLength);
    row[11] = ' ';

    value   = param->hammerFrames;
    row[12] = '0' + (value / 100) % 10;
    row[13] = '0' + (value / 10) % 10;
    row[14] = '0' + value % 10;
    row[15] = ' ';

    row[16] = '0' + param->hammerArc % 10;
    row[17] = ' ';

    value = table->cursor[table->page].digit;
    if (value >= 0 && (table->blink & 0x20)) {
        row[data_ov039_02098f94[value]] = ' ';
    }
    row[17] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x38, row);
}

/** Steps the pin's stun time. */
void func_ov039_02085fb4(TinPinSlammer_Scene* scene, s32 slot, s32 step) {
    OtuEditor*     table = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuBadgeParam* param = &scene->badgeParams[slot];
    u16*           counter;
    s32            place;
    s32            value;

    counter = &param->stunFrames;

    value = *counter;

    switch (table->cursor[table->page].digit % 3) {
        case 0:
            place = 100;
            break;
        case 1:
            place = 10;
            break;
        case 2:
            place = 1;
            break;
    }

    value += place * step;

    if (value < 0) {
        value = 999;
    } else if (value >= 1000) {
        value = 0;
    }

    *counter = value;

    table->blink = 0;
}

/** Draws SINGLE MENU 3's stun-time row. */
void func_ov039_02086060(TinPinSlammer_Scene* scene, s32 slot) {
    OtuEditor* block = func_ov039_02098b70(OTU_STAGE(scene));
    s32        value = scene->badgeParams[slot].stunFrames;
    char       row[0x20];
    s32        column;

    row[0] = '0' + (value / 100) % 10;
    row[1] = '0' + (value / 10) % 10;
    row[2] = '0' + value % 10;
    row[3] = ' ';

    column = block->cursor[block->page].digit;

    if (column >= 0 && (block->blink & 0x20)) {
        row[data_ov039_02098e50[column]] = ' ';
    }

    row[3] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x40, row);
}

/** Redraws the editor's current page: its title, then each row's label and value. */
void func_ov039_02086174(TinPinSlammer_Scene* scene) {
    OtuEditor* block = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));

    func_02010b50(OTU_TEXT(scene));

    switch (block->page) {
        case 0:
            Text_RenderToScreen(OTU_TEXT(scene), 0, 0, Otu_str_0209a178);
            func_ov039_02083bb0(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x08, Otu_str_0209a188);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x10, Otu_str_0209a194);
            func_ov039_02083ed4(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x18, Otu_str_0209a1a0);
            func_ov039_020840c0(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x20, Otu_str_0209a1a8);
            func_ov039_020842bc(scene, 0);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x28, Otu_str_0209a1b0);
            func_ov039_020842bc(scene, 1);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x30, Otu_str_0209a1b8);
            func_ov039_020842bc(scene, 2);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x38, Otu_str_0209a1c0);
            func_ov039_020842bc(scene, 3);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x40, Otu_str_0209a1c8);
            func_ov039_020844f8(scene, 0);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x48, Otu_str_0209a1d0);
            func_ov039_020844f8(scene, 1);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x50, Otu_str_0209a1d8);
            func_ov039_020844f8(scene, 2);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x58, Otu_str_0209a1e0);
            func_ov039_020844f8(scene, 3);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x60, Otu_str_0209a1e8);
            func_ov039_020844f8(scene, 4);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x68, Otu_str_0209a1f0);
            func_ov039_020844f8(scene, 5);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x70, Otu_str_0209a1f8);
            func_ov039_020844f8(scene, 6);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x78, Otu_str_0209a200);
            func_ov039_020844f8(scene, 7);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x80, Otu_str_0209a208);
            func_ov039_020844f8(scene, 8);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x88, Otu_str_0209a210);
            func_ov039_020844f8(scene, 9);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x90, Otu_str_0209a218);
            func_ov039_0208462c(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x98, Otu_str_0209a224);
            func_ov039_02084738(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0xA0, Otu_str_0209a22c);
            func_ov039_02084874(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0xA8, Otu_str_0209a234);
            func_ov039_020849a4(scene);
            return;

        case 1:
            Text_RenderToScreen(OTU_TEXT(scene), 0, 0, Otu_str_0209a23c);
            func_ov039_02083bb0(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x08, Otu_str_0209a24c);
            func_ov039_02084ac4(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x10, Otu_str_0209a258);
            func_ov039_02084c34(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x18, Otu_str_0209a264);
            func_ov039_02084d80(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x20, Otu_str_0209a26c);
            func_ov039_02084f08(scene);
            return;

        case 2:
            Text_RenderToScreen(OTU_TEXT(scene), 0, 0, Otu_str_0209a278);
            func_ov039_02083bb0(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x08, Otu_str_0209a288);
            func_ov039_020851bc(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x10, Otu_str_0209a294);
            func_ov039_02085388(scene, block->pinId);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x18, Otu_str_0209a2a0);
            func_ov039_020855e0(scene, block->pinId);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x20, Otu_str_0209a2a8);
            func_ov039_02085770(scene, block->pinId);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x28, Otu_str_0209a2b4);
            func_ov039_020858e4(scene, block->pinId);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x30, Otu_str_0209a2c0);
            func_ov039_02085b30(scene, block->pinId);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x38, Otu_str_0209a2c8);
            func_ov039_02085e54(scene, block->pinId);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x40, Otu_str_0209a2d4);
            func_ov039_02086060(scene, block->pinId);
            return;

        default:
            return;
    }
}

/**
 * The editor stage's enter: sets up the display and the text layer, resets
 * the cursors and draws the first page.
 */
void func_ov039_02086728(TinPinSlammer_Scene* scene) {
    // Never written; func_0203a96c is only handed its address.
    char       pad[0x8];
    OtuEditor* block = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    s32        i;

    Display_SetMainLayers(0);
    Display_SetSubLayers(0x11);

    g_DisplaySettings.engineState[1].blendMode   = 0;
    g_DisplaySettings.engineState[1].blendLayer0 = 0;
    g_DisplaySettings.engineState[1].blendLayer1 = 0x20;

    block->page = 0;

    for (i = 0; i < 3; i++) {
        block->cursor[i].digit      = -1;
        block->cursor[i].drawnDigit = -1;
        block->cursor[i].row        = 0;
        block->cursor[i].drawnRow   = 1;
    }

    block->blink = 0;
    block->pinId = 0;

    func_ov039_02082520(scene);
    func_ov039_02086174(scene);
    func_0203a96c(pad);
    EasyFade_FadeBothDisplays(1, 0, 0x1000);
}

/** The editor stage's exit: releases its text layer and fades out. */
void func_ov039_020867d4(TinPinSlammer_Scene* scene) {
    func_ov039_02098b70(OTU_STAGE(scene));

    func_ov039_020825b0(scene);
    EasyFade_FadeBothDisplays(2, 0x10, 0x1000);
}

/**
 * The editor's per-frame input. Up and down move the cursor between rows, or
 * step the selected digit once one is selected; left and right move between
 * digits; A selects the row (or, on SINGLE MENU 1's first row, starts a match)
 * and B deselects it (or leaves); L and R turn the page. Then redraws the
 * cursor and the current row, and advances the blink.
 */
void func_ov039_02086808(TinPinSlammer_Scene* scene) {
    OtuEditor* block = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    s32        step;

    switch (SysControl.buttonState.pressedButtons) {
        case 0x40:

            if (block->cursor[block->page].digit < 0) {
                // No digit selected: move the cursor up a row, wrapping to the last.
                switch (block->page) {
                    case 0:
                        block->cursor[block->page].row -= 1;
                        if (block->cursor[block->page].row < 0) {
                            block->cursor[block->page].row = 0x14;
                        }
                        break;
                    case 1:
                        block->cursor[block->page].row -= 1;
                        if (block->cursor[block->page].row < 0) {
                            block->cursor[block->page].row = 0x3;
                        }
                        break;
                    case 2:
                        block->cursor[block->page].row -= 1;
                        if (block->cursor[block->page].row < 0) {
                            block->cursor[block->page].row = 0x7;
                        }
                        break;
                }
            } else {
                // A digit is selected: step it up.
                switch (block->page) {
                    case 0:
                        step = block->cursor[block->page].row;
                        switch (step) {
                            default:
                                break;
                            case 1:
                                func_ov039_02083cbc(scene, 1);
                                break;
                            case 2:
                                func_ov039_02084020(scene, 1);
                                break;
                            case 3:
                            case 4:
                            case 5:
                            case 6:
                                func_ov039_020841a0(scene, step - 3, 1);
                                break;
                            case 7:
                            case 8:
                            case 9:
                            case 10:
                            case 11:
                            case 12:
                            case 13:
                            case 14:
                            case 15:
                            case 16:
                                func_ov039_02084458(scene, step - 7, 1);
                                break;
                            case 17:
                                func_ov039_020845dc(scene, 1);
                                break;
                            case 18:
                                func_ov039_020846c4(scene, 1);
                                break;
                            case 19:
                                func_ov039_020847ec(scene, 1);
                                break;
                            case 20:
                                func_ov039_02084944(scene, 1);
                                break;
                        }
                        break;
                    case 1:
                        step = block->cursor[block->page].row;
                        switch (step) {
                            default:
                                break;
                            case 0:
                                func_ov039_02084a3c(scene, 1);
                                break;
                            case 1:
                                func_ov039_02084b94(scene, 1);
                                break;
                            case 2:
                                func_ov039_02084d20(scene, 1);
                                break;
                            case 3:
                                func_ov039_02084e1c(scene, 1);
                                break;
                        }
                        break;
                    case 2:
                        step = block->cursor[block->page].row;
                        switch (step) {
                            default:
                                break;
                            case 0:
                                func_ov039_02085124(scene, 1);
                                func_ov039_02085388(scene, block->pinId);
                                func_ov039_020855e0(scene, block->pinId);
                                func_ov039_02085770(scene, block->pinId);
                                func_ov039_020858e4(scene, block->pinId);
                                func_ov039_02085b30(scene, block->pinId);
                                func_ov039_02085e54(scene, block->pinId);
                                func_ov039_02086060(scene, block->pinId);
                                break;
                            case 1:
                                func_ov039_020852c0(scene, block->pinId, 1);
                                break;
                            case 2:
                                func_ov039_0208554c(scene, block->pinId, 1);
                                break;
                            case 3:
                                func_ov039_020856c8(scene, block->pinId, 1);
                                break;
                            case 4:
                                func_ov039_02085818(scene, block->pinId, 1);
                                break;
                            case 5:
                                func_ov039_02085a64(scene, block->pinId, 1);
                                break;
                            case 6:
                                func_ov039_02085cb0(scene, block->pinId, 1);
                                break;
                            case 7:
                                func_ov039_02085fb4(scene, block->pinId, 1);
                                break;
                        }
                        break;
                }
            }
            break;

        case 0x80:

            if (block->cursor[block->page].digit < 0) {
                // No digit selected: move the cursor down a row, wrapping to the first.
                switch (block->page) {
                    case 0:
                        block->cursor[block->page].row += 1;
                        if (block->cursor[block->page].row >= 21) {
                            block->cursor[block->page].row = 0;
                        }
                        break;
                    case 1:
                        block->cursor[block->page].row += 1;
                        if (block->cursor[block->page].row >= 4) {
                            block->cursor[block->page].row = 0;
                        }
                        break;
                    case 2:
                        block->cursor[block->page].row += 1;
                        if (block->cursor[block->page].row >= 8) {
                            block->cursor[block->page].row = 0;
                        }
                        break;
                }
            } else {
                // A digit is selected: step it down.
                switch (block->page) {
                    case 0:
                        step = block->cursor[block->page].row;
                        switch (step) {
                            default:
                                break;
                            case 1:
                                func_ov039_02083cbc(scene, -1);
                                break;
                            case 2:
                                func_ov039_02084020(scene, -1);
                                break;
                            case 3:
                            case 4:
                            case 5:
                            case 6:
                                func_ov039_020841a0(scene, step - 3, -1);
                                break;
                            case 7:
                            case 8:
                            case 9:
                            case 10:
                            case 11:
                            case 12:
                            case 13:
                            case 14:
                            case 15:
                            case 16:
                                func_ov039_02084458(scene, step - 7, -1);
                                break;
                            case 17:
                                func_ov039_020845dc(scene, -1);
                                break;
                            case 18:
                                func_ov039_020846c4(scene, -1);
                                break;
                            case 19:
                                func_ov039_020847ec(scene, -1);
                                break;
                            case 20:
                                func_ov039_02084944(scene, -1);
                                break;
                        }
                        break;
                    case 1:
                        step = block->cursor[block->page].row;
                        switch (step) {
                            default:
                                break;
                            case 0:
                                func_ov039_02084a3c(scene, -1);
                                break;
                            case 1:
                                func_ov039_02084b94(scene, -1);
                                break;
                            case 2:
                                func_ov039_02084d20(scene, -1);
                                break;
                            case 3:
                                func_ov039_02084e1c(scene, -1);
                                break;
                        }
                        break;
                    case 2:
                        step = block->cursor[block->page].row;
                        switch (step) {
                            default:
                                break;
                            case 0:
                                func_ov039_02085124(scene, -1);
                                func_ov039_02085388(scene, block->pinId);
                                func_ov039_020855e0(scene, block->pinId);
                                func_ov039_02085770(scene, block->pinId);
                                func_ov039_020858e4(scene, block->pinId);
                                func_ov039_02085b30(scene, block->pinId);
                                func_ov039_02085e54(scene, block->pinId);
                                func_ov039_02086060(scene, block->pinId);
                                break;
                            case 1:
                                func_ov039_020852c0(scene, block->pinId, -1);
                                break;
                            case 2:
                                func_ov039_0208554c(scene, block->pinId, -1);
                                break;
                            case 3:
                                func_ov039_020856c8(scene, block->pinId, -1);
                                break;
                            case 4:
                                func_ov039_02085818(scene, block->pinId, -1);
                                break;
                            case 5:
                                func_ov039_02085a64(scene, block->pinId, -1);
                                break;
                            case 6:
                                func_ov039_02085cb0(scene, block->pinId, -1);
                                break;
                            case 7:
                                func_ov039_02085fb4(scene, block->pinId, -1);
                                break;
                        }
                        break;
                }
            }
            break;

        case 0x20:
            if (block->cursor[block->page].digit >= 0) {
                // Move the cursor one digit left, wrapping to the row's last digit.
                switch (block->page) {
                    case 0:
                        step = block->cursor[block->page].row;
                        switch (step) {
                            default:
                                break;
                            case 1:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 8;
                                }
                                break;
                            case 2:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 15;
                                }
                                break;
                            case 3:
                            case 4:
                            case 5:
                            case 6:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 17;
                                }
                                break;
                            case 7:
                            case 8:
                            case 9:
                            case 10:
                            case 11:
                            case 12:
                            case 13:
                            case 14:
                            case 15:
                            case 16:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 15;
                                }
                                break;
                            case 17:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 3;
                                }
                                break;
                            case 18:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 7;
                                }
                                // fallthrough
                            case 19:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 11;
                                }
                                // fallthrough
                            case 20:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 3;
                                }
                                break;
                        }
                        break;
                    case 1:
                        step = block->cursor[block->page].row;
                        switch (step) {
                            default:
                                break;
                            case 0:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 11;
                                }
                                break;
                            case 1:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 15;
                                }
                                break;
                            case 2:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 3;
                                }
                                break;
                            case 3:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 9;
                                }
                                break;
                        }
                        break;
                    case 2:
                        step = block->cursor[block->page].row;
                        switch (step) {
                            default:
                                break;
                            case 0:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 2;
                                }
                                break;
                            case 1:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 7;
                                }
                                break;
                            case 2:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 1;
                                }
                                break;
                            case 3:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 3;
                                }
                                break;
                            case 4:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 5;
                                }
                                break;
                            case 5:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 5;
                                }
                                break;
                            case 6:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 11;
                                }
                                break;
                            case 7:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 2;
                                }
                                break;
                        }
                        break;
                }
            }
            break;

        case 0x10:
            if (block->cursor[block->page].digit >= 0) {
                // Move the cursor one digit right, wrapping to the row's first digit.
                switch (block->page) {
                    case 0:
                        step = block->cursor[block->page].row;
                        switch (step) {
                            default:
                                break;
                            case 1:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 9) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 2:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 16) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 3:
                            case 4:
                            case 5:
                            case 6:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 18) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 7:
                            case 8:
                            case 9:
                            case 10:
                            case 11:
                            case 12:
                            case 13:
                            case 14:
                            case 15:
                            case 16:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 16) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 17:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 4) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 18:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 8) {
                                    block->cursor[block->page].digit = 0;
                                }
                                // fallthrough
                            case 19:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 12) {
                                    block->cursor[block->page].digit = 0;
                                }
                                // fallthrough
                            case 20:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 4) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                        }
                        break;
                    case 1:
                        step = block->cursor[block->page].row;
                        switch (step) {
                            default:
                                break;
                            case 0:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 12) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 1:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 16) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 2:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 4) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 3:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 10) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                        }
                        break;
                    case 2:
                        step = block->cursor[block->page].row;
                        switch (step) {
                            default:
                                break;
                            case 0:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 3) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 1:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 8) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 2:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 2) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 3:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 4) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 4:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 6) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 5:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 6) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 6:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 12) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 7:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 3) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                        }
                        break;
                }
            }
            break;

        case 0x1:
            // Select the row's first digit; on SINGLE MENU 1's first row, start a match.

            if (block->cursor[block->page].digit < 0) {
                if (block->page == 0 && block->cursor[block->page].row == 0) {
                    func_ov039_02098a40(OTU_STAGE(scene), &OtuScene_WirelessBoard);
                } else {
                    block->cursor[block->page].digit = 0;
                }
            }
            break;

        case 0x2:
            // Deselect the digit, or leave the editor if none is selected.

            if (block->cursor[block->page].digit >= 0) {
                block->cursor[block->page].digit = -1;
            } else {
                func_ov039_02098a40(OTU_STAGE(scene), NULL);
            }
            break;

        case 0x200:
            // Previous page, fully redrawn.
            block->page -= 1;

            if (block->page < 0) {
                block->page = 2;
            }

            block->blink                          = 0;
            block->cursor[block->page].drawnDigit = -2;
            block->cursor[block->page].drawnRow   = -2;
            func_ov039_02086174(scene);
            break;

        case 0x100:
            // Next page, fully redrawn.
            block->page += 1;

            if (block->page >= 3) {
                block->page = 0;
            }

            block->blink                          = 0;
            block->cursor[block->page].drawnDigit = -2;
            block->cursor[block->page].drawnRow   = -2;
            func_ov039_02086174(scene);
            break;

        default:
            break;
    }

    func_ov039_02083bb0(scene);

    switch (block->page) {
        case 0:
            step = block->cursor[block->page].row;
            switch (step) {
                default:
                    break;
                case 1:
                    func_ov039_02083ed4(scene);
                    break;
                case 2:
                    func_ov039_020840c0(scene);
                    break;
                case 3:
                case 4:
                case 5:
                case 6:
                    func_ov039_020842bc(scene, step - 3);
                    break;
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                    func_ov039_020844f8(scene, step - 7);
                    break;
                case 17:
                    func_ov039_0208462c(scene);
                    break;
                case 18:
                    func_ov039_02084738(scene);
                    break;
                case 19:
                    func_ov039_02084874(scene);
                    break;
                case 20:
                    func_ov039_020849a4(scene);
                    break;
            }
            break;
        case 1:
            step = block->cursor[block->page].row;
            switch (step) {
                default:
                    break;
                case 0:
                    func_ov039_02084ac4(scene);
                    break;
                case 1:
                    func_ov039_02084c34(scene);
                    break;
                case 2:
                    func_ov039_02084d80(scene);
                    break;
                case 3:
                    func_ov039_02084f08(scene);
                    break;
            }
            break;
        case 2:
            step = block->cursor[block->page].row;
            switch (step) {
                default:
                    break;
                case 0:
                    func_ov039_020851bc(scene);
                    break;
                case 1:
                    func_ov039_02085388(scene, block->pinId);
                    break;
                case 2:
                    func_ov039_020855e0(scene, block->pinId);
                    break;
                case 3:
                    func_ov039_02085770(scene, block->pinId);
                    break;
                case 4:
                    func_ov039_020858e4(scene, block->pinId);
                    break;
                case 5:
                    func_ov039_02085b30(scene, block->pinId);
                    break;
                case 6:
                    func_ov039_02085e54(scene, block->pinId);
                    break;
                case 7:
                    func_ov039_02086060(scene, block->pinId);
                    break;
            }
            break;
    }

    block->blink++;
}
