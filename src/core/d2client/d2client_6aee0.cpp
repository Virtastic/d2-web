// 1.13c D2Client+0x6AEE0, written from the disassembly. stdcall 6: unit, p12 (draw arg), pass, x, y, lift; no result.
// A null unit or link (+0x54) is fatal 0x271. flag = 0xD07E(); the node chain [[unit+0x54]+0x2c] (next +0xA4) must be non-empty.
// With the flag set (and a type-4 unit only when 0xC65E(unit) == 3): 0xD084(0xC1AE(unit, 0xC232(unit, 0, &X, &Y))), then
// Gcc414 == 1 subtracts and == 2 adds G8bc48 / 4 (toward zero) to X, and Y += 0x10. Otherwise X = 0xC1D2(unit) - (G9960 - Gcc418),
// Y = 0xC1F6(unit) - (G995c - 8), and a type-4 unit with x and y both != -1 takes them. The link's +0x30 record adds +0x34 to X and
// +0x38, +0x3C to Y; lift subtracts 8 from Y. A node draws when its +0x34 test matches pass, kind 6 has +8, kind 8 has +0x3C:
// the Data row (*sgptDataTables +0xBBC, count +0xBC0, stride 0x84; out of range or null: fatal 0x2D1) and a 0x48-byte frame
// {+0 = node+0x18>>8, +5 = 1, +0x14 = id, +0x40 = 0xC610(unit) & 0xFF} go to 0x6E2C0(EAX=-1; 0, &frame, &local) (zero returns);
// then, when node+0x18>>8 is below node+0x1C>>8, 0xD096(&frame, node+0x20+X, node+0x24+Y, p12, row[0x7C], 0).
#include <cstring>
#include "cl_env.h"
extern "C" void d2c_6aee0(uint32_t unit, uint32_t p12, uint32_t pass, uint32_t px, uint32_t py, uint32_t lift,
                          uint32_t gA, uint32_t gB, uint32_t gC, const cl::Env& e) {
    const auto P = [](const void* p) { return (uint32_t)(uintptr_t)p; };
    const auto fatal = [&](uint32_t line) { e.H(0xbf80, 0xB20D359Fu, e.H(0xbf92, line)); e.H(0x3664, 0xffffffffu); };
    if (!unit || !e.R(unit + 0x54)) { fatal(0x271); return; }
    const uint32_t flag = e.H(0xd07e);
    uint32_t n = e.R(e.R(unit + 0x54) + 0x2c);
    if (!n) return;
    uint32_t X, Y;
    if (flag && (e.R(unit) != 4 || e.H(0xc65e, unit) == 3)) {
        uint32_t l18 = 0, l20 = 0;
        const uint32_t r = e.H(0xc232, unit, 0, P(&l18), P(&l20));
        e.H(0xd084, e.H(0xc1ae, unit, r));
        const uint32_t m = e.R(gB);
        const int32_t q = (int32_t)e.R(gC) / 4;
        if (m == 1) l18 -= (uint32_t)q;
        else if (m == 2) l18 += (uint32_t)q;
        X = l18; Y = l20 + 0x10;
    } else {
        const uint32_t s = e.R(gA + 4) - e.R(gB + 4);
        X = e.H(0xc1d2, unit) - s;
        const uint32_t t = e.R(gA) - 8;
        Y = e.H(0xc1f6, unit) - t;
        if (e.R(unit) == 4 && px != 0xffffffffu && py != 0xffffffffu) { X = px; Y = py; }
    }
    const uint32_t pa = e.R(unit + 0x54), pp = pa ? e.R(pa + 0x30) : 0;
    X += pp ? e.R(pp + 0x34) : 0;
    Y += pp ? e.R(pp + 0x38) : 0;
    Y += pp ? e.R(pp + 0x3c) : 0;
    if (lift) Y -= 8;
    do {
        const uint32_t n34 = e.R(n + 0x34);
        uint32_t f = ((n34 != 0) == (pass != 0)) ? 1 : 0;
        const uint32_t kind = e.R(n);
        if (kind == 6 && !e.R(n + 8)) f = 0;
        if (!(kind == 8 && !e.R(n + 0x3c)) && f) {
            const int32_t hi = (int32_t)e.R(n + 0x18) >> 8;
            const uint32_t id = e.R(n + 4);
            if ((int32_t)id < 0 || (int32_t)id >= (int32_t)e.R(e.dt + 0xbc0)) { fatal(0x2d1); return; }
            const uint32_t row = e.R(e.dt + 0xbbc) + id * 0x84u;
            if (!row) { fatal(0x2d1); return; }
            uint32_t fr[18] = {}, snap[18], loc = 0;
            fr[0x14 / 4] = id;
            fr[0x40 / 4] = e.H(0xc610, unit) & 0xff;
            fr[0x34 / 4] = 0;
            fr[0] = (uint32_t)hi;
            ((uint8_t*)fr)[5] = 1;
            memcpy(snap, fr, sizeof fr);
            const uint32_t ok = e.H(0x6e2c0, 0, P(fr), P(&loc), 0, 0, 0xffffffffu);
            if (e.blk) e.blk(e.ctx, 0x6e2c0, P(snap), 18);
            if (!ok) return;
            if (hi < ((int32_t)e.R(n + 0x1c) >> 8)) {
                memcpy(snap, fr, sizeof fr);
                e.Xs(0, 0);
                e.H(0xd096, P(fr), e.R(n + 0x20) + X, e.R(n + 0x24) + Y, p12, e.R8(row + 0x7c));
                if (e.blk) e.blk(e.ctx, 0xd096, P(snap), 18);
            }
        }
        n = e.R(n + 0xa4);
    } while (n);
}
