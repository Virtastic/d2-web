// src/core/gfx/d2ddraw_batch_9c13.cpp -- four UNMAPPED 1.13c D2DDraw.dll CRT leaves reimplemented from
// the cached decompiles in build-wasm/ghidra-d2ddraw/decomp/0x<RVA>.c (alpha lane, no spec/vector).
// All four are MSVC 2003 (VC7.1) Release integer helpers with no callees and no globals.
#include <cstdint>

// 1.13c D2DDraw+0x9C2E (25 B) __IsZeroMan: 1 when the three dwords at p are all zero. Caller 0x9CC2.
extern "C"
uint32_t d2ddraw_sub_9C2E(const uint32_t* p) {
    for (int i = 0; i < 3; i++) if (p[i] != 0) return 0;
    return 1;
}

// 1.13c D2DDraw+0x9C13 (27 B) __CopyMan: copy three dwords src -> dst, no return. Caller 0x9CC2.
extern "C"
void d2ddraw_sub_9C13(uint32_t* dst, const uint32_t* src) {
    for (int i = 0; i < 3; i++) dst[i] = src[i];
}

// 1.13c D2DDraw+0x0A297 (45 B) ___shr_12: shift the 96-bit little-endian value m[0..2] right by one bit.
extern "C"
void d2ddraw_sub_A297(uint32_t* m) {
    uint32_t mid = m[1];
    m[1] = (mid >> 1) | (m[2] << 31);
    m[2] = m[2] >> 1;
    m[0] = (m[0] >> 1) | (mid << 31);
}

// 1.13c D2DDraw+0x0A269 (46 B) ___shl_12: shift the 96-bit little-endian value m[0..2] left by one bit.
extern "C"
void d2ddraw_sub_A269(uint32_t* m) {
    uint32_t lo = m[0], mid = m[1];
    m[0] = lo * 2;
    m[1] = mid * 2 | (lo >> 31);
    m[2] = (m[2] << 1) | (mid >> 31);
}
