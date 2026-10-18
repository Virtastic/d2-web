// src/core/gfx/d2ddraw_batch_crtfp.cpp -- six UNMAPPED 1.13c D2DDraw.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-d2ddraw/decomp/0x<RVA>.c (alpha lane, no spec/vector yet). Retail
// base 0x6F8C0000; every RVA below is retail's.
//
// All six rows are MSVC 2003 (VC7.1) Release CRT floating-point support the linker pulled into the
// DLL: the x87 control-word translation pair, the __control87/__controlfp entry points, the 12-byte
// mantissa copy/zero leaves and the carry-propagating dword add. None has a D2 caller in our engine;
// what is modelled is the retail contract. The x87 control word and the FPU
// state are host seams: under wasm there is no x87, so the hardware word lives in this module's
// g_hw_cw and every helper is an exact pure transform of it.
#include <cstdint>
#include <emscripten.h>

// Retail's x87 control word as the init image leaves it (CRT default 0x027F: all exceptions masked,
// round-to-nearest, 53-bit precision). __control87/__controlfp are the only writers.
static uint32_t g_hw_cw = 0x027Fu;

// 1.13c D2DDraw+0x0A082 (__abstract_cw): translate a hardware x87 control word (retail reads it in BX,
// 16 bits) to the CRT's abstract form. Exception masks 0x01/0x04/0x08/0x10/0x20 map to
// 0x10/0x8/0x4/0x2/0x1; precision 0x40 -> 0x80000; rounding 0x400/0x800/0xC00 -> 0x100/0x200/0x300;
// denormal 0x000 -> 0x20000, 0x200 -> 0x10000; infinity control 0x1000 -> 0x40000.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2ddraw_abstract_cw(uint32_t hw_cw) {
    const uint32_t bx = hw_cw & 0xFFFFu;
    uint32_t u = 0;
    if (bx & 0x1u) u |= 0x10;
    if (bx & 0x4u) u |= 0x8;
    if (bx & 0x8u) u |= 0x4;
    if (bx & 0x10u) u |= 0x2;
    if (bx & 0x20u) u |= 0x1;
    if (bx & 0x2u) u |= 0x80000;
    switch (bx & 0xC00u) {
    case 0x400: u |= 0x100; break;
    case 0x800: u |= 0x200; break;
    case 0xC00: u |= 0x300; break;
    }
    if ((bx & 0x300u) == 0) u |= 0x20000;
    else if ((bx & 0x300u) == 0x200u) u |= 0x10000;
    if (bx & 0x1000u) u |= 0x40000;
    return u;
}

// 1.13c D2DDraw+0x0A114 (__hw_cw): the inverse translation, abstract (retail reads it in EBX) to the
// hardware x87 control word. Exception masks 0x10/0x8/0x4/0x2/0x1 -> 0x01/0x04/0x08/0x10/0x20;
// precision 0x80000 -> 0x40; rounding 0x100/0x200/0x300 -> 0x400/0x800/0xC00; denormal
// 0x000 -> 0x300, 0x10000 -> 0x200; infinity control 0x40000 -> 0x1000.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2ddraw_hw_cw(uint32_t abstract_cw) {
    const uint32_t ebx = abstract_cw;
    uint32_t u = (ebx & 0x10u) != 0;
    if (ebx & 0x8u) u |= 4;
    if (ebx & 0x4u) u |= 8;
    if (ebx & 0x2u) u |= 0x10;
    if (ebx & 0x1u) u |= 0x20;
    if (ebx & 0x80000u) u |= 2;
    switch (ebx & 0x300u) {
    case 0x100: u |= 0x400; break;
    case 0x200: u |= 0x800; break;
    case 0x300: u |= 0xC00; break;
    }
    if ((ebx & 0x30000u) == 0) u |= 0x300;
    else if ((ebx & 0x30000u) == 0x10000u) u |= 0x200;
    if (ebx & 0x40000u) u |= 0x1000;
    return u;
}

// 1.13c D2DDraw+0x0A1A2 (__control87): read the abstract word from the current hardware word, replace
// the bits under Mask with NewValue & Mask, program the hardware word, and return the new abstract
// word. The FPU write is the g_hw_cw assignment (no x87 in wasm).
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2ddraw_control87(uint32_t new_value, uint32_t mask) {
    const uint32_t cur = d2ddraw_abstract_cw(g_hw_cw);
    const uint32_t val = (cur & ~mask) | (new_value & mask);
    g_hw_cw = d2ddraw_hw_cw(val);
    return val;
}

// 1.13c D2DDraw+0x0A1D4 (__controlfp): __control87 with the mask's __EM_INVALID bit (0x80000) cleared
// -- the control word masks 0xFFF7FFFF, so NewValue's invalid-exception bit never reaches the FPU.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2ddraw_controlfp(uint32_t new_value, uint32_t mask) {
    return d2ddraw_control87(new_value, mask & 0xFFF7FFFFu);
}

// 1.13c D2DDraw+0x09AA7 (__setdefaultprecision): __controlfp(0x10000, 0x30000) -- select the default
// (53-bit) precision. Returns nothing in retail; the resulting control word is the observable state.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2ddraw_setdefaultprecision(void) {
    d2ddraw_controlfp(0x10000u, 0x30000u);
}

// 1.13c D2DDraw+0x0A1EA (___addl): a + b stored to *out; returns the carry out (1 when the sum wrapped
// below either addend, retail's `sum < a || sum < b` tests the 32-bit wrap).
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2ddraw_addl(uint32_t a, uint32_t b, uint32_t* out) {
    const uint32_t sum = a + b;
    *out = sum;
    return (sum < a || sum < b) ? 1u : 0u;
}
