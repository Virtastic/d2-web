// src/core/d2win/d2win_b0d0.cpp -- 1.13c D2Win.dll scrollbar step, from the cached Ghidra decompile
// (build-wasm/ghidra-d2win/decomp/0xB0D0.c). Alpha lane: implementation + marker only.
#include <cstdint>
#include <emscripten.h>

extern "C" [[noreturn]] void d2_mcp_startup_overflow(uint32_t message);
extern "C" uint32_t d2win_sub_88A0(int32_t* c, int32_t idx);

// 1.13c D2Win+0xB0D0
// Scrollbar step. c is a type-5 control (else fatal 0x99); delta (EAX in retail) >0 steps the
// position (dword[0x13]) down by one and sets dword[0x10], <0 steps up and sets dword[0x11].
// Does nothing if dword[0x16] (disabled) is set. Position clamped to [0, dword[0x12]]; forwards to
// the linked list control dword[0x15] via 88A0 and calls callback dword[0x17](arg). Returns 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_B0D0(int32_t* c, int32_t delta, uint32_t arg) {
    if (c[0] != 5) d2_mcp_startup_overflow(0x99);
    if (c[0x16]) return 0;
    if (delta >= 1) { c[0x10] = 1; c[0x13]--; }
    else if (delta < 0) { c[0x11] = 1; c[0x13]++; }
    if (c[0x13] < 0) c[0x13] = 0;
    else if (c[0x12] < c[0x13]) c[0x13] = c[0x12];
    // 32-bit retail-layout links; ponytail: wasm64 addresses must fit 32 bits
    if (c[0x15]) d2win_sub_88A0((int32_t*)(intptr_t)c[0x15], c[0x13]);
    if (c[0x17]) ((void(*)(uint32_t))(intptr_t)c[0x17])(arg);
    return 0;
}
