#!/usr/bin/env python3
"""Map ov039 .data addresses to the functions that reference them.

Answers "which code owns this .data range", which is what decides the TU split:
a data range can only be claimed by a TU whose .text also sorts in the same
direction, so its consumers have to be in the same TU.

    python tools/ov039_data_owners.py 0x209a2fc 0x209a3ac
"""
import re
import sys
from collections import defaultdict

SYMS = "config/usa/arm9/overlays/ov039/symbols.txt"
RELS = "config/usa/arm9/overlays/ov039/relocs.txt"


def functions():
    out = []
    for line in open(SYMS):
        m = re.match(r"(\S+) kind:function\(arm,size=(0x[0-9a-f]+)\) addr:(0x[0-9a-f]+)", line)
        if m:
            out.append((int(m.group(3), 16), int(m.group(2), 16), m.group(1)))
    out.sort()
    return out


def main():
    lo = int(sys.argv[1], 16) if len(sys.argv) > 1 else 0x0209A16C
    hi = int(sys.argv[2], 16) if len(sys.argv) > 2 else 0x0209A2E0
    fns = functions()
    owners = defaultdict(list)
    for line in open(RELS):
        m = re.match(r"from:(0x[0-9a-f]+) kind:(\w+) to:(0x[0-9a-f]+)", line)
        if not m:
            continue
        src, tgt = int(m.group(1), 16), int(m.group(3), 16)
        if not (lo <= tgt < hi):
            continue
        fn = next((n for a, s, n in fns if a <= src < a + s), None)
        owners[fn or f"raw@{src:#x}"].append(src)

    print(f"data {lo:#x}-{hi:#x} referenced by:\n")
    for fn in sorted(owners, key=lambda f: (f is None, f)):
        print(f"  {fn}  ({len(owners[fn])} refs)")
    real = [f for f in owners if f and not f.startswith("raw@")]
    print(f"\ndistinct consumer functions: {len(real)}")
    if real:
        print(f"text range spanned: {min(f for f in real)!r} .. {max(f for f in real)!r}")


if __name__ == "__main__":
    main()
