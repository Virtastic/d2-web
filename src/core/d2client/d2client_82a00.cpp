// src/core/d2client/d2client_82a00.cpp -- 1.13c D2Client+0x82A00, EAX = packet, stack: packet 2; ret 4.
#include "d2client_82a00.h"
#include <cstring>
#include <emscripten.h>

namespace {
uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
uint32_t rd16(uint32_t a) { uint16_t v; memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
uint32_t rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
constexpr uint32_t kFileRva = 0xB2000000u | 0xD4964u;   // "..\Source\D2Client\UNIT\Item.cpp"
}

// 1.13c D2Client+0x82A00
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_82a00(const D2c82a00G* g, const D2c82a00Env* e, uint32_t eax, uint32_t p1) {
    const uint32_t pl = *g->player;
    if (!pl) return;
    const uint32_t id = rd32(eax + 4);
    uint32_t u = g->hash[id & 0x7f];
    for (; u; u = rd32(u + 0xE4)) if (rd32(u + 0xC) == id) break;
    if (!u) return;
    if (rd32(u) != 4) { e->fatal(0x5a); return; }
    const uint32_t inv = rd32(pl + 0x60);
    if (!inv) { e->fatal(0x98d); return; }
    const uint32_t f = e->c6d0(inv, u);
    if (!f) { e->fatal(0x98f); return; }
    if (f != u) { e->fatal(0x990); return; }
    if (rd32(pl) == 0) e->ca0c(rd32(pl + 0x60));
    if (e->c1a8(u, 0x100, 0x96, kFileRva) == 0) e->c544(pl, u);
    e->caa2(inv, rd16(p1 + 4));
    e->f82980(u, 0, 0, rd8(p1 + 8), 0, rd8(p1 + 0x10));
    e->c7d8(inv, u);
    const uint32_t c = e->c202(inv);
    e->f162d0(c);
    e->f81aa0(u);
}
