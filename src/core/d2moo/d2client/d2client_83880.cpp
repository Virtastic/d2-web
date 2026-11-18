// src/core/d2client/d2client_83880.cpp -- 1.13c D2Client+0x83880, pkt in EAX, plain ret.
#include "d2client_83880.h"
#include <cstring>
#include <emscripten.h>

namespace {
uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
constexpr uint32_t kUnitHash = 0xB2000000u | 0x10A608u;  // EDX of 0xA4E20: the unit hash table of type pkt[8] (0x200 bytes per type)
}

// 1.13c D2Client+0x83880
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_83880(D2c83880G* g, const D2c83880Env* e, const uint8_t* pkt) {
    uint8_t L[24];
    e->ca90((uint32_t)(uintptr_t)(pkt + 0xd), (uint32_t)pkt[2] - 0xd, 0, L);
    uint32_t oid; memcpy(&oid, pkt + 9, 4);
    const uint32_t t = pkt[8];
    const uint32_t owner = e->fa4e20(oid & 0x7f, oid, kUnitHash + t * 0x200, t);
    if (!owner) return;
    uint32_t id; memcpy(&id, pkt + 4, 4);
    uint32_t found = 0;
    for (uint32_t x = g->hash[id & 0x7f]; x; x = rd32(x + 0xE4))
        if (rd32(x + 0xC) == id) { if (rd32(x) != 4) { e->fatal(0x5a); return; } found = x; break; }
    uint32_t item;
    if (found) {
        uint16_t w4, w6; memcpy(&w4, L + 4, 2); memcpy(&w6, L + 6, 2);
        const uint32_t f = L[0x10];
        e->f82820(L[8], 0, w4, w6, 0);
        e->ca24(found, f);
        item = found;
    } else {
        item = e->f833f0(0, pkt, L, 0, 0);
    }
    const uint32_t inv = rd32(owner + 0x60);
    if (!inv) { e->fatal(0xb34); return; }
    if (e->ca6c(inv, item, 1)) {
        uint32_t lc; memcpy(&lc, L + 0xC, 4);
        if (e->ca3c(lc, 8) == 0) {
            const uint32_t pi = rd32(*g->player + 0x60);
            if (!pi) { e->fatal(0xb3a); return; }
            e->c7d8(pi, 0);
            e->c202(pi);
            e->f162d0();
        }
        e->f81ff0(owner, item, 0);
    }
    e->f81aa0(item);
}
