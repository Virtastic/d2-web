// 1.13c D2Game+0x6AD60 (IterateNearbyQuestTargets): EDI = ctx {game, unit, w2, w3, w4}, plain ret, returns 0.
// Written from the disassembly as a rule over an environment E (hookenv.h). The five ctx words are copied to a local block; 0xA544(unit, &tmp);
// g = 0xA478(unit); walk g+0x74 (next at +0xE8), skipping the unit itself and type 5:
//   type 0 (data [n+0x10] != 0x11) or type 1 (data != 0xC): 0xB180(n, &tmp, 3) != 0 calls 0xF42D0(ctx[0], ctx[1], n, 0)
//   type 2 with [n+4] == 0xB, [n+0x10] == 0: 0xAB86(unit, n) <= 2 (signed) sets block[1] = n and calls 0x6D740 (ECX = the local block).
#pragma once
#include <cstdint>

namespace d2g6ad60 {
template <class E> void Run(E& e, uint32_t ctx)
{
    uint32_t blk[5];
    for (int i = 0; i < 5; i++) blk[i] = e.rd32(ctx + 4 * i);
    const uint32_t unit = e.rd32(ctx + 4);
    uint32_t tmp[2] = {};
    e.h(0xA544, 0, 0, 0, 0, 0, unit, e.stk(tmp));
    const uint32_t g = e.h(0xA478, 0, 0, 0, 0, 0, e.rd32(ctx + 4));
    for (uint32_t n = e.rd32(g + 0x74); n; n = e.rd32(n + 0xE8)) {
        const uint32_t u = e.rd32(ctx + 4), t = e.rd32(n), d = e.rd32(n + 0x10);
        if (n == u || t == 5) continue;
        if (t == 0 || t == 1) {
            if (d == (t ? 0xCu : 0x11u)) continue;
            if (e.h(0xB180, 0, 0, 0, 0, 0, n, e.stk(tmp), 3))
                e.h(0xF42D0, 0, 0, 0, 0, 0, e.rd32(ctx), e.rd32(ctx + 4), n, 0);
        } else if (t == 2 && e.rd32(n + 4) == 0xB && d == 0) {
            if ((int32_t)e.h(0xAB86, 0, 0, 0, 0, 0, u, n) > 2) continue;
            blk[1] = n;
            e.h(0x6D740, 0, e.stk(blk));
            e.blk(0x6D740, blk, 5);
        }
    }
}
}  // namespace d2g6ad60
