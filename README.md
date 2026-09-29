# Fluxograma ECA

Fluxograma interativo do curso de **Engenharia de Controle e Automação**
(currículo 20241). Você marca as disciplinas que já cursou, e ele mostra o
que está liberado para cursar, o que ainda está bloqueado e por quê,
incluindo as exigências de carga horária.

**Use no navegador (PC ou celular):**
<https://nettoflix.github.io/DiagramasECA/web/>

![Modo Situação: concluídas em verde, disponíveis em azul, bloqueadas em cinza](docs/situacao.png)

> **Aviso:** projeto pessoal e **não oficial**. Os pré-requisitos e as
> cargas horárias foram conferidos com o currículo 20241 do curso, mas
> confira sempre no CAGR antes da matrícula. Quem está num currículo mais
> antigo pode ter pré-requisitos e códigos diferentes.

## O que dá para fazer

- **Marcar o que você já cursou.** Um toque ou clique numa disciplina marca
  ou desmarca. Desmarcar uma disciplina também desmarca, em cascata, as que
  dependiam dela.
- **Ver o que está liberado.** No modo *Situação*:
  - **verde ✓**: concluída;
  - **azul**: disponível para cursar agora;
  - **cinza**: bloqueada.
- **Saber o que falta.** Tocar numa disciplina bloqueada mostra quais
  pré-requisitos faltam e quantas horas-aula ainda são necessárias.
- **Acompanhar a carga horária.** O cabeçalho soma as horas-aula
  obrigatórias concluídas. Três disciplinas exigem um mínimo de horas
  obrigatórias concluídas (o "Pré CH" do currículo):

  | Disciplina | Exige |
  |---|---|
  | Ética e Aspectos de Segurança (DAS5402) | 2.300 h/a |
  | Gestão Econômica e de Investimentos (EPS7076) | 900 h/a |
  | Projeto de Fim de Curso (DAS5512) | 3.000 h/a |

- **Ver as áreas do curso.** No modo *Grupo*, as disciplinas são coloridas
  por área: Informática, Controle, Automação, Mecânica, Elétrica e Física e
  Cálculo. Disciplinas de duas áreas aparecem com a caixa dividida na
  diagonal. Clicar num grupo na legenda destaca só as disciplinas dele.
- **Seguir as dependências.** No PC, passar o mouse sobre uma disciplina
  destaca de onde ela vem e o que ela libera.
- **Navegar com zoom.**
  - `Ver tudo`, `−` e `+`;
  - os botões `1ª`…`10ª` focam numa fase, e Shift+clique noutra fase foca
    o intervalo entre elas;
  - pinça ou roda do mouse para zoom, e arrastar para mover.

O progresso fica salvo **só no seu navegador**: sem login e sem servidor.

![Modo Grupo: disciplinas coloridas por área](docs/grupo.png)

## Como o projeto está organizado

O repositório tem duas versões do mesmo fluxograma, que usam os mesmos
dados:

| Pasta / arquivo | O que é |
|---|---|
| `web/` | **Versão web**, a que os colegas usam. Um único `index.html`, com os dados embutidos. Veja [`web/README.md`](web/README.md). |
| `*.cpp`, `*.h`, `Diagramas2.pro` | **Aplicativo desktop** em Qt 5 (C++), a versão original. |
| `files/disciplinas.txt` | **Fonte única dos dados do curso**, organizada por fase: disciplinas, posição na grade, pré-requisitos, horas-aula e grupos. O formato está descrito no cabeçalho do próprio arquivo. O app lê o arquivo ao abrir, e `disciplinas.py` o lê para os scripts Python. |
| `files/saved.txt` | Linhas do fluxograma (coordenadas) e o progresso salvo pelo app desktop. |
| `gerador_linhas/` | Scripts que **geram automaticamente** as linhas entre as disciplinas (roteamento ortogonal otimizado). Veja [`gerador_linhas/README.md`](gerador_linhas/README.md). |
| `docs/` | Imagens deste README. |

### Fluxo de dados

```
files/disciplinas.txt  ──(Qt: posições medidas)──►  gerador_linhas/  ──►  files/saved.txt
   │                                                               │
   └──────────────────►  web/gerar_dados.py  ◄────────────────────┘
                                 │
                                 ▼
                          web/index.html
```

## Atualizar o currículo

Quando mudar algum pré-requisito, disciplina, carga horária ou grupo:

1. Edite `files/disciplinas.txt` (não precisa recompilar: o app lê o
   arquivo da pasta `files/` ao lado do executável).
2. Se o **layout** mudou (disciplina nova ou que mudou de lugar), regere as
   linhas:

   ```bash
   cd gerador_linhas
   ./render.sh ../files/saved.txt medicao              # mede as posições reais
   cp medicao_geometria.txt dados/geometria_medida.txt
   python3 route2.py 200000 1 saida.json               # rode algumas sementes e fique com a de menor custo
   python3 validate.py saida.json
   ```

   Depois copie as linhas para `files/saved.txt`. O passo a passo completo
   está em `gerador_linhas/README.md`.
3. Regere os dados da versão web:

   ```bash
   python3 web/gerar_dados.py
   ```

4. Faça commit e push. O GitHub Pages publica o novo `web/index.html`.

Se só mudou pré-requisito, carga horária ou grupo (sem mexer no layout),
basta o passo 3. Porém, uma relação de pré-requisito **nova** precisa de uma
linha nova, e então também é preciso regerar as linhas (passo 2).

## Aplicativo desktop (Qt)

Requer Qt 5 (testado com Qt 5.9.6) e um compilador C++.

```bash
qmake Diagramas2.pro
make
./Diagramas2
```

O app lê e grava `files/saved.txt` na pasta do executável.

Controles:
- **Tecla `1`:** salva o progresso.
- **Tecla `2`:** recarrega.
- **Zoom:** barra de ferramentas, Ctrl+roda do mouse, Ctrl+0, Ctrl+= e Ctrl+−.
- **Mover:** arrastar com o botão do meio.

Há também um modo de edição manual de linhas, usado antes do gerador
automático:
- **Shift** (apertar e soltar) liga e desliga o modo.
- Clique na disciplina de origem e depois nos pontos da linha.
- **H** ou **V** seguradas travam o segmento na horizontal ou na vertical.
- **`+`** e **`-`** trocam de linha.
- **C** apaga todas as linhas.

## Créditos

Feito por **Nettoflix**. Encontrou um pré-requisito errado ou tem uma ideia?
Abra uma *issue* aqui no GitHub ou me chame.
