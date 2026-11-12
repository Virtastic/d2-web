// src/core/d2client/d2client_82b20.cpp -- 1.13c D2Client+0x82B20, stdcall(pkt, pkt2), ret 8.
#include "d2client_82b20.h"
#include <cstring>
#include <emscripten.h>

namespace {
uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
uint32_t rd16(uint32_t a) { uint16_t v; memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
uint32_t rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
}

// 1.13c D2Client+0x82B20
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_82b20(const D2c82b20G* g, const D2c82b20Env* e, uint32_t pkt, uint32_t p2) {
    const uint32_t pl = *g->player;
    if (!pl) { e->fatal(0x957); return; }
    const uint32_t id = rd32(pkt + 4);
    uint32_t u = g->hash[id & 0x7f];
    for (; u; u = rd32(u + 0xE4)) if (rd32(u + 0xC) == id) break;
    if (!u) return;
    if (rd32(u) != 4) { e->fatal(0x5a); return; }
    // the mode byte is stored into the low byte of the packet-2 argument slot, and the whole slot is pushed to 0xCA24
    const uint32_t slot = (p2 & ~0xFFu) | rd8(p2 + 0x10);
    e->f82820(rd8(p2 + 8), u, 0, 0, 0, 0);
    e->ca24(u, slot);
    const uint32_t inv = rd32(pl + 0x60);
    if (!inv) { e->fatal(0x963); return; }
    e->ca06(u, rd16(p2 + 4));
    e->ca48(u, rd16(p2 + 6));
    if (e->c1cc(u, 0xd)) e->f81ff0(u, pl, 0);
    if (e->ca6c(inv, u, 2)) {
        const uint32_t r = e->f1630(u);
        if (!e->ca84(inv, u, r)) { e->fatal(0x970); return; }
    }
    e->fbe2b0();
    e->f81aa0(u);
}
