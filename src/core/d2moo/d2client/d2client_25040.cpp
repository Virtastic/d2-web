// 1.13c D2Client+0x25040 -- unit mode switch (ESI = unit, EDI = new mode, stack arg = p1), decompile-derived call order.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_25040.h"

static uint32_t rd32(const uint8_t* p, unsigned o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }
static void wr32(uint8_t* p, unsigned o, uint32_t v) { std::memcpy(p + o, &v, 4); }
static const uint8_t* rec_of(const uint8_t* u) {            // **(u+0x14)
    const uint8_t* c = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(rd32(u, 0x14)));
    return reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(rd32(c, 0)));
}
static void panel_end(uint8_t* u, const D2c25040Env* e) {
    const uint32_t a = e->f1660(u);
    const uint32_t b = e->f1630(u);
    e->c67c(u, e->c1f0(u, b, a));
}
static void tail(uint8_t* u, const uint8_t* r) {
    const uint32_t f = rd32(u, 0xc4);
    wr32(u, 0xc4, r[rd32(u, 0x10) + 0x118] == 0 ? (f | 0x20) : (f & ~0x20u));
}

// 1.13c D2Client+0x25040
void d2_client_25040(uint8_t* u, int32_t mode, uint32_t p1, const D2c25040Env* e) {
    const uint8_t* r = rec_of(u);
    if (mode >= 8) { e->fatal(0x210); return; }
    const uint32_t cur = rd32(u, 0x10);
    if (mode == 0) {
        if (cur == 2 && rd32(r, 0xe4) != 0x100) {
            wr32(u, 0x44, r[0x129]);
            e->c43c(u, 3); e->refresh(u, 0);
            wr32(u, 0xc4, rd32(u, 0xc4) & ~2u);
            panel_end(u, e); tail(u, r); return;
        }
    } else if (mode == 2 && cur == 0 && rd32(r, 0xdc) != 0x100) {
        e->c43c(u, 1); e->refresh(u, 0);
        wr32(u, 0xc4, rd32(u, 0xc4) & ~2u);
        wr32(u, 0x44, r[0x12b]);
        tail(u, r); return;
    }
    wr32(u, 0xc4, p1 ? (rd32(u, 0xc4) | 2) : (rd32(u, 0xc4) & ~2u));
    if (static_cast<uint32_t>(mode) != cur) {
        if (mode == 4 || mode == 2 || mode == 5) {
            if (cur != 5 && cur != 2 && cur != 4) e->ca1e(u);
        } else if (mode == 0 && cur != 0) {
            panel_end(u, e);
        }
        e->c43c(u, static_cast<uint32_t>(mode)); e->refresh(u, 0);
    }
    wr32(u, 0x44, r[mode + 0x129]);
    tail(u, r);
}
