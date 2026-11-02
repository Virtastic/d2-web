// 1.13c D2Game+0x4EA30: eax = unit, ecx = amt, esi = game (+0x1320 + (key & 0x7F) * 4 buckets; node +0xC key, +0xE4 next); `dt` = *sgptDataTables (+0xBF0).
// Finds the unit's linked record through 0xA4C0 (+0x44, +0x24 chain), then the 0x122 unit with that key: amt == 0 resets its stat 6 to its maximum, otherwise
// the unit gives up amt scaled by 0x1EC0 / 100 through 0x4DF20 and amt less that is returned. Callees: 0xA4C0, 0xA3A0, 0xA472, 0x1220, 0x1EC0, 0x4DF20.
#include "d2game_seam4e.h"
extern "C" int32_t d2game_4ea30(uint32_t unit, int32_t amt, uint32_t g, uint32_t dt, const D2Seam* e)
{
    if (!unit) return amt;
    const uint32_t r = e->call(0xA4C0, 0, 0, 0, 0, 0, unit);
    if (!r) return amt;
    if ((int32_t)d2rd32(dt + 0xBF0) <= 3) return amt;
    const uint32_t p1 = d2rd32(r + 0x44);
    if (!p1) return amt;
    const uint32_t cell = d2rd32(p1) + 0x24;
    uint32_t node = d2rd32(cell);
    if (!node) return amt;
    while (d2rd8(node) & 1) {
        node = d2rd32(node + 0x14);
        if (!node) return amt;
    }
    if (!g) {
        const uint32_t m = e->call(0xA2DA, 0, 0, 0, 0, 0, 0x2D3);
        e->call(0xA2C2, 0, 0, 0, 0, 0, 0x6FD19FF9u, m);
        e->call(0x2A3C, 0, 0, 0, 0, 0, 0xFFFFFFFFu);
        return amt;
    }
    const uint32_t key = d2rd32(node + 4);
    uint32_t piv = d2rd32(g + 0x1320 + (key & 0x7F) * 4);
    if (!piv) return amt;
    while (d2rd32(piv + 0xC) != key) {
        piv = d2rd32(piv + 0xE4);
        if (!piv) return amt;
    }
    if (d2rd32(piv) != 1 || d2rd32(piv + 4) != 0x122) return amt;
    if (amt == 0) {
        const uint32_t mx = e->call(0xA3A0, 0, 0, 0, 0, 0, piv);
        e->call(0xA472, 0, 0, 0, 0, 0, piv, 6, mx, 0);
        return amt;
    }
    const uint32_t rec = e->call(0x1220, 0x122);
    if (!rec) return amt;
    const int32_t v = (int16_t)d2rd16(rec + 0x170);
    if (v < 0) return amt;
    const int32_t m = (int32_t)e->call(0x1EC0, (uint32_t)v);
    const int32_t prod = (int32_t)((uint32_t)m * (uint32_t)amt);
    const int32_t hi = (int32_t)(((int64_t)prod * (int64_t)0x51EB851F) >> 32) >> 5;
    const int32_t q = hi + (int32_t)((uint32_t)hi >> 31);
    const int32_t got = (int32_t)e->call(0x4DF20, (uint32_t)q, 0, 0, piv);
    return (int32_t)((uint32_t)amt - (uint32_t)got);
}
