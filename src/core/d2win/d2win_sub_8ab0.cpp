// src/core/d2win/d2win_sub_8ab0.cpp -- from the cached Ghidra decompile (build-wasm/ghidra-d2win/decomp/0x8AB0.c).
#include <cstdint>
#include <unistd.h>
#include <emscripten.h>

// 1.13c D2Win+0x8AB0
// Scroll-list set-top-line (fastcall: ecx = control, edx = value). Type-4 control: dword[0x18] =
// min(signed(value, >=2^31 -> -1), dword[0x15] - 1). Null control: fatal 0x262; wrong type: 0x263
// (Fog #10265 -> #10024 -> _exit(-1)).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_sub_8AB0(int32_t* c, uint32_t v) {
    if (!c || c[0] != 4) _exit(-1);
    if (v > 0x7fffffffu) v = 0xffffffffu;
    uint32_t m = (uint32_t)c[0x15] - 1u;
    c[0x18] = (int32_t)v < (int32_t)m ? (int32_t)v : (int32_t)m;
}
