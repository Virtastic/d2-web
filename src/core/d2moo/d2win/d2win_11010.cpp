// src/core/d2win/d2win_11010.cpp -- 1.13c D2Win.dll+0x11010, popup menu (type 11): add an entry
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

struct D2Win11010Env {
    uint32_t* popup;                                         // 0x6F9A9E24
    void (*setFont)(int32_t font);                           // D2Win+0x12FE0 (ECX)
    int32_t (*textWidth)(const uint16_t* s, int32_t len);    // D2Win+0x12080 (ECX, EDX)
};

// 1.13c D2Win+0x11010 (ordinal 10206). At most 8 entries of 0x84 bytes from +0x44; the entry
// is a raw 0x84-byte copy of `s`. Width [5] grows to the widest entry; height [6] = 20 * count.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_11010(const uint16_t* s, const D2Win11010Env* env) {
    int32_t* p = (int32_t*)ptr(*env->popup);
    if (p[0] != 0xB) fatal(0x74);
    int32_t n = p[0x10];
    if (n >= 8) return 0;
    std::memcpy((uint8_t*)p + 0x44 + n * 0x84, s, 0x84);
    env->setFont(1);
    int32_t w = env->textWidth(s, u16len(s));
    if (p[5] > w) w = p[5];
    p[5] = w;
    ++n;
    p[6] = n * 0x14;
    p[0x10] = n;
    return 1;
}
