#!/usr/bin/env python3
"""Dump the ov039 overlay's bytes at given virtual addresses.

The overlay binary `build/usa/build/arm9_ov039.bin` is a flat image loaded at
0x020824A0 -- the address of the module's first `.text` byte -- so a virtual
address maps to a file offset by subtracting that base.

    python tools/ov039_bytes.py 0x02098e30 0x02098f18
    python tools/ov039_bytes.py --words 0x02098e30:0x02098f3c
"""
import struct
import sys
from pathlib import Path

OVL = Path("build/usa/build/arm9_ov039.bin")
BASE = 0x020824A0


def main():
    args = sys.argv[1:]
    words = False
    if args and args[0] == "--words":
        words = True
        args = args[1:]

    data = OVL.read_bytes()
    for a in args:
        if ":" in a:
            lo, hi = (int(x, 16) for x in a.split(":"))
        else:
            lo = hi = int(a, 16)

        for v in range(lo, hi, 4 if words else 1):
            o = v - BASE
            if o < 0 or o + 4 > len(data):
                print(f"{v:#x}: outside the overlay image")
                continue
            if words:
                print(f"{v:#x}  {struct.unpack_from('<I', data, o)[0]:#010x}")
            else:
                print(f"{v:#x}  " + " ".join(f"{b:02x}" for b in data[o:o + 4]))


if __name__ == "__main__":
    main()
