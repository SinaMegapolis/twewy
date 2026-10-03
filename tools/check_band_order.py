#!/usr/bin/env python3
"""Check that OtuFieldAccess.c's source order still holds.

OtuFieldAccess.c is one translation unit that carries the whole overlay region's
function groups, in the order their dependencies require: a file that calls a
function defined further down needs that function's declaration to come first.
Nothing in C enforces that, and getting it wrong does not fail at the point of
the mistake -- it fails much later as a wall of `redeclared` and `illegal access
to local variable` errors. That has happened twice, at 138 errors and 67.

(These groups used to be fifteen .inc files `#include`d at the bottom of the
file; they are now inlined directly, which is why this tool no longer follows an
`#include` list.)

So: this walks the translation unit in the order the compiler sees it and
reports any symbol whose first use precedes its first declaration or definition.
Zero is the pass condition.

    python tools/check_band_order.py
"""
import glob
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BAND_DIR = os.path.join(ROOT, 'src', 'Debug', 'Sugata', 'TinPinSlammer')
FIELD = os.path.join(BAND_DIR, 'OtuFieldAccess.c')
HEADER = os.path.join(ROOT, 'include', 'Debug', 'Sugata', 'TinPinSlammer.h')

NAME = r'((?:func_ov039_[0-9a-f]+)|(?:data_(?:ov039_)?[0-9a-z]+))'
DEF = re.compile(r'^[A-Za-z_][\w \t\*]*?\b' + NAME + r'\s*\([^;{]*?\)\s*\{')
DECL = re.compile(r'^[A-Za-z_][\w \t\*]*?\b' + NAME +
                  r'(?:\s*\([^;{]*?\)|\s*\[[^\]]*\](?:\s*\[[^\]]*\])?)\s*;')
# Indented forms too: declarations written inside a function body. A statement
# such as `return table[i];` matches this shape as well, so anything starting
# with a keyword is excluded -- an earlier version of this check deleted twelve
# return statements on the strength of exactly that mistake.
DECL_IN = re.compile(r'^[ \t]+(?!return\b|if\b|for\b|while\b|switch\b|case\b|goto\b|break\b|'
                     r'continue\b|else\b|do\b)(?:extern[ \t]+)?[A-Za-z_][\w \t\*]*\b' + NAME +
                     r'[ \t]*(?:\([^;{]*?\)|\[[^\]]*\](?:[ \t]*\[[^\]]*\])?)[ \t]*;')
ANY = re.compile(r'\b' + NAME + r'\b')
BLOCK = re.compile(r'/\*.*?\*/', re.S)
LINE_COMMENT = re.compile(r'//.*$')


def blank_comments(t):
    """Blank out comments, preserving newlines so line offsets survive.

    Per-line stripping does not work: a block comment spanning ten lines leaves
    its middle lines as bare `* ...`, and every function named in a comment then
    reads as a call site.
    """
    return BLOCK.sub(lambda m: ''.join('\n' if c == '\n' else ' ' for c in m.group(0)), t)


def sources():
    # The old `#include "Otu*.inc"` bands are inlined now; there is only the
    # header and the single translation unit to walk.
    yield 'TinPinSlammer.h', HEADER
    yield 'OtuFieldAccess.c', FIELD


def main():
    declared_at, first_use = {}, {}
    decl_site, use_site = {}, {}
    # ONE running offset across the whole TU. Resetting per file made every
    # header declaration look later than everything in a following file, and
    # reported 29 problems that did not exist.
    off = 0
    for where, path in sources():
        t = blank_comments(open(path, encoding='latin-1', errors='replace').read())
        for raw in t.split('\n'):
            line = LINE_COMMENT.sub('', raw)
            m = DEF.match(line) or DECL.match(line) or DECL_IN.match(line)
            if m:
                n = m.group(1)
                declared_at.setdefault(n, off)
                decl_site.setdefault(n, where)
            else:
                for n in set(ANY.findall(line)):
                    if n not in first_use:
                        first_use[n] = off
                        use_site[n] = (where, line.strip()[:76])
            off += len(raw) + 1

    late = sorted((n for n, o in first_use.items()
                   if n in declared_at and declared_at[n] > o),
                  key=lambda n: first_use[n])

    print('OtuFieldAccess TU: %d symbols used, %d declared or defined'
          % (len(first_use), len(declared_at)))
    if not late:
        print('source order OK: every call has a declaration ahead of it')
        return 0

    print('\n%d symbol(s) used before they are declared. The source order in'
          % len(late))
    print('OtuFieldAccess.c is wrong, or a declaration has been removed:\n')
    for n in late:
        print('  %-24s used in %-22s declared in %s'
              % (n, use_site[n][0], decl_site[n]))
        print('  %-24s   %s' % ('', use_site[n][1]))
    return 1


if __name__ == '__main__':
    sys.exit(main())