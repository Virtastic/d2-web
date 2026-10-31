#!/usr/bin/env bash
# Helpers for link-d2.sh: FN probes (src/core/fn/*.cpp) are optional. A probe that does not compile or
# link against this tree is dropped with one PROBE-SKIP line; engine sources still fail the build.

# probe_objs_from_link_log <log>: the src/core/fn objects wasm-ld names as referencing an undefined symbol.
probe_objs_from_link_log() {
  grep -E 'undefined symbol|referenced by' "$1" | grep -oE '[^ :()/]*src_core_fn_[A-Za-z0-9_]+\.o' | sort -u
}

# probe_skip_line <file> <log>: "PROBE-SKIP <file>: <first error>"
probe_skip_line() {
  local err
  err=$(grep -m1 -E 'error|undefined symbol' "$2" 2>/dev/null | cut -c1-200)
  echo "PROBE-SKIP $1: ${err:-unknown}"
}
