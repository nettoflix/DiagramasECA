#!/usr/bin/env python3
"""Gera os dados da versão web (disciplinas, pré-requisitos e linhas) e os
embute em web/index.html, entre os marcadores DADOS-INICIO / DADOS-FIM.

Fontes (as mesmas do app Qt):
  - ../files/disciplinas.txt                   disciplinas, pré-requisitos, horas e grupos
  - ../gerador_linhas/dados/geometria_medida.txt  posição de cada diagrama
  - ../files/saved.txt                         linhas (coordenadas do container)

Uso:  python3 gerar_dados.py
"""
import json
import os
import re
import sys

AQUI = os.path.dirname(os.path.abspath(__file__))
PROJ = os.path.dirname(AQUI)

sys.path.insert(0, PROJ)
from disciplinas import ler_disciplinas  # noqa: E402

lidas = ler_disciplinas()
var2name = {d['id']: d['nome'] for d in lidas}
prereq = {d['id']: d['pre'] for d in lidas if d['pre']}
horas = {d['id']: (d['ha'], d['ob']) for d in lidas}
preCH = {d['id']: d['preCH'] for d in lidas if d['preCH']}
grupos = {d['id']: d['grupos'] for d in lidas if d['grupos']}

geo = {}
for line in open(os.path.join(PROJ, 'gerador_linhas', 'dados', 'geometria_medida.txt'), encoding='utf-8'):
    if line.startswith('DIAG\t'):
        _, name, row, col, x, y, w, h = line.rstrip('\n').split('\t')
        geo[name] = dict(row=int(row), col=int(col), x=int(x), y=int(y))


def codigo(var):
    """MTM_3110 -> MTM3110; optativas não têm código de disciplina."""
    return '' if var.startswith('OPT') else var.replace('_', '')


disciplinas = []
for var, name in var2name.items():
    g = geo[name]
    disciplinas.append({
        'id': var,
        'codigo': codigo(var),
        'nome': name,
        'fase': g['col'] + 1,
        'x': g['x'],
        'y': g['y'] + 50,          # topo da caixa visível (margin-top: 50px no Qt)
        'pre': prereq.get(var, []),
        'ha': horas[var][0],
        'ob': horas[var][1],
        'preCH': preCH.get(var, 0),
        'grupos': grupos.get(var, []),
    })
disciplinas.sort(key=lambda d: (d['fase'], d['y']))

# linhas: a origem é a chave do JSON; o destino é a caixa em cuja borda
# esquerda a polilinha termina (x = borda - 3)
name2var = {v: k for k, v in var2name.items()}
saved = json.load(open(os.path.join(PROJ, 'files', 'saved.txt'), encoding='utf-8'))['Diagramas']
linhas = []
for nome_origem, obj in saved.items():
    for pts in obj['lines']:
        P = [[p['x'], p['y']] for p in pts]
        if len(P) < 2:
            # linha começada no modo de edição e salva sem ser terminada
            print(f'aviso: ignorando linha de {len(P)} ponto(s) em "{nome_origem}"')
            continue
        ex, ey = P[-1]
        alvo = [n for n, g in geo.items() if ex == g['x'] - 3 and g['y'] + 50 <= ey < g['y'] + 159]
        if len(alvo) != 1:
            raise SystemExit(f'linha de {nome_origem} termina fora de uma borda: {P[-1]}')
        linhas.append({'de': name2var[nome_origem], 'para': name2var[alvo[0]], 'pts': P})

# confere: exatamente uma linha por pré-requisito
esperado = {(s, t) for t, ss in prereq.items() for s in ss}
obtido = {(l['de'], l['para']) for l in linhas}
if esperado != obtido or len(linhas) != len(esperado):
    raise SystemExit(f'linhas não batem com os pré-requisitos: faltam {esperado - obtido}, sobram {obtido - esperado}')

dados = {'largura': 3108, 'altura': 2033, 'caixa': [159, 109],
         'disciplinas': disciplinas, 'linhas': linhas}
bloco = json.dumps(dados, ensure_ascii=False, separators=(',', ':'))

html_path = os.path.join(AQUI, 'index.html')
html = open(html_path, encoding='utf-8').read()
novo = re.sub(r'(/\*DADOS-INICIO\*/).*?(/\*DADOS-FIM\*/)',
              lambda m: m.group(1) + bloco + m.group(2), html, flags=re.S)
if novo == html and '/*DADOS-INICIO*/' not in html:
    raise SystemExit('marcadores DADOS-INICIO/DADOS-FIM não encontrados em index.html')
open(html_path, 'w', encoding='utf-8').write(novo)
print(f'{len(disciplinas)} disciplinas, {len(linhas)} linhas -> {html_path}')
