#!/bin/sh
# Compila (uma vez) a cópia instrumentada do app, carrega um JSON de linhas e
# salva a renderização do Qt em PNG + a geometria medida dos diagramas.
# uso: ./render.sh <arquivo.json> <prefixo_saida>
#   gera <prefixo>.png, <prefixo>_small.png e <prefixo>_geometria.txt
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
PROJ=$(dirname "$HERE")
QMAKE=${QMAKE:-qmake}
B=${BUILD_DIR:-$HERE/build}
if [ ! -x "$B/Diagramas2" ]; then
  mkdir -p "$B/files"
  cp "$PROJ"/*.cpp "$PROJ"/*.h "$PROJ"/*.ui "$PROJ"/*.pro "$PROJ"/resources.qrc "$B"/
  rm -f "$B"/moc_* "$B"/qrc_* "$B"/ui_* "$B"/diagramas2_plugin_import.cpp
  cp "$HERE/probe/main.cpp" "$B/main.cpp"
  touch "$B/files/saved.txt"
  (cd "$B" && "$QMAKE" Diagramas2.pro >/dev/null && make -j"$(nproc)" 2>&1 | { grep -E " error" >&2 || true; }; test -x Diagramas2)
fi
cp "$1" "$B/files/saved.txt"
OUT=$(cd "$(dirname "$2")" && pwd)/$(basename "$2")
# roda na tela real (DISPLAY) para medir o tamanho de janela verdadeiro
(cd "$B" && PROBE_PNG="$OUT.png" timeout 20 ./Diagramas2 2>/dev/null > "${OUT}_geometria.txt")
python3 -c "
from PIL import Image
im=Image.open('$OUT.png'); im.resize((im.width//2, im.height//2), Image.LANCZOS).save('${OUT}_small.png')"
echo "ok: $OUT.png"
