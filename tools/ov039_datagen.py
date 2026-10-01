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
