#!/usr/bin/env bash
# Counting semaphore for heavy engine builds: D2_MAX_BUILDS slots (default 2), one flock per slot file
# under the shared build-wasm directory. Source this and call `d2_build_slot_acquire <lock-dir>`; it
# waits until a slot is free, holds it on fd 9 until the process exits (or fd 9 is closed) and sets
# D2_BUILD_SLOT to the slot number. `D2_BUILD_SLOT_NOWAIT=1` returns 1 instead of waiting.
# Merge builds try their slot first but never get extra ones: the cap is a hard cap.
d2_build_slot_acquire() {
  local dir=$1 max="${D2_MAX_BUILDS:-2}" i waited="" ids
  case "$max" in ''|*[!0-9]*|0) max=2;; esac
  mkdir -p "$dir"
  ids=$(seq 0 $((max - 1)))
  D2_BUILD_SLOT=""
  while [ -z "$D2_BUILD_SLOT" ]; do
    for i in $ids; do
      exec 9>"$dir/.build.lock.$i"
      if flock -n 9; then D2_BUILD_SLOT=$i; break; fi
      exec 9>&-
    done
    [ -n "$D2_BUILD_SLOT" ] && break
    [ "${D2_BUILD_SLOT_NOWAIT:-0}" = 1 ] && return 1
    [ -n "$waited" ] || echo "==> waiting for a build slot ($max busy, $dir/.build.lock.*)"
    waited=1; sleep "${D2_BUILD_SLOT_POLL:-5}"
  done
  export D2_BUILD_SLOT
}
