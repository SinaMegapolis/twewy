#!/usr/bin/env python3
"""Map the ov039 .data anchor constants to their overlay addresses.

The result screens' row builders reach a block of `s32` GX 12.4 coordinates by
address rather than by name, and mwcc decides where each object lands by sorting
.data by size and, among equal sizes, by creation order. That makes the address
order unpredictable from the source alone.

So this reads the built overlay and locates each declared constant by value,
reporting only the values that occur exactly once. Anything ambiguous is
reported as such rather than guessed -- a wrong guess here silently places a
constant at the wrong address and costs a match percentage with no obvious
cause.

    python tools/ov039_anchor.py
"""
import re
import struct
from pathlib import Path

OVL = Path("build/usa/build/arm9_ov039.bin")
BASE = 0x020824A0
SRC = Path("src/Debug/Sugata/TinPinSlammer/OtuMenuText.c")

# .data range the overlay's data TU claims.
DATA_LO, DATA_HI = 0x0209A140, 0x0209A49C


def declared():
    """The `s32 OtuText_* = <int>;` constants, in declaration order."""
    text = SRC.read_text(encoding="latin1")
    out = []
    for m in re.finditer(r"^s32\s+(OtuText_\w+)\s*=\s*(-?0x[0-9a-fA-F]+|-?\d+)\s*;",
                         text, re.M):
        name = m.group(1)
        val = int(m.group(2), 0)
        out.append((name, val))
    return out


def occurrences(data, val):
    """Every word-aligned .data address holding `val`."""
    pat = struct.pack("<i", val)
    start = DATA_LO - BASE
    end = DATA_HI - BASE
    hits = []
    j = data.find(pat, start, end)
    while j >= 0:
        # Alignment is relative to the start of .data, not to the previous hit.
        if (j - start) % 4 == 0:
            hits.append(BASE + j)
        j = data.find(pat, j + 1, end)
    return hits


def main():
    data = OVL.read_bytes()
    for name, val in declared():
        hits = occurrences(data, val)
        if len(hits) == 1:
            print(f"  {hits[0]:#010x}  {name} = {val:#x}")
        elif not hits:
            print(f"  {'--------':>10}  {name} = {val:#x}   NOT IN .data")
        else:
            addrs = " ".join(f"{a:#x}" for a in hits)
            print(f"  {'AMBIGUOUS':>10}  {name} = {val:#x}   at {addrs}")


if __name__ == "__main__":
    main()
