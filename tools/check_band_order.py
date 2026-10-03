#!/usr/bin/env python3
"""Check that each ov039 region file declares symbols before it uses them.

The overlay is split into one translation unit per contiguous `.text` range
(OtuFieldAccess.c, OtuBoard.c, ...). Within a file, a function that calls a
function defined further down still needs a declaration ahead of it, or C99
implicitly declares it `int (...)` and the real definition then collides. The
shared header OtuFieldAccessShared.h declares everything the regions share up
front, so this check should always pass -- it is here to catch a declaration
that is removed or a file that stops including the shared header.

    python tools/check_band_order.py

Zero problems is the pass condition.
"""
import glob
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BAND_DIR = os.path.join(ROOT, 'src', 'Debug', 'Sugata', 'TinPinSlammer')
SHARED = os.path.join(BAND_DIR, 'OtuFieldAccessShared.h')

NAME = r'((?:func_ov039_[0-9a-f]+)|(?:data_(?:ov039_)?[0-9a-z]+))'
DEF = re.compile(r'^[A-Za-z_][\w \t\*]*?\b' + NAME + r'\s*\([^;{]*?\)\s*\{')
DECL = re.compile(r'^[A-Za-z_][\w \t\*]*?\b' + NAME +
                  r'(?:\s*\([^;{]*?\)|\s*\[[^\]]*\](?:\s*\[[^\]]*\])?)\s*;')
# Indented forms too: declarations written inside a function body. A statement
# such as `return table[i];` matches this shape as well, so anything starting
# with a keyword is excluded.
DECL_IN = re.compile(r'^[ \t]+(?!return\b|if\b|for\b|while\b|switch\b|case\b|goto\b|break\b|'
                     r'continue\b|else\b|do\b)(?:extern[ \t]+)?[A-Za-z_][\w \t\*]*\b' + NAME +
                     r'[ \t]*(?:\([^;{]*?\)|\[[^\]]*\](?:[ \t]*\[[^\]]*\])?)[ \t]*;')
ANY = re.compile(r'\b' + NAME + r'\b')
BLOCK = re.compile(r'/\*.*?\*/', re.S)
LINE_COMMENT = re.compile(r'//.*$')


def blank_comments(t):
    """Blank out comments, preserving newlines so line offsets survive."""
    return BLOCK.sub(lambda m: ''.join('\n' if c == '\n' else ' ' for c in m.group(0)), t)


def region_files():
    for p in sorted(glob.glob(os.path.join(BAND_DIR, 'Otu*.c'))):
        if 'OtuFieldAccessShared.h' in open(p, encoding='latin-1', errors='replace').read():
            yield p


def check_one(path):
    declared_at, first_use = {}, {}
    decl_site, use_site = {}, {}
    off = 0
    for where, p in (('OtuFieldAccessShared.h', SHARED), (os.path.basename(path), path)):
        t = blank_comments(open(p, encoding='latin-1', errors='replace').read())
        for raw in t.split('\n'):
            line = LINE_COMMENT.sub('', raw)
            m = DEF.match(line) or DECL.match(line) or DECL_IN.match(line)
            if m:
                declared_at.setdefault(m.group(1), off)
                decl_site.setdefault(m.group(1), where)
            else:
                for n in set(ANY.findall(line)):
                    if n not in first_use:
                        first_use[n] = off
                        use_site[n] = (where, line.strip()[:76])
            off += len(raw) + 1
    late = sorted((n for n, o in first_use.items()
                   if n in declared_at and declared_at[n] > o),
                  key=lambda n: first_use[n])
    return late, len(first_use), len(declared_at), use_site, decl_site


def main():
    bad = 0
    for path in region_files():
        late, used, declared, use_site, decl_site = check_one(path)
        name = os.path.basename(path)
        if not late:
            print('%-22s source order OK (%d symbols used, %d declared)'
                  % (name, used, declared))
            continue
        bad += 1
        print('\n%-22s %d symbol(s) used before they are declared:'
              % (name, len(late)))
        for n in late:
            print('  %-24s used in %-22s declared in %s'
                  % (n, use_site[n][0], decl_site[n]))
            print('  %-24s   %s' % ('', use_site[n][1]))
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
