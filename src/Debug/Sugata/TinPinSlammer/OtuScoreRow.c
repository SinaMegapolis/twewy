#include "Debug/Sugata/TinPinSlammer.h"
#include "Engine/Text.h"

/**
 * @file OtuScoreRow.c
 * @brief The result screens' three-digit number rows.
 *
 * The Otosu result screens show the pin tray and each menu's score as rows of
 * zero-padded three-digit fields. This is the code that formats one such row:
 * pick the source array, expand six halfwords into digits, blank whichever
 * column the stage has hidden, and hand the string to the text renderer.
 */

/**
 * @brief Step one of the four shared countdown anchors by a decimal place.
 *
 * The single-group counterpart to func_ov039_02084458: there is no slot block
 * to index, so `value / 4` picks between four separate anchor objects rather
 * than four words one block apart.  `step` is +1 or -1.
 *
 * `value` packs both halves of the address: the quotient picks which of the
 * four anchors is being animated and the remainder is the decimal place within
 * it, so the call below passes `value % 4` and func_ov039_02083af4's own
 * `place % 4` is redundant for values that came from here.
 */
void func_ov039_02084020(TinPinSlammer_Scene* scene, s32 step) {
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    s32*        anchor;
    s32         value = OTU_TEXT_ROW(table, table->index)->value;

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

    // The remainder, not the quotient: the target's rotate-based sequence
    // (`lsr #31`, `rsb ... lsl #0x1e`, `add ... ror #0x1e`) is mwcc's expansion
    // of `% 4` for an unknown sign, and it only appears when this is spelled as
    // a modulus rather than as a second division.
    func_ov039_02083af4(anchor, step, OTU_TEXT_ROW(table, table->index)->value % 4);

    table->flags = 0;
}

/**
 * @brief Draw the four-anchor row: one `d.ddd` group per anchor.
 *
 * The same body as func_ov039_020844f8 with the shared countdown block replaced
 * by the four standalone anchors above -- which is why it sits at y = 0x18,
 * the first of the result screen's fixed-point rows.
 */
void func_ov039_020840c0(TinPinSlammer_Scene* scene) {
    char        row[0x20];
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    s32*        src   = &data_ov039_0209a39c[0];
    s32         value;

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

    value = OTU_TEXT_ROW(table, table->index)->value;
    if (value >= 0 && (table->flags & 0x20)) {
        row[data_ov039_02099034[value]] = ' ';
    }
    row[0x17] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x18, row);
}

/**
 * @brief Step one of the pin-tray / score-row slots by a decimal place.
 *
 * The stepper for the rows func_ov039_020842bc draws.  `which` selects the row
 * the same way that draw call does -- 0 is the pin tray, 1 through 3 the three
 * menus' score rows -- and `value / 3` is both the slot within that row and the
 * digit within the slot, so `value % 3` picks the place (100, 10 or 1) to move
 * it by.
 *
 * The sentinel handling is the same as the draw: 0x130 reads as an empty slot,
 * and the written-back count wraps just below it -- below zero becomes 0x131,
 * 0x132 and above becomes zero, and a slot that lands on zero becomes the
 * 0x130 sentinel rather than the digit 0.
 */
// Nonmatching: 88.0%. The `which` switch and its four-way jump table, both
// address computations, the row read, the sentinel handling, the three-way
// place branch (out-of-line block for == 0, a second for == 1, a conditional
// move for == 2), both clamps and the trailing flag clear all match.
//
// The gap is the divide. The target divides `value` by 3 twice -- once for the
// slot index and once for the remainder -- and mwcc folds the two into a single
// `smull` here, so the byte offset lands in a different register, the second
// divide and its two helpers are missing, and the offset is recomputed at the
// store instead of being carried. There is no spelling that avoids this: every
// arrangement of `value / 3` and `value % 3` -- one expression, two locals, or
// split across a branch -- compiles to one divide and one multiply-by-3
// subtract. (`value % 3` on its own does match; see func_ov039_02085124.) So
// this is a codegen limit rather than a spelling.
void func_ov039_020841a0(TinPinSlammer_Scene* scene, s32 which, s32 step) {
    u16*        row;
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    s32         value;
    s32         place;
    s32         v;

    // Same source selection, and the same missing `default`, as the row draw.
    switch (which) {
        case 0:
            row = OTU_PIN_TRAY(scene, which);
            break;
        case 1:
        case 2:
        case 3:
            row = OTU_SCORE_ROW(scene, which - 1);
            break;
    }

    value = OTU_TEXT_ROW(table, table->index)->value;

    // The target keeps the quotient's byte offset in a register from here to the
    // store, and divides a second time for the remainder, so the slot index is a
    // local of its own rather than a third spelling of `value / 3`.
    {
        s32 slot = value / 3;

        v = row[slot];

        if (v == 0x130) {
            v = 0;
        } else {
            v = v + 1;
        }

        // Three places by `value % 3`, and a switch rather than a compare chain
        // because that is the shape the target has: the == 0 and == 1 arms each
        // branch out to their own block and == 2 is the conditional-move arm.
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
            v = 0x131;
        } else {
            if (v >= 0x132) {
                v = 0;
            }
            v = (v == 0) ? 0x130 : v - 1;
        }

        row[slot] = v;
    }

    table->flags = 0;
}

/**
 * @brief Draw one three-digit-per-slot number row.
 *
 * `which` selects the row: 0 is the pin tray, 1 through 3 are the three menus'
 * score rows.  Six slots are read from the selected source and each becomes a
 * three-digit field followed by a space, so the row is 24 characters; the
 * terminator goes at index 0x18.
 *
 * Slot value 0x130 is the "no pin" sentinel and renders as 000.
 *
 * When the stage reports its current countdown column (`value >= 0`) and sets
 * the 0x20 hide flag, the character at that column's offset is blanked, which is
 * how a hidden column keeps the row's shape without a digit in it.
 */
// Nonmatching: 99.95%, which is the ceiling here -- the single remaining
// difference is one constant-pool `.word` symbol name. Every instruction,
// field access, branch and the size match exactly.
//
// Three things had to be written the way they are, each of which is worth
// keeping if this function is revisited:
//
//   * `row` is `char[0x1D]`, not `char[0x18]`. Only 0x19 bytes are ever
//     touched (six 4-byte groups plus a terminator at 0x18), but declaring the
//     exact size makes mwcc split the frame 8 bytes differently -- saving r3
//     as well -- which costs four instructions of prologue and epilogue.
//   * the digit expansion is open-coded rather than a call to
//     `func_ov039_02083974`: that helper walks backwards from a digit count,
//     while the target writes three digits forwards and open-codes three
//     divisions per slot.
//   * the source pointer is advanced at the *end* of the loop body. Written as
//     `*src++`, mwcc fuses the load and the increment into a post-indexed
//     `ldrh r1, [r4], #2` and drops an instruction the target keeps.
void func_ov039_020842bc(TinPinSlammer_Scene* scene, s32 which) {
    char        row[0x1D];
    u16*        src;
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    s32         value;
    s32         i;

    // No `default`: the target's bounds check sends anything past 3 straight to
    // the formatting loop with the source pointer left as case 0 left it, so the
    // out-of-range case is not a separate assignment.
    switch (which) {
        case 0:
            src = OTU_PIN_TRAY(scene, which);
            break;
        case 1:
        case 2:
        case 3:
            src = OTU_SCORE_ROW(scene, which - 1);
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
        src = (u16*)((u8*)src + 2);
    }

    value = OTU_TEXT_ROW(table, table->index)->value;
    if (value >= 0 && (table->flags & 0x20)) {
        row[data_ov039_020990b4[value]] = ' ';
    }
    row[0x18] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, which * 8 + 0x20, row);
}

/**
 * @brief Step one of a menu's four countdown columns by a decimal place.
 *
 * `value / 4` selects which of the four coordinates in the slot's block is
 * being animated, so the four columns are the thousands, hundreds, tens and
 * ones of one number, and `value % 4` is the place within it that
 * func_ov039_02083af4 is told to move.  `step` is +1 or -1, and both the divide
 * and the modulus truncate toward zero -- which is why the target spells the
 * divide out as a shift-plus-sign-fixup and the modulus as a rotate.
 *
 * Clears the table's flags afterwards, so the hide-column check in the row
 * builders re-evaluates from scratch on the next frame.
 */
// Nonmatching: 99.88%, which is its ceiling here -- the residual is one
// constant-pool `.word` symbol name and nothing else, the same one that keeps
// func_ov039_020844f8 at 99.91%. The target's countdown base is an unnamed
// delinker symbol, `data_ov039_0209a3dc`; this build reaches the same address
// through the named `OtuText_Shadow`, which is what OTU_COUNTDOWN_COLUMN spells.
void func_ov039_02084458(TinPinSlammer_Scene* scene, s32 slot, s32 step) {
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    s32*        base  = OTU_COUNTDOWN_COLUMN(slot);
    s32*        column;
    s32         value = OTU_TEXT_ROW(table, table->index)->value;

    // The slot base is formed once, before the switch; only the four-way
    // selection of a word within it is branched on.  Case 0 assigns the base
    // to itself rather than falling through, which is what produces the target's
    // redundant `mov` in that arm.
    switch (value / 4) {
        case 0:
            column = base;
            break;
        case 1:
            column = base + 1;
            break;
        case 2:
            column = base + 2;
            break;
        case 3:
            column = base + 3;
            break;
    }

    // The remainder, not the quotient -- see func_ov039_02084020, which is this
    // function for the shared anchors and matches once the modulus is spelled
    // as a modulus.
    func_ov039_02083af4(column, step, OTU_TEXT_ROW(table, table->index)->value % 4);

    table->flags = 0;
}

/**
 * @brief Draw one menu's fixed-point row: four `d.ddd` groups.
 *
 * The counterpart to func_ov039_020842bc for the rows that show a Q12.12
 * coordinate rather than a pin count. Four six-byte groups tile the buffer at
 * 0x0, 0x6, 0xC and 0x12, and the last group's trailing space doubles as the
 * terminator -- the target overwrites it with a NUL rather than reserving a
 * separate byte.
 */
// Nonmatching: 99.82%, which is its ceiling here -- the residual is two
// constant-pool `.word` symbol names and nothing else.
//
// `row` is declared `char[0x20]` although only 0x18 bytes are used, and the
// trailing space at 0x17 is written *before* the blanking check and then
// overwritten with the terminator *after* it. Both are deliberate: the exact
// buffer size makes mwcc split the frame differently and save r3, and writing
// the two stores adjacently lets it delete the first as dead.
void func_ov039_020844f8(TinPinSlammer_Scene* scene, s32 slot) {
    char        row[0x20];
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    s32*        src   = OTU_COUNTDOWN_COLUMN(slot);
    s32         value;

    func_ov039_02083a20((OtuTextDigits*)&row[0x0], src[0]);
    row[0x5] = ' ';
    func_ov039_02083a20((OtuTextDigits*)&row[0x6], src[1]);
    row[0xB] = ' ';
    func_ov039_02083a20((OtuTextDigits*)&row[0xC], src[2]);
    row[0x11] = ' ';
    func_ov039_02083a20((OtuTextDigits*)&row[0x12], src[3]);
    row[0x17] = ' ';

    value = OTU_TEXT_ROW(table, table->index)->value;
    if (value >= 0 && (table->flags & 0x20)) {
        row[data_ov039_02099074[value]] = ' ';
    }
    row[0x17] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, slot * 8 + 0x40, row);
}

/**
 * @brief Step the shared tight x-coordinate by a decimal place.
 *
 * The single-column counterpart to func_ov039_02084458: no slot, no switch, and
 * the column is the one global rather than a slot-relative block.
 */
// Nonmatching: 89.4%. Everything matches -- the two loads, the argument setup,
// the call and the flag clear -- except the divide. The target truncates
// `value / 4` with the rotate-based correction (`lsr #31`, `rsb`<<30, `ror`),
// while this build emits the shift-based one (`asr #1`, `add`>>30, `asr #2`).
// Both are three instructions and both round toward zero; mwcc simply picks
// between the two idioms itself, and casting the operand either side of the
// division (including to the parameter's own `u32`) changes which one it picks
// without ever producing the target's.
void func_ov039_020845dc(TinPinSlammer_Scene* scene, s32 step) {
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));

    // The row pointer is cached in a local here, against this overlay's usual
    // habit of re-deriving it. Found by decomp-permuter, and it is what tips
    // mwcc onto the rotate-based divide the target uses for a call argument.
    // No cached row pointer here, unlike the rest of the overlay: decomp-permuter
    // found that caching one improves its own byte score (615 -> 425) but not
    // objdiff's, and its remaining suggestion -- dividing the value as unsigned
    // -- reaches 89.35% by replacing the target's three-instruction signed
    // divide with a single `lsr #2`. Neither beats the plain form, so this
    // stays as the straightforward version.
    func_ov039_02083af4(&OtuText_XTight, step, OTU_TEXT_ROW(table, table->index)->value / 4);

    table->flags = 0;
}

/**
 * @brief Draw the single `d.ddd` value row.
 *
 * The narrowest row in the set: one fixed-point group at the shared tight x
 * coordinate, positioned well down the screen at y=0x90. Same blanking rule as
 * the others -- the stage names a column and sets the 0x20 flag, and that
 * character is blanked.
 */
// Nonmatching: 99.74%, which is its ceiling here -- two constant-pool `.word`
// symbol names and nothing else. Note there is deliberately no explicit space
// store after the group: the target relies on func_ov039_02083a20's own
// trailing space and writes only the NUL terminator, and adding a redundant
// `row[5] = ' '` costs a register (91.6% -> 99.7%).
void func_ov039_0208462c(TinPinSlammer_Scene* scene) {
    char        row[0x20];
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    s32         value;

    func_ov039_02083a20((OtuTextDigits*)&row[0x0], OtuText_XTight);

    value = OTU_TEXT_ROW(table, table->index)->value;
    if (value >= 0 && (table->flags & 0x20)) {
        row[data_ov039_02098e88[value]] = ' ';
    }
    row[0x5] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x90, row);
}

/**
 * @brief Step one of two shared y-coordinates by a decimal place.
 *
 * `value / 4` picks between them: zero steps the underflow coordinate, one the
 * overflow coordinate, anything above leaves both alone. Both live in the same
 * anchor block as `OtuText_XTight`, which is what func_ov039_020845dc steps
 * unconditionally.
 *
 * The select is `== 0` / `== 1` rather than a switch, so there is no jump
 * table -- unlike func_ov039_02084458's four-way one.
 */
// Nonmatching: 76.0%. The divide, both comparisons, the two coordinate
// addresses, the call and the flag clear are all right, and the size matches.
// Two gaps, both the same as the other steppers plus one new:
//
//   * the divide for the call argument, as everywhere else in this family;
//   * mwcc if-converts the `== 1` test into a conditional load hoisted above
//     the `beq`, where the target keeps the two branches separate and loads
//     the default inside the `== 0` arm. Hoisting the quotient into a local, or
//     reordering the tests, does not change it.
void func_ov039_020846c4(TinPinSlammer_Scene* scene, s32 step) {
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    s32*        column;
    s32         value = OTU_TEXT_ROW(table, table->index)->value;

    if (value / 4 == 0) {
        column = &OtuText_YOverflow;
    } else if (value / 4 == 1) {
        column = &data_ov039_0209a38c;
    }

    func_ov039_02083af4(column, step, OTU_TEXT_ROW(table, table->index)->value / 4);

    table->flags = 0;
}

// Nonmatching: 99.89%, which is its ceiling -- one constant-pool `.word` symbol
// name. Note the deliberate absence of a trailing space store: the target emits
// only the NUL terminator after the last group, and writing the space as well
// costs a register (93.0%).
void func_ov039_02084738(TinPinSlammer_Scene* scene) {
    char        row[0x20];
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    s32         value;

    func_ov039_02083a20((OtuTextDigits*)&row[0x0], data_ov039_0209a38c);
    row[0x5] = ' ';
    func_ov039_02083a20((OtuTextDigits*)&row[0x6], OtuText_YOverflow);

    value = OTU_TEXT_ROW(table, table->index)->value;
    if (value >= 0 && (table->flags & 0x20)) {
        row[data_ov039_02098ef8[value]] = ' ';
    }
    row[0xB] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x98, row);
}

// Nonmatching: 99.71%, ceiling -- one pool symbol name. No trailing space
// store, as above.
void func_ov039_02084874(TinPinSlammer_Scene* scene) {
    char        row[0x20];
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    s32         value;

    func_ov039_02083a20((OtuTextDigits*)&row[0x0], OtuText_YUnderflow);
    row[0x5] = ' ';
    func_ov039_02083a20((OtuTextDigits*)&row[0x6], OtuText_XWide);
    row[0xB] = ' ';
    func_ov039_02083a20((OtuTextDigits*)&row[0xC], OtuText_YTiny);
    value = OTU_TEXT_ROW(table, table->index)->value;
    if (value >= 0 && (table->flags & 0x20)) {
        row[data_ov039_02098fc4[value]] = ' ';
    }
    row[0x11] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0xA0, row);
}

// Nonmatching: 100% -- objdiff reports MATCH.
void func_ov039_020849a4(TinPinSlammer_Scene* scene) {
    char        row[0x20];
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    s32         value;

    func_ov039_02083a20((OtuTextDigits*)&row[0x0], data_ov039_0209a388);

    value = OTU_TEXT_ROW(table, table->index)->value;
    if (value >= 0 && (table->flags & 0x20)) {
        row[data_ov039_02098e98[value]] = ' ';
    }
    row[0x5] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0xA8, row);
}

// Nonmatching: 99.71%, ceiling -- one pool symbol name.
void func_ov039_02084ac4(TinPinSlammer_Scene* scene) {
    char        row[0x20];
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    s32         value;

    func_ov039_02083a20((OtuTextDigits*)&row[0x0], OtuText_YFar);
    row[0x5] = ' ';
    func_ov039_02083a20((OtuTextDigits*)&row[0x6], OtuText_YMid);
    row[0xB] = ' ';
    func_ov039_02083a20((OtuTextDigits*)&row[0xC], OtuText_YTight);
    value = OTU_TEXT_ROW(table, table->index)->value;
    if (value >= 0 && (table->flags & 0x20)) {
        row[data_ov039_02098f64[value]] = ' ';
    }
    row[0x11] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x8, row);
}

// Nonmatching: 99.66%, ceiling -- pool symbol names.
void func_ov039_02084c34(TinPinSlammer_Scene* scene) {
    char        row[0x20];
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    s32         value;

    func_ov039_02083a20((OtuTextDigits*)&row[0x0], OtuText_XFar);
    row[0x5] = ' ';
    func_ov039_02083a20((OtuTextDigits*)&row[0x6], OtuText_XMid);
    row[0xB] = ' ';
    func_ov039_02083a20((OtuTextDigits*)&row[0xC], OtuText_YSmall);
    row[0x11] = ' ';
    func_ov039_02083a20((OtuTextDigits*)&row[0x12], OtuText_XNear);
    value = OTU_TEXT_ROW(table, table->index)->value;
    if (value >= 0 && (table->flags & 0x20)) {
        row[data_ov039_02098ff4[value]] = ' ';
    }
    row[0x17] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x10, row);
}

// Nonmatching: 100% -- objdiff reports MATCH. This one *does* write the
// trailing space before the terminator, unlike func_ov039_02084738 two
// functions above; the two shapes are not interchangeable.
void func_ov039_02084d80(TinPinSlammer_Scene* scene) {
    char        row[0x20];
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    s32         value;

    func_ov039_02083a20((OtuTextDigits*)&row[0x0], data_ov039_0209a398);
    row[0x5] = ' ';

    value = OTU_TEXT_ROW(table, table->index)->value;
    if (value >= 0 && (table->flags & 0x20)) {
        row[data_ov039_02098e68[value]] = ' ';
    }
    row[0x5] = 0;
    row[0x5] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x18, row);
}

/*
 * The steppers. All four below share a shape and differ only in which
 * coordinates they reach and how they select between them.
 */

// Nonmatching: 74.9%. The three tests, all three coordinate addresses, the call
// and the flag clear are right and the size matches. Two gaps: the divide form
// as everywhere in this family, and mwcc emitting the three == tests as a
// flat chain of conditional loads where the target branches to a block for
// == 0 and to a second for == 1. Reordering the arms to put the fall-through
// case last gets closest but never reaches the target's shape.
void func_ov039_020847ec(TinPinSlammer_Scene* scene, s32 step) {
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    s32*        column;
    s32         value = OTU_TEXT_ROW(table, table->index)->value;

    // Ordered so the `== 0` test is the branch that jumps to a block of its own,
    // the `== 1` test the one that jumps past it, and `== 2` the conditional
    // load that falls through. That is the shape the target emits; a flat
    // if/else-if chain of the same tests produces a different one.
    if (value / 4 == 2) {
        column = &OtuText_YTiny;
    } else if (value / 4 == 1) {
        column = &OtuText_XWide;
    } else if (value / 4 == 0) {
        column = &OtuText_YUnderflow;
    }

    func_ov039_02083af4(column, step, OTU_TEXT_ROW(table, table->index)->value / 4);

    table->flags = 0;
}

// Nonmatching: 84.3%. The bare ldreq with no lse arm is the load-bearing
// detail: defaulting the column to a fixed address instead adds a movne the
// target does not have (72.5%). What remains is the divide form and one
// register-allocation difference.
void func_ov039_02084944(TinPinSlammer_Scene* scene, s32 step) {
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    s32*        column;

    // No `else`: the target emits a bare `ldreq` and leaves the column
    // uninitialised when the quotient is non-zero, so this must not default it
    // to a fixed address either -- doing so costs a `movne` the target lacks.
    if (OTU_TEXT_ROW(table, table->index)->value / 4 == 0) {
        column = &data_ov039_0209a388;
    }

    func_ov039_02083af4(column, step, OTU_TEXT_ROW(table, table->index)->value / 4);

    table->flags = 0;
}

// Nonmatching: 74.9%, same shape and same two gaps as func_ov039_020847ec.
void func_ov039_02084a3c(TinPinSlammer_Scene* scene, s32 step) {
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    s32*        column;
    s32         value = OTU_TEXT_ROW(table, table->index)->value;

    if (value / 4 == 2) {
        column = &OtuText_YTight;
    } else if (value / 4 == 1) {
        column = &OtuText_YMid;
    } else if (value / 4 == 0) {
        column = &OtuText_YFar;
    }

    func_ov039_02083af4(column, step, OTU_TEXT_ROW(table, table->index)->value / 4);

    table->flags = 0;
}

// Nonmatching: 90.0%. The four-way switch, its jump table, all four coordinates,
// the call and the flag clear match and the size is exact; only the divide form
// and a register choice differ.
void func_ov039_02084b94(TinPinSlammer_Scene* scene, s32 step) {
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    s32*        column;
    s32         value = OTU_TEXT_ROW(table, table->index)->value;

    switch (value / 4) {
        case 0:
            column = &OtuText_XFar;
            break;
        case 1:
            column = &OtuText_XMid;
            break;
        case 2:
            column = &OtuText_YSmall;
            break;
        case 3:
            column = &OtuText_XNear;
            break;
    }

    func_ov039_02083af4(column, step, OTU_TEXT_ROW(table, table->index)->value / 4);

    table->flags = 0;
}

// Nonmatching: 84.3%, same shape and same gaps as func_ov039_02084944.
void func_ov039_02084d20(TinPinSlammer_Scene* scene, s32 step) {
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    s32*        column;

    if (OTU_TEXT_ROW(table, table->index)->value / 4 == 0) {
        column = &data_ov039_0209a398;
    }

    func_ov039_02083af4(column, step, OTU_TEXT_ROW(table, table->index)->value / 4);

    table->flags = 0;
}

// ---------------------------------------------------------------------------
// The countdown steppers, 0x02084e1c onwards.
//
// These seventeen functions are one family: eight steppers that advance a
// digit column by one decimal place, eight row builders that format the same
// columns as text, and the pair at the front (02084e1c / 02084f08) which work on
// two globals rather than the scene.
//
// Every stepper follows the same template:
//   1. reach the row table and read the active row's `value`;
//   2. divide that by a small constant to pick which digit place is animating
//      (2, 3, 4 or 5 -- one function per column width);
//   3. multiply the step by 1, 10 or 100 to get the place's weight;
//   4. add that into the column's counter and clamp it to the column's range;
//   5. clear the table's flags so the row builders redraw.
//
// The clamp is always the same shape -- "below the floor becomes the ceiling,
// at or above the ceiling becomes zero" -- which reads like a deliberate
// wrap-around rather than a saturate: a counter driven past its range lands on
// the opposite end rather than sticking. The row builders then format the
// result. Whether that is intentional in the original or an artefact of how the
// clamp was written cannot be told from the code alone.
// ---------------------------------------------------------------------------

/**
 * @brief Step the five-digit score counter, 0x02084e1c.
 *
 * The widest of the family: five places (10000 down to 1), so `value / 5` picks
 * the place and there are five jump-table cases rather than a comparison chain.
 *
 * The two counters are file-scope globals rather than scene fields, and which
 * one is stepped depends on `value / 2` -- an even value steps the big one, an
 * odd one the small. That is the same parity test the two-entry selection in
 * the row builder (02084f08) reads back.
 */
// Nonmatching: 64.5%. Five-way jump table present and the two globals are right, but the
// target picks its counter by a 5-way switch on value / 5 *after* the parity
// test, and reaches the 10000 weight from the pool; this build folds the two
// divisions differently and materialises the constant. Divide-form gap.
void func_ov039_02084e1c(TinPinSlammer_Scene* scene, s32 step) {
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    s32*        counter;
    s32         place;

    // Two separate globals, chosen by the parity of value / 2.
    if (OTU_TEXT_ROW(table, table->index)->value / 2 == 0) {
        counter = &data_ov039_0209a394;
    } else {
        counter = &data_ov039_0209a390;
    }

    // Five places, so this is a jump table. The `case 0` arm loads its weight
    // from the pool rather than materialising it, which is the same trick
    // 02084f08 uses and is why 0x2710 needs a pool word here.
    switch (OTU_TEXT_ROW(table, table->index)->value / 5) {
        case 0:
            place = 0x2710;
            break;
        case 1:
            place = 0x3E8;
            break;
        case 2:
            place = 0x64;
            break;
        case 3:
            place = 0xA;
            break;
        case 4:
            place = 1;
            break;
    }

    *counter += place * step;

    // Clamp to [0, 99999], wrapping rather than sticking.
    if (*counter >= 100000) {
        *counter = 0;
    } else if (*counter < 0) {
        *counter = 99999;
    }

    table->flags = 0;
}

/**
 * @brief Step the tray counter, 0x02085124.
 *
 * The one member of the family that steps the table's own `trayCount` field
 * rather than a scene digit column, and the only one whose range is the pin
 * sentinel 0x130 rather than a round number.
 *
 * Three places (100, 10, 1) selected by `value % 3`, compared rather than
 * jump-tabled. The clamp wraps at 0x130/0x131: below zero becomes 0x130, at or
 * above 0x131 becomes zero. 0x130 is the same "no pin here" value
 * func_ov039_020824a0 fills the tray with.
 */
void func_ov039_02085124(TinPinSlammer_Scene* scene, s32 step) {
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    s32         place;
    s32         count;

    // Read once, before the place switch, and clamp in a local: spelling this
    // as `trayCount +=` and comparing the field twice is what forces the
    // reloads, and reading it after the switch sinks the load past the target's.
    count = table->trayCount;

    // The remainder, not the quotient. func_ov039_020841a0 reads the same field
    // for both -- the quotient as the slot index and this as the digit within
    // it -- and its target divides twice; here only the modulus is needed, so
    // mwcc's `smull`/multiply/subtract expansion is the whole of it.
    switch (OTU_TEXT_ROW(table, table->index)->value % 3) {
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
    table->trayCount = count;

    table->flags = 0;
}

/**
 * @brief Step one of four two-digit columns, 0x020852c0.
 *
 * Four columns within a single digit block, selected by `value / 4` as a
 * jump-table case; two places (10 and 1) by `value / 2`. The counter is a
 * `u8` and wraps at 100.
 */
// Nonmatching: 62.0%. The four-way jump table and the wrap-at-100 clamp are
// right. The switch has to stay a switch: selecting the byte with
// `column + value / 4` instead is semantically identical, compiles with no jump
// table at all, and costs 21 points (62.0% -> 41.4%). So the target really does
// branch four ways here rather than computing an offset. What is left is the
// divide-form difference the rest of the steppers also have.
void func_ov039_020852c0(TinPinSlammer_Scene* scene, s32 slot, s32 step) {
    OtuTextRow*     table   = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuDigitColumn* column  = OTU_DIGIT_COLUMN(scene, slot);
    u8*             counter = (u8*)column;
    s32             place;

    // Four columns one byte apart, so the pointer is the base plus a case value.
    //
    // The switch has to stay a switch. Selecting the byte with
    // `column + value / 4` instead is semantically identical and compiles to
    // no jump table at all, which costs 21 points (62.0% -> 41.4%): the target
    // really does branch four ways here rather than computing an offset.
    switch (OTU_TEXT_ROW(table, table->index)->value / 4) {
        case 0:
            counter = (u8*)column;
            break;
        case 1:
            counter = (u8*)column + 1;
            break;
        case 2:
            counter = (u8*)column + 2;
            break;
        case 3:
            counter = (u8*)column + 3;
            break;
    }

    // Two places by a separate division of the same value.
    if (OTU_TEXT_ROW(table, table->index)->value / 2 == 0) {
        place = 10;
    } else {
        place = 1;
    }

    *counter += place * step;

    if (*counter < 0) {
        *counter = 99;
    } else if (*counter >= 100) {
        *counter = 0;
    }

    table->flags = 0;
}

/**
 * @brief Step a single digit, 0x0208554c.
 *
 * The narrowest column: one `u8`, one decimal place. `value / 2` picks the
 * place's weight (10 or 1) and nothing else, and the counter wraps at 10 --
 * below zero becomes 9, at or above 10 becomes zero.
 *
 * The column itself is the base of the digit block; unlike 020852c0 there is no
 * per-column offset, because there is only one column.
 */
// Nonmatching: 49.0%. The column and the +4 offset are right. Two problems: the counter is
// stepped through a u8* and the target keeps it as a byte address on the
// column struct, and the target computes value / 2 twice in two different
// instruction forms where this build folds it to one.
void func_ov039_0208554c(TinPinSlammer_Scene* scene, s32 slot, s32 step) {
    OtuTextRow*     table   = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuDigitColumn* column  = OTU_DIGIT_COLUMN(scene, slot);
    u8*             counter = (u8*)column;
    s32             place;

    // Which byte, then which weight: one test of value / 2, which the target
    // folds into a single branch -- the counter pointer and the weight move
    // together.
    if (OTU_TEXT_ROW(table, table->index)->value / 2 == 0) {
        place   = 10;
        counter = (u8*)column + 4;
    } else {
        place = 1;
    }

    *counter += place * step;

    if (*counter < 0) {
        *counter = 9;
    } else if (*counter >= 10) {
        *counter = 0;
    }

    table->flags = 0;
}

/**
 * @brief Step a column through the shared place stepper, 0x020856c8.
 *
 * The one member of the family that delegates rather than doing the arithmetic
 * itself: it hands the column's current value, the step and the place index to
 * func_ov039_02083af4, the same helper the countdown column steppers use.
 *
 * `value / 4` picks the place, and the pre-step value is read back *after* the
 * call and re-clamped -- the target reloads it rather than keeping it in a
 * register, so the value written is whatever the helper left behind. The clamp
 * is against 0x8000 rather than the column's own range, which is a saturating
 * test rather than the family's wrapping one: at or above 0x8000 becomes zero.
 */
// Nonmatching: 67.1%. Was 28.6% on a real modelling error rather than a codegen
// gap: the target does not pass the halfword's address to func_ov039_02083af4,
// it spills the value, passes the address of the spill, and reloads it
// afterwards. That is also why the value clamped below is the helper's output
// and not the pre-step one. Fixed; what remains is the two-step address form.
void func_ov039_020856c8(TinPinSlammer_Scene* scene, s32 slot, s32 step) {
    OtuTextRow*     table  = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuDigitColumn* column = OTU_DIGIT_COLUMN(scene, slot);
    s16*            counter;
    s32             temp;

    // value / 4 picks the place *and* which halfword: +6 when the quotient is
    // zero, +0 otherwise, folded into one branch by the target.
    counter = (s16*)column;
    if (OTU_TEXT_ROW(table, table->index)->value / 4 == 0) {
        counter = &column->unk_06;
    }

    // Through a stack temporary, not the field itself. The target loads the
    // halfword with ldrsh, spills it, hands the stepper the *address of the
    // spill*, then reloads it and stores it back -- which is also why the
    // clamped value is the helper's output and not the pre-step one. Passing
    // `counter` directly is semantically the same and compiles quite differently.
    temp = *counter;
    func_ov039_02083af4(&temp, step, OTU_TEXT_ROW(table, table->index)->value / 4);

    if (temp >= 0x8000) {
        temp = 0;
    }

    *counter = temp;

    table->flags = 0;
}

/**
 * @brief Step a two-place column at +0x08, 0x02085818.
 *
 * Three places (100, 10, 1) by `value / 3`, but *two* candidate fields: an
 * earlier division of the same value by 2 picks between +0x08 and +0x0A, so the
 * column holds two independent three-place counters and the parity selects
 * which one animates. Range is 0..999.
 */
// Nonmatching: 52.5%. Correct fields, parity test and 0..999 wrap clamp. The target forms
// the column address before dereferencing the row and does the two divisions
// in separate instruction forms; both are folded here.
void func_ov039_02085818(TinPinSlammer_Scene* scene, s32 slot, s32 step) {
    OtuTextRow*     table  = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuDigitColumn* column = OTU_DIGIT_COLUMN(scene, slot);
    s32*            counter;
    s32             place;

    // The parity of value / 2 picks which of the two counters.
    if (OTU_TEXT_ROW(table, table->index)->value / 2 == 0) {
        counter = &column->unk_08;
    } else {
        counter = &column->unk_0A;
    }

    switch (OTU_TEXT_ROW(table, table->index)->value / 3) {
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

    *counter += place * step;

    if (*counter < 0) {
        *counter = 999;
    } else if (*counter >= 1000) {
        *counter = 0;
    }

    table->flags = 0;
}

/**
 * @brief Step a two-place column at +0x0C, 0x02085a64.
 *
 * The same shape as 02085818 one field pair along: two counters selected by the
 * parity of `value / 2`, three places by `value / 3`, range 0..999.
 */
// Nonmatching: 52.5%. Same shape and same gaps as 02085818, one field pair along.
void func_ov039_02085a64(TinPinSlammer_Scene* scene, s32 slot, s32 step) {
    OtuTextRow*     table  = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuDigitColumn* column = OTU_DIGIT_COLUMN(scene, slot);
    s32*            counter;
    s32             place;

    if (OTU_TEXT_ROW(table, table->index)->value / 2 == 0) {
        counter = &column->unk_0C;
    } else {
        counter = &column->unk_0E;
    }

    switch (OTU_TEXT_ROW(table, table->index)->value / 3) {
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

    *counter += place * step;

    if (*counter < 0) {
        *counter = 999;
    } else if (*counter >= 1000) {
        *counter = 0;
    }

    table->flags = 0;
}

/**
 * @brief Step a three-place column at +0x18, 0x02085fb4.
 *
 * One counter only -- no parity selection, so `value / 3` is the sole divisor --
 * three places by `value / 3`, range 0..999. The column is reached through the
 * same 0x1C stride as the others.
 */
// Nonmatching: 73.6%. Correct field, place weights and wrap clamp; the column address is
// reached with the stride multiply last here and first in the others.
void func_ov039_02085fb4(TinPinSlammer_Scene* scene, s32 slot, s32 step) {
    OtuTextRow*     table  = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuDigitColumn* column = OTU_DIGIT_COLUMN(scene, slot);
    s32             place;

    switch (OTU_TEXT_ROW(table, table->index)->value / 3) {
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

    column->unk_18 += place * step;

    if (column->unk_18 < 0) {
        column->unk_18 = 999;
    } else if (column->unk_18 >= 1000) {
        column->unk_18 = 0;
    }

    table->flags = 0;
}

// ---------------------------------------------------------------------------
// The row builders.
//
// Each one formats the counters a stepper above advances, as fixed-width digit
// groups separated by spaces, then hands the row to Text_RenderToScreen at a
// fixed y. The x is always 0x38; only the y differs, and it runs in a fixed
// order down the screen: 0x08 tray count, 0x10 four pairs, 0x18 single pair,
// 0x20 five-digit pair, 0x20 d.ddd, 0x28 three-digit pair, 0x30 the same again,
// 0x38 the six-group fixed-point row.
//
// All of them share the tail: if the active row's value is non-negative and the
// table's flags have 0x20 set, the column named by a layout table is blanked,
// then the trailing space is overwritten with a NUL and the row is rendered.
// ---------------------------------------------------------------------------

/**
 * @brief The tray count as three digits, 0x020851bc.
 *
 * The one row builder that reads the table's own `trayCount` rather than a digit
 * column, and the only one that renders the counter with a +1 bias -- so a tray
 * holding the 0x130 "no pin" sentinel displays as 0.
 */
// Nonmatching: 96.5%. Register allocation only -- the target keeps the table in r0 across
// the digit extraction and mine reloads it. Frame and pool words match.
void func_ov039_020851bc(TinPinSlammer_Scene* scene) {
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    char        row[0x20];
    s32         value = table->trayCount + 1;

    row[3] = ' ';

    row[0] = '0' + (value / 100) % 10;
    row[1] = '0' + (value / 10) % 10;
    row[2] = '0' + value % 10;

    value = OTU_TEXT_ROW(table, table->index)->value;
    if (value >= 0 && (table->flags & 0x20)) {
        row[data_ov039_02098e5c[value]] = ' ';
    }
    row[3] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x8, row);
}

/**
 * @brief Four two-digit groups, 0x02085388.
 *
 * The widest row in the set: four counters one byte apart in a single digit
 * column, each rendered as two digits and each followed by a space, for twelve
 * characters. It is the display counterpart of func_ov039_020852c0, which steps
 * one of those four bytes.
 */
// Nonmatching: 96.3%. Was 42.4% written as a loop, which cannot produce the
// target at all: the target emits no index arithmetic, reading the four
// counters as four separate byte loads at column+0..3 and filling the row with
// eight stores. Writing the four groups out is what fixed it, not a tweak to
// the loop. What is left is register allocation.
void func_ov039_02085388(TinPinSlammer_Scene* scene, s32 slot) {
    OtuTextRow*     table  = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuDigitColumn* column = OTU_DIGIT_COLUMN(scene, slot);
    char            row[0x20];
    s32             value;

    // Written out rather than looped. The target emits the four groups in an
    // interleaved order -- each group's two digits, then its space, then
    // straight on to the next group -- with no index arithmetic, so a loop
    // cannot produce it: the four counters are read as four separate byte
    // loads at column+0 through column+3 and the row is filled by eight stores.
    value  = ((u8*)column)[0] + 1;
    row[0] = '0' + (value / 10) % 10;
    row[1] = '0' + value % 10;
    row[2] = ' ';

    value  = ((u8*)column)[1] + 1;
    row[3] = '0' + (value / 10) % 10;
    row[4] = '0' + value % 10;
    row[5] = ' ';

    value  = ((u8*)column)[2] + 1;
    row[6] = '0' + (value / 10) % 10;
    row[7] = '0' + value % 10;
    row[8] = ' ';

    value   = ((u8*)column)[3] + 1;
    row[9]  = '0' + (value / 10) % 10;
    row[10] = '0' + value % 10;
    row[11] = ' ';

    value = OTU_TEXT_ROW(table, table->index)->value;
    if (value >= 0 && (table->flags & 0x20)) {
        row[data_ov039_02098ed8[value]] = ' ';
    }
    row[11] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x10, row);
}

/**
 * @brief One two-digit group, 0x020855e0.
 *
 * The display counterpart of func_ov039_0208554c: the single `u8` at the
 * column's +4, rendered as two digits with the same +1 bias.
 *
 * Note the address arithmetic runs the other way round from the steppers: the
 * stride multiply happens first and the +0x41000 after, so this one is spelled
 * with OTU_DIGIT_COLUMN_STRIDED rather than OTU_DIGIT_COLUMN.
 */
// Nonmatching: 96.7%. Register allocation only; frame and pool words match.
void func_ov039_020855e0(TinPinSlammer_Scene* scene, s32 slot) {
    OtuTextRow*     table  = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuDigitColumn* column = OTU_DIGIT_COLUMN(scene, slot);
    char            row[0x20];
    s32             value = column->pad_00[4] + 1;

    row[0] = '0' + (value / 10) % 10;
    row[1] = '0' + value % 10;
    row[2] = ' ';

    value = OTU_TEXT_ROW(table, table->index)->value;
    if (value >= 0 && (table->flags & 0x20)) {
        row[data_ov039_02098e48[value]] = ' ';
    }
    row[2] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x18, row);
}

/**
 * @brief One Q12.12 value as `d.ddd`, 0x02085770.
 *
 * Delegates the formatting to func_ov039_02083a20 -- the same helper
 * func_ov039_020844f8 uses -- and reads the column's +6 halfword. Six bytes of
 * output, so the trailing space lands at index 5.
 */
void func_ov039_02085770(TinPinSlammer_Scene* scene, s32 slot) {
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    // Deliberately over-declared: only six bytes are written, but the target's
    // stack frame is 0x20 and sizing this to 6 shrinks the frame to 8 bytes.
    char row[0x20];
    s32  value;

    func_ov039_02083a20((OtuTextDigits*)row, *(s16*)((u8*)scene + 0x41EF6 + slot * 0x1C));

    row[5] = ' ';

    value = OTU_TEXT_ROW(table, table->index)->value;
    if (value >= 0 && (table->flags & 0x20)) {
        row[data_ov039_02098e78[value]] = ' ';
    }
    row[5] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x20, row);
}

/**
 * @brief Two three-digit groups, 0x020858e4.
 *
 * The display counterpart of func_ov039_02085818: both of that function's
 * counters (+0x08 and +0x0A) rendered side by side, three digits each, with a
 * space after the first.
 */
// Nonmatching: 89.7%. Register allocation only -- same shape as 02085b30, and the pool
// words and frame match.
void func_ov039_020858e4(TinPinSlammer_Scene* scene, s32 slot) {
    OtuTextRow*     table  = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuDigitColumn* column = OTU_DIGIT_COLUMN(scene, slot);
    char            row[0x20];
    s32             value;

    row[3] = ' ';

    value  = column->unk_08;
    row[0] = '0' + (value / 100) % 10;
    row[1] = '0' + (value / 10) % 10;
    row[2] = '0' + value % 10;

    value  = column->unk_0A;
    row[4] = '0' + (value / 100) % 10;
    row[5] = '0' + (value / 10) % 10;
    row[6] = '0' + value % 10;

    value = OTU_TEXT_ROW(table, table->index)->value;
    if (value >= 0 && (table->flags & 0x20)) {
        row[data_ov039_02098ea8[value]] = ' ';
    }
    row[7] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x28, row);
}

/**
 * @brief Two three-digit groups, again, 0x02085b30.
 *
 * The display counterpart of func_ov039_02085a64: the +0x0C and +0x0E counters,
 * formatted exactly as 020858e4 formats the +0x08 and +0x0A pair.
 */
// Nonmatching: 89.7%. Register allocation only; same as 020858e4.
void func_ov039_02085b30(TinPinSlammer_Scene* scene, s32 slot) {
    OtuTextRow*     table  = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuDigitColumn* column = OTU_DIGIT_COLUMN(scene, slot);
    char            row[0x20];
    s32             value;

    row[3] = ' ';

    value  = column->unk_0C;
    row[0] = '0' + (value / 100) % 10;
    row[1] = '0' + (value / 10) % 10;
    row[2] = '0' + value % 10;

    value  = column->unk_0E;
    row[4] = '0' + (value / 100) % 10;
    row[5] = '0' + (value / 10) % 10;
    row[6] = '0' + value % 10;

    value = OTU_TEXT_ROW(table, table->index)->value;
    if (value >= 0 && (table->flags & 0x20)) {
        row[data_ov039_02098ec0[value]] = ' ';
    }
    row[7] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x30, row);
}

/**
 * @brief A stepped Q12.12 column, read back and blanked, 0x02085cb0.
 *
 * The odd one out among the row builders: it calls the place stepper
 * func_ov039_02083af4 rather than a formatter, so it advances a column rather
 * than displaying one, and then clamps the result to 0..999 the same way
 * func_ov039_02085818 does. With no Text_RenderToScreen call it is really a
 * second stepper that happens to sit among the row builders and to delegate the
 * arithmetic.
 */
// Nonmatching: 79.7%. The first draft was simply wrong -- it called the place
// stepper and then added the weighted step as well, which double-counted.
// Rewritten off the target as one 12-way switch on the row value, whose cases
// pick a halfword and fall through into one of three shared bodies, which took
// it from 30% to 78%. What is left is register allocation plus the two paired
// jump tables: the target emits one switch to choose the counter and a second,
// identical one to choose the body, and this build shares the first.
//
// The four counters live at +0x10, +0x12, +0x14 and +0x16, selected four cases
// at a time. The bodies are not interchangeable, which is why this cannot be
// written as a loop over the four:
//
//   * +0x10 and +0x12 delegate to func_ov039_02083af4 with place = value / 4;
//   * +0x14 does the place arithmetic itself, with place = (value - 8) / 3;
//   * +0x16 adds the raw step and clamps to [1, 9] -- a range of one to nine,
//     not zero to nine, so this one counts a column rather than a digit.
//
// Note the second clamp. Every other stepper in the family wraps to zero at the
// top of its range; this one wraps to *one*. So +0x16 is a 1-based column index
// that is being animated, not a digit being counted.
void func_ov039_02085cb0(TinPinSlammer_Scene* scene, s32 slot, s32 step) {
    OtuTextRow*     table  = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuDigitColumn* column = OTU_DIGIT_COLUMN(scene, slot);
    u16*            counter;
    s32             value = OTU_TEXT_ROW(table, table->index)->value;

    // Four counters, four cases each. Written as one switch with the cases
    // falling through, because that is what produces the target's paired
    // jump tables -- one choosing the counter, one choosing the body.
    switch (value) {
        case 0:
        case 1:
        case 2:
        case 3:
            counter = &column->unk_10;
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            counter = &column->unk_12;
            break;
        case 8:
        case 9:
        case 10:
            counter = &column->unk_14;
            break;
        case 11:
            counter = &column->unk_16;
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
            // Through a stack temporary, as in 020856c8: the target spills the
            // halfword, hands the stepper the address of the spill, and reloads
            // and stores back afterwards. This one reads it *signed* -- the two
            // later bodies read the same pointer `ldrh`.
            s32 temp = *(s16*)counter;

            func_ov039_02083af4(&temp, step, value / 4);

            *counter = temp;
            break;
        }
        case 8:
        case 9:
        case 10: {
            // Three places, weighted, wrapping at 999 -- the same shape as
            // func_ov039_02085818 but with the quotient biased by the case.
            s32 place;

            switch ((value - 8) / 3) {
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

            *counter += place * step;

            if (*counter < 0) {
                *counter = 999;
            } else if (*counter >= 1000) {
                *counter = 0;
            }
            break;
        }
        case 11:
            // The 1-based column: raw step, no weighting, wraps within [1, 9].
            *counter += step;

            if (*counter < 1) {
                *counter = 9;
            } else if (*counter >= 10) {
                *counter = 1;
            }
            break;
    }

    table->flags = 0;
}

/**
 * @brief The six-group fixed-point row, 0x02085e54.
 *
 * The longest row in the set: two Q12.12 values through func_ov039_02083a20 and
 * four plain three-digit counters, laid out as six space-separated groups --
 * twenty-four characters, the widest thing the result screen draws. The
 * digit-column base is reached with the strided form, as in 02085770.
 */
// Nonmatching: 62.4%. The six-group layout is right but the fourth group is a guess: the
// target loads two halfwords at +0x14 and +0x16 and the two d.ddd groups come
// from +0x10 and +0x12, which this build has, but the ordering of the second
// d.ddd call and the trailing group is not established.
void func_ov039_02085e54(TinPinSlammer_Scene* scene, s32 slot) {
    OtuTextRow*     table  = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuDigitColumn* column = OTU_DIGIT_COLUMN_STRIDED(scene, slot);
    char            row[0x20];
    s32             value;

    func_ov039_02083a20((OtuTextDigits*)(row + 0x00), column->unk_10);
    func_ov039_02083a20((OtuTextDigits*)(row + 0x06), column->unk_12);
    row[12] = ' ';

    value   = column->unk_14;
    row[13] = '0' + (value / 100) % 10;
    row[14] = '0' + (value / 10) % 10;
    row[15] = '0' + value % 10;
    row[16] = ' ';

    value   = column->unk_16;
    row[17] = '0' + (value / 100) % 10;
    row[18] = '0' + (value / 10) % 10;
    row[19] = '0' + value % 10;
    row[20] = ' ';

    func_ov039_02083a20((OtuTextDigits*)(row + 0x15), 0);

    value = OTU_TEXT_ROW(table, table->index)->value;
    if (value >= 0 && (table->flags & 0x20)) {
        row[data_ov039_02098f94[value]] = ' ';
    }
    row[23] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x38, row);
}

/**
 * @brief The two five-digit counters, 0x02084f08.
 *
 * The display counterpart of func_ov039_02084e1c: the two file-scope counters
 * that function steps, rendered as two five-digit groups separated by a space --
 * twelve characters. No +1 bias here, unlike the tray row.
 */
// Nonmatching: full MATCH.
void func_ov039_02084f08(TinPinSlammer_Scene* scene) {
    OtuTextRow* table = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    char        row[0x20];
    s32         value;

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

    value = OTU_TEXT_ROW(table, table->index)->value;
    if (value >= 0 && (table->flags & 0x20)) {
        row[data_ov039_02098f3c[value]] = ' ';
    }
    row[11] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x20, row);
}
