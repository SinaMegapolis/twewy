#!/usr/bin/env python3
"""Report the real emitted size of each function against the target's.

objdiff's per-function `size` in report.json is the *target's* size, and its
fuzzy match percent tolerates a length mismatch -- a function missing six
instructions still scores in the high 80s.  That hides a whole class of bug, so
this reads the symbol sizes out of the delinked target object and the current
object directly and prints the difference.

A non-zero delta is the single most useful thing to know before reading a diff:
if the build is short by N instructions, every instruction after the divergence
point is shifted and the diff view shows far more damage than really exists.

    python tools/permsize.py [unit-substring]
"""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from perm_score import read_elf

UNITS = [
    "src/Debug/Sugata/TinPinSlammer/OtuScene",
    "src/Debug/Sugata/TinPinSlammer/OtuSceneEntry",
    "src/Debug/Sugata/TinPinSlammer/OtuVBlank",
    "src/Debug/Sugata/TinPinSlammer/OtuGxInit",
    "src/Debug/Sugata/TinPinSlammer/OtuText",
    "src/Debug/Sugata/TinPinSlammer/OtuMenuText",
]


def sizes(path):
    """{symbol: size} for every sized function symbol in an object."""
    data = open(path, "rb").read()
    _sections, symbols = read_elf(data)
    return {n: s for n, _shndx, _v, s in symbols if s and n.startswith(("func_", "Otu", "Mini"))}


def main():
    filt = sys.argv[1] if len(sys.argv) > 1 else ""
    print(f"{'function':30} {'target':>7} {'build':>7} {'delta':>6}")
    worst = []
    for unit in UNITS:
        if filt and filt not in unit:
            continue
        t_o = Path("build/usa/delinks") / f"{unit}.o"
        c_o = Path("build/usa") / f"{unit}.o"
        if not (t_o.exists() and c_o.exists()):
            continue
        t, c = sizes(t_o), sizes(c_o)
        for name in sorted(t):
            if name not in c or not name.startswith("func_"):
                continue
            d = c[name] - t[name]
            flag = "" if d == 0 else "   <-- SIZE MISMATCH"
            if d:
                worst.append((abs(d), name, t[name], c[name]))
            print(f"{name:30} {t[name]:>7} {c[name]:>7} {d:>+6}{flag}")
    if worst:
        worst.sort(reverse=True)
        print(f"\n{len(worst)} function(s) with a size mismatch; largest: "
              f"{worst[0][1]} target {worst[0][2]} build {worst[0][3]}")


if __name__ == "__main__":
    main()
