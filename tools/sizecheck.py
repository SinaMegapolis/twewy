#!/usr/bin/env python3
"""Compare a TU's function sizes against the target's.

A per-function match% can look like pure register-allocation noise when the real
problem is upstream: if an earlier function in the same TU is a different size,
every later function's literal pool shifts and all of its `ldr rX, [pc, #off]`
displacements report as mismatches.

    python tools/sizecheck.py src/Debug/Sugata/TinPinSlammer/OtuSceneEntry
"""
import json
import re
import sys

SYMS = "config/usa/arm9/overlays/ov039/symbols.txt"
REPORT = "build/usa/report.json"


def target_sizes():
    out = {}
    for line in open(SYMS):
        m = re.match(r"(\S+) kind:function\(arm,size=(0x[0-9a-f]+)\)", line)
        if m:
            out[m.group(1)] = int(m.group(2), 16)
    return out


def main():
    unit = sys.argv[1]
    tgt = target_sizes()
    rep = json.load(open(REPORT))
    for u in rep["units"]:
        if unit not in u.get("name", ""):
            continue
        print(f"{'function':28} {'target':>8} {'build':>8}  delta  match%")
        for f in sorted(u.get("functions", []), key=lambda x: int(x["address"])):
            n = f["name"]
            t = tgt.get(n)
            c = int(f["size"])
            if t is None:
                print(f"{n:28} {'-':>8} {c:>8}")
                continue
            d = c - t
            flag = "" if d == 0 else "  <-- SIZE MISMATCH"
            print(
                f"{n:28} {t:>8} {c:>8}  {d:>+5}  "
                f"{round(f.get('fuzzy_match_percent') or 0, 2):>6}{flag}"
            )


if __name__ == "__main__":
    main()
