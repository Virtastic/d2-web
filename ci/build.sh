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
# One tag per recipe: worktrees on different Dockerfile versions never retag each other's image.
IMAGE="${BUILDER_IMAGE:-d2-builder64:$(sha256sum "$SRC/Dockerfile.builder64" | cut -c1-12)}"
cd "$SRC"
source "$(dirname "${BASH_SOURCE[0]}")/../tools/artifact-lock.sh"
d2_artifact_lock "$SRC"
if [ -L build-wasm ] || [ -L build-wasm/site ]; then
  echo "FATAL: refusing to build through a borrowed build-wasm/site symlink." >&2
  exit 1
fi

[ -f third_party/StormLib/CMakeLists.txt ] || { echo "FATAL: submodules not checked out (git submodule update --init)" >&2; exit 1; }

LOCK_DIR="$(cd "$(git rev-parse --git-common-dir 2>/dev/null || echo .git)/.." && pwd)/build-wasm"
mkdir -p "$LOCK_DIR"
# Rebuild the builder only when its recipe changed: the image records the Dockerfile's hash.
# The check, the rebuild and the ID read happen under one shared lock, so a parallel build never
# retags the image between another build's check and its use. Builds themselves stay parallel.
TOOLCHAIN_ID=$(
  flock 8
  want=$(sha256sum Dockerfile.builder64 | cut -c1-12)
  # Content-addressed tag: worktrees with different recipes never retag each other's image.
  IMAGE="${IMAGE%%:*}:$want"
  have=$(docker image inspect "$IMAGE" --format '{{index .Config.Labels "d2.dockerfile"}}' 2>/dev/null || true)
  if [ "$want" != "$have" ]; then
    echo "==> building $IMAGE (Dockerfile.builder64 $want)" >&2
    docker build --label "d2.dockerfile=$want" -t "$IMAGE" -f Dockerfile.builder64 . >&2
  fi
  docker image inspect "$IMAGE" --format '{{.Id}}'
) 8>"$LOCK_DIR/.builder-image.lock"
[[ "$TOOLCHAIN_ID" =~ ^sha256:[0-9a-f]{64}$ ]] || { echo "FATAL: invalid builder image ID" >&2; exit 1; }
# Pin every step, including stage/cache keys, to this one immutable ID, resolved once.
export BUILDER_IMAGE="$TOOLCHAIN_ID"

# Engine builds are rationed on this box: every worktree
# queues on the same slot pool in the main checkout. The worktree artifact lock
# also spans compilation, staging and cache publication; separate trees still build in parallel.
# D2_BUILD_SLOTS parallel builds share this checkout (default 8).
SLOTS="${D2_BUILD_SLOTS:-8}"; got=""; T0=$(date +%s)
# Merge builds jump the queue: a merge-* tree (or D2_BUILD_PRIORITY=merge)
# tries two slots ordinary builds never take before the shared pool.
IDS=$(seq 0 $((SLOTS - 1)))
case "$(basename "$PWD")" in merge-*) D2_BUILD_PRIORITY="${D2_BUILD_PRIORITY:-merge}";; esac
[ "${D2_BUILD_PRIORITY:-}" = merge ] && IDS="m0 m1 $IDS"
while [ -z "$got" ]; do
  for i in $IDS; do
    exec 9>"$LOCK_DIR/.build.lock.$i"
    if flock -n 9; then got=$i; break; fi
    exec 9>&-
  done
  [ -n "$got" ] || { [ -n "${waited:-}" ] || echo "==> waiting for a build slot ($SLOTS busy, $LOCK_DIR/.build.lock.*)"; waited=1; sleep 5; }
done
echo "==> build slot $got of $SLOTS"
WAIT=$(( $(date +%s) - T0 ))

echo "==> link (in $TOOLCHAIN_ID)"
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
# D2_COMPILER_CACHE (default on; =0 disables) shares compiler results across worktrees, never
# mutable build objects, so a build recompiles only what changed.
CACHE_ARGS=()
if [ "${D2_COMPILER_CACHE:-1}" = 1 ]; then
  mkdir -p "$LOCK_DIR/compiler-cache"
  CACHE_ARGS=(-v "$LOCK_DIR/compiler-cache:/d2-compiler-cache" -e D2_COMPILER_CACHE=1)
fi
MSYS_NO_PATHCONV=1 docker run --rm "${CACHE_ARGS[@]}" \
  -e D2_TOOLCHAIN_ID="$TOOLCHAIN_ID" -e HOME=/tmp -e D2_CXXFLAGS="${D2_CXXFLAGS:-}" -e D2_OUTPUT_OWNER="$(id -u):$(id -g)" \
  -v "$MOUNT:/build" -v "$MOUNT:$MOUNT:ro" "$TOOLCHAIN_ID" bash wasm-build/link-d2.sh

echo "==> synthetic smoke.mpq"
NODE_BIN="$(command -v node || command -v node.exe || true)"
[ -n "$NODE_BIN" ] || { echo "FATAL: node/node.exe not found" >&2; exit 1; }
"$NODE_BIN" tools/mksmoke.mjs

echo "==> precompress engine (in $TOOLCHAIN_ID)"
MSYS_NO_PATHCONV=1 docker run --rm \
  -e D2_OUTPUT_OWNER="$(id -u):$(id -g)" -v "$MOUNT:/build" -v "$MOUNT:$MOUNT:ro" "$TOOLCHAIN_ID" \
  bash -c 'bash wasm-build/make_br.sh && chown "$D2_OUTPUT_OWNER" play/d2.js.br play/d2.wasm.br'

flock -u 9; exec 9>&-

bash ci/stage.sh
if [ -z "${D2_CXXFLAGS:-}" ]; then bash tools/engine-cache.sh put . || true; else rm -f play/.engine-key; fi # custom builds never publish as normal engines
# Custom engines keep their own compiler configuration and never seed release worktrees.
# One line per build in the main checkout's timing.log.
mkdir -p "$(dirname "$LOCK_DIR")/wasm-build/harness-out"
printf '%s\tbuild\t%s\twait=%s\tdur=%s\n' "$(date -Is)" "$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo '?')" "$WAIT" "$(( $(date +%s) - T0 - WAIT ))" \
  >>"$(dirname "$LOCK_DIR")/wasm-build/harness-out/timing.log" 2>/dev/null || true
