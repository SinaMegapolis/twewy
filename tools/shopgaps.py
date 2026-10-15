#!/usr/bin/env python3
"""List Shop units sorted by remaining unmatched code bytes."""
import json

r = json.load(open('build/usa/report.json'))
rows = []
for u in r['units']:
    if not u['name'].startswith('src/Interface/Menu/Shop'):
        continue
    m = u['measures']
    tc = int(m.get('total_code', 0))
    mc = int(m.get('matched_code', 0))
    rows.append((tc - mc, u['name'], mc, tc))
rows.sort(reverse=True)
todo = 0
for gap, name, mc, tc in rows:
    if gap == 0:
        continue
    print('%-48s %6d / %6d  (gap %d)' % (name.replace('src/Interface/Menu/Shop/', ''), mc, tc, gap))
    todo += gap
print('remaining unmatched code bytes: %d' % todo)
