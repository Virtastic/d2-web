// 1.13c D2Game+0xD040 (binds a monster of class 0x1EC to a partner of class 0x1B3 or 0x1B9 and queues an AI event for it):
// esi = unit, ecx = partner, eax = event value, stack = (pGame, event arg), ret 8. Written from the 1.13c disassembly as a rule
// over E (see d2game_c310.h). Rule: a null unit or partner returns (eax = the event value). The unit's class (0x18A0, eax =
// unit, plain ret) must lie in 0..monstats count (dt+0xA80) and be 0x1EC (else eax = the class); the partner's class (out of
// range: -1) must be 0x1B3 or 0x1B9 (else eax = class - 0x1B9). Then 0xA3CA(unit, 0x8F, 1), 0xAC76(unit, partner),
// 0xAC76(partner, unit), 0x8E900 (eax = unit, ecx = game, stack 5; ret 4) and the event block 0x8E680 (eax = game, ecx = unit):
// byte 0 = 5, byte 2 |= 4, +4 = -1, +0x14 = value, +0x18 = arg, +0x2C = 0, linked at the head of game+0xB8's list
// +0xA04 + 4 * kList[unit type] (+0x1C = old head, +0x20 = 0, old head +0x20 = block). The unit's flags (+0xC4) lose bits 1..3;
// 0x9D230 (ecx = game, edx = unit, stack = (player's +0x14 +0x28 word for a type-1 unit with one, else 0), 0x10); ret 8)
// gives the result.
#pragma once
#include <cstdint>

namespace d040 {
static const int32_t kList[6] = { 0, 1, 3, 2, 4, -1 };   // retail .rdata 0xFA3DC: unit type -> game list index
template <class E> uint32_t BindPartner(E& e, uint32_t dt, uint32_t val, uint32_t partner, uint32_t unit, uint32_t game, uint32_t arg)
{
    if (!unit || !partner) return val;
    const int32_t c = (int32_t)e.h(0x18A0, unit);
    const int32_t n = (int32_t)e.rd32(dt + 0xA80);
    if (c < 0 || c >= n || c != 0x1EC) return (uint32_t)c;
    int32_t d = (int32_t)e.h(0x18A0, partner);
    if (d < 0 || d >= n) d = -1;
    if (d != 0x1B3 && d != 0x1B9) return (uint32_t)(d - 0x1B9);
    e.h(0xA3CA, 0, 0, 0, 0, 0, unit, 0x8F, 1);
    e.h(0xAC76, 0, 0, 0, 0, 0, unit, partner);
    e.h(0xAC76, 0, 0, 0, 0, 0, partner, unit);
    e.h(0x8E900, unit, game, 0, 0, 0, 5);
    const uint32_t p = e.h(0x8E680, game, unit);
    e.wr32(p, (e.rd32(p) & ~0xFFu) | 5u | 0x40000u);
    e.wr32(p + 4, 0xFFFFFFFFu);
    e.wr32(p + 0x14, val);
    e.wr32(p + 0x18, arg);
    e.wr32(p + 0x2C, 0);
    const uint32_t slot = e.rd32(game + 0xB8) + 0xA04 + 4 * (uint32_t)kList[e.rd32(unit) % 6];
    const uint32_t head = e.rd32(slot);
    e.wr32(p + 0x1C, head);
    e.wr32(p + 0x20, 0);
    if (head) e.wr32(head + 0x20, p);
    e.wr32(slot, p);
    e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) & 0xFFFFFFF1u);
    uint32_t v = 0;
    if (e.rd32(unit) == 1 && e.rd32(unit + 0x14)) v = e.rd32(e.rd32(unit + 0x14) + 0x28);
    return e.h(0x9D230, 0, game, unit, 0, 0, v, 0x10);
}
}
