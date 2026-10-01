#!/usr/bin/env python3
"""Split the scene entry function out of OtuScene.c into its own file.

The entry function's `.text` (0x02082978+) precedes OtuScene.c's `.text` while
its `.data` (0x0209a140) follows OtuScene.c's `.data` (0x0209a2e0), and dsd
requires a TU's sections to sort in the same order -- so they cannot coexist
as separate TUs yet.  Keeping the entry function in its own file lets it be
wired in once the tutorial-string `.data` range is decompiled.
"""
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SRC = ROOT / "src/Debug/Sugata/TinPinSlammer/OtuScene.c"
DST = ROOT / "src/Debug/Sugata/TinPinSlammer/OtuSceneEntry.c"

MARKER = "/**\n * @brief Scene entry point for the wireless variant."
HEADER = (
    '#include "Debug/Sugata/TinPinSlammer.h"\n'
    '#include "Display.h"\n'
    '#include "Engine/Core/Memory.h"\n'
    '#include "Engine/EasyTask.h"\n\n'
)


def main():
    text = SRC.read_text(encoding="utf-8")
    if MARKER not in text:
        print("nothing to split")
        return
    i = text.index(MARKER)
    entry = text[i:]
    SRC.write_text(text[:i].rstrip() + "\n", encoding="utf-8")
    DST.write_text(HEADER + entry, encoding="utf-8")
    print(f"split {len(entry)} chars into {DST.name}")


if __name__ == "__main__":
    main()
