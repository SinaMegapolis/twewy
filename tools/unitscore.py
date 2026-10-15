#!/usr/bin/env python3
"""unitscore.py <unit-name> -- per-function objdiff match% summary for one unit,
using the already-built base object.  Quicker than a full report.json regen."""
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(r'E:\Git\twewy')
unit = sys.argv[1]
out = ROOT / 'build' / 'usa' / 'tmp' / 'unitscore.json'
out.parent.mkdir(parents=True, exist_ok=True)
subprocess.run([str(ROOT / 'objdiff-cli.exe'), 'diff', '-p', '.', '-u', unit,
                '-o', str(out), '--format', 'json'], cwd=ROOT, capture_output=True)
d = json.load(open(out))
syms = d.get('left', {}).get('symbols', [])
total = matched = 0
bad = []
for s in syms:
    if 'match_percent' not in s or s.get('kind') not in ('SYMBOL_FUNCTION', None):
        continue
    size = int(s.get('size') or 0)
    total += size
    pct = float(s['match_percent'])
    if pct >= 100.0:
        matched += size
    else:
        bad.append((pct, size, s['name']))
print('%s: %d / %d = %.2f%%  (%d fns, %d matched)' % (
    unit, matched, total, 100.0 * matched / total if total else 0,
    len(syms), len(syms) - len(bad)))
for pct, size, name in sorted(bad)[:40]:
    print('  %7.2f%% %5d  %s' % (pct, size, name))
