#!/usr/bin/env bash
# tools/engine-cache.sh put|get|has [worktree]   -- built engines keyed by tools/engine-key.sh, shared by every worktree.
#   put: store this tree's play/d2.{js,wasm,*.br} under its key (ci/build.sh does this after a successful build)
#   get: copy the cached engine for this tree's key into play/ and stamp play/.engine-key (mtime refreshed)
#   has: exit 0 when the cache has this tree's key
set -euo pipefail
[ -z "${D2_CXXFLAGS:-}" ] || { echo "engine-cache: custom compiler flags cannot use the normal cache" >&2; exit 1; }
cmd=${1:-has}; WT=$(cd "${2:-.}" && pwd); MAIN="$(dirname "$(git -C "$WT" rev-parse --path-format=absolute --git-common-dir)")"
source "$(dirname "${BASH_SOURCE[0]}")/artifact-lock.sh"
# has only reads the shared cache under its per-key lock; it never touches this worktree's artifacts,
# so it must not queue behind a build holding the worktree lock (land-main waited 19 min on it).
[ "$cmd" = has ] || d2_artifact_lock "$WT"
KEY=$(bash "$WT/tools/engine-key.sh" "$WT"); C="$MAIN/build-wasm/engine-cache/$KEY"
case "$KEY" in *-dirty) [ "$cmd" = has ] && exit 1; [ "$cmd" = get ] && { echo "engine-cache: tree has uncommitted engine-source edits; build it" >&2; exit 1; };; esac
# Cache publishers and readers share a per-key lock. A reader must never copy
# half of one build and half of another, even when separate worktrees share a key.
mkdir -p "$MAIN/build-wasm/engine-cache"
exec 7>"$MAIN/build-wasm/engine-cache/.$KEY.lock"
flock -x 7
case "$cmd" in
  put) [ -f "$WT/play/d2.wasm" ] || exit 1; mkdir -p "$C"; for f in d2.js d2.wasm d2.js.br d2.wasm.br; do [ -f "$WT/play/$f" ] && cp -f "$WT/play/$f" "$C/$f"; done
       echo "$KEY" > "$WT/play/.engine-key"; echo "engine-cache: stored $KEY"
       # A tree dirty only by untracked files (the private probes every checkout holds) still proves its
       # committed source compiles: record that for land-main's gate (D2_ENGINE_KEY_COMMITTED=1 has).
       if [ -z "$(git -C "$WT" status --porcelain --untracked-files=no -- src wasm-build third_party ci/build.sh)" ]; then
         CK=$(D2_ENGINE_KEY_COMMITTED=1 bash "$WT/tools/engine-key.sh" "$WT"); mkdir -p "$MAIN/build-wasm/engine-cache/built"; : > "$MAIN/build-wasm/engine-cache/built/$CK"
       fi;;
  has) [ -f "$C/d2.wasm" ] || { [ "${D2_ENGINE_KEY_COMMITTED:-0}" = 1 ] && [ -f "$MAIN/build-wasm/engine-cache/built/$KEY" ]; };;
  get) [ -f "$C/d2.wasm" ] || exit 1; mkdir -p "$WT/play"; for f in d2.js d2.wasm d2.js.br d2.wasm.br; do [ -f "$C/$f" ] && cp -f --reflink=auto "$C/$f" "$WT/play/$f"; done
       echo "$KEY" > "$WT/play/.engine-key"; echo "engine-cache: $KEY -> play/ (cache hit)";;
esac
