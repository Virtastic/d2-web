// 1.13c D2Game+0x6CF90 (ObjectOperateWithItemSound): ecx = think context {+0 game, +4 unit, +8 owner, +0x10 monster id}, plain ret, returns 1.
// Rule: id = 0xA556(0xA478(unit)); P = 0xA4C0(owner); X = P[game byte 0x6D * 4 + 0x1C]; when 0xAB02(id, &local) is non-zero, 0xAC1C(X, local).
// A null unit or one with mode 0: mode 1 (0xA4A2), the record of the monster id (0xA520) and a mode-1 frame send (0x8EA80), return 1.
// A unit in mode 1 or 2 whose owner is null, has +0 != 0, or (+0x6C == 0, 0xA6A6(owner+0x60) == 0 and 0xA4C0(owner)[0x4C] == 0) sends a 5-byte packet
// {0x63, unit id}: 0xA4B4(X, packet + 5), 0xAA70 (EAX = packet; stack the client's 0xA4C0(owner)[0x9C] when owner[0] == 0, else 0), then 0xDFAE0 (EAX = owner; 2, unit id or -1).
#pragma once
#include <cstdint>

namespace r6cf90 {
template <class E> uint32_t Operate(E& e, uint32_t ctx)
{
    const uint32_t game = e.rd32(ctx);
    const uint32_t id = e.h(0xA556, 0, 0, 0, 0, 0, e.h(0xA478, 0, 0, 0, 0, 0, e.rd32(ctx + 4)));
    const uint32_t P = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(ctx + 8));
    const uint32_t X = e.rd32(P + e.rd8(game + 0x6D) * 4 + 0x1C);
    uint32_t local = 0;
    if (e.h(0xAB02, 0, 0, 0, 0, 0, id, e.stk(&local))) e.h(0xAC1C, 0, 0, 0, 0, 0, X, local);
    uint32_t unit = e.rd32(ctx + 4);
    if (!unit || e.rd32(unit + 0x10) == 0) {
        e.h(0xA4A2, 0, 0, 0, 0, 0, unit, 1);
        const uint32_t rec = e.h(0xA520, 0, 0, 0, 0, 0, e.rd32(ctx + 0x10));
        e.h(0x8EA80, 0, e.rd32(ctx), e.rd32(ctx + 4), 0, 0, 1, (e.rd32(rec + 0xDC) >> 8) + 1 + e.rd32(e.rd32(ctx) + 0xA8), 0, 0, 0);
        return 1;
    }
    const uint32_t mode = e.rd32(unit + 0x10);
    if (mode != 2 && mode != 1) return 1;
    uint32_t owner = e.rd32(ctx + 8);
    if (owner && e.rd32(owner) == 0) {
        if (e.rd8(owner + 0x6C)) return 1;
        if (e.h(0xA6A6, 0, 0, 0, 0, 0, e.rd32(owner + 0x60))) return 1;
        if (e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, owner) + 0x4C)) return 1;
    }
    unit = e.rd32(ctx + 4);
    const uint32_t uid = unit ? e.rd32(unit + 0xC) : 0xFFFFFFFFu;
    uint32_t b[2] = { 0x63u | (uid << 8), uid >> 24 };
    e.h(0xA4B4, 0, 0, 0, 0, 0, X, e.stk(b) + 5);
    owner = e.rd32(ctx + 8);
    uint32_t cl = 0;
    if (owner && e.rd32(owner) == 0) cl = e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, owner) + 0x9C);
    e.h(0x8AA70, e.stk(b), 0, 0, 0, 0, cl);
    e.blk(0x8AA70, b, 2, 3);
    unit = e.rd32(ctx + 4);
    e.h(0xDFAE0, e.rd32(ctx + 8), 0, 0, 0, 0, 2, unit ? e.rd32(unit + 0xC) : 0xFFFFFFFFu);
    return 1;
}
}
