#!/usr/bin/env python3
"""Gera os dados da versão web (disciplinas, pré-requisitos e linhas) e os
embute em web/index.html, entre os marcadores DADOS-INICIO / DADOS-FIM.

Fontes (as mesmas do app Qt):
  - ../files/disciplinas.txt                   disciplinas, pré-requisitos, horas e grupos
                                               (e o texto inteiro, para o editor)
  - ../files/linhas.json                       posição de cada caixa e as linhas,
                                               calculadas por web/roteador.js (o app
                                               Qt gera/atualiza este arquivo ao abrir)

Uso:  python3 gerar_dados.py
"""
import json
import os
import re
import sys

AQUI = os.path.dirname(os.path.abspath(__file__))
PROJ = os.path.dirname(AQUI)

sys.path.insert(0, PROJ)
from disciplinas import ARQUIVO, ler_disciplinas, ler_grupos  # noqa: E402

lidas = ler_disciplinas()
var2name = {d['id']: d['nome'] for d in lidas}
prereq = {d['id']: d['pre'] for d in lidas if d['pre']}
horas = {d['id']: (d['ha'], d['ob']) for d in lidas}
preCH = {d['id']: d['preCH'] for d in lidas if d['preCH']}
grupos = {d['id']: d['grupos'] for d in lidas if d['grupos']}

cache = json.load(open(os.path.join(PROJ, 'files', 'linhas.json'), encoding='utf-8'))
geo = {var: dict(col=g['coluna'], x=g['x'], y=g['y'], w=g['w'], h=g['h'])
       for var, g in cache['entrada']['caixas'].items()}
esperado = {(s, t) for t, ss in prereq.items() for s in ss}
if set(geo) != set(var2name) or {tuple(a) for a in cache['entrada']['arestas']} != esperado:
    raise SystemExit('files/linhas.json está desatualizado em relação a files/disciplinas.txt: '
                     'abra o app Qt uma vez para recalcular as linhas')

def codigo(var):
    """MTM_3110 -> MTM3110; o que não tem cara de código (ex.: OPT_PROF8, as
    optativas) fica sem código. Mesma regra de codigoDisciplina() em curriculo.js."""
    return var.replace('_', '', 1) if re.fullmatch(r'[A-Z]{3}_?\d{4}', var) else ''


disciplinas = []
for var, name in var2name.items():
    g = geo[var]
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

linhas = [{'de': l['de'], 'para': l['para'], 'pts': l['pts']} for l in cache['linhas']]

# confere: exatamente uma linha por pré-requisito
obtido = {(l['de'], l['para']) for l in linhas}
if esperado != obtido or len(linhas) != len(esperado):
    raise SystemExit(f'linhas não batem com os pré-requisitos: faltam {esperado - obtido}, sobram {obtido - esperado}')

# tamanho do container Qt: colunas de 300 px (título da fase) e margem de 9 px
largura = max(g['x'] for g in geo.values()) + 300 + 9
altura = max(g['y'] + g['h'] for g in geo.values()) + 9
# texto: o disciplinas.txt original, para o editor (editor.html) abrir o
# currículo padrão
dados = {'largura': largura, 'altura': altura, 'caixa': [159, 109], 'grupos': ler_grupos(),
         'disciplinas': disciplinas, 'linhas': linhas,
         'texto': open(ARQUIVO, encoding='utf-8').read()}
bloco = json.dumps(dados, ensure_ascii=False, separators=(',', ':'))

html_path = os.path.join(AQUI, 'index.html')
html = open(html_path, encoding='utf-8').read()
novo = re.sub(r'(/\*DADOS-INICIO\*/).*?(/\*DADOS-FIM\*/)',
              lambda m: m.group(1) + bloco + m.group(2), html, flags=re.S)
if novo == html and '/*DADOS-INICIO*/' not in html:
    raise SystemExit('marcadores DADOS-INICIO/DADOS-FIM não encontrados em index.html')
open(html_path, 'w', encoding='utf-8').write(novo)
print(f'{len(disciplinas)} disciplinas, {len(linhas)} linhas -> {html_path}')
