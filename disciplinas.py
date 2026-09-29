"""Leitor de files/disciplinas.txt (formato descrito no cabeçalho do arquivo),
o mesmo que Widget::carregarDisciplinas() lê no app Qt.

    from disciplinas import ler_disciplinas
    for d in ler_disciplinas():
        d['id'], d['nome'], d['fase'], d['linha'], d['ha'], d['ob'],
        d['preCH'], d['pre'], d['grupos']
"""
import os
import re

ARQUIVO = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'files', 'disciplinas.txt')
GRUPOS = {'informatica', 'controle', 'automacao', 'mecanica', 'eletrica', 'fisica_calculo'}


def ler_disciplinas(caminho=ARQUIVO):
    disciplinas = []
    fase = 0
    ultima_linha = {}
    for n, texto in enumerate(open(caminho, encoding='utf-8'), 1):
        texto = texto.strip()
        if not texto or texto.startswith('#'):
            continue
        onde = f'{caminho}:{n}'
        m = re.fullmatch(r'\[\s*fase\s+(\d+)\s*\]', texto, re.I)
        if m:
            fase = int(m.group(1))
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
        if set(d['grupos']) - GRUPOS:
            raise SystemExit(f'{onde}: grupo desconhecido: {sorted(set(d["grupos"]) - GRUPOS)}')
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
    for d in disciplinas:
        desconhecidos = set(d['pre']) - set(ids)
        if desconhecidos:
            raise SystemExit(f'{caminho}: pré-requisito desconhecido em {d["id"]}: {sorted(desconhecidos)}')
    return disciplinas
