#!/usr/bin/env bash
# Helpers for link-d2.sh: FN probes (src/core/fn/*.cpp) are optional. A probe that does not compile or
# link against this tree is dropped with one PROBE-SKIP line; engine sources still fail the build.

# probe_objs_from_link_log <log>: the src/core/fn objects wasm-ld names inside an ERROR block: the header itself
# ("error: <obj.o>: undefined symbol: x", the usual form) and its ">>>" detail lines ("referenced by", "defined in"
# of a duplicate symbol, and a debug build's object-only continuation line). Warning blocks (">>> defined as" of a
# signature mismatch) are not errors and name nothing.
probe_objs_from_link_log() {
  awk '/^[^ >].*(error|warning):/ { inerr = ($0 ~ /wasm-ld: error:/) } /^$/ { inerr = 0 } inerr && (/^wasm-ld: error:/ || /^>>>/)' "$1" |
    grep -oE '[^ :()/]*src_core_fn_[A-Za-z0-9_]+\.o' | sort -u
}

# probe_link_retry: link_engine (defined by the caller) over $OBJS into $LINKLOG. On failure, drop the probe objects
# the log names (this build only; the sources are never touched), print one PROBE-SKIP line per object and relink.
# Dropping a probe can expose a second one that needed it, so up to 6 rounds; an error that names no probe
# (an engine source) fails at once. Returns 0 on a link, 1 otherwise (log tail on stderr).
probe_link_retry() {
  local round bad o keep
  for round in 1 2 3 4 5 6; do
    if link_engine >"$LINKLOG" 2>&1; then return 0; fi
    bad=$(probe_objs_from_link_log "$LINKLOG")
    [ -n "$bad" ] || break
    keep=""
    for o in $OBJS; do
      if echo "$bad" | grep -qxF "$(basename "$o")"; then
        echo "PROBE-SKIP $o: $(grep -m1 -E 'undefined symbol|duplicate symbol' "$LINKLOG" | cut -c1-160) (link)"
        basename "$o" .o | sed 's/^src_core_fn_//' >> "$SKIPPED_FILE"
      else keep="$keep $o"; fi
    done
    OBJS="$keep"
  done
  tail -40 "$LINKLOG" >&2
  return 1
}

# probe_skip_line <file> <log>: "PROBE-SKIP <file>: <first error>"
probe_skip_line() {
  local err
  err=$(grep -m1 -E 'error|undefined symbol' "$2" 2>/dev/null | cut -c1-200)
  echo "PROBE-SKIP $1: ${err:-unknown}"
}

# compile_one "<src>|<obj>": compile one stale source (needs $CXXFLAGS and dependency_digest). An FN probe that fails
# to compile costs only its own object: PROBE-SKIP line, no object left, exit 0. An engine source fails the build.
compile_one() {
  local f="${1%%|*}" o="${1##*|}"
  case "$f" in
    src/core/fn/*)
      local log="$o.log"
      if ! em++ $CXXFLAGS -MMD -MP -MF "$o.d" -MT "$o" -c "$f" -o "$o" >"$log" 2>&1; then
        probe_skip_line "$f" "$log"; rm -f "$o" "$o.d" "$o.inputs"; return 0
      fi ;;
    *) em++ $CXXFLAGS -MMD -MP -MF "$o.d" -MT "$o" -c "$f" -o "$o" || return 1 ;;
  esac
  dependency_digest "$o.d" > "$o.inputs"
}
