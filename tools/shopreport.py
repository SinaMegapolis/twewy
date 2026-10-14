#!/usr/bin/env python3
"""List Shop units with per-function fuzzy scores, sorted worst-first."""
import json, sys, os

path = os.path.join('build', 'usa', 'report.json')
r = json.load(open(path))
prefix = 'src/Interface/Menu/Shop'
only = sys.argv[1] if len(sys.argv) > 1 else None
for u in r['units']:
    if not u['name'].startswith(prefix):
        continue
    if only and only not in u['name']:
        continue
    m = u['measures']
    g = lambda k, d='0': m.get(k, d)
    print('==== %-48s fns=%2s ok=%2s code=%s/%s (%.1f%%) data=%.1f%%' % (
        u['name'], g('total_functions'), g('matched_functions'),
        g('matched_code'), g('total_code'), float(g('matched_code_percent')),
        float(g('matched_data_percent', '0'))))
    for f in sorted(u['functions'], key=lambda x: float(x.get('fuzzy_match_percent', 100))):
        print('   %-42s %6s  %5.1f%%' % (f['name'], f['size'], float(f.get('fuzzy_match_percent', 100))))
