// src/core/d2win/d2win_10ed0.cpp -- 1.13c D2Win.dll+0x10ED0, destroy the popup menu
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000) and the cached Ghidra decompile or, where Ghidra defines no function, the
// disassembly. Imported callees and D2Win state are seams on the Env struct, passed last, so the
// retail argument order is preserved. Prior art: D2MOO 1.10f reference/D2MOO/source/D2Win/
// (control type numbering and header layout only).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

struct D2Win10ED0Env {
    uint32_t* popup;                       // 0x6F9A9E24
    int32_t (*destroy)(uint8_t* ctl);      // D2Win+0x17FC0
};

// 1.13c D2Win+0x10ED0 (ordinal 10119)
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_10ED0(const D2Win10ED0Env* env) {
    if (!*env->popup) return 0;
    int32_t r = env->destroy(ptr(*env->popup));
    *env->popup = 0;
    return r;
}
