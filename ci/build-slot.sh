#!/usr/bin/env bash
# Counting semaphore for heavy engine builds: D2_MAX_BUILDS slots (default 2), one flock per slot file
# under the shared build-wasm directory. Source this and call `d2_build_slot_acquire <lock-dir>`; it
# waits until a slot is free, holds it on fd 9 until the process exits (or fd 9 is closed) and sets
# D2_BUILD_SLOT to the slot number. `D2_BUILD_SLOT_NOWAIT=1` returns 1 instead of waiting.
# Merge builds try their slot first but never get extra ones: the cap is a hard cap.
# The last slot is reserved for D2_BUILD_ROLE=merge or coverage (when the cap is above 1): other builds never take it, so a
# queue of agent builds cannot starve a merge or the shared prove. Merge, coverage and prove builds may use any slot, the reserved one first.
d2_build_slot_acquire() {
  local dir=$1 max="${D2_MAX_BUILDS:-2}" i waited="" ids
  case "$max" in ''|*[!0-9]*|0) max=2;; esac
  mkdir -p "$dir"
  # D2_BUILD_ROLE=main (the build that lands main, from the main checkout) has its own slot outside the cap:
  # nothing else ever takes .build.lock.main, so landing main never queues behind agent, prove or merge builds.
  if [ "${D2_BUILD_ROLE:-}" = main ]; then
    exec 9>"$dir/.build.lock.main"; flock 9; D2_BUILD_SLOT=main; export D2_BUILD_SLOT; return 0
  fi
  ids=$(seq 0 $((max - 1)))
  if [ "${D2_BUILD_ROLE:-}" = merge ] || [ "${D2_BUILD_ROLE:-}" = coverage ] || [ "${D2_BUILD_ROLE:-}" = prove ]; then ids=$(seq $((max - 1)) -1 0)
  elif [ "$max" -gt 1 ]; then ids=$(seq 0 $((max - 2))); fi
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

# One build per tree key. Call `d2_build_key_lock <lock-dir> <engine-key>` before taking a slot: identical trees (same
# engine key, e.g. many prove worktrees merged onto the same main) queue on one flock here without holding a slot, and the
# one behind finds the first build's engine in the cache. Dirty keys are never deduped. Held on fd 6 until the process exits.
d2_build_key_lock() {
  local dir=$1 key=$2
  case "$key" in ''|*-dirty|*/*) return 1;; esac
  mkdir -p "$dir"
  exec 6>"$dir/.build-key.$key.lock"
  flock -x 6
}
