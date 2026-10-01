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
        row[OtuScoreFieldOffset[value]] = ' ';
    }
    row[0x18] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, which * 8 + 0x20, row);
}

/**
 * @brief Step one of a menu's four countdown columns by a decimal place.
 *
 * `value / 4` selects which of the four coordinates in the slot's block is
 * being animated, and the same quotient is passed on as the decimal place, so
 * the four columns are the thousands, hundreds, tens and ones of one number.
 * `step` is +1 or -1, and the divide truncates toward zero in both places --
 * which is why the target spells it out as a shift-plus-sign-fixup rather than
 * a shift alone.
 *
 * Clears the table's flags afterwards, so the hide-column check in the row
 * builders re-evaluates from scratch on the next frame.
 */
// Nonmatching: 89.2%. The switch, its four-way jump table, the slot base
// arithmetic, all four column offsets and the trailing flag clear match, and
// the size is exact. What is left is register allocation plus the divide:
//
//   * the target keeps the slot base in r1 and the two arguments in r7/r6;
//     this build uses r3 and r6/r5. The prologue differs by one saved
//     register as a result.
//   * the target divides `value / 4` twice -- a shift-based form for the
//     switch selector and a rotate-based one for the call's third argument --
//     while mwcc folds the two into a single division here. Re-deriving the
//     value from the row does not help: the reload is folded too.
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

    // Deliberately not reusing the switch's quotient. The target divides again
    // here, in a different instruction form (a rotate-based correction rather
    // than a shift-based one), and mwcc folds the two together when the
    // expression is spelled identically in both places.
    func_ov039_02083af4(column, step, OTU_TEXT_ROW(table, table->index)->value / 4);

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
        row[OtuFixedFieldOffset[value]] = ' ';
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
        row[OtuTightFieldOffset[value]] = ' ';
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
        column = &OtuText_YLow;
    } else if (value / 4 == 1) {
        column = &OtuText_YOverflow;
    }

    func_ov039_02083af4(column, step, OTU_TEXT_ROW(table, table->index)->value / 4);

    table->flags = 0;
}
