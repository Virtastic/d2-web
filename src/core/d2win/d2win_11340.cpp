// src/core/d2win/d2win_11340.cpp -- 1.13c D2Win.dll+0x11340, add an array of (a, b, c) triples through D2Win+0x11240
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

struct D2Win11340Env {
    // D2Win+0x11240 (ctl in ECX, a in EDX, b and c on the stack).
    void (*add)(uint8_t* ctl, uint32_t a, uint32_t b, uint32_t c);
};

// 1.13c D2Win+0x11340 (ordinal 10129). `n` is unsigned; 12-byte entries.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_11340(uint8_t* ctl, const uint32_t* arr, uint32_t n,
                                                    const D2Win11340Env* env) {
    if (!ctl) fatal(0x4D);
    for (; n; --n, arr += 3) env->add(ctl, arr[0], arr[1], arr[2]);
}
