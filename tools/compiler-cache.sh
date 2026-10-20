#!/usr/bin/env bash
# Sourced by link-d2.sh. Immutable Docker image ID includes clang, emcc, sysroot and config.
# Direct invocations conservatively hash the installed upstream toolchain instead.
d2_toolchain_key() {
  {
    sha256sum "$ROOT/Dockerfile.builder64" "$ROOT/tools/compiler-cache.sh" "$ROOT/wasm-build/link-d2.sh"
    if [ -n "${D2_TOOLCHAIN_ID:-}" ]; then
      printf '%s\n' "$D2_TOOLCHAIN_ID"
    else
      local emroot
      emroot=$(dirname "$(readlink -f "$(command -v em++)")")
      find "$(dirname "$emroot")" -type f ! -path '*/__pycache__/*' ! -name '*.lock' -print0 |
        sort -z | xargs -0 sha256sum
    fi
  } | sha256sum | cut -d' ' -f1
}
d2_enable_compiler_cache() {
  [ "${D2_COMPILER_CACHE:-0}" = 1 ] || return 0
  [ -x /usr/bin/ccache ] && [ -d /d2-compiler-cache ] || {
    echo 'FATAL: compiler cache requested without ccache and its dedicated mount' >&2; return 1;
  }
  # No user config, header exclusions, sloppiness or shared object directories. Preprocessor
  # mode hashes the actual included header contents, including the system headers.
  local setting
  for setting in ${!CCACHE_@}; do unset "$setting"; done
  export CCACHE_CONFIGPATH=/dev/null CCACHE_DIR=/d2-compiler-cache
  export CCACHE_NAMESPACE="d2-$TOOLCHAIN_KEY" CCACHE_COMPILERCHECK=content
  export CCACHE_SLOPPINESS='' CCACHE_NODIRECT=1 CCACHE_MAXSIZE=2G
  export CCACHE_BASEDIR="$ROOT" EM_COMPILER_WRAPPER=/usr/bin/ccache
}
