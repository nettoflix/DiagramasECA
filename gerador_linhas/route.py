#!/usr/bin/env python3
"""Orthogonal router for the DiagramasECA prerequisite lines.

Reads diagram names/prerequisites from files/disciplinas.txt and the measured widget
geometry (probe output), routes every prerequisite -> diagram edge leaving the
right side of the prerequisite and entering the left side of the dependent
diagram, and writes the JSON consumed by Widget::loadLines().
"""
import json
import math
import random
import sys
from collections import defaultdict

import numpy as np

import os
HERE = os.path.dirname(os.path.abspath(__file__))
PROJECT = os.path.dirname(HERE)                       # raiz do projeto Qt
PROBE = os.environ.get('GEOMETRIA', os.path.join(HERE, 'dados', 'geometria_medida.txt'))

# ----------------------------------------------------------------------------
# Input
# ----------------------------------------------------------------------------
sys.path.insert(0, PROJECT)
from disciplinas import ler_disciplinas  # noqa: E402

lidas = ler_disciplinas()
var2name = {d['id']: d['nome'] for d in lidas}
prereq = {d['id']: d['pre'] for d in lidas if d['pre']}

geo = {}
for line in open(PROBE, encoding='utf-8'):
    if line.startswith('DIAG\t'):
        _, name, row, col, x, y, w, h = line.rstrip('\n').split('\t')
        geo[name] = dict(row=int(row), col=int(col), x=int(x), y=int(y), w=int(w), h=int(h))

assert set(var2name.values()) == set(geo), set(var2name.values()) ^ set(geo)

MARGIN_TOP = 50  # "margin-top: 50px" in Diagram::paintDiagramColor
BOX = {}         # name -> (x0, y0, x1, y1), visible rectangle, exclusive right/bottom
for n, g in geo.items():
    BOX[n] = (g['x'], g['y'] + MARGIN_TOP, g['x'] + g['w'], g['y'] + g['h'])

col_x = {g['col']: g['x'] for g in geo.values()}
row_y = {g['row']: g['y'] for g in geo.values()}
NCOL = max(col_x) + 1
NROW = max(row_y)
occupied = {(g['row'], g['col']): n for n, g in geo.items()}

edges = []
for tgt_var, srcs in prereq.items():
    for s in srcs:
        edges.append((var2name[s], var2name[tgt_var]))
edges.sort(key=lambda e: (geo[e[0]]['col'], geo[e[0]]['row'], geo[e[1]]['col'], geo[e[1]]['row']))
NE = len(edges)
for s, t in edges:
    assert geo[s]['col'] < geo[t]['col'], (s, t)

# ----------------------------------------------------------------------------
# Lanes
# ----------------------------------------------------------------------------
PORT_K = [-2, -1, 0, 1, 2]
PORT_STEP = 18
CH_K = list(range(-4, 5))
CH_STEP = 14
COR_K = [-1, 0, 1]
COR_STEP = 14


def port_y(name, k):
    x0, y0, x1, y1 = BOX[name]
    return y0 + (y1 - y0) // 2 + PORT_STEP * k


def start_x(name):   # same convention as Widget::mousePressEvent (right edge + 2)
    return BOX[name][2] + 2


def end_x(name):     # square cap of the 4px pen reaches the border
    return BOX[name][0] - 3


def chan_x(c, k):    # vertical channel between column c and c+1
    left = col_x[c] + 159
    right = col_x[c + 1]
    return left + (right - left) // 2 + CH_STEP * k


def corr_y(g, k):    # horizontal corridor between row g and g+1 (g=0: under the titles)
    top = row_y[g + 1] + MARGIN_TOP
    return top - 28 + COR_STEP * k


def row_free(r, c_from, c_to):
    return all((r, c) not in occupied for c in range(c_from, c_to + 1))


# ----------------------------------------------------------------------------
# Templates
# ----------------------------------------------------------------------------
def templates(s, t):
    cs, rs = geo[s]['col'], geo[s]['row']
    ct, rt = geo[t]['col'], geo[t]['row']
    out = []
    for k in range(cs, ct):
        if row_free(rs, cs + 1, k) and row_free(rt, k + 1, ct - 1):
            out.append(('T1', k))
    lo, hi = min(rs, rt), max(rs, rt)
    for a in range(cs, ct - 1):
        if not row_free(rs, cs + 1, a):
            continue
        for b in range(a + 1, ct):
            if not row_free(rt, b + 1, ct - 1):
                continue
            for g in range(max(0, lo - 1), min(NROW - 1, hi) + 1):
                out.append(('T2', a, g, b))
    return out


TEMPLATES = [templates(s, t) for s, t in edges]


def n_vlegs(tpl):
    return 1 if tpl[0] == 'T1' else 2


def points(e, st):
    s, t = edges[e]
    tpl, ks, kt, vl, cl = st
    ya, yb = port_y(s, ks), port_y(t, kt)
    xs, xt = start_x(s), end_x(t)
    if tpl[0] == 'T1':
        if ya == yb:
            return [(xs, ya), (xt, yb)]
        X = chan_x(tpl[1], vl[0])
        return [(xs, ya), (X, ya), (X, yb), (xt, yb)]
    _, a, g, b = tpl
    Xa, Xb, Y = chan_x(a, vl[0]), chan_x(b, vl[1]), corr_y(g, cl)
    return [(xs, ya), (Xa, ya), (Xa, Y), (Xb, Y), (Xb, yb), (xt, yb)]


# ----------------------------------------------------------------------------
# Cost
# ----------------------------------------------------------------------------
W_BEND = 8.0
W_LEN = 0.015
W_CROSS = 12.0
W_TOUCH = 150.0
W_CLOSE = 300.0
W_CENTER = 1.5
W_BOX = 5000.0


def segs(pts):
    H, V = [], []
    for (x1, y1), (x2, y2) in zip(pts, pts[1:]):
        if y1 == y2 and x1 != x2:
            H.append((y1, min(x1, x2), max(x1, x2)))
        elif x1 == x2 and y1 != y2:
            V.append((x1, min(y1, y2), max(y1, y2)))
        elif x1 != x2 and y1 != y2:
            raise ValueError('diagonal segment')
    return np.array(H, dtype=float).reshape(-1, 3), np.array(V, dtype=float).reshape(-1, 3)


BOX_ARR = np.array([BOX[n] for n in geo], dtype=float)
BOX_NAMES = list(geo)


def self_cost(e, pts):
    H, V = segs(pts)
    bends = len(pts) - 2
    length = sum(abs(x2 - x1) + abs(y2 - y1) for (x1, y1), (x2, y2) in zip(pts, pts[1:]))
    c = W_BEND * bends + W_LEN * length
    # safety: no segment may enter any box (4px clearance)
    B = BOX_ARR
    for y, xa, xb in H:
        hit = (y >= B[:, 1] - 4) & (y < B[:, 3] + 4) & (xb > B[:, 0] - 1) & (xa < B[:, 2])
        c += W_BOX * hit.sum()
    for x, ya, yb in V:
        hit = (x >= B[:, 0] - 4) & (x < B[:, 2] + 4) & (yb > B[:, 1]) & (ya < B[:, 3])
        c += W_BOX * hit.sum()
    return c


def cross_cost(Ha, Va, Hb, Vb):
    """Pairwise cost between two groups of segments that belong to different edges."""
    c = 0.0
    for H, V in ((Ha, Vb), (Hb, Va)):
        if len(H) and len(V):
            hy, hx1, hx2 = H[:, 0:1], H[:, 1:2], H[:, 2:3]
            vx, vy1, vy2 = V[:, 0], V[:, 1], V[:, 2]
            inter = (vx >= hx1) & (vx <= hx2) & (hy >= vy1) & (hy <= vy2)
            touch = inter & ((vx == hx1) | (vx == hx2) | (hy == vy1) | (hy == vy2))
            c += W_CROSS * (inter & ~touch).sum() + W_TOUCH * touch.sum()
    for A, B in ((Ha, Hb), (Va, Vb)):
        if len(A) and len(B):
            d = np.abs(A[:, 0:1] - B[:, 0])
            ov = np.minimum(A[:, 2:3], B[:, 2]) - np.maximum(A[:, 1:2], B[:, 1])
            close = (d < 10) & (ov > -8)
            c += W_CLOSE * close.sum() + 4 * W_CLOSE * (close & (d == 0) & (ov > 0)).sum()
    return c


def side_cost(ks):
    if not ks:
        return 0.0
    c = W_CENTER * abs(sum(ks))
    if len(set(ks)) != len(ks):
        c += 1000
    return c


class State:
    def __init__(self, st):
        self.st = st
        self.pts = [points(e, st[e]) for e in range(NE)]
        self.seg = [segs(p) for p in self.pts]
        self.self_c = [self_cost(e, self.pts[e]) for e in range(NE)]

    def total(self):
        c = sum(self.self_c)
        for i in range(NE):
            for j in range(i + 1, NE):
                c += cross_cost(*self.seg[i], *self.seg[j])
        c += sum(side_cost(ks) for ks in self.sides().values())
        return c

    def sides(self, only=None):
        d = defaultdict(list)
        for e, (s, t) in enumerate(edges):
            if only is None or ('R', s) in only:
                d[('R', s)].append(self.st[e][1])
            if only is None or ('L', t) in only:
                d[('L', t)].append(self.st[e][2])
        return d


def delta(state, changes):
    """changes: {edge: new_state_tuple}. Returns (delta_cost, new_pts, new_segs, new_self)."""
    E = list(changes)
    Eset = set(E)
    rest = [i for i in range(NE) if i not in Eset]
    Hr = np.concatenate([state.seg[i][0] for i in rest])
    Vr = np.concatenate([state.seg[i][1] for i in rest])
    old, new = 0.0, 0.0
    npts, nseg, nself = {}, {}, {}
    for e in E:
        npts[e] = points(e, changes[e])
        nseg[e] = segs(npts[e])
        nself[e] = self_cost(e, npts[e])
        old += state.self_c[e] + cross_cost(*state.seg[e], Hr, Vr)
        new += nself[e] + cross_cost(*nseg[e], Hr, Vr)
    for i in range(len(E)):
        for j in range(i + 1, len(E)):
            old += cross_cost(*state.seg[E[i]], *state.seg[E[j]])
            new += cross_cost(*nseg[E[i]], *nseg[E[j]])
    affected = set()
    for e in E:
        affected.add(('R', edges[e][0]))
        affected.add(('L', edges[e][1]))
    old += sum(side_cost(v) for v in state.sides(affected).values())
    saved = {e: state.st[e] for e in E}
    for e in E:
        state.st[e] = changes[e]
    new += sum(side_cost(v) for v in state.sides(affected).values())
    for e in E:
        state.st[e] = saved[e]
    return new - old, npts, nseg, nself


def apply(state, changes, npts, nseg, nself):
    for e in changes:
        state.st[e] = changes[e]
        state.pts[e] = npts[e]
        state.seg[e] = nseg[e]
        state.self_c[e] = nself[e]


# ----------------------------------------------------------------------------
# Optimisation
# ----------------------------------------------------------------------------
out_edges = defaultdict(list)
in_edges = defaultdict(list)
for e, (s, t) in enumerate(edges):
    out_edges[s].append(e)
    in_edges[t].append(e)


def initial(rng):
    st = []
    for e, (s, t) in enumerate(edges):
        tpls = TEMPLATES[e]
        best = min(tpls, key=lambda tp: (n_vlegs(tp), rng.random()))
        st.append([best, 0, 0, [rng.choice(CH_K) for _ in range(n_vlegs(best))], 0])
    # spread ports ordered by the vertical position of the other end
    for s, es in out_edges.items():
        es = sorted(es, key=lambda e: (geo[edges[e][1]]['row'], geo[edges[e][1]]['col']))
        ks = centered(len(es))
        for e, k in zip(es, ks):
            st[e][1] = k
    for t, es in in_edges.items():
        es = sorted(es, key=lambda e: (geo[edges[e][0]]['row'], -geo[edges[e][0]]['col']))
        ks = centered(len(es))
        for e, k in zip(es, ks):
            st[e][2] = k
    return [tuple([x[0], x[1], x[2], tuple(x[3]), x[4]]) for x in st]


def centered(n):
    return {1: [0], 2: [-1, 1], 3: [-1, 0, 1], 4: [-2, -1, 1, 2], 5: [-2, -1, 0, 1, 2]}[n]


def random_move(state, rng):
    e = rng.randrange(NE)
    tpl, ks, kt, vl, cl = state.st[e]
    r = rng.random()
    if r < 0.15 and len(TEMPLATES[e]) > 1:
        ntpl = rng.choice(TEMPLATES[e])
        nvl = tuple(rng.choice(CH_K) for _ in range(n_vlegs(ntpl)))
        return {e: (ntpl, ks, kt, nvl, rng.choice(COR_K))}
    if r < 0.30:
        s = edges[e][0]
        others = [f for f in out_edges[s] if f != e]
        free = [k for k in PORT_K if k not in [state.st[f][1] for f in others]]
        if others and rng.random() < 0.5:
            f = rng.choice(others)
            tf = state.st[f]
            return {e: (tpl, tf[1], kt, vl, cl), f: (tf[0], ks, tf[2], tf[3], tf[4])}
        return {e: (tpl, rng.choice(free), kt, vl, cl)}
    if r < 0.45:
        t = edges[e][1]
        others = [f for f in in_edges[t] if f != e]
        free = [k for k in PORT_K if k not in [state.st[f][2] for f in others]]
        if others and rng.random() < 0.5:
            f = rng.choice(others)
            tf = state.st[f]
            return {e: (tpl, ks, tf[2], vl, cl), f: (tf[0], tf[1], kt, tf[3], tf[4])}
        return {e: (tpl, ks, rng.choice(free), vl, cl)}
    if r < 0.90:
        i = rng.randrange(len(vl))
        nvl = list(vl)
        if rng.random() < 0.5:
            nvl[i] = max(CH_K[0], min(CH_K[-1], vl[i] + rng.choice((-1, 1))))
        else:
            nvl[i] = rng.choice(CH_K)
        return {e: (tpl, ks, kt, tuple(nvl), cl)}
    return {e: (tpl, ks, kt, vl, rng.choice(COR_K))}


def neighbours(state, e):
    tpl, ks, kt, vl, cl = state.st[e]
    for ntpl in TEMPLATES[e]:
        if ntpl != tpl:
            n = n_vlegs(ntpl)
            yield {e: (ntpl, ks, kt, tuple(vl[:n]) + (0,) * (n - len(vl)), cl)}
    for k in PORT_K:
        if k != ks:
            yield {e: (tpl, k, kt, vl, cl)}
        if k != kt:
            yield {e: (tpl, ks, k, vl, cl)}
    for i in range(len(vl)):
        for k in CH_K:
            if k != vl[i]:
                nvl = list(vl)
                nvl[i] = k
                yield {e: (tpl, ks, kt, tuple(nvl), cl)}
    for k in COR_K:
        if k != cl:
            yield {e: (tpl, ks, kt, vl, k)}
    s, t = edges[e]
    for f in out_edges[s]:
        if f > e:
            tf = state.st[f]
            yield {e: (tpl, tf[1], kt, vl, cl), f: (tf[0], ks, tf[2], tf[3], tf[4])}
    for f in in_edges[t]:
        if f > e:
            tf = state.st[f]
            yield {e: (tpl, ks, tf[2], vl, cl), f: (tf[0], tf[1], kt, tf[3], tf[4])}


def polish(state):
    improved = True
    while improved:
        improved = False
        for e in range(NE):
            for ch in neighbours(state, e):
                d, a, b, c = delta(state, ch)
                if d < -1e-9:
                    apply(state, ch, a, b, c)
                    improved = True


def anneal(seed, iters):
    rng = random.Random(seed)
    state = State(initial(rng))
    cur = state.total()
    T0, T1 = 30.0, 0.05
    for it in range(iters):
        T = T0 * (T1 / T0) ** (it / iters)
        ch = random_move(state, rng)
        d, a, b, c = delta(state, ch)
        if d < 0 or rng.random() < math.exp(-d / T):
            apply(state, ch, a, b, c)
            cur += d
    polish(state)
    return state, state.total()


def report(state):
    ncross = ntouch = nclose = 0
    for i in range(NE):
        for j in range(i + 1, NE):
            (Ha, Va), (Hb, Vb) = state.seg[i], state.seg[j]
            for H, V in ((Ha, Vb), (Hb, Va)):
                if len(H) and len(V):
                    hy, hx1, hx2 = H[:, 0:1], H[:, 1:2], H[:, 2:3]
                    vx, vy1, vy2 = V[:, 0], V[:, 1], V[:, 2]
                    inter = (vx >= hx1) & (vx <= hx2) & (hy >= vy1) & (hy <= vy2)
                    touch = inter & ((vx == hx1) | (vx == hx2) | (hy == vy1) | (hy == vy2))
                    ncross += (inter & ~touch).sum()
                    ntouch += touch.sum()
            for A, B in ((Ha, Hb), (Va, Vb)):
                if len(A) and len(B):
                    d = np.abs(A[:, 0:1] - B[:, 0])
                    ov = np.minimum(A[:, 2:3], B[:, 2]) - np.maximum(A[:, 1:2], B[:, 1])
                    nclose += ((d < 10) & (ov > -8)).sum()
    bends = sum(len(p) - 2 for p in state.pts)
    return dict(crossings=int(ncross), touches=int(ntouch), close=int(nclose), bends=bends,
                box_hits=int(sum(c >= W_BOX for c in state.self_c)))


def write_json(state, path):
    per = {n: [] for n in geo}
    for e, (s, t) in enumerate(edges):
        per[s].append([{'x': int(x), 'y': int(y)} for x, y in state.pts[e]])
    doc = {'Diagramas': {n: {'isActive': False, 'lines': per[n]} for n in sorted(geo)}}
    with open(path, 'w', encoding='utf-8') as f:
        f.write(json.dumps(doc, indent=4, ensure_ascii=False, sort_keys=True) + '\n')


if __name__ == '__main__':
    iters = int(sys.argv[1]) if len(sys.argv) > 1 else 40000
    seeds = [int(x) for x in sys.argv[2].split(',')] if len(sys.argv) > 2 else [1, 2, 3, 4]
    outp = sys.argv[3] if len(sys.argv) > 3 else os.path.join(HERE, 'saved_v1.json')
    print('edges', NE, 'max out', max(len(v) for v in out_edges.values()),
          'max in', max(len(v) for v in in_edges.values()))
    best = None
    for sd in seeds:
        state, cost = anneal(sd, iters)
        print('seed', sd, 'cost', round(cost, 2), report(state), flush=True)
        if best is None or cost < best[1]:
            best = (state, cost)
    state = best[0]
    for e, (s, t) in enumerate(edges):
        print(f'{s[:28]:28s} -> {t[:28]:28s} {state.st[e][0]} ks={state.st[e][1]} kt={state.st[e][2]} vl={state.st[e][3]} cl={state.st[e][4]}')
    write_json(state, outp)
    print('written', outp, 'cost', round(best[1], 2))
