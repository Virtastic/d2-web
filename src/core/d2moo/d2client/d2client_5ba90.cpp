// 1.13c D2Client+0x5BA90 party-member name bars (EAX unit, EBX &y; stack &x, &toggle; ret 8). Globals (addresses in G, retail
// pointers 32-bit): view mode 0x11C284, party list head 0x11BC14 (node +0x10 unit id, +0x1C kind, +0x22 group word, +0x66 name,
// +0x80 next), kind colours 0x1177C8 (jump table 0xB0BCA4: kind 0..6 = +0, +4, +0x10, +0xC, +8, +0x18, +0x14). grp = the group
// word of 0x9D630 (EDX unit id) for a type-0 unit, else 0xFFFF. grp 0xFFFF, mode != 0 or an empty list: 0. For each node: its
// group word counts when two or more nodes share it; equal to grp, a node id other than the unit's and kind <= 6 with a non-zero
// colour c: 0x5B5A0 (EAX 0, ECX/EDX node id; y), 0x5AEC0 (node+0x66, y, x), toggle set: x -= toggle, toggle = 0, else toggle =
// 10, x += 10; 0xD096 (blk[18] zero with blk[13] = c, y, 0x3C, -1, 1, 0); y += 0x38. Returns 1 when any node drew.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_callee.h"

struct D2c5ba90G { uint32_t mode, head, cols; };
static uint32_t rd32(uint32_t a) { uint32_t x; memcpy(&x, (const void*)(uintptr_t)a, 4); return x; }
static uint16_t rd16(uint32_t a) { uint16_t x; memcpy(&x, (const void*)(uintptr_t)a, 2); return x; }
static void wr32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2c_5ba90(const D2cSvc& v, const D2c5ba90G* g, uint32_t unit, uint32_t py, uint32_t px,
                                                  uint32_t pt) {
    uint16_t grp = 0xffff;
    if (unit && rd32(unit) == 0) {
        const uint32_t n = d2c_call(v, 0x9d630, 0, 0, rd32(unit + 0xc), 0, unit);
        if (n) grp = rd16(n + 0x22);
    }
    const uint32_t uid = unit ? rd32(unit + 0xc) : 0xffffffffu;
    if (grp == 0xffff || rd32(g->mode) != 0 || !rd32(g->head)) return 0;
    static const uint8_t off[7] = { 0, 4, 0x10, 0xc, 8, 0x18, 0x14 };
    bool drew = false;
    for (uint32_t cur = rd32(g->head); cur; cur = rd32(cur + 0x80)) {
        int cnt = 0;
        const uint16_t w = rd16(cur + 0x22);
        for (uint32_t e = rd32(g->head); e; e = rd32(e + 0x80)) if (rd16(e + 0x22) == w) ++cnt;
        if ((cnt > 1 ? w : 0xffff) != grp) continue;
        const uint32_t id = rd32(cur + 0x10);
        if (id == uid) continue;
        const uint32_t kind = rd32(cur + 0x1c);
        if (kind > 6) continue;
        const uint32_t c = rd32(g->cols + off[kind]);
        if (!c) continue;
        d2c_call(v, 0x5b5a0, 0, id, id, 0, 0, rd32(py));
        d2c_call(v, 0x5aec0, 0, 0, 0, 0, 0, cur + 0x66, rd32(py), rd32(px));
        if (rd32(pt)) { wr32(px, rd32(px) - rd32(pt)); wr32(pt, 0); }
        else { wr32(pt, 10); wr32(px, rd32(px) + 10); }
        uint32_t blk[18] = {};
        blk[13] = c;
        d2c_call(v, 0xd096, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)blk, rd32(py), 0x3c, 0xffffffffu, 1, 0);
        wr32(py, rd32(py) + 0x38);
        drew = true;
    }
    return drew ? 1 : 0;
}
