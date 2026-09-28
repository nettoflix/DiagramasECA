# Fluxograma ECA — versão web

`index.html` é a versão web do app: um arquivo único, sem dependências
além das fontes do Google Fonts, com os dados embutidos. Abre em qualquer
navegador, inclusive no celular.

## O que faz

- Clicar numa disciplina marca ou desmarca como concluída. Desmarcar uma
  disciplina também desmarca, em cascata, as que dependem dela.
- Clicar numa disciplina bloqueada mostra quais pré-requisitos faltam.
- Passar o mouse numa disciplina destaca os pré-requisitos e os
  dependentes dela.
- Zoom:
  - roda do mouse ou pinça;
  - botões `Ver tudo`, `−` e `+`;
  - `1ª`…`10ª` foca uma fase, e Shift+clique noutra fase foca o
    intervalo;
  - atalhos de teclado `0`, `+` e `-`.
- Arrastar move o mapa.
- O progresso fica salvo no próprio navegador (`localStorage`): cada
  pessoa tem o seu, sem servidor.

## Publicar para todo mundo (GitHub Pages)

1. Suba a pasta `web/` para um repositório no GitHub.
2. Em *Settings → Pages*, escolha a branch e a pasta onde está o
   `index.html`.
3. O link `https://<usuario>.github.io/<repo>/` passa a funcionar em
   qualquer sistema.

Qualquer hospedagem de arquivos estáticos serve (Netlify, Cloudflare
Pages...), já que é só um arquivo HTML.

## Atualizar os dados

Os dados embutidos vêm das mesmas fontes do app Qt:

| Fonte | Conteúdo |
|---|---|
| `../widget.cpp` | nomes das disciplinas e pré-requisitos |
| `../gerador_linhas/dados/geometria_medida.txt` | posição de cada caixa |
| `../files/saved.txt` | linhas (geradas por `gerador_linhas/route2.py`) |

Depois de mudar o currículo ou as linhas:

```bash
python3 gerar_dados.py
```

O script reescreve o bloco entre `/*DADOS-INICIO*/` e `/*DADOS-FIM*/` em
`index.html` e confere que existe exatamente uma linha por pré-requisito.
Se o layout mudar, gere a geometria e as linhas antes; veja
`../gerador_linhas/README.md`.

As coordenadas da página são as mesmas do container do app Qt, sem
conversão: o `<svg>` usa esse sistema, e o zoom só muda o `viewBox`, a
"câmera". É a mesma ideia da `QGraphicsView` no app desktop.
