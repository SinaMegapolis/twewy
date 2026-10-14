#!/usr/bin/env python3
"""Aggregate match stats for the Shop TUs from build/usa/report.json."""
import json

r = json.load(open('build/usa/report.json'))
units = [u for u in r['units'] if u['name'].startswith('src/Interface/Menu/Shop')]
tc = mc = tf = mf = 0
for u in units:
    m = u['measures']
    tc += int(m.get('total_code', 0))
    mc += int(m.get('matched_code', 0))
    tf += int(m.get('total_functions', 0))
    mf += int(m.get('matched_functions', 0))
print('Shop TUs: %d units' % len(units))
print('code matched : %d / %d  = %.2f%%' % (mc, tc, 100.0 * mc / tc))
print('fn matched   : %d / %d  = %.2f%%' % (mf, tf, 100.0 * mf / tf))
# Excluding the huge undecompiled Shop.c dispatcher TU
sub = [u for u in units if u['name'] != 'src/Interface/Menu/Shop']
stc = sum(int(u['measures'].get('total_code', 0)) for u in sub)
smc = sum(int(u['measures'].get('matched_code', 0)) for u in sub)
print('sub-TUs only : %d / %d  = %.2f%%' % (smc, stc, 100.0 * smc / stc))
