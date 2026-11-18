// src/core/d2win/d2win_f790.cpp -- 1.13c D2Win.dll+0x0F790, button (type 6): set the caption from a string id
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

struct D2WinF790Env {
    // D2Lang #10003: the UTF-16 string for an id.
    const uint16_t* (*getString)(uint32_t id);
};

// 1.13c D2Win+0x0F790 (ordinal 10049). Retail: ctl in ECX, the +0x60 value in EDX, string id and
// the +0x264 value on the stack. The caption is copied unbounded to +0x64.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_F790(int32_t* ctl, int32_t a, uint32_t strId, int32_t c,
                                                   const D2WinF790Env* env) {
    if (ctl[0] != 6) fatal(0x96);
    ctl[0x18] = a;
    ctl[0x99] = c;
    u16cpy((uint16_t*)((uint8_t*)ctl + 0x64), env->getString(strId));
}
