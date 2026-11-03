// 1.13c D2Game+0x6C900 (slow-damage field: scale the life of the units in a circle, then a 4 x 4 ring of missiles): ecx = ctx {+0 game, +4 unit, +8 unit2}, edx = env {+4 percent, +8 radius}, no result.
// Written from the 1.13c disassembly as a rule over an environment E: every callee that is not a field read is one E::h(rva, ...) call.
// room = 0xA478(unit); (x, y) = the unit's position (types 2, 4, 5: words 3/4 of the object at unit+0x2C; other types 0xA454/0xA3B8 of it, 0 when null).
// 0xA502(game+0x1C, &list, room, x, y, radius, callback 0x69C60, &req) fills the 7-word list ({+4 array, +0x18 count}); the 14-word req holds {0x583, 0, 0, x, y, radius}; 0xA4F0(&list).
// Each non-null array unit of type 0 (mode not 0, not 0x11) or type 1 (mode not 0, not 0xC) whose +0xC6 bit 0 is clear gets stat 6 = ((percent * (0xA42A(unit, 6, 0) >> 8)) / -100 style
// magic, 0xAE147AE1, sar 5, plus the sign bit) << 8 through 0xA4D2(unit, 6, value, 0). 0xA4FC(&list). Then i, j = 0..3: a 0x17-word request {3, unit2, -, -, 0x3E, x, y,
// i even ? 5(i+1) : -5(i+1), j even ? 5(j+1) : -5(j+1), ..., word 12 = 0xA42A(unit2, 0xC, 0)/5 clamped 1..8} to 0x6F930 (ecx game, edx request).
#pragma once
#include <cstdint>

namespace r6c900 {
template <class E> void Run(E& e, uint32_t ctx, uint32_t env)
{
    const uint32_t game = e.rd32(ctx);
    auto pos = [&](uint32_t& x, uint32_t& y) {
        const uint32_t un = e.rd32(ctx + 4), obj = e.rd32(un + 0x2C);
        const int32_t t = (int32_t)e.rd32(un);
        const bool direct = t == 2 || (t > 3 && t <= 5);
        x = direct ? e.rd32(obj + 0xC) : (obj ? e.h(0xA454, 0, 0, 0, 0, 0, obj) : 0);
        const uint32_t un2 = e.rd32(ctx + 4), obj2 = e.rd32(un2 + 0x2C);
        const int32_t t2 = (int32_t)e.rd32(un2);
        const bool direct2 = t2 == 2 || (t2 > 3 && t2 <= 5);
        y = direct2 ? e.rd32(obj2 + 0x10) : (obj2 ? e.h(0xA3B8, 0, 0, 0, 0, 0, obj2) : 0);
    };
    const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, e.rd32(ctx + 4));
    uint32_t x, y;
    pos(x, y);
    uint32_t req1[14] = {0}, list[7] = {0};
    const uint32_t radius = e.rd32(env + 8);
    req1[4] = y; req1[5] = radius; req1[0] = 0x583; req1[3] = x;
    e.h(0xA502, 0, 0, 0, 0, 0, e.rd32(game + 0x1C), e.stk(list), room, x, y, radius, 0x6FC89C60u /* arg 8 is &req1: beyond the 10 logged words */);
    e.h(0xA4F0, 0, 0, 0, 0, 0, e.stk(list));
    const int32_t count = (int32_t)list[6];
    for (int32_t i = 0; i < count; i++) {
        const uint32_t un = e.rd32(list[1] + 4 * i);
        if (!un) continue;
        const uint32_t t = e.rd32(un);
        if (t != 0 && t != 1) continue;
        if (e.rd8(un + 0xC6) & 1) continue;
        const uint32_t mode = e.rd32(un + 0x10);
        if (mode == 0 || mode == (t ? 0xCu : 0x11u)) continue;
        const int32_t lv = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, un, 6, 0);
        const int32_t p = (int32_t)(e.rd32(env + 4) * (uint32_t)(lv >> 8));
        const int32_t hi = (int32_t)(((int64_t)p * (int64_t)(int32_t)0xAE147AE1u) >> 32);
        const int32_t s = hi >> 5;
        e.h(0xA4D2, 0, 0, 0, 0, 0, un, 6, (uint32_t)(s + (int32_t)((uint32_t)s >> 31)) << 8, 0);
    }
    e.h(0xA4FC, 0, 0, 0, 0, 0, e.stk(list));
    for (int32_t i = 0; i < 4; i++) for (int32_t j = 0; j < 4; j++) {
        uint32_t req[0x17] = {0};
        req[0] = 3;
        pos(req[5], req[6]);
        req[7] = (uint32_t)((i & 1) ? -5 * (i + 1) : 5 * (i + 1));
        req[8] = (uint32_t)((j & 1) ? -5 * (j + 1) : 5 * (j + 1));
        req[1] = e.rd32(ctx + 8); req[4] = 0x3E;
        const int32_t l5 = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, e.rd32(ctx + 8), 0xC, 0) / 5;
        req[12] = l5 < 1 ? 1u : l5 > 8 ? 8u : (uint32_t)l5;
        e.h(0x6F930, 0, game, e.stk(req), 0, 0);
        e.blk(0x6F930, req, 23);
    }
}
}
