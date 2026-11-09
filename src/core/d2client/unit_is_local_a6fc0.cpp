// 1.13c D2Client+0xA6FC0 (55 B, stdcall p): u = 0x51A80 (); if u && u[0] == 2 and the data record 0xC226 (u[4]) has a negative signed byte at +0x167, returns 0;
// else returns (0x51A80 () == p).
#include <cstdint>
#include <emscripten.h>

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a6fc0(uintptr_t p, uintptr_t (*f51a80)(), uintptr_t (*fc226)(uint32_t id)) {
    uintptr_t u = f51a80();
    if (u && *(const uint32_t*)u == 2) {
        uintptr_t d = fc226(*(const uint32_t*)(u + 4));
        if (*(const int8_t*)(d + 0x167) < 0) return 0;
    }
    return f51a80() == p;
}
