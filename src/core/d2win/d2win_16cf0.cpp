// src/core/d2win/d2win_16cf0.cpp -- 1.13c D2Win.dll+0x16CF0, add an item to a list control (ANSI text)
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

struct D2Win16CF0Env {
    // D2Lang Unicode::win2Unicode (dst, src, count).
    void (*win2Unicode)(uint16_t* dst, const char* src, int32_t n);
    // D2Win+0x16B90 (list, text, a1, a2, a3, head flag, select flag).
    void (*addItem)(uint8_t* list, const uint16_t* text, uint32_t a1, uint8_t a2, uint32_t a3,
                    int32_t head, int32_t select);
};

// 1.13c D2Win+0x16CF0 (ordinal 10175; list in ECX, ANSI text in EDX, five stack args): converts
// into a zeroed 256-unit buffer and forwards to 0x16B90.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_16CF0(uint8_t* list, const char* text, uint32_t a1, uint8_t a2,
                                                    uint32_t a3, int32_t head, int32_t select,
                                                    const D2Win16CF0Env* env) {
    uint16_t buf[0x100];
    std::memset(buf, 0, sizeof buf);
    env->win2Unicode(buf, text, 0x100);
    env->addItem(list, buf, a1, a2, a3, head, select);
}
