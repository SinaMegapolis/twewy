#!/usr/bin/env python3
"""fold_parts.py -- inline the Shop_pN.c part files back into Shop.c.

Shop.c keeps its SharedPart header include; each `#include "Shop_pN.c"` line is
replaced by the part's contents (minus its own `#include "ShopPart.h"` line).
Run only after all parts compile:
    python tools/fold_parts.py            # rewrites src/Interface/Menu/Shop.c
"""
import re
from pathlib import Path

ROOT = Path(r'E:\Git\twewy')
SHOP = ROOT / 'src' / 'Interface' / 'Menu' / 'Shop.c'
text = SHOP.read_text(encoding='utf-8')
out = []
for line in text.splitlines(keepends=True):
    m = re.match(r'\s*#include "(Shop_p\d+\.c)"\s*$', line)
    if not m:
        out.append(line)
        continue
    part = ROOT / 'src' / 'Interface' / 'Menu' / m.group(1)
    ptext = part.read_text(encoding='utf-8')
    ptext = re.sub(r'^\s*#include "ShopPart.h"\s*\n', '', ptext)
    out.append('// ---- %s ----\n' % m.group(1))
    out.append(ptext.rstrip('\n') + '\n')
SHOP.write_text(''.join(out), encoding='utf-8')
print('folded', SHOP)
