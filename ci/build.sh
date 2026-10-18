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

# Engine builds are rationed on this box: every worktree
# queues on the same lock in the main checkout. Staging (ci/stage.sh) stays outside it.
LOCK_DIR="$(cd "$(git rev-parse --git-common-dir 2>/dev/null || echo .git)/.." && pwd)/build-wasm"
mkdir -p "$LOCK_DIR"
# D2_BUILD_SLOTS parallel builds share this checkout (default 8).
SLOTS="${D2_BUILD_SLOTS:-5}"; got=""; T0=$(date +%s)
while [ -z "$got" ]; do
  for i in $(seq 0 $((SLOTS - 1))); do
    exec 9>"$LOCK_DIR/.build.lock.$i"
    if flock -n 9; then got=$i; break; fi
    exec 9>&-
  done
  [ -n "$got" ] || { [ -n "${waited:-}" ] || echo "==> waiting for a build slot ($SLOTS busy, $LOCK_DIR/.build.lock.*)"; waited=1; sleep 5; }
done
echo "==> build slot $got of $SLOTS"
WAIT=$(( $(date +%s) - T0 ))

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

flock -u 9; exec 9>&-

bash ci/stage.sh
bash tools/engine-cache.sh put . || true   # share this build with every worktree (tools/engine-sync.sh)
# One line per build in the main checkout's timing.log.
mkdir -p "$(dirname "$LOCK_DIR")/wasm-build/harness-out"
printf '%s\tbuild\t%s\twait=%s\tdur=%s\n' "$(date -Is)" "$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo '?')" "$WAIT" "$(( $(date +%s) - T0 - WAIT ))" \
  >>"$(dirname "$LOCK_DIR")/wasm-build/harness-out/timing.log" 2>/dev/null || true
