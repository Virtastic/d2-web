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

# _probe_nm_hits <nm> <setfile> <nm flags...>: ONE nm run (xargs only batches if the argument list is long) over every
# src_core_fn_ object in $OBJS, the symbols looked up in memory (awk) against the lines of <setfile>. Prints the basenames of
# the probes with at least one symbol in the set, in $OBJS order. (-A prefixes each symbol with "<path>: ".) An nm whose
# output does not carry that prefix on every line (a wrapper, an nm without -A) cannot attribute symbols, so the probes are
# then listed one nm run each, as before.
_probe_nm_hits() {
  local nm=$1 set=$2 o fn="" out hits; shift 2
  for o in $OBJS; do case "${o##*/}" in src_core_fn_*) fn="$fn$o"$'\n' ;; esac; done
  [ -n "$fn" ] || return 0
  out=$(printf '%s' "$fn" | xargs -r -d '\n' "$nm" -A "$@" 2>/dev/null)
  if [ -n "$out" ] && ! awk 'FNR == NR { if ($0 != "") f[$0] = 1; next }
       $0 != "" { i = index($0, ": "); if (!i || !(substr($0, 1, i - 1) in f)) exit 1 }' <(printf '%s' "$fn") <(echo "$out"); then
    out=$(printf '%s' "$fn" | while read -r o; do "$nm" "$@" "$o" 2>/dev/null | sed "s|^|$o: |"; done)
  fi
  hits=$(echo "$out" | awk 'FNR == NR { want[$0] = 1; next }
         { i = index($0, ": "); if (i && (substr($0, i + 2) in want)) hit[substr($0, 1, i - 1)] = 1 }
         END { for (p in hit) print p }' "$set" -)
  [ -n "$hits" ] || return 0
  printf '%s\n' $OBJS | awk 'FNR == NR { h[$0] = 1; next } ($0 in h) { n = split($0, p, "/"); if (p[n] ~ /^src_core_fn_/) print p[n] }' <(echo "$hits") -
  return 0
}

# probe_objs_blamed_on_engine <log>: wasm-ld keeps only live code, so an engine port with undefined symbols links until
# a probe calls into it, and then the error names the ENGINE object. Name the probes in $OBJS whose undefined symbols
# an engine object blamed by the log defines (nm; D2_NM overrides). Dropped for this build only, like any skipped probe.
# The engine objects' definitions and every probe's undefined symbols are each listed in one nm run, not one per object.
probe_objs_blamed_on_engine() {
  local nm eng engobjs o set
  nm=${D2_NM:-$(command -v emnm || command -v llvm-nm || echo nm)}
  eng=$(sed -nE 's/^wasm-ld: error: ([^ ]+\.o): .*/\1/p' "$1" | xargs -r -n1 basename | grep -v '^src_core_fn_' | sort -u)
  [ -n "$eng" ] || return 0
  engobjs=$(for o in $OBJS; do echo "${o##*/} $o"; done | awk 'FNR == NR { e[$0] = 1; next } ($1 in e) { print $2 }' <(echo "$eng") -)
  [ -n "$engobjs" ] || return 0
  set=$(mktemp)
  echo "$engobjs" | xargs -r -d '\n' "$nm" -j -g --defined-only 2>/dev/null | sort -u > "$set"
  [ -s "$set" ] && _probe_nm_hits "$nm" "$set" -j -u
  rm -f "$set"
  return 0
}

# probe_objs_by_symbol <log>: the error block does not always name the probe (a header without an object, a ">>>" list cut short),
# but it always names the SYMBOL. Name the probes in $OBJS that reference an undefined symbol the log reports, or that define a
# symbol the log reports as duplicated (nm -C: wasm-ld prints demangled names; D2_NM overrides). Dropped for this build only.
# One nm run per question (undefined, defined) over all probes, not one per object.
probe_objs_by_symbol() {
  local nm undef dup set o
  nm=${D2_NM:-$(command -v emnm || command -v llvm-nm || echo nm)}
  undef=$(sed -nE 's/^wasm-ld: error: ([^ ]+: )?undefined symbol: (.*[^ ])[ ]*$/\2/p' "$1" | sort -u)
  dup=$(sed -nE 's/^wasm-ld: error: duplicate symbol: (.*[^ ])[ ]*$/\1/p' "$1" | sort -u)
  [ -n "$undef$dup" ] || return 0
  set=$(mktemp)
  {
    if [ -n "$undef" ]; then echo "$undef" > "$set"; _probe_nm_hits "$nm" "$set" -C -j -u; fi
    if [ -n "$dup" ]; then echo "$dup" > "$set"; _probe_nm_hits "$nm" "$set" -C -j -g --defined-only; fi
  } > "$set.out"
  printf '%s\n' $OBJS | awk 'FNR == NR { h[$0] = 1; next } { n = split($0, p, "/"); if (p[n] in h) print p[n] }' "$set.out" -
  rm -f "$set" "$set.out"
  return 0
}

# probe_engine_objs_quarantinable <log>: with D2_QUARANTINE=1, the ENGINE objects (not src_core_fn_) a link error names, minus the
# sources in D2_QUARANTINE_KEEP (space-separated src paths: the rows being proved and the files the branch changed). Dropped for
# this build only; one QUARANTINED <src file> line each. Rows that need a quarantined file then fail BUILD-FAIL on their own.
probe_engine_objs_quarantinable() {
  local o b src k keepobj
  keepobj=$(for k in ${D2_QUARANTINE_KEEP:-}; do echo "$k" | tr / _ | sed "s/\.cpp$/.o/"; done)
  awk "/^[^ >].*(error|warning):/ { inerr = (\$0 ~ /wasm-ld: error:/) } /^$/ { inerr = 0 } inerr && (/^wasm-ld: error:/ || /^>>>/)" "$1" |
    grep -oE "[^ :()/]*src_[A-Za-z0-9_]+\.o" | grep -v "^src_core_fn_" | sort -u | while read -r b; do
      echo "$keepobj" | grep -qxF "$b" || echo "$b"
    done
}

# probe_link_retry: link_engine (defined by the caller) over $OBJS into $LINKLOG. On failure, drop the probe objects
# the log names (this build only; the sources are never touched), print one PROBE-SKIP line per object and relink.
# Dropping a probe can expose a second one that needed it, so up to D2_PROBE_ROUNDS (12) rounds; an error that names no probe
# (an engine source) fails at once. Returns 0 on a link, 1 otherwise (log tail on stderr).
probe_link_retry() {
  local round bad o keep
  for round in $(seq 1 ${D2_PROBE_ROUNDS:-12}); do
    if link_engine >"$LINKLOG" 2>&1; then return 0; fi
    bad=$( { probe_objs_from_link_log "$LINKLOG"; probe_objs_by_symbol "$LINKLOG"; } | sort -u)
    [ -n "$bad" ] || bad=$(probe_objs_blamed_on_engine "$LINKLOG")
    if [ -z "$bad" ] && [ "${D2_QUARANTINE:-0}" = 1 ]; then
      bad=$(probe_engine_objs_quarantinable "$LINKLOG")
      for o in $bad; do
        src=$(for f in ${SRCS:-}; do [ "$(echo "$f" | tr / _ | sed "s/\.cpp$/.o/")" = "$o" ] && echo "$f"; done)
        echo "QUARANTINED ${src:-$o}"
      done
    fi
    [ -n "$bad" ] || break
    keep=""
    for o in $OBJS; do
      if echo "$bad" | grep -qxF "$(basename "$o")"; then
        case "$(basename "$o")" in
          src_core_fn_*)
            echo "PROBE-SKIP $o: $( (grep -F "$(basename "$o")" "$LINKLOG" || cat "$LINKLOG") | grep -m1 -E "undefined symbol|duplicate symbol" | cut -c1-160) (link)"
            basename "$o" .o | sed "s/^src_core_fn_//" >> "$SKIPPED_FILE" ;;
        esac
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
