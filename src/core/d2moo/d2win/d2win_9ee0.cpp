// src/core/d2win/d2win_9ee0.cpp -- 1.13c D2Win.dll+0x09EE0, account list (type 12): copy the selected name out
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000) and the cached Ghidra decompile. Prior art: D2MOO 1.10f
// reference/D2MOO/source/D2Win/ (control type numbering and header layout only).
#include <cstdint>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

// 1.13c D2Win+0x09EE0 (ordinal 10197): returns 0 with no selection, else strcpy and 1.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_9EE0(int32_t* ctl, char* out) {
    if (ctl[0] != 0xC) fatal(0xB9);
    const char* s = (const char*)ptr((uint32_t)ctl[0x11]);
    if (!s) return 0;
    char c;
    do { c = *s++; *out++ = c; } while (c != 0);
    return 1;
}
