#!/usr/bin/env python3
"""sweep2.py <file.c> <unit-under-src> <function> <variants.json>
Whole-function source variants scored with the repo's fndiff.ps1 (PowerShell).
Variants are complete function definitions (strings).  Best written back + rebuilt.
"""
import sys
import json
import re
import subprocess

path, unit, fn, vfile = sys.argv[1:5]
variants = json.load(open(vfile))
orig = open(path, 'rb').read().decode('latin1')

m = None
for cand in re.finditer(r'(// Nonmatching[^\n]*\n)?((static )?[\w\*]+[ \*]+' + re.escape(fn) + r'\()', orig):
    brace, semi = orig.find('{', cand.end()), orig.find(';', cand.end())
    if brace != -1 and (semi == -1 or brace < semi):
        m = cand
        break
assert m, 'definition not found'
start = m.start()
body_end = orig.find('\n}\n', m.end())
if body_end == -1:
    body_end = orig.find('\n}', m.end())
    end = body_end + 2
else:
    end = body_end + 3


def write(body):
    open(path, 'wb').write((orig[:start] + body.rstrip('\n') + '\n' + orig[end:]).encode('latin1'))


def score():
    r = subprocess.run(['pwsh', '-NoProfile', '-File', 'tools/fndiff.ps1', unit, fn],
                       capture_output=True, text=True)
    out = r.stdout
    if 'build failed' in out:
        return None
    mm = re.search(r'(\d+) differing rows', out)
    if 'MATCH' in out:
        return 0
    return int(mm.group(1)) if mm else None


results = []
for i, body in enumerate(variants):
    write(body)
    s = score()
    results.append((float('inf') if s is None else s, i))
    print('variant %d: %s' % (i, s), flush=True)
    if s == 0:
        break
results.sort()
best = results[0][1]
write(variants[best])
print('best: variant %d (%s differing rows); now %s' % (best, results[0][0], score()))
