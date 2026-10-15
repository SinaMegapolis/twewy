#!/usr/bin/env python3
"""probe.py -- compile individual functions from a source file in isolation and
diff each against the target Shop.o.  Faster than partdiff for iterating on one
function while a sibling in the same file is still broken.

Usage:
    python tools/probe.py <src.c> [fn ...]
    python tools/probe.py src/Interface/Menu/Shop_p5.c func_ov043_020af6b4
    python tools/probe.py src/Interface/Menu/Shop_p5.c            # all in file
"""
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
    edges = [m.start() for m in ms] + [len(text)]
    out = []
    for i in range(len(ms)):
        out.append((ms[i].group(1), text[edges[i]:edges[i + 1]].rstrip() + '\n'))
    return out


def norm(obj, fn):
    out = subprocess.run(['arm-none-eabi-objdump', '-dr', str(obj), '--disassemble=' + fn],
                         capture_output=True, text=True).stdout
    lines = []
    for line in out.splitlines():
        m = re.match(r'^\s+[0-9a-f]+:\s*(.*)$', line)
        if not m:
            continue
        l = m.group(1).rstrip()
        l = re.sub(r'@\s*[0-9a-f]+.*$', '', l).rstrip()
        l = re.sub(r'<[^>]*>', '<sym>', l)
        l = re.sub(r'\b(b\w*|blx?)\s+[0-9a-f]+\s+<sym>', r'\1 <sym>', l)
        l = re.sub(r'^eb[0-9a-f]{6}\s+bl\s+<sym>$', 'bl <sym>', l)
        lines.append(l.rstrip())
    return lines


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
        f.write_text(INC + blk)
        obj = TMP / (name + '.o')
        r = subprocess.run(['pwsh', '-NoProfile', '-File', 'tools/mwcc.ps1',
                            '-src', str(f.relative_to(ROOT)), '-obj', str(obj.relative_to(ROOT))],
                           capture_output=True, text=True, cwd=ROOT)
        if r.returncode != 0:
            em = re.search(re.escape(name) + r'\.c:(\d+): (.*)', r.stdout + r.stderr)
            print('%-22s COMPILE FAIL%s' % (name, (' @' + em.group(1) + ' ' + em.group(2)) if em else ''))
            continue
        a = norm(TGT, name)
        b = norm(obj, name)
        if not a:
            print('%-22s NO-TARGET' % name)
            continue
        diffs = sum(1 for i in range(max(len(a), len(b)))
                    if (a[i] if i < len(a) else '') != (b[i] if i < len(b) else ''))
        if diffs == 0:
            nmatch += 1
            print('%-22s MATCH' % name)
        else:
            print('%-22s %d/%d rows differ' % (name, diffs, len(a)))
    print('---')
    print('%d / %d match' % (nmatch, len(bl)))


if __name__ == '__main__':
    main()
