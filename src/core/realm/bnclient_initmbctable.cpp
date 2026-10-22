// src/core/realm/bnclient_initmbctable.cpp -- 1.13c BNClient.dll row s3687 (MSVC 2003 CRT ___initmbctable).
#include <cstdint>
#include <emscripten.h>

// 1.13c BNClient+0x067C7 (30 B, callers 0x42D7/0x450A): once-only; if the flag (D:0x1D030) is clear,
// __setmbcp(-3) (default ANSI code page) then set the flag. Returns 0.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t bn_initmbctable(int32_t *flag, void (*setmbcp)(int32_t)) {
    if (*flag == 0) { setmbcp(-3); *flag = 1; }
    return 0;
}
