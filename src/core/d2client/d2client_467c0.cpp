// D2Client 0x467C0: releases the eight owned handles at 0x119757..0x119773.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
// 1.13c D2Client+0x467C0 ; h = the eight dwords from 0x119757 (stride 4); each nonzero one is released through
// 0xA3D20 (no arguments; EAX happens to hold the handle on the first test only) and zeroed, in retail's order.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_467c0(uint32_t* h, D2cHost host) {
    static const uint8_t order[8] = { 4, 5, 3, 7, 0, 1, 2, 6 };   // 9767, 976B, 9763, 9773, 9757, 975B, 975F, 976F
    for (uint8_t i : order) if (h[i]) { D2cCall c = {}; c.rva = 0xA3D20; host(&c); h[i] = 0; }
}
