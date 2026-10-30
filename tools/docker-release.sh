#!/usr/bin/env bash
# Build the runtime image from the staged site and push it to ghcr as <tag> and latest.
# Usage: tools/docker-release.sh <tag> [--no-push]   (run ci/build.sh first; needs `gh auth refresh -s write:packages`)
set -euo pipefail
cd "$(dirname "$0")/.."
TAG="${1:?usage: docker-release.sh <tag> [--no-push]}"
IMAGE="${D2_IMAGE:-ghcr.io/virtastic/d2-web}"
[ -f build-wasm/site/assets/patch-113c.patch ] || { echo "FATAL: no patch shim staged -- stage with retail/D2/patch_d2.mpq or D2_PATCH_SHIM_SRC" >&2; exit 1; }
[ -f "build-wasm/site/e/$(cat build-wasm/site/engine-version.txt 2>/dev/null)/d2.wasm" ] && [ -f build-wasm/site/serve.mjs ] || { echo "FATAL: no staged site -- run ci/build.sh" >&2; exit 1; }
[ -f build-wasm/site/widebg/manifest.json ] && [ ! -L build-wasm/site/widebg/manifest.json ] || { echo "FATAL: no widescreen frontend staged (widebg/) -- the image would open 4:3; stage with .widebg or D2_WIDEBG_DIR" >&2; exit 1; }
CTX=$(mktemp -d); trap 'rm -rf "$CTX"' EXIT
# The image ships the widescreen frontend (widebg/, the default view) and the 1.13c patch shim; vectors/, diag/ and any retail link stay out. -h copies files, never links.
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
