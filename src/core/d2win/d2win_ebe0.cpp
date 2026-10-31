// src/core/d2win/d2win_ebe0.cpp -- 1.13c D2Win.dll+0x0EBE0, progress bar (type 10) draw
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

extern "C" int64_t d2win_ftol2_18AF4(uint64_t bits);

struct D2WinEBE0Env {
    // D2CMP #10049(palette, 0x100, r, g, b): nearest palette index.
    uint8_t (*nearestColor)(const uint8_t* palette, int32_t n, int32_t r, int32_t g, int32_t b);
    const uint8_t* palette;   // 0x6F922338
    // D2Gfx #10014: fill a rectangle (x0, y0, x1, y1, colour, draw mode).
    void (*fillRect)(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color, int32_t mode);
};

// The x87 product fild(int32) * fmul(float32), rounded to nearest-even at 53 bits (the MSVC CRT
// default precision), as IEEE double bits, computed with integers only. |a| * mantissa is exact
// in 55 bits; the result always lies in the double normal range.
static uint64_t d2win_mul_i32_f32(int32_t a, uint32_t f) {
    uint64_t sign = (uint64_t)(((uint32_t)a >> 31) ^ (f >> 31)) << 63;
    uint32_t fe = (f >> 23) & 0xFF, fm = f & 0x7FFFFF;
    if (fe == 0xFF) {
        if (fm || a == 0) return 0xFFF8000000000000ull;          // NaN, or 0 * inf: the x87 default NaN
        return sign | 0x7FF0000000000000ull;                       // +-inf
    }
    uint64_t ua = a < 0 ? (uint64_t)(0 - (int64_t)a) : (uint64_t)a;
    uint64_t m = fe ? (uint64_t)(fm | 0x800000) : (uint64_t)fm;
    int32_t e = fe ? (int32_t)fe - 150 : -149;
    uint64_t p = ua * m;
    if (p == 0) return sign;
    int32_t k = 63;
    while (!((p >> k) & 1)) --k;
    if (k > 52) {
        int32_t sh = k - 52;
        uint64_t q = p >> sh, rem = p & ((1ull << sh) - 1), half = 1ull << (sh - 1);
        if (rem > half || (rem == half && (q & 1))) ++q;
        if (q >> 53) { q >>= 1; ++sh; }
        p = q; e += sh;
    } else {
        p <<= (52 - k); e -= (52 - k);
    }
    uint64_t be = (uint64_t)(e + 52 + 1023);
    return sign | (be << 52) | (p & 0xFFFFFFFFFFFFFull);
}

// 1.13c D2Win+0x0EBE0. Only when the control has bits 0 and 2: fill (x, y) .. (x + fill, y + h)
// in the palette's nearest (0xFF, 0, 0) with draw mode 5, where fill = __ftol2([0x14] * float at
// +0x40). Returns 1 then, else 0. Retail pushes the colour as a dword whose upper three bytes are
// stack leftovers (the control pointer's); D2Gfx reads the low byte, which is what is passed here.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_EBE0(int32_t* ctl, const D2WinEBE0Env* env) {
    if (ctl[0] != 10) fatal(0x4F);
    uint32_t fl = (uint32_t)ctl[2];
    if (!(fl & kBm2) || !(fl & kBm0)) return 0;
    uint8_t c = env->nearestColor(env->palette, 0x100, 0xFF, 0, 0);
    int32_t x = ctl[3], y = ctl[4], h = ctl[6];
    int32_t fill = (int32_t)d2win_ftol2_18AF4(d2win_mul_i32_f32(ctl[5], (uint32_t)ctl[0x10]));
    env->fillRect(x, y, fill + x, h + y, c, 5);
    return 1;
}
