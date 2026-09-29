# Fluxograma ECA — versão web

Feito por **Nettoflix** — <https://nettoflix.github.io/DiagramasECA/web/>

`index.html` é a versão web do app, com os dados do currículo 20241
embutidos. Usa também `roteador.js` e `curriculo.js` (da mesma pasta), e
nenhuma outra dependência além das fontes do Google Fonts. Abre em qualquer
navegador, inclusive no celular, e também direto do disco (`file://`).

| Arquivo | O que é |
|---|---|
| `index.html` | A página, com os dados padrão embutidos (gerados por `gerar_dados.py`). |
| `editor.html` | Editor de currículo em formulário: lê e grava um `disciplinas.txt` sem precisar mexer no texto. |
| `curriculo.js` | Lê e escreve um `disciplinas.txt`, monta a grade como o app Qt e chama o roteador. |
| `roteador.js` | Calcula as linhas entre as disciplinas. É o mesmo arquivo usado pelo app Qt. |
| `gerar_dados.py` | Embute os dados padrão (e o texto de `disciplinas.txt`, para o editor) em `index.html`. |

## O que faz

- Clicar numa disciplina marca ou desmarca como concluída. Desmarcar uma
  disciplina também desmarca, em cascata, as que dependem dela.
- Clicar numa disciplina bloqueada mostra quais pré-requisitos faltam e,
  quando for o caso, quantas horas-aula obrigatórias ainda faltam.
- Carga horária: cada disciplina mostra as suas horas-aula (H/A). Três
  disciplinas também exigem um mínimo de horas-aula **obrigatórias**
  concluídas, que é o "Pré CH" do currículo 20241: Ética e Aspectos de
  Segurança (2.300), Gestão Econômica e de Investimentos (900) e Projeto
  de Fim de Curso (3.000). Optativas não entram nessa soma. As horas vêm
  de `ha:`/`preCH:` em `../files/disciplinas.txt`.
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

## Carregar outro currículo

O botão **Carregar currículo…** abre um arquivo no formato de
`../files/disciplinas.txt` (descrito no cabeçalho dele). A página:

1. confere o arquivo e, se houver erro, mostra as primeiras linhas com
   problema e continua no currículo atual;
2. guarda o texto no navegador (`localStorage`) e recarrega;
3. monta a grade e calcula as linhas (uns 3 s, com o aviso "Calculando as
   linhas…"). O resultado também fica guardado, e as próximas aberturas
   são instantâneas.

**Currículo padrão** volta aos dados embutidos. O progresso marcado é o
mesmo nos dois casos: fica salvo pelo código de cada disciplina.

## Editar um currículo (sem mexer no texto)

**Editar currículo** abre `editor.html` com o currículo que está na tela
(o padrão ou o carregado). O editor também abre qualquer `.txt` pelo botão
**Abrir arquivo…**, ou começa do zero com **Novo**. É pensado para a
coordenação e a secretaria manterem o arquivo oficial, e para o estudante
fazer a própria versão.

- Cada fase é um bloco, com uma linha de formulário por disciplina:
  código, nome, horas-aula, obrigatória ou optativa, Pré CH, fase e linha
  da grade (a posição na coluna; ↑ e ↓ trocam com a vizinha).
- Pré-requisitos: digite o código ou o nome e escolha na lista, que só
  oferece disciplinas de fases anteriores. Mudar o código de uma
  disciplina oferece trocar também nos pré-requisitos das outras; remover
  uma disciplina tira ela dos pré-requisitos.
- Grupos temáticos: nome, identificador e cor; cada disciplina marca os
  seus.
- Os problemas aparecem na hora, na própria disciplina e numa lista no
  topo (código repetido, pré-requisito numa fase igual ou posterior, linha
  ocupada...). O editor confere o resultado também com `Curriculo.ler()`,
  o mesmo leitor do botão *Carregar currículo…*.
- **Baixar .txt** salva o arquivo; **Ver no fluxograma** mostra o
  currículo na página (fica guardado só no navegador, como um arquivo
  carregado). Enquanto isso, o trabalho fica num rascunho no navegador.
- As "Observações" são os comentários do topo do arquivo. Comentários no
  meio do arquivo se perdem ao salvar pelo editor, e as colunas são
  realinhadas.

Para trocar o currículo oficial, baixe o `.txt`, substitua
`../files/disciplinas.txt` e siga "Atualizar o currículo" no README
principal.

Para que as linhas saiam idênticas às do app Qt, `curriculo.js` reproduz o
leitor de `widget.cpp` e a geometria do `QGridLayout`: margens de 9 px,
colunas de 300 px com 10 px entre elas, título de 200 px, linhas de 159 px
com 6 px entre elas, e colunas e linhas vazias sem ocupar espaço. Também
passa ao roteador as mesmas opções que o app (60 mil iterações, sementes 1, 2 e 3).
O roteador usa um gerador aleatório com semente, então o resultado é o
mesmo no navegador e no Qt. Ao mudar o método em `roteador.js`, mude
também `versao` no fim dele, para a página descartar as linhas guardadas.

## Atualizar os dados padrão

Os dados embutidos vêm das mesmas fontes do app Qt:

| Fonte | Conteúdo |
|---|---|
| `../files/disciplinas.txt` | disciplinas, pré-requisitos, horas-aula e grupos |
| `../files/linhas.json` | posição de cada caixa e linhas (calculadas por `roteador.js`) |

Depois de mudar o currículo ou as linhas:

```bash
python3 gerar_dados.py
```

O script reescreve o bloco entre `/*DADOS-INICIO*/` e `/*DADOS-FIM*/` em
`index.html` e confere que existe exatamente uma linha por pré-requisito.
Se as posições ou os pré-requisitos mudarem, abra o app Qt uma vez antes,
para ele atualizar `../files/linhas.json`. O script avisa se esse arquivo
estiver desatualizado.

As coordenadas da página são as mesmas do container do app Qt, sem
conversão: o `<svg>` usa esse sistema, e o zoom só muda o `viewBox`, a
"câmera". É a mesma ideia da `QGraphicsView` no app desktop.
