// 1.13c D2Game+0x6AE60 (D2GAME_6AE60_TriggerNearbyStatesAtUnit): EDI = pUnit (may be null), [esp+4] = p (ret 4), EBP = p inside.
// Written from the disassembly as a rule over an environment E (hookenv.h). 0xA478(unit) gives game (EDI pushed as its argument); 0xA520(unit ? unit[1] : -1) gives rec;
// 0xA544(unit, &tmp); then for each node of game+0x74 (next at +0xE8): type [n+0] == 0 and [n+0x10] != 0x11 calls 0xAB86(unit, n); when the result is
// <= rec+0x178 + 1 (signed) it calls 0xF42D0(p, unit, n, 1).
#pragma once
#include <cstdint>

namespace d2g6ae60 {
template <class E> void Run(E& e, uint32_t unit, uint32_t p)
{
    const uint32_t game = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    const uint32_t rec = e.h(0xA520, 0, 0, 0, 0, 0, unit ? e.rd32(unit + 4) : 0xFFFFFFFFu);
    uint32_t tmp[2] = {};
    e.h(0xA544, 0, 0, 0, 0, 0, unit, e.stk(tmp));
    for (uint32_t n = e.rd32(game + 0x74); n; n = e.rd32(n + 0xE8)) {
        if (e.rd32(n) != 0 || e.rd32(n + 0x10) == 0x11) continue;
        const int32_t r = (int32_t)e.h(0xAB86, 0, 0, 0, 0, 0, unit, n);
        if (r <= (int32_t)(e.rd32(rec + 0x178) + 1)) e.h(0xF42D0, 0, 0, 0, 0, 0, p, unit, n, 1);
    }
}
}  // namespace d2g6ae60
