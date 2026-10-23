// src/core/d2win/d2win_sub_1a184.cpp -- 1.13c D2Win.dll power-of-ten scaler (CRT 12-byte long-double
// helper), from the cached Ghidra decompile (build-wasm/ghidra-d2win/decomp/0x1A184.c).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// Host seams: pos/neg power tables (retail 0x6f900cc0 / 0x6f900e20, 12-byte entries) and sub_19F4E (multiply x *= y).
struct D2Win1A184Env {
    const uint8_t* posTable;
    const uint8_t* negTable;
    void (*mul)(void* x, const void* y);
};

// 1.13c D2Win+0x1A184
// x = 12-byte value (exponent word at +10); exp10 = power of ten; noClear==0 zeroes x's first word first.
// Walks exp10 in octal digits: each digit d!=0 multiplies x by table[level*0x54 + d*12]; if that entry's first
// word > 0x7fff, a copy with the dword at +2 decremented is used instead.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_sub_1a184(int32_t* x, uint32_t exp10, int32_t noClear, const D2Win1A184Env* env) {
    if (exp10 == 0) return;
    const uint8_t* base = env->posTable;
    if ((int32_t)exp10 < 0) { exp10 = -exp10; base = env->negTable; }
    if (noClear == 0) *(uint16_t*)x = 0;
    while (exp10 != 0) {
        uint32_t d = exp10 & 7;
        exp10 = (uint32_t)((int32_t)exp10 >> 3);
        base += 0x54;
        if (d == 0) continue;
        const uint8_t* e = base + d * 12;
        uint8_t tmp[12];
        if (*(const uint16_t*)e > 0x7fff) {
            memcpy(tmp, e, 12);
            uint32_t v; memcpy(&v, tmp + 2, 4); v -= 1; memcpy(tmp + 2, &v, 4);
            e = tmp;
        }
        env->mul(x, e);
    }
}
