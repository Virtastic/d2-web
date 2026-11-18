// 1.13c D2Client+0x87840 wardenActivatePending (no arguments). Globals: cur 0x11BF4C, pend 0x11BF50, seed 0xE137C (retail
// pointers are 32-bit). While pend: when cur is set, call its iface's vtable slot +4 (ECX iface), 0x9E630 (ESI cur), and when
// cur is still set 0x1A00(cur) and Storm free 0xBE6C(cur, "..\Source\D2Client\WARDEN\WardenClient.cpp" (RVA 0xD4938), 0x117, 0).
// Then cur = pend, pend = 0, node+0xC = the node+0x10 entry (ECX = RVA 0xF9E1C, a module table), and the new iface's vtable
// slot +0 is called with ECX iface and (&seed copy, 4). Indirect targets reach the callee dispatcher keyed by the pointer.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_callee.h"

struct D2cWardenG { uint32_t cur, pend, seed; };
static uint32_t rd32(uint32_t a) { uint32_t x; memcpy(&x, (const void*)(uintptr_t)a, 4); return x; }

extern "C" EMSCRIPTEN_KEEPALIVE void d2c_87840(const D2cSvc& v, D2cWardenG* g) {
    while (g->pend) {
        if (g->cur) {
            const uint32_t ia = rd32(g->cur + 0xc);
            d2c_call(v, rd32(rd32(ia) + 4), 0, ia, 0, 0, 0);
            d2c_call(v, 0x9e630, 0, 0, 0, 0, g->cur);
            const uint32_t c = g->cur;
            if (c) { d2c_call(v, 0x1a00, 0, 0, 0, 0, 0, c); d2c_call(v, 0xbe6c, 0, 0, 0, 0, 0, c, 0xd4938, 0x117, 0); }
        }
        const uint32_t n = g->pend;
        g->cur = n;
        g->pend = 0;
        const uint32_t r = d2c_call(v, rd32(n + 0x10), 0, 0xf9e1c, 0, 0, 0);
        memcpy((void*)(uintptr_t)(n + 0xc), &r, 4);
        uint32_t seed = g->seed;
        const uint32_t ia = rd32(n + 0xc);
        d2c_call(v, rd32(rd32(ia)), 0, ia, 0, 0, 0, (uint32_t)(uintptr_t)&seed, 4);
    }
}
