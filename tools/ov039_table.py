#!/usr/bin/env python3
"""Read a word table out of the ov039 disassembly.

The disassembly emits `.rodata` as runs of `.byte a, b, c, ...` with a
`<label>:` line at the *start* of the run it names, so a table's bytes are the
`.byte` lines immediately *below* its label, up to the next label.  Entries are
4 bytes wide, little-endian.

The label positions are self-checking: consecutive table labels are exactly
`4 * entries` apart, so a misread length shows up immediately as a gap.

    python tools/ov039_table.py 0x02098f18 [0x020990b4 ...]
"""
import re
import sys
from pathlib import Path

ASM = Path("build/usa/asm/ov039_4.s")


def table(addr):
    text = ASM.read_text(encoding="latin1")
    # Labels in the disassembly are `data_ov039_<hex>`, with no 0x and no
    # zero-padding -- 0x02098f18 is `data_ov039_02098f18`.
    m = re.search(rf"^data_ov039_{addr & 0xFFFFFFFF:08x}:\n((?:    \.byte [^\n]*\n)+)",
                  text, re.M)
    if not m:
        return None
    raw = bytes(int(x, 16) for x in re.findall(r"0x([0-9a-f]{2})", m.group(1)))
    if len(raw) % 4:
        return None
    return [int.from_bytes(raw[i:i + 4], "little") for i in range(0, len(raw), 4)]


def main():
    for a in sys.argv[1:]:
        addr = int(a, 16)
        t = table(addr)
        if t is None:
            print(f"{addr:#x}: not a word table")
        else:
            print(f"{addr:#x}: {len(t)} entries")
            print("  " + ", ".join(str(v) for v in t))


if __name__ == "__main__":
    main()
