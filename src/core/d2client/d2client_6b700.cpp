// 1.13c D2Client+0x6B700, written from the disassembly. EAX = unit, plain ret, returns 0 or 1.
// flag = 0xD07E(). A null unit or a type outside 0..2 hands over to 0x6A860(EAX=unit) and returns 1. pp = [[unit+0x54]+0x30] (each field 0
// when absent). Flag set: 0xD084(0xC1AE(unit, 0xC232(unit, pp+0x3C, &X, &Y))), Gcc414 == 1 / 2 subtracts / adds G8bc48 / 4 to X, then
// X += pp+0x34, Y += 0x10 + pp+0x38. Otherwise X = 0xC1D2(unit) + pp+0x34 - (G9960 - Gcc418), Y = 0xC1F6(unit) + pp+0x38 - (G995c - 8) + pp+0x3C.
// 0x6A3D0(EAX=unit; X, Y, 0) zero returns 0. lo = 0x68CF0() (a byte in an otherwise unwritten slot); 0xC65E(unit) presets x and 0xC658(unit,
// &type, &class, &x, "D2Client.cpp", 0xA2F) fills them; b = 0xC610(unit) & 0xFF; 0xC5EC(unit, &o1, &o2). The weapon class wc =
// 0xC2AA(unit, w60, &local, x, 0) (w60 as in 0x6AC60); 0x69910(EAX=unit, ECX=-1) and 0x6A2F0(EAX=-1, ECX=wc, EDX=unit) both zero return 0.
// tbl = 0x69090(EAX=unit, ECX=x) (null returns 0); 0x6DCA0(unit, type, class, x, &b, &k, &s34, tbl[2], 1); row = 0x6D6D0(EAX=tbl; k, o1)
// (null: fatal 0xA56); k = 0, then 0x6EA50(EBX=unit; tbl, row, &k, b, o1) <= 0 returns 0. For each of tbl[0] bytes c with
// 0x6E670(EAX=unit; c) nonzero, a zeroed 0x48-byte frame {+0 = o1, +5 = c, +0x40 = b} goes to 0x6E2C0(EAX=-1; unit, &frame, &local) and
// on success 0xD126(&frame, X, Y, lo). Returns 1.
#include <cstring>
#include "cl_env.h"
extern "C" uint32_t d2c_6b700(uint32_t unit, uint32_t gA, uint32_t gB, uint32_t gC, const cl::Env& e) {
    const auto P = [](const void* p) { return (uint32_t)(uintptr_t)p; };
    const uint32_t flag = e.H(0xd07e);
    if (!unit || (int32_t)e.R(unit) < 0 || (int32_t)e.R(unit) > 2) { e.H(0x6a860, 0, 0, 0, 0, 0, unit); return 1; }
    const uint32_t pa = e.R(unit + 0x54), pp = pa ? e.R(pa + 0x30) : 0;
    const auto F = [&](uint32_t off) { return pp ? e.R(pp + off) : 0u; };
    uint32_t X, Y;
    if (flag) {
        uint32_t l10 = 0, l14 = 0;
        const uint32_t r = e.H(0xc232, unit, F(0x3c), P(&l10), P(&l14));
        e.H(0xd084, e.H(0xc1ae, unit, r));
        const uint32_t m = e.R(gB);
        const int32_t q = (int32_t)e.R(gC) / 4;
        if (m == 1) l10 -= (uint32_t)q;
        else if (m == 2) l10 += (uint32_t)q;
        X = l10 + F(0x34);
        Y = l14 + 0x10 + F(0x38);
    } else {
        const uint32_t p34 = F(0x34), s = e.R(gA + 4) - e.R(gB + 4);
        X = e.H(0xc1d2, unit) + (p34 - s);
        const uint32_t p38 = F(0x38), t = e.R(gA) - 8;
        Y = e.H(0xc1f6, unit) + (p38 - t);
        Y += F(0x3c);
    }
    if (!e.H(0x6a3d0, X, Y, 0, 0, 0, unit)) return 0;
    const uint32_t lo = e.H(0x68cf0) & 0xff;
    uint32_t type = e.R(unit), cls = e.R(unit + 4), x = e.H(0xc65e, unit);
    e.Xs(0xa2f);
    e.H(0xc658, unit, P(&type), P(&cls), P(&x), 0xB20D4FA4u);
    uint32_t b = e.H(0xc610, unit) & 0xff;
    uint32_t o1 = 0, o2 = 0;
    e.H(0xc5ec, unit, P(&o1), P(&o2));
    uint32_t w60;
    if ((e.R(unit + 0xc8) >> 3 & 1) && e.H(0xcbda, unit, 0x25)) {
        const uint32_t q = e.H(0xa60d0, 0, 0, 0, 0, 0, unit);
        w60 = q ? e.R(q + 0x60) : e.R(unit + 0x60);
    } else {
        w60 = e.R(unit + 0x60);
    }
    uint32_t l40 = 0;
    const uint32_t wc = e.H(0xc2aa, unit, w60, P(&l40), x, 0);
    if (!e.H(0x69910, 0, 0, 0, 0, 0, unit, 0xffffffffu) && !e.H(0x6a2f0, 0, 0, 0, 0, 0, 0xffffffffu, wc, unit)) return 0;
    const uint32_t tbl = e.H(0x69090, 0, 0, 0, 0, 0, unit, x);
    if (!tbl) return 0;
    uint32_t k = 0, s34 = e.R(unit + 0xc8) >> 3 & 1;
    {
        uint32_t a[9] = { unit, type, cls, x, P(&b), P(&k), P(&s34), e.R8(tbl + 2), 1 };
        e.Xs(a[5], a[6]);
        e.H(0x6dca0, a[0], a[1], a[2], a[3], a[4]);
        if (e.blk) e.blk(e.ctx, 0x6dca0, P(a), 9);
    }
    uint32_t row = e.H(0x6d6d0, k, o1, 0, 0, 0, tbl);
    if (!row) { e.H(0xbf80, 0xB20D359Fu, e.H(0xbf92, 0xa56)); row = e.H(0x3664, 0xffffffffu); }
    k = 0;
    {
        uint32_t a[5] = { tbl, row, P(&k), b, o1 };
        const uint32_t n = e.H(0x6ea50, a[0], a[1], a[2], a[3], a[4], 0, 0, 0, unit);
        if (e.blk) e.blk(e.ctx, 0x6ea50, P(a), 5);
        if ((int32_t)n <= 0) return 0;
    }
    for (uint32_t i = 0; i < e.R8(tbl); ++i) {
        const uint32_t c = e.R8(row + i);
        if (!e.H(0x6e670, c, 0, 0, 0, 0, unit)) continue;
        uint32_t fr[18] = {}, snap[18], l44 = 0;
        fr[0x34 / 4] = 0;
        fr[0x40 / 4] = b;
        fr[0] = o1;
        ((uint8_t*)fr)[5] = (uint8_t)c;
        memcpy(snap, fr, sizeof fr);
        const uint32_t ok = e.H(0x6e2c0, unit, P(fr), P(&l44), 0, 0, 0xffffffffu);
        if (e.blk) e.blk(e.ctx, 0x6e2c0, P(snap), 18);
        if (!ok) continue;
        memcpy(snap, fr, sizeof fr);
        e.H(0xd126, P(fr), X, Y, lo);
        if (e.blk) e.blk(e.ctx, 0xd126, P(snap), 18);
    }
    return 1;
}
