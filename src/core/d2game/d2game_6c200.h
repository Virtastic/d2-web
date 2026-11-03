// 1.13c D2Game+0x6C200 (spawn a count of pickup missiles around a unit, then six ring missiles): ecx = ctx {+0 game, +4 unit, +8 unit2}, edx = range {+4 min, +8 max}, no result.
// Written from the 1.13c disassembly as a rule over an environment E: every callee that is not a field read is one E::h(rva, ...) call.
// count = min + rand(max - min) with the unit's own LCG (seed pair at unit+0x20, multiplier 0x6AC690C5; a power-of-two span masks, any other takes the low word modulo).
// Each of count rounds: 0xAE74(cls) (-1 is the fatal 0x8F4, not driven), 0xA544(unit2, &pos), 0xA478(unit2); the in-point is pos+(2,3) when 0x1280 answers nonzero, else pos;
// 0xAE98(room, &in, &pos, 1, 0x3E01, 0x801, 1) answers r (zero skips the round). Otherwise a 0x21-word request block (game, level, class, 3, in.x, in.y, r, hword 1 |
// game+0x78 hword, word 21 = 1) with level = 1 for a null unit2, 0xA42A (type 1), 0xA3E2 (type 0) or 0xADA8(0xA556(0xA478)), kept only when above 1; 0x11490(game, req, 0)
// makes the missile m: 0xA472(m, 0x46, 1, 0), 0xA4DE(m), m+0xC4 |= 1 when m is nonnull.
// Then six rings, offsets x -6 -6 0 0 6 6, y 6 -6 6 -6 6 -6: a 0x17-word request {0x520, unit2, unit, -, code, ux, uy, ux+dx, uy+dy, 1, ..., level/5 clamped 1..8 at word 12}
// where (ux, uy) is the unit's position (types 2, 4, 5: through unit+0x2C words 3/4, other types 0xA454/0xA3B8 of that object or 0 when null), level = 0xA42A(unit2, 0xC, 0);
// 0x6F930 (ecx game, edx request). 0x6C580 is this body with other constants.
#pragma once
#include <cstdint>

namespace r6c200 {
template <class E> void Run(E& e, uint32_t ctx, uint32_t range, uint32_t clsId = 0x206D7067u, uint32_t code = 0x30)
{
    const uint32_t game = e.rd32(ctx), unit = e.rd32(ctx + 4), u2 = e.rd32(ctx + 8);
    const int32_t span = (int32_t)(e.rd32(range + 8) - e.rd32(range + 4));
    uint32_t r = 0;
    if (span > 0) {
        const uint32_t sp = unit + 0x20;
        const uint64_t x = (uint64_t)e.rd32(sp) * 0x6AC690C5u + e.rd32(sp + 4);
        e.wr32(sp, (uint32_t)x); e.wr32(sp + 4, (uint32_t)(x >> 32));
        r = ((span & (span - 1)) == 0) ? ((uint32_t)(span - 1) & (uint32_t)x) : ((uint32_t)x % (uint32_t)span);
    }
    const int32_t count = (int32_t)(e.rd32(range + 4) + r);
    for (int32_t i = 0; i < count; i++) {
        const uint32_t c = e.h(0xAE74, 0, 0, 0, 0, 0, clsId);
        if (c == 0xFFFFFFFFu) __builtin_trap();         // retail: Fog 10265/10024 then _exit(-1)
        uint32_t pos[2] = {0, 0}, in[2];
        e.h(0xA544, 0, 0, 0, 0, 0, e.rd32(ctx + 8), e.stk(pos));
        const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, e.rd32(ctx + 8));
        const bool shifted = e.h(0x1280, 0, 0, 0, 0, 0) != 0;
        in[0] = pos[0] + (shifted ? 2 : 0); in[1] = pos[1] + (shifted ? 3 : 0);
        const uint32_t args[7] = {room, e.stk(in), e.stk(pos), 1, 0x3E01, 0x801, 1};
        const uint32_t ok = e.h(0xAE98, 0, 0, 0, 0, 0, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
        if (!ok) continue;
        uint32_t req[0x21] = {0};
        req[2] = game; req[5] = c; req[6] = 3; req[7] = in[0]; req[8] = in[1]; req[9] = ok;
        req[10] = 1 | ((e.rd32(game + 0x78) & 0xFFFF) << 16); req[21] = 1;
        uint32_t lv = 1;
        if (u2) {
            const uint32_t t = e.rd32(u2);
            if (t == 1) lv = e.h(0xA42A, 0, 0, 0, 0, 0, u2, 0xC, 0);
            else if (t == 0) lv = e.h(0xA3E2, 0, 0, 0, 0, 0, u2, 0xC, 0);
            else {
                const uint32_t act = e.h(0xA556, 0, 0, 0, 0, 0, e.h(0xA478, 0, 0, 0, 0, 0, u2));
                const uint32_t d = e.rd32(u2 + 0x80);
                lv = e.h(0xADA8, 0, 0, 0, 0, 0, act, e.rd8(d + 0x6D), e.rd32(d + 0x70));
            }
            if ((int32_t)lv <= 1) lv = 1;
        }
        req[3] = lv;
        const uint32_t m = e.h(0x11490, 0, 0, 0, 0, 0, game, e.stk(req), 0);
        e.blk(0x11490, req, 0x21);
        e.h(0xA472, 0, 0, 0, 0, 0, m, 0x46, 1, 0);
        e.h(0xA4DE, 0, 0, 0, 0, 0, m);
        if (m) e.wr32(m + 0xC4, e.rd32(m + 0xC4) | 1);
    }
    static const int32_t dx[6] = {-6, -6, 0, 0, 6, 6}, dy[6] = {6, -6, 6, -6, 6, -6};
    for (int k = 0; k < 6; k++) {
        uint32_t req[0x17] = {0};
        req[0] = 0x520;
        const uint32_t un = e.rd32(ctx + 4), obj = e.rd32(un + 0x2C);
        const int32_t t = (int32_t)e.rd32(un);
        const bool direct = t == 2 || (t > 3 && t <= 5);
        const uint32_t ux = direct ? e.rd32(obj + 0xC) : (obj ? e.h(0xA454, 0, 0, 0, 0, 0, obj) : 0);
        const uint32_t un2 = e.rd32(ctx + 4), obj2 = e.rd32(un2 + 0x2C);
        const int32_t t2 = (int32_t)e.rd32(un2);
        const bool direct2 = t2 == 2 || (t2 > 3 && t2 <= 5);
        const uint32_t uy = direct2 ? e.rd32(obj2 + 0x10) : (obj2 ? e.h(0xA3B8, 0, 0, 0, 0, 0, obj2) : 0);
        req[5] = ux; req[6] = uy; req[7] = ux + (uint32_t)dx[k]; req[8] = uy + (uint32_t)dy[k];
        req[1] = e.rd32(ctx + 8); req[2] = e.rd32(ctx + 4); req[9] = 1; req[4] = code;
        int32_t lv = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, e.rd32(ctx + 8), 0xC, 0) / 5;
        req[12] = lv < 1 ? 1u : lv > 8 ? 8u : (uint32_t)lv;
        e.h(0x6F930, 0, game, e.stk(req), 0, 0);
        e.blk(0x6F930, req, 23);
    }
}
}
