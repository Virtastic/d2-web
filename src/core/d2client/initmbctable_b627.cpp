// src/core/d2client/initmbctable_b627.cpp -- MSVC 2003 CRT ___initmbctable.
#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0xb627 (30 B, callers 0x8AA1/0x886E): once-only; if the flag (DAT_6fbd4ad8) is clear,
// __setmbcp(-3) (0xB4D7, default ANSI code page) then set the flag. Returns 0.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2c_initmbctable(int32_t *flag, void (*setmbcp)(int32_t)) {
    if (*flag == 0) { setmbcp(-3); *flag = 1; }
    return 0;
}
