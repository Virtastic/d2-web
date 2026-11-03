// D2Game+0x50800 -- SkillNec_BuildUnitStatPacket: EAX = unit, ECX = game, no stack args, plain ret. Reads the unit's two coordinates (type 2, 4 or 5: dwords +0xC and +0x10 of the record at unit +0x2C;
// other types: 0xA454 / 0xA3B8 on that record, 0 when it is null), asks the room (0xA478) for a free spot 0xAD30(room, x, y, 0xAD00(unit, 0x8000)), then 0xA694(room, {x, y}, 0xA71E(unit), 0x3C01, 1);
// zero: return. Otherwise unit flags |= 0x402000E; 0xAF920(EAX = game, EBX = unit); 0xA496(unit, 0); the 8-dword block {1, unit, 0, 0, 0, 0x6400 (byte +0x15 = 0x64), 0, 0} goes to 0x75E10(game, block, 1);
// 0x9F730(ESI = unit); 0x8E6F0(EAX = node) for each kind-8 node of the list at unit +0xDC (next at +0x24); 0xA3B2(unit) non-zero: 0xAE08(unit, 0, -1);
// 0xE19C0(EAX = y, ECX = x, EDX = the 0xA694 result; game, unit, 0, 0).
#include "d2game_seam4e.h"
// 1.13c D2Game+0x50800
static inline bool d2game_50800_direct(uint32_t unit)
{
    const int32_t t = (int32_t)d2rd32(unit);
    return t == 2 || (t > 3 && t <= 5);
}
extern "C" void d2game_50800(uint32_t unit, uint32_t game, const D2Seam* e)
{
    auto pv = [&]() -> uint32_t {   // the +0x10 / 0xA3B8 value
        if (d2game_50800_direct(unit)) return d2rd32(d2rd32(unit + 0x2C) + 0x10);
        const uint32_t p = d2rd32(unit + 0x2C);
        return p ? e->call(0xA3B8, 0, 0, 0, 0, 0, p) : 0;
    };
    auto bv = [&]() -> uint32_t {   // the +0xC / 0xA454 value
        if (d2game_50800_direct(unit)) return d2rd32(d2rd32(unit + 0x2C) + 0xC);
        const uint32_t p = d2rd32(unit + 0x2C);
        return p ? e->call(0xA454, 0, 0, 0, 0, 0, p) : 0;
    };
    const uint32_t p1 = pv();
    const uint32_t b1 = bv();
    const uint32_t m = e->call(0xAD00, 0, 0, 0, 0, 0, unit, 0x8000);
    const uint32_t room = e->call(0xA478, 0, 0, 0, 0, 0, unit);
    e->call(0xAD30, 0, 0, 0, 0, 0, room, b1, p1, m);
    uint32_t pair[2];
    pair[0] = bv();
    pair[1] = pv();
    const uint32_t a71 = e->call(0xA71E, 0, 0, 0, 0, 0, unit);
    const uint32_t room2 = e->call(0xA478, 0, 0, 0, 0, 0, unit);
    const uint32_t r = e->call(0xA694, 0, 0, 0, 0, 0, room2, (uint32_t)(uintptr_t)pair, a71, 0x3C01, 1);
    if (!r) return;
    d2wr32(unit + 0xC4, d2rd32(unit + 0xC4) | 0x402000E);
    e->call(0xAF920, game, 0, 0, unit);
    e->call(0xA496, 0, 0, 0, 0, 0, unit, 0);
    uint32_t q[8] = {};
    q[0] = 1; q[1] = unit;
    ((uint8_t*)q)[0x15] = 0x64;
    e->call(0x75E10, 0, 0, 0, 0, 0, game, (uint32_t)(uintptr_t)q, 1);
    e->blk(0x75E10, q, 8);
    e->call(0x9F730, 0, 0, 0, 0, unit);
    for (uint32_t n = d2rd32(unit + 0xDC); n;) {
        const uint32_t nx = d2rd32(n + 0x24);
        if (d2rd8(n) == 8) e->call(0x8E6F0, n);
        n = nx;
    }
    if (e->call(0xA3B2, 0, 0, 0, 0, 0, unit)) e->call(0xAE08, 0, 0, 0, 0, 0, unit, 0, 0xFFFFFFFFu);
    e->call(0xE19C0, pair[1], pair[0], r, 0, 0, game, unit, 0, 0);
}
