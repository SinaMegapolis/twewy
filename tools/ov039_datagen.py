#!/usr/bin/env python3
"""Prepend ov039's .data non-string objects to the generated string file.

The .data range 0x0209a140-0x0209a2fc holds three kinds of object: pointer
tables, stage descriptors, and the Shift-JIS menu labels.  The string generator
only handles the last kind, so this writes the other two ahead of it, in the
order the target lays them out -- mwcc sorts by size ascending, so the
declaration order below is not free choice.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DST = ROOT / "src/Debug/Sugata/TinPinSlammer/OtuMenuText.c"

HEADER = '''#include "Debug/Sugata/TinPinSlammer.h"

/**
 * @brief Character offset of each countdown column in the pin-count row.
 *
 * func_ov039_02083ed4 assembles six digit groups into one buffer and indexes
 * this by the row's current value to find which character that column owns, so
 * it can blank the column without disturbing the rest of the row.
 */
const u8 OtuTextFieldOffset[7] = {0, 1, 3, 5, 6, 7, 9};

/**
 * @brief The scene's process-stage dispatch tables.
 *
 * These live in `.rodata`, immediately before this TU's `.data` range.
 * `MainOvlDisp_GetProcessStage` returns the index; each table maps it to the
 * routine for the scene at that stage.  Index 1 is the per-frame update and
 * index 2 the teardown, so each table pairs one scene variant's entry point
 * with its own update and teardown.
 */
const OtuStageHandler OtuScene_PlainHandlers[3] = {
    func_ov039_02082ae0,
    func_ov039_02082e98,
    func_ov039_020831cc,
};

const OtuStageHandler OtuScene_WirelessHandlers[3] = {
    func_ov039_02082c50,
    func_ov039_02082ff8,
    func_ov039_020831d8,
};

/**
 * @brief The three debug-sequence names, all the same string.
 *
 * The target holds three copies of the pointer rather than one string and three
 * pointers, and the scene entry points index it.  Reproduced literally: mwcc
 * emits one pool word per object, so collapsing it to a single
 * `static const char*` would change the emitted references.
 */
static const char kOtuSequenceName[] = "Seq_Otosu()";

const char* const OtuScene_SequenceNames[3] = {
    kOtuSequenceName,
    kOtuSequenceName,
    kOtuSequenceName,
};

/*
 * Otosu .data objects: the scene's stage descriptors, its debug-sequence name,
 * and the three result-screen menus' labels and scrollbar separators.
 *
 * This TU owns 0x0209a140-0x0209a2fc in one range because dsd allows a single
 * .data entry per TU, and the natural groupings (stage descriptors, then the
 * strings) are not contiguous.
 *
 * The Shift-JIS label bytes come from tools/ov039_strgen.py, which reads them
 * out of build/usa/asm/ov039_4.s; the build compiles this file with -enc SJIS.
 * mwcc sorts .data objects by size ascending and breaks ties with a heapsort
 * over creation order, so the declaration order here is load-bearing -- do not
 * reorder.
 */

/**
 * @brief The wireless stage's companion descriptor.
 *
 * Three routines the wireless stage sequences through -- one more than the
 * single-routine companion the first stage uses, so it is a bare pointer array.
 */
void* const OtuScene_WirelessStage_Companion[3] = {
    func_ov039_02087acc,
    func_ov039_02087b04,
    func_ov039_02087b74,
};

/**
 * @brief The stage the scene enters when the wireless link needs attention.
 */
OtuSceneStage OtuScene_WirelessStage = {
    func_ov039_02087ac4,
    func_ov039_02087ac8,
    &OtuScene_WirelessStage_Companion,
    0,
};

/**
 * @brief The scene's first stage: the board/pin-logic stage.
 *
 * Runs after the shared scene setup and owns 0x3C bytes of state.
 */
OtuSceneStageCompanion OtuScene_FirstStage_Companion = {
    func_ov039_02086808,
};

OtuSceneStage OtuScene_FirstStage = {
    func_ov039_02086728,
    func_ov039_020867d4,
    &OtuScene_FirstStage_Companion,
    0x3C,
};

/**
 * @brief The stage the wireless board entry point enters instead.
 *
 * Same shape as the first stage but with a much larger state block (0x2D0) and
 * its own companion routine list at 0x0209a32c.
 */
OtuSceneStageCompanion OtuScene_WirelessBoard_Companion = {
    func_ov039_020897dc,
};

OtuSceneStage OtuScene_WirelessBoard = {
    func_ov039_02089780,
    func_ov039_020897d0,
    &OtuScene_WirelessBoard_Companion,
    0x2D0,
};

/**
 * @brief Companion routine lists for the two remaining stage descriptors.
 *
 * Both are six-entry pointer arrays; the stage descriptors at 0x0209a3bc and
 * 0x0209a3cc point at them.
 */
void* const OtuScene_MenuStage_Companion[6] = {
    func_ov039_02089f68,
    func_ov039_0208a098,
    func_ov039_0208a324,
    func_ov039_0208a354,
    func_ov039_0208a3f4,
    func_ov039_0208a454,
};

void* const OtuScene_ResultStage_Companion[6] = {
    func_ov039_02089950,
    func_ov039_02089a80,
    func_ov039_02089d3c,
    func_ov039_02089d6c,
    func_ov039_02089e0c,
    func_ov039_02089e78,
};

OtuSceneStage OtuScene_MenuStage = {
    func_ov039_020898a8,
    func_ov039_02089918,
    OtuScene_MenuStage_Companion,
    0x2D0,
};

OtuSceneStage OtuScene_ResultStage = {
    func_ov039_02089ec0,
    func_ov039_02089f30,
    OtuScene_ResultStage_Companion,
    0x2D0,
};

/**
 * @brief The text helpers' anchor-point coordinates, as GX 12.4 fixed point.
 *
 * Each is referenced individually -- the row-drawing code picks one per anchor
 * with a `switch`, and the layout pass reads all three of a group.  The three
 * groups are the near/mid/far x and y coordinates the Otosu score rows are
 * laid out at.
 */
s32 OtuText_XFar = 0x2000;
s32 OtuText_XNear = 0x1000;
s32 OtuText_XTight = 0x133;
s32 OtuText_XWide = 0x8000;
s32 OtuText_XMid = 0x4000;
s32 OtuText_YSmall = 0x400;
s32 OtuText_YFar = 0x3000;
s32 OtuText_YTight = 0x133;
s32 OtuText_YUnderflow = -0x2000;
s32 OtuText_YOverflow = -0x2000;
s32 OtuText_YTiny = 0x800;
s32 OtuText_YMid = 0x9800;

/** Singletons the text helpers read alongside the anchor coordinates above. */
s32 OtuText_Shadow = 0x8000;
s32 OtuText_LineHeight = 0x148;
s32 OtuText_TabStop = 0x99A;

/**
 * @brief The per-row layout table, 37 words of GX 12.4 coordinates.
 *
 * Read by the result-screen row builders, which index it to place each of the
 * three menus' fields.  Values are emitted from the ROM rather than derived.
 */
const s32 OtuText_RowLayout[37] = {
    0x14CD, 0x7AB8, 0x171, 0x948, 0x1614, 0x7548, 0x19A, 0x91F, 0x175C, 0x7000, 0x19A, 0x8CD,
    0x187B, 0x6AB8, 0x1C3, 0x87B, 0x19C3, 0x6548, 0x1EC, 0x829, 0x1B0A, 0x6000, 0x214, 0x800,
    0x1C52, 0x5AB8, 0x214, 0x7AE, 0x1D9A, 0x5548, 0x23D, 0x75C, 0x1EB8, 0x5000, 0x266, 0x733,
    0x2000,
};

/**
 * @brief A text block's four coordinate words, swapped by the menu entry point.
 *
 * `OtuScene_WirelessMenu` and `OtuScene_ResultMenu` are exchanged at scene setup
 * depending on which save record is loaded, so both are plain 16-byte objects
 * assigned through a stack temporary.
 */
OtuTextBlock OtuScene_WirelessMenu = {0x5000, 0x4CD, 0x4CD, 0x3000};
OtuTextBlock OtuScene_ResultMenu = {0x2800, 0x266, 0x4CD, 0x7000};

/**
 * @brief Overlay-global state cleared by every scene entry point.
 */
OtuSceneSlotState OtuScene_SlotState;

'''


def main():
    body = DST.read_text(encoding="shift_jis")
    DST.write_text(HEADER + body, encoding="shift_jis")
    print(f"prepended {len(HEADER)} chars of descriptors to {DST.name}")


if __name__ == "__main__":
    main()
