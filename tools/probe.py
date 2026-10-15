#!/usr/bin/env python3
"""probe.py -- compile individual functions from a source file in isolation and
score each with objdiff against the target Shop.o.  Works even while sibling
functions in the same file are still broken.

Usage:
    python tools/probe.py <src.c> [fn ...]
    python tools/probe.py src/Interface/Menu/Shop_p5.c func_ov043_020af6b4
    python tools/probe.py src/Interface/Menu/Shop_p5.c            # all in file
"""
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(r'E:\Git\twewy')
TGT = ROOT / 'build' / 'usa' / 'delinks' / 'src' / 'Interface' / 'Menu' / 'Shop.o'
TMP = ROOT / 'build' / 'usa' / 'tmp' / 'probe'
INC = '#include "ShopPart.h"\n\n'

DEF = re.compile(r'^(?:[A-Za-z_][A-Za-z0-9_ \*]*?)\s*(func_ov043_[0-9a-f]+)\([^)]*\)\s*\{', re.M)


def blocks(text):
    ms = list(DEF.finditer(text))
    # Everything before the first function definition is the file-scope prelude
    # (includes, shared typedefs).  prepend it so probe sees file-scope types.
    prelude = text[:ms[0].start()] if ms else ''
    # if probing the folded Shop.c, drop the part includes (they would pull in
    # every function and collide with the one we extract).
    prelude = re.sub(r'(?m)^\s*#include\s+"Shop_p\d+\.c"\s*$', '', prelude)
    edges = [m.start() for m in ms] + [len(text)]
    return [(ms[i].group(1), prelude + text[edges[i]:edges[i + 1]].rstrip() + '\n') for i in range(len(ms))]


def fn_match_percent(json_path, fn):
    d = json.load(open(json_path))
    for side in ('left', 'right'):
        for sym in d.get(side, {}).get('symbols', []):
            if sym.get('name') == fn and 'match_percent' in sym:
                return float(sym['match_percent'])
    return None


def main():
    TMP.mkdir(parents=True, exist_ok=True)
    src = ROOT / sys.argv[1]
    want = set(sys.argv[2:])
    bl = blocks(src.read_text())
    if want:
        bl = [b for b in bl if b[0] in want]
    nmatch = 0
    for name, blk in bl:
        f = TMP / (name + '.c')
        f.write_text(blk)
        obj = TMP / (name + '.o')
        r = subprocess.run(['pwsh', '-NoProfile', '-File', 'tools/mwcc.ps1',
                            '-src', str(f.relative_to(ROOT)), '-obj', str(obj.relative_to(ROOT))],
                           capture_output=True, text=True, cwd=ROOT)
        if r.returncode != 0:
            em = re.search(re.escape(name) + r'\.c:(\d+): (.*)', r.stdout + r.stderr)
            print('%-22s COMPILE FAIL%s' % (name, (' @' + em.group(1) + ' ' + em.group(2)) if em else ''))
            continue
        dj = TMP / (name + '.json')
        subprocess.run([str(ROOT / 'objdiff-cli.exe'), 'diff', '-1', str(TGT), '-2', str(obj),
                        name, '-o', str(dj), '--format', 'json'], capture_output=True, cwd=ROOT)
        pct = fn_match_percent(dj, name)
        if pct is None:
            print('%-22s ?' % name)
        elif pct >= 100.0:
            nmatch += 1
            print('%-22s MATCH' % name)
        else:
            print('%-22s %.2f%%' % (name, pct))
    print('---')
    print('%d / %d match' % (nmatch, len(bl)))


if __name__ == '__main__':
    main()
