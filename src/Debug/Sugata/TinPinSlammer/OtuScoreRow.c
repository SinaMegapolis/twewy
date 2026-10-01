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
