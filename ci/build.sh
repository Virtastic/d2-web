#!/usr/bin/env bash
# Build the engine and stage a versioned site in build-wasm/site/. Runs by hand on any box with
# docker+node, and as the GitHub Actions build step. Thin on purpose: every step is a script that
# can be re-run alone.
#   1. d2-builder64:1 image (built once; rebuilt only when Dockerfile.builder64 changes)
#   2. wasm-build/link-d2.sh inside it        -> play/d2.{js,wasm}
#   3. synthetic smoke MPQ (tools/mksmoke.mjs) -> play/smoke.mpq + smoke.json (expected values)
#   4. make_br.sh, then a COPY of play/ into build-wasm/site/ that version-engine.sh stamps.
#      A copy, because version-engine rewrites index.html in place and moves the engine into
#      e/<hash>/; doing that to the tracked play/ dirties the checkout on every build.
set -euo pipefail
SRC="${SRC:-$(cd "$(dirname "$0")/.." && pwd)}"
IMAGE="${BUILDER_IMAGE:-d2-builder64:1}"
cd "$SRC"

[ -f third_party/StormLib/CMakeLists.txt ] || { echo "FATAL: submodules not checked out (git submodule update --init)" >&2; exit 1; }

# Rebuild the builder only when its recipe changed: the image records the Dockerfile's hash.
want=$(sha256sum Dockerfile.builder64 | cut -c1-12)
have=$(docker image inspect "$IMAGE" --format '{{index .Config.Labels "d2.dockerfile"}}' 2>/dev/null || true)
if [ "$want" != "$have" ]; then
  echo "==> building $IMAGE (Dockerfile.builder64 $want)"
  docker build --label "d2.dockerfile=$want" -t "$IMAGE" -f Dockerfile.builder64 .
fi

echo "==> link (in $IMAGE)"
# Create the output parent as the invoking user; otherwise the root container creates it
# and a fresh Linux checkout cannot stage build-wasm/site after compilation.
mkdir -p build-wasm
# The container builds as root because Windows/WSL bind mounts can expose existing outputs under
# a SID-derived uid that differs from both WSL and Git Bash. link-d2.sh restores ownership of only
# its generated outputs before exit, so Linux CI and self-hosted checkouts do not retain root files.
# HOME=/tmp: emscripten writes a cache-config under $HOME.
# pwd -W: git-bash on Windows, where the daemon wants C:/... and MSYS would otherwise mangle
# the "/c/...:/build" pair into a semicolon list. MSYS_NO_PATHCONV stops the second mangling.
MOUNT="$(pwd -W 2>/dev/null || pwd)"
MSYS_NO_PATHCONV=1 docker run --rm \
  -e HOME=/tmp -e D2_CXXFLAGS="${D2_CXXFLAGS:-}" -e D2_OUTPUT_OWNER="$(id -u):$(id -g)" \
  -v "$MOUNT:/build" "$IMAGE" bash wasm-build/link-d2.sh

echo "==> synthetic smoke.mpq"
NODE_BIN="$(command -v node || command -v node.exe || true)"
[ -n "$NODE_BIN" ] || { echo "FATAL: node/node.exe not found" >&2; exit 1; }
"$NODE_BIN" tools/mksmoke.mjs

echo "==> precompress engine (in $IMAGE)"
MSYS_NO_PATHCONV=1 docker run --rm \
  -e D2_OUTPUT_OWNER="$(id -u):$(id -g)" -v "$MOUNT:/build" "$IMAGE" \
  bash -c 'bash wasm-build/make_br.sh && chown "$D2_OUTPUT_OWNER" play/d2.js.br play/d2.wasm.br'

echo "==> stage build-wasm/site"
mkdir -p build-wasm/site
find build-wasm/site -mindepth 1 -maxdepth 1 -exec rm -rf {} +
cp play/index.html play/streamfs.js play/gameplay-model.mjs play/gameplay-shell.mjs play/serve.mjs play/smoke.mpq play/smoke.json play/d2.js play/d2.wasm play/d2.js.br play/d2.wasm.br build-wasm/site/
cp -r play/assets build-wasm/site/assets
# Committed ground truth, served so scenarios can fetch it into the page.
cp -r vectors build-wasm/site/vectors
[ -d build-wasm/diag ] && cp -r build-wasm/diag build-wasm/site/diag   # diagnosis captures (D2_ITEMGEN_FILE=diag/...), never committed

# Retail MPQs for the retail-gated scenarios, when this box has them (never in CI): one dir
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
