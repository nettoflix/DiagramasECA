/*
 * Lê um files/disciplinas.txt e monta os dados da versão web (o mesmo
 * formato que web/gerar_dados.py embute em index.html), com as linhas
 * calculadas por roteador.js.
 *
 * Reproduz o que o app Qt faz, para que as linhas saiam idênticas às dele:
 *   - o leitor segue as regras de Widget::carregarDisciplinas() (widget.cpp);
 *   - a grade segue o QGridLayout do app: margens de 9 px, colunas de 300 px
 *     (largura do título da fase) com 10 px entre elas, título com 200 px de
 *     altura e linhas de 159 px (a caixa) com 6 px entre elas. Colunas e
 *     linhas vazias não ocupam espaço;
 *   - o roteador recebe as mesmas opções que o app usa (widget.cpp,
 *     Widget::atualizarLinhas).
 *
 * ES5, como roteador.js, para poder rodar também no QJSEngine do Qt 5.9.
 * Depende de Roteador (roteador.js) só em gerar().
 */
var Curriculo = (function () {
    'use strict';

    // cores dos grupos declarados sem "cor:", pela ordem de declaração (a
    // mesma paleta de widget.cpp e disciplinas.py)
    var PALETA = ['#b9a5e6', '#76c9bd', '#f3b75c', '#e79b87', '#efdb6c', '#9dc0e7',
                  '#a8d08d', '#f2a7c3', '#c9b79c', '#8fd3e8', '#d9a3e0', '#b5c46e'];
    var GRADE = {margem: 9, larguraColuna: 300, espacoH: 10, alturaTitulo: 200,
                 caixa: 159, espacoV: 6, margemTopo: 50};
    var OPCOES_ROTEADOR = {iteracoes: 60000, sementes: [1, 2, 3]};

    function lista(valor) {
        var out = [], partes = valor.split(',');
        for (var i = 0; i < partes.length; i++)
            if (partes[i].trim()) out.push(partes[i].trim());
        return out;
    }

    function numero(valor) {
        return /^\d+$/.test(valor) ? parseInt(valor, 10) : null;
    }

    /*
     * Retorna {disciplinas: [...], fases: [números com cabeçalho],
     *          grupos: [{id, nome, cor}], erros: [...]}.
     * Cada disciplina: {id, nome, fase, linha, ha, ob, preCH, pre, grupos}.
 * Também devolve cabecalho: os comentários do topo do arquivo, sem o "#".
     */
    function ler(texto) {
        var disciplinas = [], fases = [], grupos = [], porGrupo = {}, erros = [];
        var emGrupos = false;  // dentro da seção [Grupos]
        var porId = {}, nomes = {}, ultimaLinha = {}, ocupado = {}, numLinhaDe = {};
        var fase = 0;
        var linhas = texto.split(/\r\n|\r|\n/);
        var cabecalho = [], noCabecalho = true;  // comentários do topo, até o primeiro dado
        function erro(n, msg) { erros.push('linha ' + n + ': ' + msg); }

        for (var n = 1; n <= linhas.length; n++) {
            var linha = linhas[n - 1].trim();
            if (noCabecalho && linha.charAt(0) === '#') cabecalho.push(linha.replace(/^# ?/, ''));
            if (!linha || linha.charAt(0) === '#') continue;
            noCabecalho = false;
            if (/^\[\s*grupos\s*\]$/i.test(linha)) { emGrupos = true; continue; }
            var m = /^\[\s*fase\s+(\d+)\s*\]$/i.exec(linha);
            if (m) {
                emGrupos = false;
                fase = parseInt(m[1], 10);
                if (fase < 1) erro(n, 'fase deve ser 1 ou maior');
                else if (fases.indexOf(fase) < 0) fases.push(fase);
                continue;
            }
            if (emGrupos) {
                // ID | Nome exibido | cor: #rrggbb   (nome e cor opcionais)
                var cg = linha.split('|').map(function (c) { return c.trim(); });
                if (!/^[A-Za-z0-9_-]+$/.test(cg[0])) {
                    erro(n, 'identificador de grupo inválido (use letras sem acento, números, _ ou -): ' + cg[0]);
                    continue;
                }
                if (porGrupo[cg[0]]) { erro(n, 'grupo repetido: ' + cg[0]); continue; }
                var cor = '', grupoValido = true;
                for (var q = 2; q < cg.length; q++) {
                    var pc = cg[q].indexOf(':');
                    var ch = (pc < 0 ? cg[q] : cg[q].slice(0, pc)).trim(), vl = pc < 0 ? '' : cg[q].slice(pc + 1).trim();
                    if (ch === 'cor' && /^#[0-9A-Fa-f]{6}$/.test(vl)) cor = vl.toLowerCase();
                    else { erro(n, 'campo inválido no grupo (esperado "cor: #rrggbb"): ' + cg[q]); grupoValido = false; }
                }
                if (!grupoValido) continue;
                var gr = {id: cg[0], nome: cg.length > 1 && cg[1] ? cg[1] : cg[0],
                          cor: cor || PALETA[grupos.length % PALETA.length]};
                grupos.push(gr);
                porGrupo[gr.id] = gr;
                continue;
            }
            if (fase < 1) { erro(n, 'disciplina antes de um cabeçalho [Fase N]'); continue; }

            var campos = linha.split('|').map(function (c) { return c.trim(); });
            if (campos.length < 2 || !campos[0] || !campos[1]) {
                erro(n, 'esperado "CÓDIGO | Nome | ha: ..."');
                continue;
            }
            var id = campos[0], nome = campos[1];
            if (porId[id]) { erro(n, 'código repetido: ' + id); continue; }
            if (nomes[nome]) { erro(n, 'nome repetido: ' + nome); continue; }

            var d = {id: id, nome: nome, fase: fase, linha: (ultimaLinha[fase] || 0) + 1,
                     ha: null, ob: true, preCH: 0, pre: [], grupos: []};
            var valido = true;
            for (var i = 2; i < campos.length; i++) {
                var p = campos[i].indexOf(':');
                var chave = (p < 0 ? campos[i] : campos[i].slice(0, p)).trim();
                var valor = p < 0 ? '' : campos[i].slice(p + 1).trim();
                if (chave === 'ha' || chave === 'linha' || chave === 'preCH') {
                    var v = numero(valor);
                    if (v === null) { erro(n, 'número inválido em "' + campos[i] + '"'); valido = false; }
                    else d[chave] = v;
                } else if (chave === 'pre' || chave === 'grupos') {
                    d[chave] = lista(valor);
                } else if (chave === 'optativa' && !valor) {
                    d.ob = false;
                } else {
                    erro(n, 'campo desconhecido: ' + campos[i]);
                    valido = false;
                }
            }
            if (d.ha === null) { erro(n, 'falta a carga horária (ha: N)'); valido = false; }
            if (d.linha < 1) { erro(n, 'a linha da grade deve ser 1 ou maior'); valido = false; }
            else if (ocupado[d.linha + ',' + fase]) {
                erro(n, 'a linha ' + d.linha + ' da fase ' + fase + ' já está ocupada');
                valido = false;
            }
            if (!valido) continue;

            disciplinas.push(d);
            porId[id] = d;
            nomes[nome] = true;
            numLinhaDe[id] = n;
            ultimaLinha[fase] = d.linha;
            ocupado[d.linha + ',' + fase] = true;
        }

        // pré-requisitos e grupos só depois de ler tudo: podem citar
        // disciplinas e grupos declarados mais adiante no arquivo
        for (var j = 0; j < disciplinas.length; j++) {
            var dj = disciplinas[j];
            for (var k2 = 0; k2 < dj.grupos.length; k2++)
                if (!porGrupo[dj.grupos[k2]])
                    erro(numLinhaDe[dj.id], 'grupo não declarado em [Grupos]: ' + dj.grupos[k2]);
            for (var k = 0; k < dj.pre.length; k++) {
                var pr = porId[dj.pre[k]];
                if (!pr) erro(numLinhaDe[dj.id], 'pré-requisito desconhecido: ' + dj.pre[k]);
                else if (pr.fase >= dj.fase)
                    erro(numLinhaDe[dj.id], 'o pré-requisito ' + pr.id + ' precisa estar numa fase anterior à ' + dj.fase);
            }
        }
        return {disciplinas: disciplinas, fases: fases, grupos: grupos, erros: erros,
                cabecalho: cabecalho.join('\n')};
    }

    function preencher(texto, largura) {
        while (texto.length < largura) texto += ' ';
        return texto;
    }

    /*
     * O inverso de ler(): {disciplinas, fases, grupos, cabecalho} -> texto
     * no formato de files/disciplinas.txt, com as colunas alinhadas. Cada
     * fase sai com as disciplinas na ordem de "linha:". "linha:" só é
     * escrito se alguma disciplina não estiver logo abaixo da anterior (aí
     * vai em todas, para as colunas ficarem alinhadas). Os comentários fora
     * do cabeçalho se perdem.
     */
    function escrever(lido) {
        var out = [], i, j;
        var cabecalho = (lido.cabecalho || '').replace(/\s+$/, '');
        if (cabecalho) {
            var cl = cabecalho.split('\n');
            for (i = 0; i < cl.length; i++) out.push(cl[i] ? '# ' + cl[i] : '#');
            out.push('');
        }

        var grupos = lido.grupos || [];
        if (grupos.length) {
            var gId = 0, gNome = 0;
            for (i = 0; i < grupos.length; i++) {
                gId = Math.max(gId, grupos[i].id.length);
                gNome = Math.max(gNome, (grupos[i].nome || grupos[i].id).length);
            }
            out.push('[Grupos]');
            for (i = 0; i < grupos.length; i++) {
                var g = grupos[i];
                out.push(preencher(g.id, gId) + ' | ' + preencher(g.nome || g.id, gNome) +
                         (g.cor ? ' | cor: ' + g.cor : ''));
            }
            out.push('');
        }

        var porFase = {}, fases = (lido.fases || []).slice();
        for (i = 0; i < lido.disciplinas.length; i++) {
            var d = lido.disciplinas[i];
            if (!porFase[d.fase]) porFase[d.fase] = [];
            porFase[d.fase].push(d);
            if (fases.indexOf(d.fase) < 0) fases.push(d.fase);
        }
        fases.sort(function (a, b) { return a - b; });
        var comLinha = false, wId = 0, wNome = 0, wLinha = 0, wHa = 0;
        for (i = 0; i < fases.length; i++) {
            var ordem = (porFase[fases[i]] || []).sort(function (a, b) { return a.linha - b.linha; });
            for (j = 0; j < ordem.length; j++) {
                if (ordem[j].linha !== (j ? ordem[j - 1].linha : 0) + 1) comLinha = true;
                wId = Math.max(wId, ordem[j].id.length);
                wNome = Math.max(wNome, ordem[j].nome.length);
                wLinha = Math.max(wLinha, ('linha: ' + ordem[j].linha).length);
                wHa = Math.max(wHa, ('ha: ' + ordem[j].ha).length);
            }
        }
        for (i = 0; i < fases.length; i++) {
            out.push('[Fase ' + fases[i] + ']');
            var ds = porFase[fases[i]] || [];
            for (j = 0; j < ds.length; j++) {
                var dj = ds[j];
                var campos = [preencher(dj.id, wId), preencher(dj.nome, wNome)];
                if (comLinha) campos.push(preencher('linha: ' + dj.linha, wLinha));
                campos.push(preencher('ha: ' + dj.ha, wHa));
                if (dj.preCH) campos.push('preCH: ' + dj.preCH);
                if (dj.pre && dj.pre.length) campos.push('pre: ' + dj.pre.join(', '));
                if (dj.grupos && dj.grupos.length) campos.push('grupos: ' + dj.grupos.join(', '));
                if (dj.ob === false) campos.push('optativa');
                out.push(campos.join(' | ').replace(/\s+$/, ''));
            }
            out.push('');
        }
        return out.join('\n');
    }

    /*
     * Posição de cada caixa como o QGridLayout do app a calcula. A fase N
     * é a coluna N-1 da grade; o título da fase ocupa a linha 0.
     * Retorna {caixas: {id: {x, y, w, h, linha, coluna}}, colunaX: {fase: x},
     *          largura, altura}.
     */
    function montarGrade(lido) {
        var g = GRADE, i;
        var fases = lido.fases.slice().sort(function (a, b) { return a - b; });
        var colunaX = {}, x = g.margem;
        for (i = 0; i < fases.length; i++) {
            colunaX[fases[i]] = x;
            x += g.larguraColuna + g.espacoH;
        }
        var usadas = {}, linhas = [];
        for (i = 0; i < lido.disciplinas.length; i++) usadas[lido.disciplinas[i].linha] = true;
        for (var r in usadas) linhas.push(+r);
        linhas.sort(function (a, b) { return a - b; });
        var linhaY = {}, y = g.margem + (fases.length ? g.alturaTitulo + g.espacoV : 0);
        for (i = 0; i < linhas.length; i++) {
            linhaY[linhas[i]] = y;
            y += g.caixa + g.espacoV;
        }
        var caixas = {};
        for (i = 0; i < lido.disciplinas.length; i++) {
            var d = lido.disciplinas[i];
            caixas[d.id] = {x: colunaX[d.fase], y: linhaY[d.linha], w: g.caixa, h: g.caixa,
                            linha: d.linha, coluna: d.fase - 1};
        }
        return {caixas: caixas, colunaX: colunaX,
                largura: x - g.espacoH + g.margem,
                altura: y - (linhas.length ? g.espacoV : 0) + g.margem};
    }

    // mesma entrada que Widget::entradaRoteador() monta no app
    function entradaRoteador(lido, grade) {
        var arestas = [];
        for (var i = 0; i < lido.disciplinas.length; i++) {
            var d = lido.disciplinas[i];
            for (var j = 0; j < d.pre.length; j++) arestas.push([d.pre[j], d.id]);
        }
        return {caixas: grade.caixas, arestas: arestas, margemTopo: GRADE.margemTopo};
    }

    // DAS_5334 -> DAS5334; o que não tem cara de código de disciplina
    // (ex.: OPT_PROF8, as optativas) fica sem código
    function codigoDisciplina(id) {
        return /^[A-Z]{3}_?\d{4}$/.test(id) ? id.replace('_', '') : '';
    }

    /*
     * texto do disciplinas.txt -> {dados, erros}. dados tem o formato da
     * constante DADOS de index.html (null se houver erros).
     */
    function gerar(texto, opcoes) {
        var lido = ler(texto);
        if (lido.erros.length) return {dados: null, erros: lido.erros};
        if (!lido.disciplinas.length) return {dados: null, erros: ['nenhuma disciplina encontrada']};
        var grade = montarGrade(lido);
        var R = typeof Roteador !== 'undefined' ? Roteador : require('./roteador.js');
        var rotas = R.rotear(entradaRoteador(lido, grade), opcoes || OPCOES_ROTEADOR);
        var disciplinas = lido.disciplinas.map(function (d) {
            var c = grade.caixas[d.id];
            return {id: d.id, codigo: codigoDisciplina(d.id), nome: d.nome, fase: d.fase,
                    x: c.x, y: c.y + GRADE.margemTopo, pre: d.pre, ha: d.ha, ob: d.ob,
                    preCH: d.preCH, grupos: d.grupos};
        });
        disciplinas.sort(function (a, b) { return (a.fase - b.fase) || (a.y - b.y); });
        return {
            erros: [],
            dados: {largura: grade.largura, altura: grade.altura,
                    caixa: [GRADE.caixa, GRADE.caixa - GRADE.margemTopo],
                    grupos: lido.grupos, disciplinas: disciplinas, linhas: rotas.linhas}
        };
    }

    return {ler: ler, escrever: escrever, montarGrade: montarGrade, entradaRoteador: entradaRoteador,
            codigoDisciplina: codigoDisciplina, gerar: gerar, OPCOES_ROTEADOR: OPCOES_ROTEADOR,
            PALETA: PALETA};
})();

if (typeof module !== 'undefined' && module.exports)
    module.exports = Curriculo;
