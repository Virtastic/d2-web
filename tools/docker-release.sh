#!/usr/bin/env bash
# Build the runtime image from the staged site and push it to ghcr as <tag> and latest.
# Usage: tools/docker-release.sh <tag> [--no-push]   (run ci/build.sh first; needs `gh auth refresh -s write:packages`)
set -euo pipefail
cd "$(dirname "$0")/.."
TAG="${1:?usage: docker-release.sh <tag> [--no-push]}"
IMAGE="${D2_IMAGE:-ghcr.io/virtastic/d2-web}"
[ -f build-wasm/site/assets/patch-113c.patch ] || { echo "FATAL: no patch shim staged -- run node tools/setup.mjs \"/path/to/Diablo II\", then bash ci/build.sh" >&2; exit 1; }
node tools/patch-shim.mjs --check build-wasm/site/assets/patch-113c.patch   # exits 1 unless it is the 1.13c Patch_D2.mpq
[ -f "build-wasm/site/e/$(cat build-wasm/site/engine-version.txt 2>/dev/null)/d2.wasm" ] && [ -f build-wasm/site/serve.mjs ] || { echo "FATAL: no staged site -- run ci/build.sh" >&2; exit 1; }
if [ -e build-wasm/site/widebg/manifest.json ]; then
  [ ! -L build-wasm/site/widebg/manifest.json ] || { echo "FATAL: widebg/manifest.json is a link -- restage with ci/stage.sh (it copies .widebg/ with cp -rL)" >&2; exit 1; }
  for k in trademarkscreenexp gameselectscreenexp characterselectscreenexp charactercreationscreenexp creditsbckgexpand bnet; do
    grep -q "\"$k\"" build-wasm/site/widebg/manifest.json || echo "WARNING: widebg/manifest.json has no \"$k\" -- that screen (the Battle.net lobby for bnet) falls back to the game's 4:3 background" >&2
  done
else echo "WARNING: no widescreen menu backgrounds staged (widebg/) -- the menus use the game's 4:3 backgrounds; stage with .widebg/ or D2_WIDEBG_DIR" >&2; fi
[ -f build-wasm/site/door-art/meta.json ] || { echo "FATAL: no title-screen art staged (door-art/) -- run node tools/setup.mjs \"/path/to/Diablo II\", then bash ci/build.sh" >&2; exit 1; }
[ "$(ls build-wasm/site/freeres/*.dc6 2>/dev/null | wc -l)" -eq 16 ] && [ -f build-wasm/site/freeres/LICENSE.md ] || { echo "FATAL: no widescreen HUD art staged (freeres/: 16 DC6 + LICENSE.md) -- the wide bar would not draw; run ci/stage.sh (art is tracked in play/freeres/)" >&2; exit 1; }
CTX=$(mktemp -d); trap 'rm -rf "$CTX"' EXIT
# The image ships the BYO door art, the widescreen menu backgrounds when staged (widebg/) and the 1.13c patch shim; vectors/, diag/ and any retail link stay out. -h copies files, never links.
tar -h -C build-wasm/site --exclude=./vectors --exclude=./diag --exclude=./retail --exclude=./assets/combat-sound-queue.mjs --exclude=./assets/item-cursor-art.mjs --exclude=./assets/waypoint-arrival-evidence.mjs --exclude=./assets/weather-evidence.mjs -cf - . | tar -C "$CTX" -xf -
cp Dockerfile docker-entrypoint.sh "$CTX/"
docker build -t "$IMAGE:$TAG" -t "$IMAGE:latest" \
  --label org.opencontainers.image.source=https://github.com/Virtastic/d2-web \
  --label org.opencontainers.image.version="$TAG" "$CTX"
[ "${2:-}" = "--no-push" ] && exit 0
gh auth token | docker login ghcr.io -u "$(gh api user -q .login)" --password-stdin
docker push "$IMAGE:$TAG"
docker push "$IMAGE:latest"
echo "pushed $IMAGE:$TAG"
