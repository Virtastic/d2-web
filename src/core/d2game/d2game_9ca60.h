// Monster flag-and-notify step: EAX = unit, EDI = request {owner, _, done*, _, _, count}, stack = game (ret 4).
// The unit qualifies when it is a type-1 unit whose data byte +0x16 has bit 3, else when #A580(0, unit) is nonzero; a class-0xFA unit (in table
// range) passes either way. Then, with request count below 0x14, *done == 0 and owner byte +8 bit 4 clear: 0xDFFD0(ESI = unit; 0x10, 0), the
// owner bit is set, 0xF22C0(EAX = game, ESI = unit; 0x14), and the result is 1. Anything else gives 0.
#pragma once
#include <cstdint>

namespace g9ca60 {
template <class E> uint32_t Step(E& e, uint32_t unit, uint32_t req, uint32_t game, int32_t classCount)
{
    // 1.13c D2Game+0x9CA60
    uint32_t data;
    const bool ok = (unit && e.rd32(unit) == 1 && (data = e.rd32(unit + 0x14)) && (e.rd8(data + 0x16) & 8))
                    || e.h(0xA580, 0, 0, 0, 0, 0, 0, unit) != 0;
    const int32_t cls = unit ? (int32_t)e.rd32(unit + 4) : -1;
    const bool special = unit && cls >= 0 && cls < classCount && cls == 0xFA;
    if (!special && !ok) return 0;
    if (e.rd32(req + 0x14) >= 0x14) return 0;
    const uint32_t done = e.rd32(req + 8);
    if (!done || e.rd32(done)) return 0;
    const uint32_t owner = e.rd32(req);
    if (e.rd8(owner + 8) & 0x10) return 0;
    e.h(0xDFFD0, 0, 0, 0, 0, unit, 0x10, 0);
    e.wr8(owner + 8, e.rd8(owner + 8) | 0x10);
    e.h(0xF22C0, game, 0, 0, 0, unit, 0x14);
    return 1;
}
}
