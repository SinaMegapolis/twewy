#include "Debug/Sugata/TinPinSlammer.h"
#include "Engine/Text.h"

/**
 * @file OtuScoreRow.c
 * @brief The debug editor's rows (see OtuEditor): a draw function and a stepper
 *        for each row of SINGLE MENU 1 to 3.
 *
 * A row is drawn as fixed-width digit groups, and the selected digit blinks:
 * when `blink & 0x20` the character at the cursor's column is blanked. Steppers
 * move the selected digit's place by `step` (+1 or -1), wrap the value within
 * the row's range, and restart the blink so the edited digit shows.
 *
 * Unless noted, each value is Q12.12 and drawn as `d.ddd`; SINGLE MENU 1 and 2
 * edit the overlay's tuning constants (listed in TinPinSlammer.h), SINGLE
 * MENU 3 the OtuBadgeParam record of one pin.
 */

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
