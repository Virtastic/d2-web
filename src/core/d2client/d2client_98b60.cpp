// 1.13c D2Client+0x98B60 inventory-grid click (EAX flag (low byte tested, all 32 bits passed on), ECX x, EDX y; stack unit, grid
// owner; ret 8). Globals (addresses in G): grid geometry 0x10B2D4..0x10B3F5 (flag 0: y base 0x10B3E4 / cell 0x10B3F4 (byte), x base
// 0x10B3EC / cell 0x10B3F5; flag set with 0x119854 clear: 0x10B39C / 0x10B3AC, 0x10B3A4 / 0x10B3AD; with 0x119854 set: 0x10B2D4 /
// 0x10B2E4, 0x10B2DC / 0x10B2E5), 0x119854, viewed unit 0x11BBFC, 0x11BC28, 0x11BC98, 0xFADE4. r0 = 0xC8A4(unit, eax, [0x119854]);
// item = 0xC95E(owner, (y - yb) / yc, (x - xb) / xc, &l1, &l2, r0, eax) (unsigned divides). No item: 0. 0xC202(owner) clear and
// 0xC856(item) set: 0x81CB0 (EAX item) set: 0; else 0x14940 (AL 0x4C, ECX item+0xC), 0x81CE0 (EAX item), 1.
// 0xC814(item) set: a = item+0x2C's +0xC / +0x10 for unit types 2, 4, 5, else 0xC3BE / 0xC2EC (a non-null +0x2C) or 0;
// 0xCA12(item, 2) set: 0; viewed unit with 0xC1FC(u, 0x36) set: 0; 0x923F0 clear: 0; 0xFADE4 set and 0xC820(item) 'box ':
// 0x11BC98 = 1; 0x148E0 (AL 0x20 over EAX, ECX item+0xC, EDX a; b), 0x81CE0 (EAX item), 0x88A70 (EBX 0x81D70(); 0, 0, 0, 0),
// 0x11BC28 = 0, 1. Otherwise 0xC8F8(item) set: rec = 0xC1BA(item+4); kinds 'trs ', 'bks ', 'bkd ': 0x7EFA0 (EAX rec; item); 1 either way.
// Any other path: 0.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_callee.h"

struct D2c98b60G { uint32_t mode, grid, unit, bc28, bc98, fade4; };
static uint32_t rd32(uint32_t a) { uint32_t x; memcpy(&x, (const void*)(uintptr_t)a, 4); return x; }
static uint8_t rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
static void wr32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2c_98b60(const D2cSvc& v, const D2c98b60G* g, uint32_t fl, uint32_t x, uint32_t y,
                                                  uint32_t unit, uint32_t owner) {
    const uint32_t r0 = d2c_call(v, 0xc8a4, 0, 0, 0, 0, 0, unit, fl, rd32(g->mode));
    uint32_t yb, yc, xb, xc;
    if ((fl & 0xff) == 0) { yb = 0x110; yc = 0x120; xb = 0x118; xc = 0x121; }
    else if (rd32(g->mode)) { yb = 0; yc = 0x10; xb = 8; xc = 0x11; }
    else { yb = 0xc8; yc = 0xd8; xb = 0xd0; xc = 0xd9; }
    const uint32_t row = (y - rd32(g->grid + yb)) / rd8(g->grid + yc);
    const uint32_t col = (x - rd32(g->grid + xb)) / rd8(g->grid + xc);
    uint32_t l1 = 0, l2 = 0;
    const uint32_t rz[5] = {}, s7[7] = { owner, row, col, (uint32_t)(uintptr_t)&l1, (uint32_t)(uintptr_t)&l2, r0, fl };   // 7 stack words
    const uint32_t it = v.call(v.ctx, 0xc95e, rz, s7);
    if (!it) return 0;
    if (!d2c_call(v, 0xc202, 0, 0, 0, 0, 0, owner) && d2c_call(v, 0xc856, 0, 0, 0, 0, 0, it)) {
        const uint32_t ax = d2c_call(v, 0x81cb0, it, 0, 0, 0, 0);
        if (ax) return 0;
        d2c_call(v, 0x14940, (ax & ~0xffu) | 0x4c, rd32(it + 0xc), 0, 0, 0);
        d2c_call(v, 0x81ce0, it, 0, 0, 0, 0);
        return 1;
    }
    if (d2c_call(v, 0xc814, 0, 0, 0, 0, 0, it)) {
        const uint32_t t = rd32(unit), p = rd32(unit + 0x2c);
        const bool direct = t == 2 || (t > 3 && t <= 5);
        const uint32_t a = direct ? rd32(p + 0xc) : p ? d2c_call(v, 0xc3be, 0, 0, 0, 0, 0, p) : 0;
        const uint32_t t2 = rd32(unit), p2 = rd32(unit + 0x2c);
        const bool direct2 = t2 == 2 || (t2 > 3 && t2 <= 5);
        const uint32_t b = direct2 ? rd32(p2 + 0x10) : p2 ? d2c_call(v, 0xc2ec, 0, 0, 0, 0, 0, p2) : 0;
        if (d2c_call(v, 0xca12, 0, 0, 0, 0, 0, it, 2)) return 0;
        const uint32_t u = rd32(g->unit);
        if (u && d2c_call(v, 0xc1fc, 0, 0, 0, 0, 0, u, 0x36)) return 0;
        if (!d2c_call(v, 0x923f0, 0, 0, 0, 0, 0)) return 0;
        uint32_t ax = rd32(g->fade4);
        if (ax) {
            ax = d2c_call(v, 0xc820, 0, 0, 0, 0, 0, it);
            if (ax == 0x20786f62) wr32(g->bc98, 1);
        }
        d2c_call(v, 0x148e0, (ax & ~0xffu) | 0x20, rd32(it + 0xc), a, 0, 0, b);
        d2c_call(v, 0x81ce0, it, 0, 0, 0, 0);
        const uint32_t bx = d2c_call(v, 0x81d70, 0, 0, 0, 0, 0);
        d2c_call(v, 0x88a70, 0, 0, 0, bx, 0, 0, 0, 0, 0);
        wr32(g->bc28, 0);
        return 1;
    }
    if (!d2c_call(v, 0xc8f8, 0, 0, 0, 0, 0, it)) return 0;
    const uint32_t rec = d2c_call(v, 0xc1ba, 0, 0, 0, 0, 0, rd32(it + 4));
    const uint32_t k = rd32(rec + 0x80);
    if (k == 0x20737274 || k == 0x20736b62 || k == 0x20646b62) d2c_call(v, 0x7efa0, rec, 0, 0, 0, 0, it);
    return 1;
}
