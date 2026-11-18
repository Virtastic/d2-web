// src/core/d2client/d2client_83790.cpp -- 1.13c D2Client+0x83790, pkt in EAX, plain ret.
#include "d2client_83790.h"
#include <cstring>
#include <emscripten.h>

namespace {
uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
constexpr uint32_t kHashRva = 0xB2000000u | 0x10AE08u;   // ECX of 0xA6EF0: the item hash
constexpr uint32_t kUnitHash = 0xB2000000u | 0x10A608u;  // EDX of 0xA4E20: the unit hash table of type pkt[8] (0x200 bytes per type)
}

// 1.13c D2Client+0x83790
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_83790(D2c83790G* g, const D2c83790Env* e, const uint8_t* pkt) {
    uint8_t L[24];
    e->ca90((uint32_t)(uintptr_t)(pkt + 0xd), (uint32_t)pkt[2] - 0xd, 0, L);
    uint32_t oid; memcpy(&oid, pkt + 9, 4);
    const uint32_t t = pkt[8];
    const uint32_t owner = e->fa4e20(oid & 0x7f, oid, kUnitHash + t * 0x200, t);
    if (!owner) return;
    uint32_t id; memcpy(&id, pkt + 4, 4);
    e->a6ef0(kHashRva, id & 0x7f, id, 4);
    const uint32_t item = e->f833f0(0, pkt, L, 0, 0);
    if (!item) { e->fatal(0xd35); return; }
    e->c40c(item, 0x46, 1, 0);
    const uint32_t inv = rd32(owner + 0x60);
    e->c7d8(inv, item);
    const uint32_t c = e->c202(inv);
    *g->b834 = c; *g->b858 = 0; *g->b854 = 5; *g->b864 = 1;
    if (c) *g->b864 = 4;
}
