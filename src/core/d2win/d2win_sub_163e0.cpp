// src/core/d2win/d2win_sub_163e0.cpp -- 1.13c D2Win.dll 0x163E0, from the cached Ghidra decompile.
#include <cstdint>
#include <emscripten.h>

extern "C" [[noreturn]] void d2_mcp_startup_overflow(uint32_t message);

// 1.13c D2Win+0x163E0
// Timed-callback poll. Type (dword[0]) must be 8 (else fatal 0x52). Active iff flags dword[2] has
// gdwBitMasks[2] (4) and gdwBitMasks[0] (1). Then if dword[0x10] < tick/1000 - dword[0x11]/1000
// and a callback dword[0xd] is set, call it with &param_1. Returns 1 if active, else 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_163E0(int32_t* c) {
    if (c[0] != 8) d2_mcp_startup_overflow(0x52);
    if (!((c[2] & 4) && (c[2] & 1))) return 0;
    uint32_t tick = (uint32_t)emscripten_get_now(); // GetTickCount, ms
    if ((uint32_t)c[0x10] < tick / 1000 - (uint32_t)c[0x11] / 1000) {
        // ponytail: 32-bit retail-layout fn pointer
        auto cb = (void (*)(int32_t**))(intptr_t)(uint32_t)c[0xd];
        if (cb) { int32_t* a[8] = {c}; cb(a); }
    }
    return 1;
}
