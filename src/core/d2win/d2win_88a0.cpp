// src/core/d2win/d2win_88a0.cpp -- 1.13c D2Win.dll ordinal 10037, from the cached Ghidra decompile
// (build-wasm/ghidra-d2win/decomp/0x88A0.c). Alpha lane: implementation + marker only.
#include <cstdint>
#include <unistd.h>
#include <emscripten.h>

extern "C" [[noreturn]] void d2_mcp_startup_overflow(uint32_t message);

// 1.13c D2Win+0xACB0: child must be type 5 (else fatal 0x65); dword[0x13] = min(idx, dword[0x12]).
static void d2win_sub_ACB0(int32_t* c, int32_t idx) {
    if (c[0] != 5) d2_mcp_startup_overflow(0x65);
    c[0x13] = idx < c[0x12] ? idx : c[0x12];
}

// 1.13c D2Win+0x88A0
// Scroll-list set-position. param_1 is a control of type 4 (dword[0]); dword[0x15] = item count,
// dword[0x12] = list head, dword[0x14] = current node, dword[0x16] = current index. Clamps the
// index to the count, walks the node chain (next at node+0x1C) up to that many steps, stores the
// node/index if a node was reached, forwards to the child at dword[0x24], returns 1.
// Null control: fatal 0x271; wrong type: fatal 0x272 (Fog #10265 -> #10024 -> _exit(-1)).
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_88A0(int32_t* c, int32_t idx) {
    if (!c) d2_mcp_startup_overflow(0x271);
    if (c[0] != 4) d2_mcp_startup_overflow(0x272);
    if (c[0x15] <= idx) idx = c[0x15];
    int32_t n = c[0x12]; // 32-bit retail-layout link; ponytail: wasm64 node addresses must fit 32 bits
    for (int32_t i = 0; i < idx; ++i) {
        int32_t nx = *(int32_t*)(intptr_t)(n + 0x1c);
        if (nx == 0) break;
        n = nx;
    }
    if (n != 0) { c[0x14] = n; c[0x16] = idx; }
    if (c[0x24]) d2win_sub_ACB0((int32_t*)(intptr_t)c[0x24], idx);
    return 1;
}
