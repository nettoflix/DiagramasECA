/*
 * Roteador das linhas de pré-requisito do fluxograma.
 *
 * Port em JavaScript de gerador_linhas/route.py + route2.py (modelo de
 * "tronco" + simulated annealing), para ser usado tanto pela versão web
 * (navegador) quanto pelo app Qt (QJSEngine). Por isso é ES5 puro: o motor
 * JS do Qt 5.9 não tem let/const, arrow functions, Map/Set etc.
 *
 * Entrada:
 *   {
 *     caixas: { <id>: {x, y, w, h, linha, coluna}, ... },  // geometria do widget
 *     arestas: [[<id do pré-requisito>, <id da disciplina>], ...],
 *     margemTopo: 50     // a caixa visível começa margemTopo px abaixo de y
 *   }
 * Saída de rotear():
 *   { linhas: [{de, para, pts: [[x, y], ...]}, ...], custo, relatorio }
 *
 * Cada pré-requisito tem uma única porta de saída na direita da caixa e um
 * "tronco" vertical de onde sai um ramo para cada disciplina que depende
 * dele; o ramo entra pela esquerda da caixa de destino. A explicação
 * completa do método está em gerador_linhas/README.md.
 */
var Roteador = (function () {
    'use strict';

    var PORT_K = [-2, -1, 0, 1, 2], PORT_STEP = 18;  // portas na lateral da caixa
    var CH_K = [-4, -3, -2, -1, 0, 1, 2, 3, 4], CH_STEP = 14;  // trilhas verticais entre colunas
    var COR_K = [-1, 0, 1], COR_STEP = 14;  // trilhas horizontais entre linhas

    var W_BEND = 8.0, W_LEN = 0.015, W_CROSS = 12.0, W_TOUCH = 150.0, W_CLOSE = 300.0;
    var W_CENTER = 1.5, W_TRUNK = 0.3, W_CORR = 0.6, W_LANE = 0.15, W_BOX = 5000.0;
    var W_CROSS_NEAR = 10.0, W_NEAR = 3.0, NEAR_D = 32, W_GAP = 150.0, W_GAP2 = 25.0;

    // Gerador pseudoaleatório com semente (Lehmer / Park-Miller): o resultado
    // é reproduzível e igual no navegador e no Qt. Só usa aritmética exata em
    // double, sem operadores de bit: no QJSEngine do Qt 5.9, "x >>> 0" pode
    // devolver valor com sinal (ou 0 dentro de funções), o que quebra
    // geradores como o mulberry32.
    var MODULO = 2147483647;
    function Aleatorio(semente) {
        var s = Math.abs(Math.floor(semente)) % MODULO;
        if (s === 0) s = 1;
        this.s = s;
        for (var i = 0; i < 10; i++) this.random();  // descarta o começo, parecido entre sementes próximas
    }
    Aleatorio.prototype.random = function () {
        this.s = (this.s * 48271) % MODULO;
        return (this.s - 1) / (MODULO - 1);
    };
    Aleatorio.prototype.randrange = function (n) { return Math.min(n - 1, Math.floor(this.random() * n)); };
    Aleatorio.prototype.choice = function (lista) { return lista[this.randrange(lista.length)]; };

    function contem(lista, v) {
        for (var i = 0; i < lista.length; i++)
            if (lista[i] === v) return true;
        return false;
    }

    function limitar(v, lo, hi) { return Math.max(lo, Math.min(hi, v)); }

    // ------------------------------------------------------------------
    // Problema: geometria, trilhas e moldes de rota (route.py)
    // ------------------------------------------------------------------
    function Problema(entrada) {
        var mt = entrada.margemTopo === undefined ? 50 : entrada.margemTopo;
        var ids = Object.keys(entrada.caixas).sort();
        var indice = {};
        var i, c;
        this.ids = ids;
        this.nNos = ids.length;
        this.box = [];   // [x0, y0, x1, y1] da caixa visível (direita/baixo exclusivos)
        this.col = [];
        this.row = [];
        var colX = {}, colDir = {}, rowY = {}, ocupado = {};
        for (i = 0; i < ids.length; i++) {
            var g = entrada.caixas[ids[i]];
            indice[ids[i]] = i;
            this.box.push([g.x, g.y + mt, g.x + g.w, g.y + g.h]);
            this.col.push(g.coluna);
            this.row.push(g.linha);
            colX[g.coluna] = g.x;
            colDir[g.coluna] = g.x + g.w;
            rowY[g.linha] = g.y;
            ocupado[g.linha + ',' + g.coluna] = true;
        }
        this.indice = indice;
        this.mt = mt;
        this.ocupado = ocupado;
        this.rowY = rowY;

        // colunas sem nenhuma caixa: posição interpolada entre as vizinhas
        var cols = Object.keys(colX).map(Number).sort(function (a, b) { return a - b; });
        for (c = cols[0]; c <= cols[cols.length - 1]; c++) {
            if (colX[c] !== undefined) continue;
            var e = c, d = c;
            while (colX[e] === undefined) e--;
            while (colX[d] === undefined) d++;
            colX[c] = Math.round(colX[e] + (colX[d] - colX[e]) * (c - e) / (d - e));
            colDir[c] = colX[c] + (colDir[e] - colX[e]);
        }
        this.colX = colX;
        this.colDir = colDir;
        this.linhasExistentes = Object.keys(rowY).map(Number).sort(function (a, b) { return a - b; });

        // arestas (sem repetições), na mesma ordem do route.py
        var vistas = {}, arestas = [];
        for (i = 0; i < entrada.arestas.length; i++) {
            var s = entrada.arestas[i][0], t = entrada.arestas[i][1];
            if (indice[s] === undefined || indice[t] === undefined)
                throw new Error('aresta com disciplina desconhecida: ' + s + ' -> ' + t);
            if (vistas[s + '>' + t]) continue;
            vistas[s + '>' + t] = true;
            var si = indice[s], ti = indice[t];
            if (this.col[si] >= this.col[ti])
                throw new Error('o pré-requisito ' + s + ' precisa estar numa fase anterior à de ' + t);
            arestas.push([si, ti]);
        }
        var self = this;
        arestas.sort(function (a, b) {
            return (self.col[a[0]] - self.col[b[0]]) || (self.row[a[0]] - self.row[b[0]]) ||
                   (self.col[a[1]] - self.col[b[1]]) || (self.row[a[1]] - self.row[b[1]]);
        });
        this.es = arestas.map(function (a) { return a[0]; });
        this.et = arestas.map(function (a) { return a[1]; });
        this.NE = arestas.length;

        this.saidas = [];   // nó -> arestas que saem dele
        this.entradas = []; // nó -> arestas que chegam nele
        for (i = 0; i < this.nNos; i++) { this.saidas.push([]); this.entradas.push([]); }
        for (i = 0; i < this.NE; i++) {
            this.saidas[this.es[i]].push(i);
            this.entradas[this.et[i]].push(i);
        }
        this.fontes = [];
        for (i = 0; i < this.nNos; i++)
            if (this.saidas[i].length) this.fontes.push(i);
        this.fontes.sort(function (a, b) { return (self.col[a] - self.col[b]) || (self.row[a] - self.row[b]); });
        this.destinos = [];
        for (i = 0; i < this.nNos; i++)
            if (this.entradas[i].length) this.destinos.push(i);

        this.moldes = [];
        for (i = 0; i < this.NE; i++)
            this.moldes.push(this.gerarMoldes(this.es[i], this.et[i]));
    }

    Problema.prototype.portY = function (n, k) {
        var b = this.box[n];
        return b[1] + Math.floor((b[3] - b[1]) / 2) + PORT_STEP * k;
    };
    Problema.prototype.inicioX = function (n) { return this.box[n][2] + 2; };  // borda direita + 2
    Problema.prototype.fimX = function (n) { return this.box[n][0] - 3; };     // ponta quadrada da caneta de 4px encosta na borda
    Problema.prototype.canalX = function (c, k) {  // trilha vertical entre as colunas c e c+1
        var esq = this.colDir[c], dir = this.colX[c + 1];
        return esq + Math.floor((dir - esq) / 2) + CH_STEP * k;
    };
    Problema.prototype.corredorY = function (r, k) {  // trilha horizontal logo acima da linha r
        return this.rowY[r] + this.mt - 28 + COR_STEP * k;
    };
    Problema.prototype.linhaLivre = function (r, c1, c2) {
        for (var c = c1; c <= c2; c++)
            if (this.ocupado[r + ',' + c]) return false;
        return true;
    };

    // T1: sai na altura da origem, desce/sobe num canal k e entra na altura
    //     do destino. T2: canal a, corredor acima da linha r, canal b.
    Problema.prototype.gerarMoldes = function (s, t) {
        var cs = this.col[s], rs = this.row[s], ct = this.col[t], rt = this.row[t];
        var out = [], k, a, b, j;
        for (k = cs; k < ct; k++)
            if (this.linhaLivre(rs, cs + 1, k) && this.linhaLivre(rt, k + 1, ct - 1))
                out.push({tipo: 1, k: k});
        var lo = Math.min(rs, rt), hi = Math.max(rs, rt);
        var linhas = this.linhasExistentes, rmax = linhas[linhas.length - 1];
        for (a = cs; a < ct - 1; a++) {
            if (!this.linhaLivre(rs, cs + 1, a)) continue;
            for (b = a + 1; b < ct; b++) {
                if (!this.linhaLivre(rt, b + 1, ct - 1)) continue;
                for (j = 0; j < linhas.length; j++) {
                    var r = linhas[j];
                    if (r >= Math.max(1, lo) && r <= Math.min(rmax, hi + 1))
                        out.push({tipo: 2, a: a, r: r, b: b});
                }
            }
        }
        return out;
    };

    function nPernas(molde) { return molde.tipo === 1 ? 1 : 2; }

    // ------------------------------------------------------------------
    // Custo (route2.py). S[nó] = [porta, tronco]; E[aresta] = [molde, porta
    // de chegada, trilha 1, trilha 2, corredor]
    // ------------------------------------------------------------------
    Problema.prototype.pontos = function (e, S, E) {
        var s = this.es[e], t = this.et[e];
        var ps = S[s][0], tr = S[s][1];
        var st = E[e], molde = st[0];
        var cs = this.col[s];
        var ya = this.portY(s, ps), yb = this.portY(t, st[1]);
        var xs = this.inicioX(s), xt = this.fimX(t);
        if (molde.tipo === 1) {
            if (ya === yb) return [[xs, ya], [xt, yb]];
            var X = this.canalX(molde.k, molde.k === cs ? tr : st[2]);
            return [[xs, ya], [X, ya], [X, yb], [xt, yb]];
        }
        var Xa = this.canalX(molde.a, molde.a === cs ? tr : st[2]);
        var Xb = this.canalX(molde.b, st[3]);
        var Y = this.corredorY(molde.r, st[4]);
        return [[xs, ya], [Xa, ya], [Xa, Y], [Xb, Y], [Xb, yb], [xt, yb]];
    };

    // une intervalos colineares: {coord: [[a, b], ...]} -> [[coord, a, b], ...]
    function unir(porCoord) {
        var out = [];
        // ordem explícita: a ordem de for..in em chaves numéricas muda entre
        // motores JS, e com ela a ordem das somas (e o resultado final)
        var chaves = Object.keys(porCoord).map(Number).sort(function (p, q) { return p - q; });
        for (var c = 0; c < chaves.length; c++) {
            var chave = chaves[c], iv = porCoord[chave];
            iv.sort(function (p, q) { return (p[0] - q[0]) || (p[1] - q[1]); });
            var a = iv[0][0], b = iv[0][1];
            for (var i = 1; i < iv.length; i++) {
                if (iv[i][0] <= b) b = Math.max(b, iv[i][1]);
                else { out.push([chave, a, b]); a = iv[i][0]; b = iv[i][1]; }
            }
            out.push([chave, a, b]);
        }
        return out;
    }

    Problema.prototype.bateEmCaixa = function (H, V) {
        var n = 0, B = this.box, i, j;
        for (i = 0; i < H.length; i++) {
            var y = H[i][0], xa = H[i][1], xb = H[i][2];
            for (j = 0; j < B.length; j++)
                if (y >= B[j][1] - 4 && y < B[j][3] + 4 && xb > B[j][0] - 1 && xa < B[j][2]) n++;
        }
        for (i = 0; i < V.length; i++) {
            var x = V[i][0], ya = V[i][1], yb = V[i][2];
            for (j = 0; j < B.length; j++)
                if (x >= B[j][0] - 4 && x < B[j][2] + 4 && yb > B[j][1] && ya < B[j][3]) n++;
        }
        return n;
    };

    // árvore de um pré-requisito: segmentos unidos e custo próprio
    Problema.prototype.montarRede = function (n, S, E) {
        var Hs = {}, Vs = {}, cantos = {}, nCantos = 0, i, j, e;
        var saidas = this.saidas[n];
        for (i = 0; i < saidas.length; i++) {
            var p = this.pontos(saidas[i], S, E);
            for (j = 0; j + 1 < p.length; j++) {
                var x1 = p[j][0], y1 = p[j][1], x2 = p[j + 1][0], y2 = p[j + 1][1];
                if (y1 === y2) (Hs[y1] = Hs[y1] || []).push([Math.min(x1, x2), Math.max(x1, x2)]);
                else (Vs[x1] = Vs[x1] || []).push([Math.min(y1, y2), Math.max(y1, y2)]);
            }
            for (j = 1; j + 1 < p.length; j++) {
                var k = p[j][0] + ',' + p[j][1];
                if (!cantos[k]) { cantos[k] = true; nCantos++; }
            }
        }
        var H = unir(Hs), V = unir(Vs);
        var c = W_BEND * nCantos;
        for (i = 0; i < H.length; i++) c += W_LEN * (H[i][2] - H[i][1]);
        for (i = 0; i < V.length; i++) c += W_LEN * (V[i][2] - V[i][1]);
        c += W_TRUNK * Math.abs(S[n][1]);
        for (i = 0; i < saidas.length; i++) {
            e = E[saidas[i]];
            if (e[0].tipo === 2)
                c += W_CORR * Math.abs(e[4]) + W_LANE * Math.abs(e[3]) +
                     (e[0].a !== this.col[n] ? W_LANE * Math.abs(e[2]) : 0);
            else if (e[0].k !== this.col[n])
                c += W_LANE * Math.abs(e[2]);
        }
        c += W_CENTER * Math.abs(S[n][0]);
        // cruzamentos em "+" dentro da mesma árvore (junções e cantos não contam)
        for (i = 0; i < H.length; i++)
            for (j = 0; j < V.length; j++)
                if (V[j][0] > H[i][1] && V[j][0] < H[i][2] && H[i][0] > V[j][1] && H[i][0] < V[j][2])
                    c += W_CROSS;
        // trechos paralelos da mesma árvore próximos mas não unidos
        var grupos = [H, V];
        for (var g = 0; g < 2; g++) {
            var A = grupos[g];
            for (i = 0; i < A.length; i++)
                for (j = i + 1; j < A.length; j++) {
                    var d = Math.abs(A[i][0] - A[j][0]);
                    var ov = Math.min(A[i][2], A[j][2]) - Math.max(A[i][1], A[j][1]);
                    if (d < 10 && ov > -8) c += W_CLOSE;
                }
        }
        c += W_BOX * this.bateEmCaixa(H, V);
        return {H: H, V: V, custo: c};
    };

    Problema.prototype.pertoDeCaixa = function (px, py) {
        var B = this.box;
        for (var j = 0; j < B.length; j++) {
            if (py < B[j][1] - 6 || py > B[j][3] + 6) continue;
            if ((px >= B[j][0] - 36 && px < B[j][0]) || (px > B[j][2] && px <= B[j][2] + 36)) return true;
        }
        return false;
    };

    // custo entre duas árvores diferentes
    Problema.prototype.custoPar = function (a, b) {
        var c = 0, i, j, q;
        var cruz = [[a.H, b.V], [b.H, a.V]];
        for (q = 0; q < 2; q++) {
            var H = cruz[q][0], V = cruz[q][1];
            for (i = 0; i < H.length; i++) {
                var hy = H[i][0], hx1 = H[i][1], hx2 = H[i][2];
                for (j = 0; j < V.length; j++) {
                    var vx = V[j][0];
                    if (vx < hx1 || vx > hx2 || hy < V[j][1] || hy > V[j][2]) continue;
                    if (vx === hx1 || vx === hx2 || hy === V[j][1] || hy === V[j][2]) c += W_TOUCH;
                    else {
                        c += W_CROSS;
                        if (this.pertoDeCaixa(vx, hy)) c += W_CROSS_NEAR;
                    }
                }
            }
        }
        var par = [[a.H, b.H], [a.V, b.V]];
        for (q = 0; q < 2; q++) {
            var A = par[q][0], B = par[q][1];
            for (i = 0; i < A.length; i++)
                for (j = 0; j < B.length; j++) {
                    var d = Math.abs(A[i][0] - B[j][0]);
                    if (d >= NEAR_D) continue;
                    var ov = Math.min(A[i][2], B[j][2]) - Math.max(A[i][1], B[j][1]);
                    if (d < 10) {
                        if (ov > -8) {
                            c += W_CLOSE;
                            if (d === 0 && ov > 0) c += 4 * W_CLOSE;
                        } else if (ov > -45) c += W_GAP;
                        else if (ov > -100) c += W_GAP2;
                    } else if (ov > 0) {
                        c += W_NEAR * ov * (NEAR_D - d) / NEAR_D / 100;
                    }
                }
        }
        return c;
    };

    Problema.prototype.custoChegada = function (t, E) {
        var ent = this.entradas[t], soma = 0, visto = {}, rep = false;
        for (var i = 0; i < ent.length; i++) {
            var k = E[ent[i]][1];
            soma += k;
            if (visto[k]) rep = true;
            visto[k] = true;
        }
        return W_CENTER * Math.abs(soma) + (rep ? 1000 : 0);
    };

    // ------------------------------------------------------------------
    // Estado e otimização
    // ------------------------------------------------------------------
    function Estado(P, S, E) {
        this.P = P;
        this.S = S;
        this.E = E;
        this.redes = [];
        this.par = [];  // par[i][j]: custo entre as árvores i e j (índices de nó)
        var i, j, F = P.fontes;
        for (i = 0; i < P.nNos; i++) { this.redes.push(null); this.par.push([]); }
        for (i = 0; i < F.length; i++) this.redes[F[i]] = P.montarRede(F[i], S, E);
        for (i = 0; i < F.length; i++)
            for (j = i + 1; j < F.length; j++) {
                var v = P.custoPar(this.redes[F[i]], this.redes[F[j]]);
                this.par[F[i]][F[j]] = v;
                this.par[F[j]][F[i]] = v;
            }
    }

    Estado.prototype.total = function () {
        var P = this.P, F = P.fontes, c = 0, i, j;
        for (i = 0; i < F.length; i++) {
            c += this.redes[F[i]].custo;
            for (j = i + 1; j < F.length; j++) c += this.par[F[i]][F[j]];
        }
        for (i = 0; i < P.destinos.length; i++) c += P.custoChegada(P.destinos[i], this.E);
        return c;
    };

    // dS: [[nó, [porta, tronco]], ...]; dE: [[aresta, estado], ...]
    Estado.prototype.delta = function (dS, dE) {
        var P = this.P, i, j, n, m;
        var S2 = this.S.slice(), E2 = this.E.slice();
        var N = [], emN = {}, T = [], emT = {};
        for (i = 0; i < dS.length; i++) {
            S2[dS[i][0]] = dS[i][1];
            if (!emN[dS[i][0]]) { emN[dS[i][0]] = true; N.push(dS[i][0]); }
        }
        for (i = 0; i < dE.length; i++) {
            var e = dE[i][0];
            E2[e] = dE[i][1];
            if (!emN[P.es[e]]) { emN[P.es[e]] = true; N.push(P.es[e]); }
            if (!emT[P.et[e]]) { emT[P.et[e]] = true; T.push(P.et[e]); }
        }
        var novas = {}, pares = [], antigo = 0, novo = 0;
        for (i = 0; i < N.length; i++) {
            n = N[i];
            novas[n] = P.montarRede(n, S2, E2);
            antigo += this.redes[n].custo;
            novo += novas[n].custo;
            for (j = 0; j < P.fontes.length; j++) {
                m = P.fontes[j];
                if (emN[m]) continue;
                var v = P.custoPar(novas[n], this.redes[m]);
                antigo += this.par[n][m];
                novo += v;
                pares.push([n, m, v]);
            }
        }
        for (i = 0; i < N.length; i++)
            for (j = i + 1; j < N.length; j++) {
                var w = P.custoPar(novas[N[i]], novas[N[j]]);
                antigo += this.par[N[i]][N[j]];
                novo += w;
                pares.push([N[i], N[j], w]);
            }
        for (i = 0; i < T.length; i++) {
            antigo += P.custoChegada(T[i], this.E);
            novo += P.custoChegada(T[i], E2);
        }
        return {d: novo - antigo, S: S2, E: E2, redes: novas, pares: pares};
    };

    Estado.prototype.aplicar = function (mv) {
        this.S = mv.S;
        this.E = mv.E;
        for (var n in mv.redes) this.redes[n] = mv.redes[n];
        for (var i = 0; i < mv.pares.length; i++) {
            var p = mv.pares[i];
            this.par[p[0]][p[1]] = p[2];
            this.par[p[1]][p[0]] = p[2];
        }
    };

    function centrados(n) {
        var tabela = {1: [0], 2: [-1, 1], 3: [-1, 0, 1], 4: [-2, -1, 1, 2], 5: [-2, -1, 0, 1, 2]};
        if (tabela[n]) return tabela[n];
        var out = [];  // mais chegadas que portas: repete (e o custo penaliza)
        for (var i = 0; i < n; i++) out.push(PORT_K[i % PORT_K.length]);
        return out;
    }

    function estadoInicial(P, rng) {
        var S = [], E = [], i, j;
        for (i = 0; i < P.nNos; i++) S.push(null);
        for (i = 0; i < P.fontes.length; i++) S[P.fontes[i]] = [0, rng.choice(CH_K)];
        for (i = 0; i < P.NE; i++) {
            var melhor = null, chave = null;
            for (j = 0; j < P.moldes[i].length; j++) {
                var m = P.moldes[i][j], ch = [nPernas(m), rng.random()];
                if (melhor === null || ch[0] < chave[0] || (ch[0] === chave[0] && ch[1] < chave[1])) {
                    melhor = m;
                    chave = ch;
                }
            }
            E.push([melhor, 0, rng.choice(CH_K), rng.choice(CH_K), 0]);
        }
        for (i = 0; i < P.destinos.length; i++) {
            var es = P.entradas[P.destinos[i]].slice();
            es.sort(function (a, b) {
                return (P.row[P.es[a]] - P.row[P.es[b]]) || (P.col[P.es[b]] - P.col[P.es[a]]);
            });
            var ks = centrados(es.length);
            for (j = 0; j < es.length; j++) E[es[j]][1] = ks[j];
        }
        return new Estado(P, S, E);
    }

    function copiar(x) { return x.slice(); }

    // alinha a porta de saída da origem com a de chegada no destino
    function movAlinhar(st, e, k) {
        var P = st.P, s = P.es[e], t = P.et[e];
        var ne = copiar(st.E[e]), kt = ne[1];
        ne[1] = k;
        var dE = [[e, ne]], ent = P.entradas[t];
        for (var i = 0; i < ent.length; i++) {
            var f = ent[i];
            if (f !== e && st.E[f][1] === k) {
                var nf = copiar(st.E[f]);
                nf[1] = kt;
                dE.push([f, nf]);
            }
        }
        return [[[s, [k, st.S[s][1]]]], dE];
    }

    function movAleatorio(st, rng) {
        var P = st.P;
        if (rng.random() < 0.08)
            return movAlinhar(st, rng.randrange(P.NE), rng.choice(PORT_K));
        if (rng.random() < 0.25) {
            var s = rng.choice(P.fontes), ps = st.S[s][0], tr = st.S[s][1];
            if (rng.random() < 0.4) return [[[s, [rng.choice(PORT_K), tr]]], []];
            var ntr = rng.random() < 0.5 ? limitar(tr + rng.choice([-1, 1]), CH_K[0], CH_K[CH_K.length - 1])
                                         : rng.choice(CH_K);
            return [[[s, [ps, ntr]]], []];
        }
        var e = rng.randrange(P.NE), atual = st.E[e], ne = copiar(atual);
        var r = rng.random();
        if (r < 0.2 && P.moldes[e].length > 1) {
            ne[0] = rng.choice(P.moldes[e]);
            ne[2] = rng.choice(CH_K);
            ne[3] = rng.choice(CH_K);
            ne[4] = rng.choice(COR_K);
            return [[], [[e, ne]]];
        }
        if (r < 0.45) {
            var t = P.et[e], outras = [], i;
            for (i = 0; i < P.entradas[t].length; i++)
                if (P.entradas[t][i] !== e) outras.push(P.entradas[t][i]);
            if (outras.length && rng.random() < 0.5) {
                var f = rng.choice(outras), nf = copiar(st.E[f]);
                ne[1] = nf[1];
                nf[1] = atual[1];
                return [[], [[e, ne], [f, nf]]];
            }
            var usadas = outras.map(function (x) { return st.E[x][1]; });
            var livres = PORT_K.filter(function (k) { return !contem(usadas, k); });
            ne[1] = rng.choice(livres.length ? livres : PORT_K);
            return [[], [[e, ne]]];
        }
        if (r < 0.85) {
            var idx = 2 + rng.randrange(2);
            ne[idx] = rng.random() < 0.5 ? limitar(atual[idx] + rng.choice([-1, 1]), CH_K[0], CH_K[CH_K.length - 1])
                                         : rng.choice(CH_K);
            return [[], [[e, ne]]];
        }
        ne[4] = rng.choice(COR_K);
        return [[], [[e, ne]]];
    }

    function tentar(st, mv) {
        var r = st.delta(mv[0], mv[1]);
        if (r.d < -1e-9) { st.aplicar(r); return true; }
        return false;
    }

    // busca local exaustiva até nenhuma mudança isolada melhorar o custo
    function polir(st) {
        var P = st.P, melhorou = true, e, k, i, s;
        while (melhorou) {
            melhorou = false;
            for (e = 0; e < P.NE; e++)
                for (k = 0; k < PORT_K.length; k++)
                    if (!(st.S[P.es[e]][0] === PORT_K[k] && st.E[e][1] === PORT_K[k]))
                        if (tentar(st, movAlinhar(st, e, PORT_K[k]))) melhorou = true;
            for (i = 0; i < P.fontes.length; i++) {
                s = P.fontes[i];
                for (k = 0; k < PORT_K.length; k++)
                    if (PORT_K[k] !== st.S[s][0] && tentar(st, [[[s, [PORT_K[k], st.S[s][1]]]], []])) melhorou = true;
                for (k = 0; k < CH_K.length; k++)
                    if (CH_K[k] !== st.S[s][1] && tentar(st, [[[s, [st.S[s][0], CH_K[k]]]], []])) melhorou = true;
            }
            for (e = 0; e < P.NE; e++) {
                var ne;
                for (k = 0; k < P.moldes[e].length; k++)
                    if (P.moldes[e][k] !== st.E[e][0]) {
                        ne = copiar(st.E[e]); ne[0] = P.moldes[e][k];
                        if (tentar(st, [[], [[e, ne]]])) melhorou = true;
                    }
                for (k = 0; k < PORT_K.length; k++)
                    if (PORT_K[k] !== st.E[e][1]) {
                        ne = copiar(st.E[e]); ne[1] = PORT_K[k];
                        if (tentar(st, [[], [[e, ne]]])) melhorou = true;
                    }
                for (i = 2; i <= 3; i++)
                    for (k = 0; k < CH_K.length; k++)
                        if (CH_K[k] !== st.E[e][i]) {
                            ne = copiar(st.E[e]); ne[i] = CH_K[k];
                            if (tentar(st, [[], [[e, ne]]])) melhorou = true;
                        }
                for (k = 0; k < COR_K.length; k++)
                    if (COR_K[k] !== st.E[e][4]) {
                        ne = copiar(st.E[e]); ne[4] = COR_K[k];
                        if (tentar(st, [[], [[e, ne]]])) melhorou = true;
                    }
                var ent = P.entradas[P.et[e]];
                for (i = 0; i < ent.length; i++) {
                    var f = ent[i];
                    if (f > e) {
                        ne = copiar(st.E[e]);
                        var nf = copiar(st.E[f]);
                        ne[1] = nf[1];
                        nf[1] = st.E[e][1];
                        if (tentar(st, [[], [[e, ne], [f, nf]]])) melhorou = true;
                    }
                }
            }
        }
    }

    function recozer(P, semente, iteracoes) {
        var rng = new Aleatorio(semente);
        var st = estadoInicial(P, rng);
        var T0 = 30.0, T1 = 0.05;
        for (var it = 0; it < iteracoes; it++) {
            var T = T0 * Math.pow(T1 / T0, it / iteracoes);
            var mv = movAleatorio(st, rng);
            var r = st.delta(mv[0], mv[1]);
            if (r.d < 0 || rng.random() < Math.exp(-r.d / T)) st.aplicar(r);
        }
        polir(st);
        return st;
    }

    function relatorio(st) {
        var P = st.P, F = P.fontes, cruz = 0, toques = 0, perto = 0, caixa = 0, i, j, q, a, b, x, y;
        for (i = 0; i < F.length; i++) {
            if (st.redes[F[i]].custo >= W_BOX) caixa++;
            for (j = i + 1; j < F.length; j++) {
                a = st.redes[F[i]];
                b = st.redes[F[j]];
                var cr = [[a.H, b.V], [b.H, a.V]];
                for (q = 0; q < 2; q++)
                    for (x = 0; x < cr[q][0].length; x++)
                        for (y = 0; y < cr[q][1].length; y++) {
                            var h = cr[q][0][x], v = cr[q][1][y];
                            if (v[0] < h[1] || v[0] > h[2] || h[0] < v[1] || h[0] > v[2]) continue;
                            if (v[0] === h[1] || v[0] === h[2] || h[0] === v[1] || h[0] === v[2]) toques++;
                            else cruz++;
                        }
                var pr = [[a.H, b.H], [a.V, b.V]];
                for (q = 0; q < 2; q++)
                    for (x = 0; x < pr[q][0].length; x++)
                        for (y = 0; y < pr[q][1].length; y++) {
                            var A = pr[q][0][x], B = pr[q][1][y];
                            var ov = Math.min(A[2], B[2]) - Math.max(A[1], B[1]);
                            if (Math.abs(A[0] - B[0]) < 10 && ov > -45) perto++;
                        }
            }
        }
        return {cruzamentos: cruz, toques: toques, proximas: perto, atravessamCaixa: caixa};
    }

    /*
     * opcoes: {iteracoes: N, sementes: [1, 2, ...]}. Roda o annealing para
     * cada semente e fica com o resultado de menor custo.
     */
    function rotear(entrada, opcoes) {
        opcoes = opcoes || {};
        var iteracoes = opcoes.iteracoes || 60000;
        var sementes = opcoes.sementes || [1, 2, 3];
        var P = new Problema(entrada);
        var melhor = null, custo = Infinity;
        if (P.NE > 0) {
            for (var i = 0; i < sementes.length; i++) {
                var st = recozer(P, sementes[i], iteracoes);
                var c = st.total();
                if (c < custo) { custo = c; melhor = st; }
            }
        }
        var linhas = [];
        for (var e = 0; e < P.NE; e++)
            linhas.push({de: P.ids[P.es[e]], para: P.ids[P.et[e]], pts: P.pontos(e, melhor.S, melhor.E)});
        return {linhas: linhas, custo: P.NE ? custo : 0, relatorio: melhor ? relatorio(melhor) : {}};
    }

    // ponte para quem só troca texto (QJSEngine)
    function rotearJSON(entradaJSON, opcoesJSON) {
        return JSON.stringify(rotear(JSON.parse(entradaJSON), opcoesJSON ? JSON.parse(opcoesJSON) : {}));
    }

    // mude a versão ao alterar o método: a versão web descarta as linhas guardadas
    return {rotear: rotear, rotearJSON: rotearJSON, versao: '1'};
})();

if (typeof module !== 'undefined' && module.exports)
    module.exports = Roteador;
