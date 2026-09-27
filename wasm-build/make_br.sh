#!/bin/bash
# Precompress the engine for the .br fast path. Ported from openmw-web/wasm-build/make_br.sh.
# q11: compressed once per release, served to every visitor -- spend the CPU.
set -euo pipefail
ROOT="${ROOT:-$(cd "$(dirname "$0")/.." && pwd)}"
Q="${ENGINE_Q:-11}"
# Freshness is the .br's CONTENT, never its mtime: an engine put into play/ by hand keeps the source's
# older mtime, which the old `-nt` test read as "fresh" and left a DIFFERENT build's brotli beside it
# (play/br-check.mjs has the whole story).
CHECK="$(cd "$(dirname "$0")/.." && pwd)/play/br-check.mjs"   # by the script's own location, not $ROOT
NODE_BIN="${NODE_BIN:-$(command -v node || true)}"
[ -n "$NODE_BIN" ] || { echo "make_br: node not found (needed to verify a .br belongs to its file)" >&2; exit 1; }
for f in "$ROOT"/play/d2.js "$ROOT"/play/d2.wasm; do
  [ -f "$f" ] || continue
  if [ -f "$f.br" ] && "$NODE_BIN" "$CHECK" "$f" "$f.br"; then echo "fresh (content-verified): $f.br"; continue; fi
  echo "brotli -q $Q: $f"
  brotli -f -q "$Q" -o "$f.br" "$f"
done
ls -lh "$ROOT"/play/*.br 2>/dev/null | awk '{print $5, $9}' || true
