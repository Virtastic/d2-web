// 1.13c D2Client+0x259F0 -- per-unit client refresh (ECX = unit, EAX = {p1, mode}); decompile-derived call order.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_25040.h"

static uint32_t rd32(const uint8_t* p, unsigned o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }
static void wr32(uint8_t* p, unsigned o, uint32_t v) { std::memcpy(p + o, &v, 4); }

// 1.13c D2Client+0x259F0
extern "C" void d2_client_259f0(uint8_t* u, const uint32_t* eax, const D2c259Env* e) {
    const uint8_t* c = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(rd32(u, 0x14)));
    const uint8_t* r = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(rd32(c, 0)));
    const uint32_t mode = eax[1];
    if (r[0x13a] != 0) {
        d2_client_25040(u, static_cast<int32_t>(mode), eax[0], &e->p);
    } else {
        e->p.c43c(u, mode);
        e->p.refresh(u, 0);
        const uint32_t t = rd32(u, 4), k = rd32(u, 0x10);
        if (r[0x1b5] && (t == 0x162 || t == 0x163 || t == 0x18d || t == 0x195 || t == 0x197 || t == 0x196 || t == 0x164) && k == 0)
            e->f6b300(u);
        wr32(u, 0xc4, r[k + 0xc4] ? (rd32(u, 0xc4) | 2) : (rd32(u, 0xc4) & ~2u));
        if (r[k + 0x120] == 0) e->p.ca1e(u);
        if (r[k + 0x131] == 1) wr32(u, 0xc4, rd32(u, 0xc4) | 0x100000);
        e->animate(u, r[mode + 0x110], r[0x164], r[0x165], r[0x166]);
    }
    e->advance(u);
    e->update(u);
    const uint32_t k = rd32(u, 0x10);
    wr32(u, 0xc4, r[k + 0x118] == 0 ? (rd32(u, 0xc4) | 0x20) : (rd32(u, 0xc4) & ~0x20u));
}
