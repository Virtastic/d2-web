// src/core/realm/bnclient_memcpy.cpp -- BNClient.dll VC7.1 CRT _memcpy (decomp 0x54F0.c).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// 1.13c BNClient+0x054F0 (672 B, callers 0x45AC, 0x2275): memcpy(dst, src, n).
// Retail copies backward when src < dst < src+n (overlap) and forward otherwise, i.e. memmove
// semantics; the dword/byte unrolling does not change the resulting bytes. Returns dst.
extern "C"
EMSCRIPTEN_KEEPALIVE void* bn_memcpy(void* dst, const void* src, uint32_t n) {
    return std::memmove(dst, src, n);
}
