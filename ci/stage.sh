#!/usr/bin/env bash
# Stage play/ (+ vectors, retail MPQ links) into build-wasm/site/ without building the engine.
# ci/build.sh ends with this; run it alone after a JS-only or vectors-only change -- it needs a
# play/d2.{js,wasm} from an earlier build. Never run it while a sweep is serving build-wasm/site.
set -euo pipefail
SRC="${SRC:-$(cd "$(dirname "$0")/.." && pwd)}"
cd "$SRC"
[ -f play/d2.wasm ] || { echo "FATAL: play/d2.wasm missing -- run ci/build.sh first" >&2; exit 1; }
echo "==> stage build-wasm/site"
mkdir -p build-wasm/site
find build-wasm/site -mindepth 1 -maxdepth 1 -exec rm -rf {} +
cp play/index.html play/manifest.webmanifest play/streamfs.js play/gameplay-model.mjs play/gameplay-shell.mjs play/serve.mjs play/smoke.mpq play/smoke.json play/d2.js play/d2.wasm play/d2.js.br play/d2.wasm.br build-wasm/site/
cp -r play/assets build-wasm/site/assets
# Committed ground truth, served so scenarios can fetch it into the page.
cp -r vectors build-wasm/site/vectors
[ -d build-wasm/diag ] && cp -r build-wasm/diag build-wasm/site/diag   # diagnosis captures (D2_ITEMGEN_FILE=diag/...), never committed

# with the disc MPQs and the 1.13c core's patch. Symlinks, not copies (2.3 GB).
if [ -f retail/D2/d2data.mpq ]; then
  mkdir -p build-wasm/retail
  find build-wasm/retail -mindepth 1 -maxdepth 1 -exec rm -rf {} +
  for f in retail/D2/*.mpq; do ln -s "$(pwd)/$f" "build-wasm/retail/$(basename "$f")"; done
  # retail/D2/patch_d2.mpq (the same core patch, copied there for the retail capture tool) already links it
  [ -e build-wasm/retail/patch_d2.mpq ] || ln -s "$(pwd)/retail/D2/D2SE/CORES/1.13c/Patch_D2.mpq" build-wasm/retail/patch_d2.mpq
  echo "==> retail: $(ls build-wasm/retail | wc -l) MPQs linked for the test runner"
fi
PLAY=build-wasm/site sh wasm-build/version-engine.sh
