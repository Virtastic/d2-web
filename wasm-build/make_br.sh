#!/bin/bash
# Precompress the engine for the .br fast path. Ported from openmw-web/wasm-build/make_br.sh.
# q11: compressed once per release, served to every visitor -- spend the CPU.
set -euo pipefail
ROOT="${ROOT:-$(cd "$(dirname "$0")/.." && pwd)}"
Q="${ENGINE_Q:-11}"
for f in "$ROOT"/play/d2.js "$ROOT"/play/d2.wasm; do
  [ -f "$f" ] || continue
  if [ -f "$f.br" ] && [ "$f.br" -nt "$f" ]; then echo "fresh: $f.br"; continue; fi
  echo "brotli -q $Q: $f"
  brotli -f -q "$Q" -o "$f.br" "$f"
done
ls -lh "$ROOT"/play/*.br 2>/dev/null | awk '{print $5, $9}' || true
