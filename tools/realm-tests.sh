#!/usr/bin/env bash
# Build and run the host C++ tests in src/core/realm (g++, native, assert-based).
# usage: tools/realm-tests.sh [file.test.cpp ...]   exit 0 only if every test passes
set -u
cd "$(dirname "$0")/.."
E=$(mktemp -d); trap 'rm -rf "$E"' EXIT
echo '#define EMSCRIPTEN_KEEPALIVE' > "$E/emscripten.h"
if [ $# -gt 0 ]; then files=("$@"); else files=(src/core/realm/*.test.cpp); fi
fail=0
for f in "${files[@]}"; do
  n=$(basename "$f" .test.cpp)
  if ! g++ -std=c++17 -w -UNDEBUG -I"$E" -Isrc/core/d2moo/compat -Isrc/core/d2moo/include \
       -Isrc/core/d2moo/include/D2Game -o "$E/$n" "$f" >"$E/$n.log" 2>&1; then
    echo "BUILD-FAIL $f"; head -5 "$E/$n.log"; fail=1; continue
  fi
  if "$E/$n" >"$E/$n.out" 2>&1; then echo "PASS $f"
  else echo "FAIL $f"; head -5 "$E/$n.out"; fail=1; fi
done
exit $fail
