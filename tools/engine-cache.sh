#!/usr/bin/env bash
# tools/engine-cache.sh put|get|has [worktree]   -- built engines keyed by tools/engine-key.sh, shared by every worktree.
#   put: store this tree's play/d2.{js,wasm,*.br} under its key (ci/build.sh does this after a successful build)
#   get: copy the cached engine for this tree's key into play/ and stamp play/.engine-key (mtime refreshed)
#   has: exit 0 when the cache has this tree's key
set -euo pipefail
cmd=${1:-has}; WT=$(cd "${2:-.}" && pwd); MAIN="$(dirname "$(git -C "$WT" rev-parse --path-format=absolute --git-common-dir)")"
KEY=$(bash "$WT/tools/engine-key.sh" "$WT"); C="$MAIN/build-wasm/engine-cache/$KEY"
case "$KEY" in *-dirty) [ "$cmd" = has ] && exit 1; [ "$cmd" = get ] && { echo "engine-cache: tree has uncommitted engine-source edits; build it" >&2; exit 1; };; esac
case "$cmd" in
  put) [ -f "$WT/play/d2.wasm" ] || exit 1; mkdir -p "$C"; for f in d2.js d2.wasm d2.js.br d2.wasm.br; do [ -f "$WT/play/$f" ] && cp -f "$WT/play/$f" "$C/$f"; done
       echo "$KEY" > "$WT/play/.engine-key"; echo "engine-cache: stored $KEY";;
  has) [ -f "$C/d2.wasm" ];;
  get) [ -f "$C/d2.wasm" ] || exit 1; mkdir -p "$WT/play"; for f in d2.js d2.wasm d2.js.br d2.wasm.br; do [ -f "$C/$f" ] && cp -f "$C/$f" "$WT/play/$f"; done
       echo "$KEY" > "$WT/play/.engine-key"; echo "engine-cache: $KEY -> play/ (cache hit)";;
esac
