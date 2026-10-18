// src/core/d2gfx/d2gfx_batch_5900.cpp -- 1.13c D2Gfx+0x05900 (232 B) reimplemented from the cached
// decompile (build-wasm/ghidra-d2gfx/decomp/0x5900.c). Alpha batch lane (OBJECTIVE v8.3):
// implementation + marker only, no spec, no vector. Retail callers 0x360C and 0x608C.
#include <cstdint>

// 1.13c D2Gfx+0x05900 (sub_5900, 232 B): MSVC 2003 Release CRT strcpy -- the aligned-source body
// (word writes until the 0x7EFEFEFF/0x81010100 zero-byte test fires, then the exact terminating bytes).
// cdecl(dst, src), returns dst; copies through the NUL, so the observable bytes are the same as the
// 7-byte copy at D2Gfx+0x58F0.
extern "C"
uint8_t* d2gfx_sub_5900(uint8_t* dst, const uint8_t* src) {
    uint8_t* out = dst;
    while ((*dst++ = *src++) != 0) {}
    return out;
}
