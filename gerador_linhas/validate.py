#!/usr/bin/env python3
"""Independent check of a saved.txt against widget.cpp + measured geometry."""
import json
import sys
from collections import defaultdict

import route as R

path = sys.argv[1]
doc = json.load(open(path, encoding='utf-8'))
D = doc['Diagramas']
problems = []

if set(D) != set(R.geo):
    problems.append(f'diagram names differ: {set(D) ^ set(R.geo)}')

expected = defaultdict(set)
for s, t in R.edges:
    expected[s].add(t)

left_of = {}
for n, (x0, y0, x1, y1) in R.BOX.items():
    left_of[n] = (x0, y0, x1, y1)

segs_by_src = defaultdict(list)
for s, obj in D.items():
    lines = obj['lines']
    if len(lines) > 8:
        problems.append(f'{s}: {len(lines)} lines > 8 ConnectingLine slots')
    if not isinstance(obj['isActive'], bool):
        problems.append(f'{s}: isActive não é booleano')
    found = set()
    for pts in lines:
        P = [(p['x'], p['y']) for p in pts]
        if len(P) < 2:
            problems.append(f'{s}: line with <2 points')
            continue
        x0, y0, x1, y1 = R.BOX[s]
        if not (P[0][0] == x1 + 2 and y0 <= P[0][1] < y1):
            problems.append(f'{s}: line does not start at its right edge: {P[0]}')
        # which box does it end at?
        tgt = [n for n, (a, b, c, d) in R.BOX.items() if P[-1][0] == a - 3 and b <= P[-1][1] < d]
        if len(tgt) != 1:
            problems.append(f'{s}: line end {P[-1]} is not at a left edge')
            continue
        found.add(tgt[0])
        for (xa, ya), (xb, yb) in zip(P, P[1:]):
            if xa != xb and ya != yb:
                problems.append(f'{s}: diagonal segment')
            if xb < xa:
                problems.append(f'{s}->{tgt[0]}: goes backwards')
            segs_by_src[s].append((xa, ya, xb, yb))
            # box intrusion: the 2px band around every box must stay clear
            # (ports sit at right edge + 2 and left edge - 3, just outside it)
            lo_x, hi_x = min(xa, xb), max(xa, xb)
            lo_y, hi_y = min(ya, yb), max(ya, yb)
            for n, (a, b, c, d) in R.BOX.items():
                if hi_x >= a - 2 and lo_x < c + 2 and hi_y >= b - 2 and lo_y < d + 2:
                    problems.append(f'{s}->{tgt[0]}: segment {(xa, ya, xb, yb)} touches box {n}')
    if found != expected.get(s, set()):
        problems.append(f'{s}: targets {found} != prerequisites-of {expected.get(s, set())}')
    if len(found) != len(lines):
        problems.append(f'{s}: duplicate lines to same target')

# different sources: overlap / near-parallel / false continuation / crossings
H = defaultdict(list)
V = defaultdict(list)
for s, L in segs_by_src.items():
    for xa, ya, xb, yb in L:
        if ya == yb:
            H[s].append((ya, min(xa, xb), max(xa, xb)))
        else:
            V[s].append((xa, min(ya, yb), max(ya, yb)))
srcs = list(segs_by_src)
cross = bad = 0
for i in range(len(srcs)):
    for j in range(i + 1, len(srcs)):
        a, b = srcs[i], srcs[j]
        for A, B in ((H[a], H[b]), (V[a], V[b])):
            for k1, p1, q1 in A:
                for k2, p2, q2 in B:
                    if abs(k1 - k2) < 10 and min(q1, q2) - max(p1, p2) > -45:
                        bad += 1
                        problems.append(f'parallel/collinear conflict {a[:20]} vs {b[:20]} at {k1},{k2}')
        for Hs, Vs in ((H[a], V[b]), (H[b], V[a])):
            for y, x1, x2 in Hs:
                for x, y1, y2 in Vs:
                    if x1 <= x <= x2 and y1 <= y <= y2:
                        if x in (x1, x2) or y in (y1, y2):
                            problems.append(f'T-touch between different sources at {(x, y)}')
                        cross += 1

total_lines = sum(len(o['lines']) for o in D.values())
print(f'lines={total_lines} expected={len(R.edges)} crossings={cross} conflicts={bad}')
print('max lines per diagram:', max(len(o['lines']) for o in D.values()))
print('PROBLEMS:' if problems else 'OK: no problems')
for p in sorted(set(problems)):
    print('  ', p)
