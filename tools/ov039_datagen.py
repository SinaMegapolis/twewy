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
