#!/usr/bin/env python3
"""gen_shop_protos.py -- regenerate the function prototypes in include/ShopPart.h
from the part files that are currently #include'd (uncommented) in Shop.c.

Cross-part calls need a prototype that matches the callee's definition, otherwise
mwcc creates an implicit `int f()` and then errors when the real definition is
seen.  Run this after enabling a new part in Shop.c, then build:
    python tools/gen_shop_protos.py
    ninja build/usa/src/Interface/Menu/Shop.o
"""
import re
from pathlib import Path

ROOT = Path(r'E:\Git\twewy')
SHOP = ROOT / 'src' / 'Interface' / 'Menu' / 'Shop.c'

included = re.findall(r'^\s*#include "(Shop_p\d+\.c)"', SHOP.read_text(encoding='utf-8'), re.M)

pat = re.compile(r'^([A-Za-z_][A-Za-z0-9_ \*]*?)\s*(func_ov043_[0-9a-f]+)\(([^)]*)\)\s*\{', re.M)
defs = {}
order = []
for name in included:
    p = ROOT / 'src' / 'Interface' / 'Menu' / name
    t = p.read_text(encoding='utf-8', errors='replace')
    for m in pat.finditer(t):
        ret, fname, args = m.group(1).strip(), m.group(2), re.sub(r'\s+', ' ', m.group(3).strip())
        if ret == 'static' or fname in defs:
            continue
        defs[fname] = '%s %s(%s);' % (ret, fname, args)
        order.append(fname)

hdr = (ROOT / 'include' / 'ShopPart.h').read_text(encoding='utf-8')
hdr = re.sub(r'\n// Prototypes for the part files.*?\n#endif', '\n#endif', hdr, flags=re.S)
comment = ('// Prototypes for the part files currently included in Shop.c: %s\n'
           % ', '.join(included))
hdr = hdr.replace('#endif // INTERFACE_MENU_SHOPPART_H',
                  comment + '\n'.join(defs[n] for n in order) +
                  '\n\n#endif // INTERFACE_MENU_SHOPPART_H')
(ROOT / 'include' / 'ShopPart.h').write_text(hdr, encoding='utf-8')
print('prototypes:', len(order), 'from', included)
