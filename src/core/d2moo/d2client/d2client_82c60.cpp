// src/core/d2client/d2client_82c60.cpp -- 1.13c D2Client+0x82C60, stdcall(pkt), ret 4.
#include "d2client_82c60.h"
#include <cstring>
#include <emscripten.h>

namespace {
uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
constexpr uint32_t kHashRva = 0xB2000000u | 0x10AE08u;   // ECX of 0xA6EF0 is the module address of the hash
constexpr uint32_t kFileRva = 0xB2000000u | 0xD4964u;    // "..\Source\D2Client\UNIT\Item.cpp"
}

// 1.13c D2Client+0x82C60
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_82c60(D2c82c60G* g, const D2c82c60Env* e, const uint8_t* pkt) {
    uint8_t L[24];
    e->ca90((uint32_t)(uintptr_t)(pkt + 8), (uint32_t)pkt[2] - 8, 0, L);
    const uint32_t pl = *g->player;
    if (!pl) { e->fatal(0x92c); return; }
    uint32_t id; memcpy(&id, pkt + 4, 4);
    uint32_t u = 0;
    for (uint32_t x = g->hash[id & 0x7f]; x; x = rd32(x + 0xE4))
        if (rd32(x + 0xC) == id) { if (rd32(x) != 4) { e->fatal(0x5a); return; } u = x; break; }
    const uint32_t inv = rd32(pl + 0x60);
    if (!inv) { e->fatal(0x931); return; }
    const uint32_t f = e->c6d0(inv, u);
    if (!f) return;
    if (f != u) { e->fatal(0x93a); return; }
    uint16_t slot; memcpy(&slot, L + 4, 2);
    e->caa2(inv, slot);
    e->f26410();
    if (slot < 4) g->cur[slot] = 0;
    if (rd32(pl) == 0) e->ca0c(rd32(pl + 0x60));
    if (e->c1a8(u, 0x100, 0x96, kFileRva) == 0) e->c544(pl, u);
    uint32_t lc, l10; memcpy(&lc, L + 0xC, 4); memcpy(&l10, L + 0x10, 4);
    if (e->ca3c(lc, 0x20) == 0) {
        e->f82980(u, 0, 0, L[8], 0, l10);
        e->c7d8(inv, u);
        e->c202(inv);
        e->f162d0();
        e->f81aa0();
        return;
    }
    e->a6ef0(kHashRva, id & 0x7f, id, 4);
    *g->b834 = 0; *g->b864 = 1; *g->b854 = 5; *g->b858 = 0;
}
