// src/core/d2win/d2win_sub_8a30.cpp -- from the cached Ghidra decompile (0x8A30.c, 0x8920.c).
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>

// 1.13c D2Win+0x8920 -- type-4 list control (eax): set cell [col=param_3] of row #param_2 (walk the
// +0x48 chain via +0x1C) to a copy of the UTF-16 text; an already-set cell is fatal (0x20f); a
// short chain returns 1. Null 0x1f9, wrong type 0x1fa, col >= +0x64 0x1fb: fatal (_exit(-1)).
static uint32_t d2win_sub_8920(uint8_t* c, const uint16_t* text, int32_t row, int32_t col) {
    if (!c || *(int32_t*)c != 4 || col >= *(int32_t*)(c + 0x64)) std::_Exit(-1);
    uint16_t buf[2048];
    size_t n = 0; while (text[n]) n++;
    std::memcpy(buf, text, (n + 1) * 2);
    uint32_t it = *(uint32_t*)(c + 0x48);
    for (int32_t i = 0; i < row; i++) {
        if (!it) return 1;
        it = *(uint32_t*)((uintptr_t)it + 0x1c);
    }
    if (it) {
        uint32_t* slot = (uint32_t*)((uintptr_t)it + col * 4);
        if (*slot) std::_Exit(-1);
        uint16_t* p = (uint16_t*)std::malloc((n + 1) * 2);
        *slot = (uint32_t)(uintptr_t)p;
        std::memcpy(p, buf, (n + 1) * 2);
    }
    return 1;
}

// 1.13c D2Win+0x8A30 (ordinal 10120) -- fastcall wrapper: ecx unused, eax = control (set by caller).
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2win_sub_8A30(void* ctrl, const uint16_t* text, int32_t row, int32_t col) {
    return d2win_sub_8920((uint8_t*)ctrl, text, row, col);
}
