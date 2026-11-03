// 1.13c D2Game+0x158F0 (AI: remember the closest tower): ESI = unit, stack = candidate unit (ret 4).
// Rule: the unit's AI block ai = unit pData(+0x14) +0x28 when it is a monster (type 1) with a pData, else 0. A null candidate
// stores -1 at ai+0x14. Otherwise the game (unit+0x80) is walked: bucket game+0x1320+4*(ai[0x14]&0x7F), chain link +0xE4, key +0xC.
// No bucket, or the stored id not in it: ai[0x14] = candidate+0xC. Found: 0x1900 (EAX = unit, EBX = the found unit, distance) and
// 0x1900 (EAX = unit, EBX = candidate); the candidate is stored unless it is the farther one (second > first).
// The fatal paths (null unit with a candidate, null game) report through 0xA2DA/0xA2C2 and _exit(-1): not vectored.
#pragma once
#include <cstdint>

namespace g158f0 {
template <class E> void SetClosestTower(E& e, uint32_t unit, uint32_t cand)
{
    // 1.13c D2Game+0x158F0
    uint32_t ai = 0;
    if (unit && e.rd32(unit) == 1 && e.rd32(unit + 0x14)) ai = e.rd32(e.rd32(unit + 0x14) + 0x28);
    if (!cand) { e.wr32(ai + 0x14, 0xFFFFFFFFu); return; }
    if (!unit) return;
    const uint32_t game = e.rd32(unit + 0x80);
    if (!game) return;
    const uint32_t id = e.rd32(ai + 0x14);
    uint32_t n = e.rd32(game + 0x1320 + (id & 0x7F) * 4);
    if (n) {
        while (e.rd32(n + 0xC) != id) {
            n = e.rd32(n + 0xE4);
            if (!n) { e.wr32(ai + 0x14, e.rd32(cand + 0xC)); return; }
        }
        const int32_t d1 = (int32_t)e.h(0x1900, unit, 0, 0, n);
        const int32_t d2 = (int32_t)e.h(0x1900, unit, 0, 0, cand);
        if (d2 > d1) return;
    }
    e.wr32(ai + 0x14, e.rd32(cand + 0xC));
}
}
