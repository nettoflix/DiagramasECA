#!/usr/bin/env python3
"""Trunk-style router: every prerequisite has a single exit port and a vertical
trunk from which one branch goes to each dependent diagram. Lines of the same
source may overlap (they always share the same colour in the app); lines of
different sources may not."""
import math
import random
import sys
from collections import defaultdict

import numpy as np

import route as R

edges, NE, geo = R.edges, R.NE, R.geo
SOURCES = sorted({s for s, _ in edges}, key=lambda n: (geo[n]['col'], geo[n]['row']))
edges_of_net = defaultdict(list)
for e, (s, t) in enumerate(edges):
    edges_of_net[s].append(e)
in_edges = R.in_edges

W_BEND = 8.0
W_LEN = 0.015
W_CROSS = 12.0
W_TOUCH = 150.0
W_CLOSE = 300.0
W_CENTER = 1.5
W_TRUNK = 0.3
W_CORR = 0.6   # keep corridor lines in the middle of the gap between rows
W_LANE = 0.15  # mild pull of non-trunk vertical legs towards the channel centre
W_BOX = 5000.0
W_CROSS_NEAR = 10.0   # extra for a crossing right next to a box side
W_NEAR = 3.0          # soft cost for long parallel runs of different trees
NEAR_D = 32
W_GAP = 150.0
W_GAP2 = 25.0
BZ = R.BOX_ARR


def near_box(px, py):
    x, y = px[:, None], py[:, None]
    inY = (y >= BZ[:, 1] - 6) & (y <= BZ[:, 3] + 6)
    nearL = (x >= BZ[:, 0] - 36) & (x < BZ[:, 0])
    nearR = (x > BZ[:, 2]) & (x <= BZ[:, 2] + 36)
    return int((inY & (nearL | nearR)).any(axis=1).sum())


def pair_cost(Ha, Va, Hb, Vb):
    c = 0.0
    for H, V in ((Ha, Vb), (Hb, Va)):
        if len(H) and len(V):
            hy, hx1, hx2 = H[:, 0:1], H[:, 1:2], H[:, 2:3]
            vx, vy1, vy2 = V[:, 0], V[:, 1], V[:, 2]
            inter = (vx >= hx1) & (vx <= hx2) & (hy >= vy1) & (hy <= vy2)
            touch = inter & ((vx == hx1) | (vx == hx2) | (hy == vy1) | (hy == vy2))
            cr = inter & ~touch
            n = cr.sum()
            c += W_CROSS * n + W_TOUCH * touch.sum()
            if n:
                ii, jj = np.nonzero(cr)
                c += W_CROSS_NEAR * near_box(V[jj, 0], H[ii, 0])
    for A, B in ((Ha, Hb), (Va, Vb)):
        if len(A) and len(B):
            d = np.abs(A[:, 0:1] - B[:, 0])
            ov = np.minimum(A[:, 2:3], B[:, 2]) - np.maximum(A[:, 1:2], B[:, 1])
            close = (d < 10) & (ov > -8)
            c += W_CLOSE * close.sum() + 4 * W_CLOSE * (close & (d == 0) & (ov > 0)).sum()
            gap = (d < 10) & (ov <= -8) & (ov > -45)   # looks like one continuous line
            gap2 = (d < 10) & (ov <= -45) & (ov > -100)
            c += W_GAP * gap.sum() + W_GAP2 * gap2.sum()
            near = (d >= 10) & (d < NEAR_D) & (ov > 0)
            c += W_NEAR * (np.where(near, ov * (NEAR_D - d) / NEAR_D, 0)).sum() / 100
    return c


def edge_points(e, S, E):
    s, t = edges[e]
    ps, tr = S[s]
    tpl, kt, vl, cl = E[e]
    cs = geo[s]['col']
    ya, yb = R.port_y(s, ps), R.port_y(t, kt)
    xs, xt = R.start_x(s), R.end_x(t)
    if tpl[0] == 'T1':
        if ya == yb:
            return [(xs, ya), (xt, yb)]
        k = tpl[1]
        X = R.chan_x(k, tr if k == cs else vl[0])
        return [(xs, ya), (X, ya), (X, yb), (xt, yb)]
    _, a, g, b = tpl
    Xa = R.chan_x(a, tr if a == cs else vl[0])
    Xb = R.chan_x(b, vl[1])
    Y = R.corr_y(g, cl)
    return [(xs, ya), (Xa, ya), (Xa, Y), (Xb, Y), (Xb, yb), (xt, yb)]


def merge(d):
    out = []
    for key, iv in d.items():
        iv.sort()
        a, b = iv[0]
        for c, dd in iv[1:]:
            if c <= b:
                b = max(b, dd)
            else:
                out.append((key, a, b))
                a, b = c, dd
        out.append((key, a, b))
    return np.array(out, dtype=float).reshape(-1, 3)


class Net:
    __slots__ = ('H', 'V', 'cost')


def build_net(n, S, E):
    Hs, Vs, corners = defaultdict(list), defaultdict(list), set()
    for e in edges_of_net[n]:
        p = edge_points(e, S, E)
        for (x1, y1), (x2, y2) in zip(p, p[1:]):
            if y1 == y2:
                Hs[y1].append((min(x1, x2), max(x1, x2)))
            else:
                Vs[x1].append((min(y1, y2), max(y1, y2)))
        corners.update(p[1:-1])
    net = Net()
    net.H, net.V = merge(Hs), merge(Vs)
    H, V = net.H, net.V
    c = W_BEND * len(corners)
    c += W_LEN * ((H[:, 2] - H[:, 1]).sum() + (V[:, 2] - V[:, 1]).sum())
    c += W_TRUNK * abs(S[n][1])
    for e in edges_of_net[n]:
        tpl, kt, vl, cl = E[e]
        if tpl[0] == 'T2':
            c += W_CORR * abs(cl) + W_LANE * abs(vl[1]) + (W_LANE * abs(vl[0]) if tpl[1] != geo[n]['col'] else 0)
        elif tpl[1] != geo[n]['col']:
            c += W_LANE * abs(vl[0])
    c += W_CENTER * abs(S[n][0])
    # "+" crossings inside the same tree (junctions/corners are fine)
    if len(H) and len(V):
        hy, hx1, hx2 = H[:, 0:1], H[:, 1:2], H[:, 2:3]
        vx, vy1, vy2 = V[:, 0], V[:, 1], V[:, 2]
        c += W_CROSS * ((vx > hx1) & (vx < hx2) & (hy > vy1) & (hy < vy2)).sum()
    # parallel pieces of the same tree that are close but not merged
    for A in (H, V):
        if len(A) > 1:
            d = np.abs(A[:, 0:1] - A[:, 0])
            ov = np.minimum(A[:, 2:3], A[:, 2]) - np.maximum(A[:, 1:2], A[:, 1])
            bad = (d < 10) & (ov > -8)
            np.fill_diagonal(bad, False)
            c += W_CLOSE * bad.sum() / 2
    B = R.BOX_ARR
    for y, xa, xb in H:
        c += W_BOX * ((y >= B[:, 1] - 4) & (y < B[:, 3] + 4) & (xb > B[:, 0] - 1) & (xa < B[:, 2])).sum()
    for x, ya, yb in V:
        c += W_BOX * ((x >= B[:, 0] - 4) & (x < B[:, 2] + 4) & (yb > B[:, 1]) & (ya < B[:, 3])).sum()
    net.cost = c
    return net


def in_side_cost(t, E):
    ks = [E[e][1] for e in in_edges[t]]
    c = W_CENTER * abs(sum(ks))
    if len(set(ks)) != len(ks):
        c += 1000
    return c


class State:
    def __init__(self, S, E):
        self.S, self.E = S, E
        self.nets = {n: build_net(n, S, E) for n in SOURCES}

    def total(self):
        c = sum(n.cost for n in self.nets.values())
        L = SOURCES
        for i in range(len(L)):
            for j in range(i + 1, len(L)):
                a, b = self.nets[L[i]], self.nets[L[j]]
                c += pair_cost(a.H, a.V, b.H, b.V)
        c += sum(in_side_cost(t, self.E) for t in in_edges)
        return c


def delta(st, dS, dE):
    """dS: {source: (ps, tr)}, dE: {edge: (tpl, kt, vl, cl)}."""
    N = set(dS) | {edges[e][0] for e in dE}
    T = {edges[e][1] for e in dE}
    rest = [n for n in SOURCES if n not in N]
    Hr = np.concatenate([st.nets[n].H for n in rest])
    Vr = np.concatenate([st.nets[n].V for n in rest])
    S2 = dict(st.S)
    S2.update(dS)
    E2 = list(st.E)
    for e, v in dE.items():
        E2[e] = v
    new_nets = {n: build_net(n, S2, E2) for n in N}
    old = new = 0.0
    for n in N:
        o, w = st.nets[n], new_nets[n]
        old += o.cost + pair_cost(o.H, o.V, Hr, Vr)
        new += w.cost + pair_cost(w.H, w.V, Hr, Vr)
    NL = list(N)
    for i in range(len(NL)):
        for j in range(i + 1, len(NL)):
            a, b = st.nets[NL[i]], st.nets[NL[j]]
            old += pair_cost(a.H, a.V, b.H, b.V)
            a, b = new_nets[NL[i]], new_nets[NL[j]]
            new += pair_cost(a.H, a.V, b.H, b.V)
    for t in T:
        old += in_side_cost(t, st.E)
        new += in_side_cost(t, E2)
    return new - old, (S2, E2, new_nets)


def apply(st, payload):
    S2, E2, new_nets = payload
    st.S, st.E = S2, E2
    st.nets.update(new_nets)


def centered(n):
    return {1: [0], 2: [-1, 1], 3: [-1, 0, 1], 4: [-2, -1, 1, 2], 5: [-2, -1, 0, 1, 2]}[n]


def initial(rng):
    S = {s: (0, rng.choice(R.CH_K)) for s in SOURCES}
    E = []
    for e in range(NE):
        tpls = R.TEMPLATES[e]
        best = min(tpls, key=lambda tp: (R.n_vlegs(tp), rng.random()))
        E.append([best, 0, (rng.choice(R.CH_K), rng.choice(R.CH_K)), 0])
    for t, es in in_edges.items():
        es = sorted(es, key=lambda e: (geo[edges[e][0]]['row'], -geo[edges[e][0]]['col']))
        for e, k in zip(es, centered(len(es))):
            E[e][1] = k
    return S, [tuple(x) for x in E]


def align_move(st, e, k):
    s, t = edges[e]
    ps, tr = st.S[s]
    tpl, kt, vl, cl = st.E[e]
    dE = {e: (tpl, k, vl, cl)}
    for f in in_edges[t]:
        if f != e and st.E[f][1] == k:
            tf = st.E[f]
            dE[f] = (tf[0], kt, tf[2], tf[3])
    return {s: (k, tr)}, dE


def random_move(st, rng):
    if rng.random() < 0.08:
        return align_move(st, rng.randrange(NE), rng.choice(R.PORT_K))
    if rng.random() < 0.25:
        s = rng.choice(SOURCES)
        ps, tr = st.S[s]
        if rng.random() < 0.4:
            return {s: (rng.choice(R.PORT_K), tr)}, {}
        ntr = max(R.CH_K[0], min(R.CH_K[-1], tr + rng.choice((-1, 1)))) if rng.random() < 0.5 else rng.choice(R.CH_K)
        return {s: (ps, ntr)}, {}
    e = rng.randrange(NE)
    tpl, kt, vl, cl = st.E[e]
    r = rng.random()
    if r < 0.2 and len(R.TEMPLATES[e]) > 1:
        return {}, {e: (rng.choice(R.TEMPLATES[e]), kt, (rng.choice(R.CH_K), rng.choice(R.CH_K)), rng.choice(R.COR_K))}
    if r < 0.45:
        t = edges[e][1]
        others = [f for f in in_edges[t] if f != e]
        if others and rng.random() < 0.5:
            f = rng.choice(others)
            tf = st.E[f]
            return {}, {e: (tpl, tf[1], vl, cl), f: (tf[0], kt, tf[2], tf[3])}
        used = {st.E[f][1] for f in others}
        return {}, {e: (tpl, rng.choice([k for k in R.PORT_K if k not in used]), vl, cl)}
    if r < 0.85:
        i = rng.randrange(2)
        nvl = list(vl)
        nvl[i] = max(R.CH_K[0], min(R.CH_K[-1], vl[i] + rng.choice((-1, 1)))) if rng.random() < 0.5 else rng.choice(R.CH_K)
        return {}, {e: (tpl, kt, tuple(nvl), cl)}
    return {}, {e: (tpl, kt, vl, rng.choice(R.COR_K))}


def neighbours(st):
    for e in range(NE):
        for k in R.PORT_K:
            if not (st.S[edges[e][0]][0] == k and st.E[e][1] == k):
                yield align_move(st, e, k)
    for s in SOURCES:
        ps, tr = st.S[s]
        for k in R.PORT_K:
            if k != ps:
                yield {s: (k, tr)}, {}
        for k in R.CH_K:
            if k != tr:
                yield {s: (ps, k)}, {}
    for e in range(NE):
        tpl, kt, vl, cl = st.E[e]
        for ntpl in R.TEMPLATES[e]:
            if ntpl != tpl:
                yield {}, {e: (ntpl, kt, vl, cl)}
        for k in R.PORT_K:
            if k != kt:
                yield {}, {e: (tpl, k, vl, cl)}
        for i in range(2):
            for k in R.CH_K:
                if k != vl[i]:
                    nvl = list(vl)
                    nvl[i] = k
                    yield {}, {e: (tpl, kt, tuple(nvl), cl)}
        for k in R.COR_K:
            if k != cl:
                yield {}, {e: (tpl, kt, vl, k)}
        for f in in_edges[edges[e][1]]:
            if f > e:
                tf = st.E[f]
                yield {}, {e: (tpl, tf[1], vl, cl), f: (tf[0], kt, tf[2], tf[3])}


def polish(st):
    improved = True
    while improved:
        improved = False
        for dS, dE in neighbours(st):
            d, payload = delta(st, dS, dE)
            if d < -1e-9:
                apply(st, payload)
                improved = True


def anneal(seed, iters):
    rng = random.Random(seed)
    st = State(*initial(rng))
    T0, T1 = 30.0, 0.05
    for it in range(iters):
        T = T0 * (T1 / T0) ** (it / iters)
        dS, dE = random_move(st, rng)
        d, payload = delta(st, dS, dE)
        if d < 0 or rng.random() < math.exp(-d / T):
            apply(st, payload)
    polish(st)
    return st


def report(st):
    ncross = ntouch = nclose = 0
    L = SOURCES
    for i in range(len(L)):
        for j in range(i + 1, len(L)):
            a, b = st.nets[L[i]], st.nets[L[j]]
            for H, V in ((a.H, b.V), (b.H, a.V)):
                if len(H) and len(V):
                    hy, hx1, hx2 = H[:, 0:1], H[:, 1:2], H[:, 2:3]
                    vx, vy1, vy2 = V[:, 0], V[:, 1], V[:, 2]
                    inter = (vx >= hx1) & (vx <= hx2) & (hy >= vy1) & (hy <= vy2)
                    touch = inter & ((vx == hx1) | (vx == hx2) | (hy == vy1) | (hy == vy2))
                    ncross += (inter & ~touch).sum()
                    ntouch += touch.sum()
            for A, B in ((a.H, b.H), (a.V, b.V)):
                if len(A) and len(B):
                    d = np.abs(A[:, 0:1] - B[:, 0])
                    ov = np.minimum(A[:, 2:3], B[:, 2]) - np.maximum(A[:, 1:2], B[:, 1])
                    nclose += ((d < 10) & (ov > -45)).sum()
    return dict(crossings=int(ncross), touches=int(ntouch), close=int(nclose),
                box=int(sum(n.cost >= W_BOX for n in st.nets.values())))


def write_json(st, path):
    import json
    per = {n: [] for n in geo}
    for e, (s, t) in enumerate(edges):
        per[s].append([{'x': int(x), 'y': int(y)} for x, y in edge_points(e, st.S, st.E)])
    doc = {'Diagramas': {n: {'isActive': False, 'lines': per[n]} for n in sorted(geo)}}
    with open(path, 'w', encoding='utf-8') as f:
        f.write(json.dumps(doc, indent=4, ensure_ascii=False, sort_keys=True) + '\n')


if __name__ == '__main__':
    iters = int(sys.argv[1])
    seed = int(sys.argv[2])
    outp = sys.argv[3]
    st = anneal(seed, iters)
    cost = st.total()
    print('seed', seed, 'cost', round(cost, 2), report(st), flush=True)
    for s in SOURCES:
        print('SRC', s[:30], st.S[s])
    for e, (s, t) in enumerate(edges):
        print(f'{s[:28]:28s} -> {t[:28]:28s} {st.E[e]}')
    write_json(st, outp)
