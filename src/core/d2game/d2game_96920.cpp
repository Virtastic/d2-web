// 1.13c D2Game+0x96920: eax = unit, stack p (ret 4). With the unit's game G = [unit +0x80] and id edi = [unit +0xC], walks the pet table [0xA4C0(unit) +0x44]
// (12-byte rows, dt+0xBF0 of them; each row heads a list, node +0 flags bit 0 skip, +4 id, +0x14 next). A node whose id has a unit in G's bucket (G +0x1320 + (id & 0x7F)*4,
// chain +0xE4, key +0xC) sends row 7 through 0x8B220(eax = unit v, ecx = [node +8], edx = id; p, 7, edi, [unit +4], [node +8], [node +0xC]) and any other row through
// 0x8B290(eax = v, ecx = G with cl = 1, edx = id; p, row, id, edi, [unit +4]).
#include "d2game_seam4e.h"
extern "C" void d2game_96920(uint32_t unit, uint32_t p, uint32_t dt, const D2Seam* e)
{
    const uint32_t r = e->call(0xA4C0, 0, 0, 0, 0, 0, unit);
    if (!unit) {   // fatal 0xA2A: retail exits the process here
        const uint32_t s = e->call(0xA2DA, 0, 0, 0, 0, 0, 0xA2A);
        e->call(0xA2C2, 0, 0, 0, 0, 0, 0, s);
        e->call(0x2A3C, 0, 0, 0, 0, 0, 0xFFFFFFFFu);
        return;
    }
    const uint32_t G = d2rd32(unit + 0x80), edi = d2rd32(unit + 0xC);
    if (!r || !G) return;
    const uint32_t q = d2rd32(r + 0x44);
    if (!q) return;
    for (int32_t row = 0; row < (int32_t)d2rd32(dt + 0xBF0); row++) {
        const uint32_t rp = d2rd32(q) + row * 12;
        if (!rp) continue;
        for (uint32_t n = d2rd32(rp); n; n = d2rd32(n + 0x14)) {
            if (d2rd8(n) & 1) continue;
            const uint32_t id = d2rd32(n + 4);
            uint32_t u = d2rd32(G + 0x1320 + (id & 0x7F) * 4);
            while (u && d2rd32(u + 0xC) != id) u = d2rd32(u + 0xE4);
            if (!u) continue;
            const uint32_t v = d2rd32(u + 4);
            if (row == 7) e->call(0x8B220, v, d2rd32(n + 8), id, 0, 0, p, 7, edi, v, d2rd32(n + 8), d2rd32(n + 0xC));
            else e->call(0x8B290, v, (G & 0xFFFFFF00u) | 1, id, 0, 0, p, (uint32_t)row, id, edi, v);
        }
    }
}
