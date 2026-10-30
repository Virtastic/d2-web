#!/usr/bin/env bash
# Stage play/ (+ vectors, retail MPQ links) into build-wasm/site/ without building the engine.
# ci/build.sh ends with this; run it alone after a JS-only or vectors-only change -- it needs a
# play/d2.{js,wasm} from an earlier build. Never run it while a sweep is serving build-wasm/site.
set -euo pipefail
SRC="${SRC:-$(cd "$(dirname "$0")/.." && pwd)}"
cd "$SRC"
# A borrowed site is read-only. Following this link would overwrite another
# run's immutable client while leaving its engine stamp unchanged.
if [ -L build-wasm ] || [ -L build-wasm/site ]; then
  echo "FATAL: refusing to stage through a borrowed build-wasm/site symlink." >&2
  echo "       unlink the borrowed site and stage into this worktree's own directory." >&2
  exit 1
fi
source "$(dirname "${BASH_SOURCE[0]}")/../tools/artifact-lock.sh"
d2_artifact_lock "$SRC"
[ -f play/d2.wasm ] || { echo "FATAL: play/d2.wasm missing -- run ci/build.sh first" >&2; exit 1; }

# A STALE ENGINE MUST NOT STAGE SILENTLY. play/d2.{js,wasm} are this worktree's own build artefacts,
# and a worktree's artefacts are as old as the last build IN THAT WORKTREE -- tools/worktree-new.sh
# seeds them from main at creation, so a worktree made yesterday stages yesterday's engine today. The
# sweep then measures the wrong build: on 2026-09-27 coord-merge2 staged a 30-hour-old wasm that
# lacked `d2_session_level_rain`, and the batch's five bootstrap rows "failed" against an engine that
# simply had no such export. Nothing said so; stage printed a normal engine stamp and the sweep ran.
# Gate on it: if any engine source is NEWER than the wasm about to be staged, the wasm cannot have
# been built from it. D2_STAGE_ALLOW_STALE=1 overrides for the deliberate case.
if [ "${D2_STAGE_ALLOW_STALE:-0}" != "1" ] && [ "$(cat play/.engine-key 2>/dev/null)" != "$(bash tools/engine-key.sh . 2>/dev/null)" ]; then
  newer=$(find src wasm-build -newer play/d2.wasm -type f \( -name '*.cpp' -o -name '*.h' -o -name '*.hpp' -o -name '*.sh' \) -not -path '*/build-*' 2>/dev/null | head -1)
  if [ -n "$newer" ]; then
    echo "FATAL: play/d2.wasm is OLDER than the engine source that would be staged." >&2
    echo "       newer than the wasm: $newer" >&2
    echo "       built: $(date -r play/d2.wasm '+%Y-%m-%d %H:%M')   source: $(date -r "$newer" '+%Y-%m-%d %H:%M')" >&2
    echo "       run ci/build.sh in THIS worktree first, or set D2_STAGE_ALLOW_STALE=1 if deliberate." >&2
    exit 1
  fi
fi
echo "==> stage build-wasm/site"
mkdir -p build-wasm/site
find build-wasm/site -mindepth 1 -maxdepth 1 -exec rm -rf {} +
cp play/index.html play/manifest.webmanifest play/streamfs.js play/entry.js play/gameplay-model.mjs play/gameplay-shell.mjs play/item-pick.mjs play/worker-guard.mjs play/frame-pack.mjs play/frame-snapshot.mjs play/render-host.mjs play/render-wire.mjs play/render-worker.mjs play/render-worker-draws.mjs play/render-worker-hud.mjs play/serve.mjs play/br-check.mjs play/smoke.mpq play/smoke.json play/d2.js play/d2.wasm build-wasm/site/
# A .br is staged only when it IS that file's brotli (play/br-check.mjs -- content, not mtime). Staging
# a stale one puts one build's bytes behind the other build's e/<ver>/ URL, and nothing else catches it:
# that URL is served immutable and the page boots without error -- on the engine the .br holds. Dropping
# it costs a slower download, never a wrong engine.
NODE_BIN="${NODE_BIN:-$(command -v node || true)}"
[ -n "$NODE_BIN" ] || NODE_BIN="$HOME/.local/bin/node"
HAVE_NODE=1; [ -x "$NODE_BIN" ] || command -v "$NODE_BIN" >/dev/null 2>&1 || HAVE_NODE=
for f in play/d2.js play/d2.wasm; do
  [ -f "$f.br" ] || continue
  [ -n "$HAVE_NODE" ] || { echo "WARNING: node not found -- $f.br NOT staged (cannot verify it belongs to $f)" >&2; continue; }
  if "$NODE_BIN" play/br-check.mjs "$f" "$f.br"; then cp "$f.br" build-wasm/site/
  else echo "WARNING: $f.br does not decompress to $f -- NOT staged (a browser would have run the engine it holds, not this one). Rebuild it: bash wasm-build/make_br.sh" >&2; fi
done
cp -r play/assets build-wasm/site/assets
# BYO front door: D2_ASSETS=byo stages a site that hosts no base archives (the page reads site-config.json).
if [ "${D2_ASSETS:-}" = "byo" ]; then echo '{"assets":"byo"}' > build-wasm/site/site-config.json; else cp play/site-config.json build-wasm/site/; fi
# The 1.13c patch shim the BYO page mounts over the player's own patch (tools/patch-shim.mjs; never committed).
SHIM_SRC="${D2_PATCH_SHIM_SRC:-retail/D2/patch_d2.mpq}"
if [ -f "$SHIM_SRC" ] && [ -n "$HAVE_NODE" ]; then "$NODE_BIN" tools/patch-shim.mjs "$SHIM_SRC" build-wasm/site/assets/patch-113c.patch
else echo "WARNING: no $SHIM_SRC -- BYO players run on their own Patch_D2.mpq" >&2; fi
# Committed ground truth, served so scenarios can fetch it into the page.
[ -d vectors ] && mkdir -p build-wasm/site/vectors && cp -ru vectors/. build-wasm/site/vectors/
FR="${D2_FREERES_DIR:-third-party/sgd2freeres}"
[ -d "$FR" ] && mkdir -p build-wasm/site/freeres && cp "$FR"/*.dc6 build-wasm/site/freeres/ || true
# Local widescreen frontend backgrounds (retail-derived, gitignored): the MAIN checkout's .widebg/, served as /widebg/.
WB="${D2_WIDEBG_DIR:-$(dirname "$(git rev-parse --path-format=absolute --git-common-dir 2>/dev/null)")/.widebg}"
[ -f "$WB/manifest.json" ] && cp -rL "$WB" build-wasm/site/widebg
[ -d build-wasm/diag ] && cp -r build-wasm/diag build-wasm/site/diag   # diagnosis captures (D2_ITEMGEN_FILE=diag/...), never committed

# with the disc MPQs and the 1.13c core's patch. Symlinks, not copies (2.3 GB).
if [ -f retail/D2/d2data.mpq ]; then
  mkdir -p build-wasm/retail
  find build-wasm/retail -mindepth 1 -maxdepth 1 -exec rm -rf {} +
  for f in retail/D2/*.mpq; do ln -s "$(pwd)/$f" "build-wasm/retail/$(basename "$f")"; done
  # retail/D2/patch_d2.mpq (the same core patch, copied there for the retail capture tool) already links it
  [ -e build-wasm/retail/patch_d2.mpq ] || ln -s "$(pwd)/retail/D2/D2SE/CORES/1.13c/Patch_D2.mpq" build-wasm/retail/patch_d2.mpq
  [ -f retail/D2/PlugY/StashBtns.dc6 ] && ln -s "$(pwd)/retail/D2/PlugY" build-wasm/retail/PlugY   # PY3: PlugY button art, served under /retail/PlugY/
  echo "==> retail: $(ls build-wasm/retail | wc -l) MPQs linked for the test runner"
fi
PLAY=build-wasm/site sh wasm-build/version-engine.sh
