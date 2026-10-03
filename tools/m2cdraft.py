#!/usr/bin/env python3
"""m2cdraft.py -- generate an m2c first draft for a range of ov039 functions.

m2c (github.com/matt-kempster/m2c) reads the repo's own dsd disassembly
directly, so there is no need to hand-transcribe.  This wraps the two commands
the repo already expects:

    tools/m2ctx.py <header-or-c> -D REGION_USA -f <ctx>   # preprocess context
    m2c.py -t arm --context <ctx> -f <fn>... ov039_4.s     # draft C

The context is generated once and cached, so subsequent calls are just m2c.

    python tools/m2cdraft.py --ctx-from include/Debug/Sugata/TinPinSlammer.h
    python tools/m2cdraft.py -f 02082724 -f 0208273c
    python tools/m2cdraft.py --range 0x020824a0:0x02082800 > draft.c

Note on range selection: m2c's -f takes a function *name*, not an address, so
`--range` resolves the addresses through the overlay's symbols.txt first.
"""
import argparse
import os
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
M2C = Path(os.environ.get("M2C_DIR", r"C:\Users\COMIRAN\AppData\Local\Temp\opencode\m2c"))
ASM = ROOT / "build" / "usa" / "asm" / "ov039_4.s"
SYMS = ROOT / "config" / "usa" / "arm9" / "overlays" / "ov039" / "symbols.txt"
WORK = Path(os.environ.get("M2C_WORK", r"C:\Users\COMIRAN\AppData\Local\Temp\opencode\ov039"))


def build_context(src_rel):
    """Run tools/m2ctx.py to preprocess a header into a decomp.me/m2c context."""
    WORK.mkdir(parents=True, exist_ok=True)
    ctx = WORK / ("ctx_" + Path(src_rel).stem + ".c")
    if not ctx.exists():
        cmd = [sys.executable, str(ROOT / "tools" / "m2ctx.py"), str(ROOT / src_rel),
               "-f", str(ctx), "-D", "REGION_USA", "-v"]
        p = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
        if not ctx.exists():
            sys.stderr.write(p.stdout + p.stderr)
            sys.exit(1)
    return ctx


def load_symbols():
    rows = []
    for line in open(SYMS):
        m = re.match(r"(\S+) kind:function\(arm,size=(0x[0-9a-f]+)\) addr:(0x[0-9a-f]+)", line)
        if m:
            rows.append((int(m.group(3), 16), int(m.group(2), 16), m.group(1)))
    rows.sort()
    return rows


def resolve(lo, hi):
    out = []
    for addr, size, name in load_symbols():
        if lo <= addr < hi:
            out.append(name)
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("-f", "--function", action="append", default=[],
                    help="function name or bare hex suffix (e.g. 02082724); repeatable")
    ap.add_argument("--range", help="lo:hi address range, e.g. 0x020824a0:0x02082800")
    ap.add_argument("--ctx-from", default="include/Debug/Sugata/TinPinSlammer.h",
                    help="source file to preprocess into the m2c context")
    ap.add_argument("--no-context", action="store_true")
    ap.add_argument("--globals", default="used", choices=["all", "used", "none"])
    a = ap.parse_args()

    if not ASM.exists():
        sys.exit(f"{ASM} missing -- run `ninja dis` (about a minute)")

    names = []
    for f in a.function:
        f = f.strip()
        if re.fullmatch(r"[0-9a-fA-F]{8}", f):
            names.append("func_ov039_" + f)
        else:
            names.append(f)
    if a.range:
        lo, hi = (int(x, 0) for x in a.range.split(":"))
        names += resolve(lo, hi)
    # de-dupe, keep order
    seen, uniq = set(), []
    for n in names:
        if n not in seen:
            seen.add(n)
            uniq.append(n)

    cmd = [sys.executable, str(M2C / "m2c.py"), "-t", "arm", "--globals", a.globals]
    if not a.no_context:
        cmd += ["--context", str(build_context(a.ctx_from))]
    for n in uniq:
        cmd += ["-f", n]
    cmd.append(str(ASM))

    p = subprocess.run(cmd, cwd=M2C, capture_output=True, text=True, encoding="utf-8",
                       errors="replace")
    sys.stdout.write(p.stdout)
    if p.returncode != 0:
        sys.stderr.write(p.stderr)


if __name__ == "__main__":
    main()
