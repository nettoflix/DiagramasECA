"""Leitor de files/disciplinas.txt (formato descrito no cabeçalho do arquivo),
o mesmo que Widget::carregarDisciplinas() lê no app Qt.

    from disciplinas import ler_disciplinas, ler_grupos
    for d in ler_disciplinas():
        d['id'], d['nome'], d['fase'], d['linha'], d['ha'], d['ob'],
        d['preCH'], d['pre'], d['grupos']
    for g in ler_grupos():      # seção [Grupos], na ordem do arquivo
        g['id'], g['nome'], g['cor']
"""
import os
import re

ARQUIVO = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'files', 'disciplinas.txt')
# cores dos grupos declarados sem "cor:", pela ordem de declaração (a mesma
# paleta de widget.cpp e web/curriculo.js)
PALETA = ['#b9a5e6', '#76c9bd', '#f3b75c', '#e79b87', '#efdb6c', '#9dc0e7',
          '#a8d08d', '#f2a7c3', '#c9b79c', '#8fd3e8', '#d9a3e0', '#b5c46e']


def ler_disciplinas(caminho=ARQUIVO):
    return _ler(caminho)[0]


def ler_grupos(caminho=ARQUIVO):
    return _ler(caminho)[1]


def _ler(caminho):
    disciplinas, grupos = [], []
    fase = 0
    em_grupos = False  # dentro da seção [Grupos]
    ultima_linha = {}
    for n, texto in enumerate(open(caminho, encoding='utf-8'), 1):
        texto = texto.strip()
        if not texto or texto.startswith('#'):
            continue
        onde = f'{caminho}:{n}'
        if re.fullmatch(r'\[\s*grupos\s*\]', texto, re.I):
            em_grupos = True
            continue
        m = re.fullmatch(r'\[\s*fase\s+(\d+)\s*\]', texto, re.I)
        if m:
            em_grupos = False
            fase = int(m.group(1))
            continue
        if em_grupos:
            # ID | Nome exibido | cor: #rrggbb   (nome e cor opcionais)
            campos = [c.strip() for c in texto.split('|')]
            if not re.fullmatch(r'[A-Za-z0-9_-]+', campos[0]):
                raise SystemExit(f'{onde}: identificador de grupo inválido: {campos[0]}')
            if any(g['id'] == campos[0] for g in grupos):
                raise SystemExit(f'{onde}: grupo repetido: {campos[0]}')
            cor = ''
            for campo in campos[2:]:
                chave, _, valor = (x.strip() for x in campo.partition(':'))
                if chave != 'cor' or not re.fullmatch(r'#[0-9A-Fa-f]{6}', valor):
                    raise SystemExit(f'{onde}: campo inválido no grupo (esperado "cor: #rrggbb"): {campo}')
                cor = valor.lower()
            grupos.append({'id': campos[0], 'nome': campos[1] if len(campos) > 1 and campos[1] else campos[0],
                           'cor': cor or PALETA[len(grupos) % len(PALETA)]})
            continue
        if fase < 1:
            raise SystemExit(f'{onde}: disciplina antes de um cabeçalho [Fase N]')
        campos = [c.strip() for c in texto.split('|')]
        if len(campos) < 2 or not campos[0] or not campos[1]:
            raise SystemExit(f'{onde}: esperado "CÓDIGO | Nome | ha: ..."')
        d = {'id': campos[0], 'nome': campos[1], 'fase': fase,
             'linha': ultima_linha.get(fase, 0) + 1,
             'ha': None, 'ob': True, 'preCH': 0, 'pre': [], 'grupos': []}
        for campo in campos[2:]:
            chave, _, valor = (x.strip() for x in campo.partition(':'))
            lista = [x.strip() for x in valor.split(',') if x.strip()]
            if chave in ('ha', 'linha', 'preCH'):
                if not valor.isdigit():
                    raise SystemExit(f'{onde}: número inválido em "{campo}"')
                d[chave] = int(valor)
            elif chave in ('pre', 'grupos'):
                d[chave] = lista
            elif chave == 'optativa' and not valor:
                d['ob'] = False
            else:
                raise SystemExit(f'{onde}: campo desconhecido: {campo}')
        if d['ha'] is None:
            raise SystemExit(f'{onde}: falta a carga horária (ha: N)')
        ultima_linha[fase] = d['linha']
        disciplinas.append(d)

    ids = [d['id'] for d in disciplinas]
    for chave in ('id', 'nome'):
        valores = [d[chave] for d in disciplinas]
        repetidos = {v for v in valores if valores.count(v) > 1}
        if repetidos:
            raise SystemExit(f'{caminho}: {chave} repetido: {sorted(repetidos)}')
    posicoes = [(d['fase'], d['linha']) for d in disciplinas]
    if len(set(posicoes)) != len(posicoes):
        raise SystemExit(f'{caminho}: duas disciplinas na mesma fase e linha')
    declarados = {g['id'] for g in grupos}
    for d in disciplinas:
        desconhecidos = set(d['pre']) - set(ids)
        if desconhecidos:
            raise SystemExit(f'{caminho}: pré-requisito desconhecido em {d["id"]}: {sorted(desconhecidos)}')
        if set(d['grupos']) - declarados:
            raise SystemExit(f'{caminho}: grupo não declarado em [Grupos] em {d["id"]}: {sorted(set(d["grupos"]) - declarados)}')
    return disciplinas, grupos
