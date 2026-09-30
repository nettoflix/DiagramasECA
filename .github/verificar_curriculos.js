#!/usr/bin/env node
// Confere files/curriculos/ com o mesmo leitor da página (web/curriculo.js):
//   - lista.txt: cada linha aponta para um .txt que existe, sem repetição;
//   - cada currículo da lista é lido sem erros e tem ao menos uma disciplina;
//   - avisa (sem falhar) dos .txt da pasta que não estão na lista e se o
//     ECA 2024 da lista divergir de files/disciplinas.txt.
// Uso: node .github/verificar_curriculos.js   (sai com 1 se houver erro)
'use strict';
const fs = require('fs');
const path = require('path');
const Curriculo = require('../web/curriculo.js');

const RAIZ = path.join(__dirname, '..');
const PASTA = path.join(RAIZ, 'files', 'curriculos');
const PADRAO = 'engenharia_de_controle_e_automacao_20241.txt';  // o mesmo ARQUIVO_PADRAO de web/index.html
const erros = [], avisos = [];

// mesma regra de lerLista() em web/index.html
const lista = fs.readFileSync(path.join(PASTA, 'lista.txt'), 'utf8').split(/\r?\n/)
  .map((l, i) => [l.trim(), i + 1]).filter(([l]) => l && !l.startsWith('#'))
  .map(([l, n]) => { const [arquivo, nome] = l.split('|').map(c => c.trim()); return { arquivo, nome, n }; });

if (!lista.length) erros.push('lista.txt: nenhum currículo');
const vistos = new Set();
for (const { arquivo, nome, n } of lista) {
  const onde = `lista.txt:${n}`;
  if (!/^[^/\\]+\.txt$/i.test(arquivo)) { erros.push(`${onde}: "${arquivo}" não é o nome de um .txt desta pasta`); continue; }
  if (!nome) avisos.push(`${onde}: ${arquivo} sem nome exibido (a lista mostra o nome do arquivo)`);
  if (vistos.has(arquivo)) { erros.push(`${onde}: ${arquivo} repetido`); continue; }
  vistos.add(arquivo);
  const caminho = path.join(PASTA, arquivo);
  if (!fs.existsSync(caminho)) { erros.push(`${onde}: ${arquivo} não existe em files/curriculos/`); continue; }
  const lido = Curriculo.ler(fs.readFileSync(caminho, 'utf8'));
  lido.erros.forEach(e => erros.push(`${arquivo}: ${e}`));
  if (!lido.erros.length && !lido.disciplinas.length) erros.push(`${arquivo}: nenhuma disciplina`);
  if (!lido.erros.length) console.log(`ok  ${arquivo} (${lido.disciplinas.length} disciplinas) — ${nome || arquivo}`);
}

for (const f of fs.readdirSync(PASTA))
  if (/\.txt$/i.test(f) && f !== 'lista.txt' && !vistos.has(f)) avisos.push(`${f} está na pasta, mas não na lista.txt`);

const essencia = t => { const l = Curriculo.ler(t); return JSON.stringify([l.disciplinas, l.grupos]); };
if (vistos.has(PADRAO) &&
    essencia(fs.readFileSync(path.join(PASTA, PADRAO), 'utf8')) !== essencia(fs.readFileSync(path.join(RAIZ, 'files', 'disciplinas.txt'), 'utf8')))
  avisos.push(`${PADRAO} difere de files/disciplinas.txt: a página vai calcular as linhas no navegador em vez de usar as embutidas`);

// formato de anotação do GitHub Actions; no terminal, fica como texto comum
const gh = !!process.env.GITHUB_ACTIONS;
avisos.forEach(a => console.log(gh ? `::warning::${a}` : `aviso: ${a}`));
erros.forEach(e => console.log(gh ? `::error::${e}` : `ERRO: ${e}`));
console.log(`${lista.length} currículos na lista, ${erros.length} erro(s), ${avisos.length} aviso(s)`);
process.exit(erros.length ? 1 : 0);
