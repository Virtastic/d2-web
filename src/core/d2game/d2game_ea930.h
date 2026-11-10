// 1.13c D2Game+0xEA930: EAX = unit, EBX = game. Calls 0xEA730(game, unit, 0x23, 0x28, max(wRange(missile 0x15B) - 100, 100), 0), then
// 0xA0000(game, 0, 1, 0x15B, x, y) with the unit's x and y: type 2 and 4..5 read path+0xC / path+0x10, other types ask thunk 0xA454 (x) / 0xA3B8 (y)
// for a non-null path (0 for a null path). Missile record = dt[0xB64] + 0x15B * 0x1A4, wRange = signed word at +0x96; dt = **(0xF829C).
#pragma once
#include <cstdint>
namespace gea930 {
template <class E> void Run(E& e, uint32_t unit, uint32_t game, uint32_t slot)
{
    // 1.13c D2Game+0xEA930
    const uint32_t dt = e.rd32(e.rd32(slot));
    const uint32_t rec = (int32_t)e.rd32(dt + 0xB6C) > 0x15B ? e.rd32(dt + 0xB64) + 0x2394C : 0;
    int32_t mx = e.rd16s(rec + 0x96) - 0x64;
    if (mx < 0x64) mx = 0x64;
    const uint32_t a[6] = { game, unit, 0x23, 0x28, (uint32_t)mx, 0 };
    e.h(0xEA730, 0, 0, 0, 0, 0, a[0], a[1], a[2], a[3]); e.blk(0xEA730, a, 6);
    const int32_t t = (int32_t)e.rd32(unit);
    const bool st = t == 2 || (t > 3 && t <= 5);
    const uint32_t path = e.rd32(unit + 0x2C);
    uint32_t y, x;
    if (st) y = e.rd32(path + 0x10); else y = path ? e.h(0xA3B8, 0, 0, 0, 0, 0, path) : 0;
    if (st) x = e.rd32(path + 0xC); else x = path ? e.h(0xA454, 0, 0, 0, 0, 0, path) : 0;
    const uint32_t b[6] = { game, 0, 1, 0x15B, x, y };
    e.h(0xA0000, 0, 0, 0, 0, 0, b[0], b[1], b[2], b[3]); e.blk(0xA0000, b, 6);
}
}
