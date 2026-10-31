// src/core/d2win/d2win_17d40.cpp -- 1.13c D2Win.dll+0x17D40, set the draw mode and run a D2Gfx callback pass
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000) and the cached Ghidra decompile or, where Ghidra defines no function, the
// disassembly. Imported callees and D2Win state are seams on the Env struct, passed last, so the
// retail argument order is preserved. Prior art: D2MOO 1.10f reference/D2MOO/source/D2Win/
// (control type numbering, list item layout, control chain fields).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

struct D2Win17D40Env {
    uint32_t callback;                                       // 0x6F8F7740, passed as a slot
    void (*setMode)(uint32_t v);                             // D2CMP #10019
    uint32_t (*run)(uint32_t a1, uint32_t cb, uint32_t a2, uint32_t a3);  // D2Gfx #10015
};

// 1.13c D2Win+0x17D40 (ordinal 10142; stdcall a1..a4).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_17D40(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4,
                                                        const D2Win17D40Env* env) {
    env->setMode(a4);
    return env->run(a1, env->callback, a2, a3);
}
