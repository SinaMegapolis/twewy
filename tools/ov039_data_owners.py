#!/usr/bin/env python3
"""Map the ov039 .data string region to the functions that reference it.

Answers "which code owns this .data range", which is what decides the TU
split: a data range can only be claimed by a TU whose .text also sorts in the
same direction, so its consumers have to be in the same TU.

    python tools/ov039_data_owners.py > owners.txt
"""
import re
import sys
from collections import defaultdict

SYMS = "config/usa/arm9/overlays/ov039/symbols.txt"
RELS = "config/usa/arm9/overlays/ov039/relocs.txt"
LO, HI = 0x0209A16C, 0x0209A2E0


def functions():
    out = []
    for line in open(SYMS):
        m = re.match(r"(\S+) kind:function\(arm,size=(0x[0-9a-f]+)\) addr:(0x[0-9a-f]+)", line)
        if m:
            out.append((int(m.group(3), 16), int(m.group(2), 16), m.group(1)))
    out.sort()
    return out


def which_fn(fns, addr):
    for a, s, n in fns:
        if a <= addr < a + s:
            return n
    return None


def main():
    fns = functions()
    sym = {}
    for line in open(SYMS):
        m = re.match(r"(\S+) kind:\S+(?:,\S+)? addr:(0x[0-9a-f]+)", line)
        if m:
            sym.setdefault(int(m.group(2), 16), m.group(1))

    owners = defaultdict(list)
    for line in open(RELS):
        m = re.match(r"from:(0x[0-9a-f]+) kind:(\w+) to:(0x[0-9a-f]+)", line)
        if not m:
            continue
        src, tgt = int(m.group(1), 16), int(m.group(3), 16)
        if not (LO <= tgt < HI):
            continue
        fn = which_fn(fns, src)
        owners[fn or f"raw@{src:#x}"].append((src, tgt))

    out = [f"data {LO:#x}-{HI:#x} referenced by:\n"]
    for fn in sorted(owners, key=lambda f: (f is None, f)):
        refs = sorted(owners[fn])
        addrs = ", ".join(f"{a:#x}" for a, _ in refs)
        out.append(f"  {fn}  ({len(refs)} refs: {addrs})")
    out.append(f"\ndistinct consumer functions: {len([f for f in owners if f and not f.startswith('raw@')])}")
    sys.stdout.write("\n".join(out) + "\n")


if __name__ == "__main__":
    main()
