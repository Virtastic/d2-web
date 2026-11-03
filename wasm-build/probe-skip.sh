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

# probe_objs_blamed_on_engine <log>: wasm-ld keeps only live code, so an engine port with undefined symbols links until
# a probe calls into it, and then the error names the ENGINE object. Name the probes in $OBJS whose undefined symbols
# an engine object blamed by the log defines (nm; D2_NM overrides). Dropped for this build only, like any skipped probe.
probe_objs_blamed_on_engine() {
  local nm eng defs o b
  nm=${D2_NM:-$(command -v emnm || command -v llvm-nm || echo nm)}
  eng=$(sed -nE 's/^wasm-ld: error: ([^ ]+\.o): .*/\1/p' "$1" | xargs -r -n1 basename | grep -v '^src_core_fn_' | sort -u)
  [ -n "$eng" ] || return 0
  defs=$(for b in $eng; do for o in $OBJS; do [ "$(basename "$o")" = "$b" ] && $nm -j -g --defined-only "$o" 2>/dev/null; done; done | sort -u)
  [ -n "$defs" ] || return 0
  for o in $OBJS; do
    case "$(basename "$o")" in src_core_fn_*) ;; *) continue ;; esac
    if $nm -j -u "$o" 2>/dev/null | sort -u | comm -12 - <(echo "$defs") | grep -q .; then basename "$o"; fi
  done
}

# probe_objs_by_symbol <log>: the error block does not always name the probe (a header without an object, a ">>>" list cut short),
# but it always names the SYMBOL. Name the probes in $OBJS that reference an undefined symbol the log reports, or that define a
# symbol the log reports as duplicated (nm -C: wasm-ld prints demangled names; D2_NM overrides). Dropped for this build only.
probe_objs_by_symbol() {
  local nm undef dup o
  nm=${D2_NM:-$(command -v emnm || command -v llvm-nm || echo nm)}
  undef=$(sed -nE 's/^wasm-ld: error: ([^ ]+: )?undefined symbol: (.*[^ ])[ ]*$/\2/p' "$1" | sort -u)
  dup=$(sed -nE 's/^wasm-ld: error: duplicate symbol: (.*[^ ])[ ]*$/\1/p' "$1" | sort -u)
  [ -n "$undef$dup" ] || return 0
  for o in $OBJS; do
    case "$(basename "$o")" in src_core_fn_*) ;; *) continue ;; esac
    if [ -n "$undef" ] && $nm -C -j -u "$o" 2>/dev/null | sort -u | comm -12 - <(echo "$undef") | grep -q .; then basename "$o"; continue; fi
    if [ -n "$dup" ] && $nm -C -j -g --defined-only "$o" 2>/dev/null | sort -u | comm -12 - <(echo "$dup") | grep -q .; then basename "$o"; fi
  done
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
