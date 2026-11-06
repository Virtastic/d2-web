// 1.13c D2Game+0x6B280 (D2GAME_6B280, ObjectOperateStandUp): fastcall, ECX = ctx, ret 0. Written from the 1.13c disassembly
// as a rule over an environment E (see hookenv.h): every callee is one E::h(rva, ...) call with the words the caller pushes.
// Rule: ctx[0] = pGame, ctx[1] = pUnit. pUnit null or pUnit+0x10 null -> 0xA544(pUnit, &tmp) [ret 8], 0xA478(pUnit,
// &out, -1, 0, 0) [5 words], sub_11E20 with ESI = pGame and one stack word (the 0xA478 result), 0xA4A2(pUnit, 2) [ret 8],
// then pUnit+0xC4 loses bit 2. Always returns 1.
#pragma once
#include <cstdint>

namespace d2g6b280 {
template <class E> uint32_t Run(E& e, uint32_t ctx)
{
    const uint32_t game = e.rd32(ctx);
    const uint32_t unit = e.rd32(ctx + 4);
    if (unit == 0 || e.rd32(unit + 0x10) == 0) {
        uint32_t tmp[2] = {};
        uint32_t out[2] = {};
        e.h(0xA544, 0, 0, 0, 0, 0, unit, e.stk(tmp), 0);
        const uint32_t r = e.h(0xA478, 0, 0, 0, 0, 0, unit, e.stk(out), 0xFFFFFFFFu, 0, 0);
        e.h(0x11E20, 0, 0, 0, 0, game, r);
        e.h(0xA4A2, 0, 0, 0, 0, 0, unit, 2);
        if (unit) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) & ~2u);
    }
    return 1;
}
}  // namespace d2g6b280
