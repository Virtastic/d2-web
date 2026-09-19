#!/bin/bash
# Canonical d2-web -> wasm build + link. Runs INSIDE d2-builder32 (Dockerfile.builder32):
#   docker run --rm -v "$PWD:/build" d2-builder32:1 bash wasm-build/link-d2.sh
# Produces build-wasm/d2.{js,wasm} and copies them into play/ (+ .br siblings). Stage into
# play/e/<hash>/ with version-engine.sh afterwards; ci/build.sh does all three.
#
# Ported shape of openmw-web/wasm-build/link-openmw.sh, none of the content: no CMake tree of
# our own yet (a handful of sources -- em++ directly), StormLib built once via its CMake and
# cached in build-wasm/stormlib/ keyed on the submodule commit.
set -euo pipefail

ROOT="${ROOT:-$(cd "$(dirname "$0")/.." && pwd)}"
BUILD="$ROOT/build-wasm"
STORM_SRC="$ROOT/third_party/StormLib"
STORM_BUILD="$BUILD/stormlib"
mkdir -p "$BUILD"
cd "$ROOT"

[ -f "$STORM_SRC/CMakeLists.txt" ] || { echo "FATAL: third_party/StormLib is empty -- git submodule update --init" >&2; exit 1; }

# --- StormLib (once per submodule commit) --------------------------------------------------
# Bundled zlib/bzip2/lzma so the link line does not depend on which emscripten ports exist in
# the sysroot. Static, -pthread to match everything else (an archive compiled without -pthread
# links fine and then faults at runtime on the first atomic -- the bad failure mode).
storm_rev=$(git -C "$STORM_SRC" rev-parse HEAD 2>/dev/null || echo unknown)
if [ ! -f "$STORM_BUILD/libstorm.a" ] || [ "$(cat "$STORM_BUILD/.rev" 2>/dev/null)" != "$storm_rev" ]; then
  echo "==> StormLib @ $storm_rev"
  rm -rf "$STORM_BUILD"; mkdir -p "$STORM_BUILD"
  emcmake cmake -S "$STORM_SRC" -B "$STORM_BUILD" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release -DSTORM_USE_BUNDLED_LIBRARIES=ON -DSTORM_SKIP_INSTALL=ON \
    -DBUILD_SHARED_LIBS=OFF -DCMAKE_C_FLAGS="-pthread -O2" -DCMAKE_CXX_FLAGS="-pthread -O2" >/dev/null
  ninja -C "$STORM_BUILD" storm
  echo "$storm_rev" > "$STORM_BUILD/.rev"
fi
[ -f "$STORM_BUILD/libstorm.a" ] || { echo "FATAL: libstorm.a did not build" >&2; exit 1; }

# --- the engine ---------------------------------------------------------------------------
# -fno-strict-aliasing: recovered code punning structs is the norm, not the exception.
# -fwrapv: signed overflow wraps, like the x86-32 MSVC build the logic came from.
# No -flto (openmw-web: wasm-ld miscompiles under it). No MEMORY64, ever, in this repo.
SRCS=$(find src -name '*.cpp' | sort)
# tools/vectorfmt is shared with the Win32 capture tool: same header, same hash, no drift.
echo "==> em++ $(echo $SRCS | wc -w) sources"
# src/core/d2moo is vendored D2MOO (MIT, 1.10f prose map) compiled as 1.13c through the
# shims in src/core/d2moo/compat; its Windows-isms are silenced, not fixed.
# D2_CXXFLAGS="-g -O1 -sASSERTIONS=2 -sSAFE_HEAP=1" turns a "memory access out of bounds" into a stack.
em++ -std=c++20 -O2 -pthread -fno-strict-aliasing -fwrapv ${D2_CXXFLAGS:-} \
  -Wno-unknown-pragmas -Wno-microsoft -Wno-unused-value \
  -DD2_VERSION_MAJOR=1 -DD2_VERSION_MINOR=13 -DD2_VERSION_PATCH=67 \
  -I"$STORM_SRC/src" -Isrc -Isrc/core/d2moo/compat -Isrc/core/d2moo/include $SRCS "$STORM_BUILD/libstorm.a" \
  -sPTHREAD_POOL_SIZE=2 -sALLOW_MEMORY_GROWTH=1 -sMAXIMUM_MEMORY=4294967296 \
  -sEXPORTED_RUNTIME_METHODS=FS,ccall,cwrap,UTF8ToString,HEAPU32,HEAPU8,HEAPU16,HEAP32 \
  -sEXPORTED_FUNCTIONS=_main,_d2_smoke_mpq,_d2_smoke_i32,_d2_prng_check,_d2_vfs_mount,_d2_txt_probe,_d2_table_bin_load,_d2_table_bin_ptr,_d2_table_bin_len,_d2_lang_probe,_d2_drlg_init,_d2_drlg_act_begin,_d2_drlg_act_end,_d2_drlg_level,_d2_drlg_out,_d2_drlg_grid,_d2_drlg_err,_d2_datatbl,_malloc,_free \
  -sENVIRONMENT=web,worker -sNO_EXIT_RUNTIME=1 \
  -o "$BUILD/d2.js"

mkdir -p play
for f in d2.js d2.wasm; do cp -f "$BUILD/$f" "play/$f"; done
ls -la play/d2.js play/d2.wasm
