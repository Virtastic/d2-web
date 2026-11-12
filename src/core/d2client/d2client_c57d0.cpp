// 1.13c D2Client+0xC57D0 -- skill do: a lobbed missile. With a local player the unit's path is pointed at the player,
// otherwise at the resolved target point. The request (kind 0x25) carries the row's missile (or, on a coin flip, its
// alternate), a frame picked by one seed step out of a five-entry module table, and the made missile gets its range,
// speed and arc from the row's two formulas and the missile row.
#include "cl_skdo.h"
extern "C" uint32_t d2c_c57d0(uint32_t u, int32_t sk, uint32_t lvl, const cl::Env& e) {
    const uint32_t r = clsk::row(e, sk);
    if (!r) return 0;
    const int32_t m = e.R16s(r + 0xea);
    if (!clsk::mis(e, m)) return 0;
    const uint32_t mrow = e.R(e.dt + 0xb64) + (uint32_t)m * 0x1a4;
    if (!mrow) return 0;
    int32_t alt = e.R16s(r + 0xec);
    if (!clsk::mis(e, alt)) alt = -1;
    const uint32_t t = e.H(0xa5f70);
    if (t) {
        const uint32_t y = e.H(0x1660, 0, 0, 0, 0, 0, 0, t);
        const uint32_t x = e.H(0x1630, 0, 0, 0, 0, 0, 0, t);
        e.H(0xc3ee, e.R(u + 0x2c), x, y);
        e.H(0xa5fb0, 1, 0, 0, 0, 0, u);
    }
    int32_t X = 0, Y = 0;
    e.Ed((uint32_t)(uintptr_t)&X);
    if (!e.H(0xa1340, (uint32_t)(uintptr_t)&Y, 0, 0, 0, 0, 0, 0, 0, u)) return 0;
    if (!t) e.H(0xc72a, u, (uint32_t)X, (uint32_t)Y);
    uint32_t d[23] = {0};
    d[0] = 0x25; d[1] = u;
    d[5] = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
    d[6] = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
    d[11] = (uint32_t)sk; d[12] = lvl; d[7] = (uint32_t)X; d[8] = (uint32_t)Y;
    uint32_t a = e.H(0xc316, u, e.R(r + 0x114), (uint32_t)sk, lvl);
    a += e.H(0xc7ec0, 0, 0, 0, 0, 0, u);
    const uint32_t rng = e.H(0xc316, u, e.R(r + 0x118), (uint32_t)sk, lvl) + e.R(mrow + 0x3c);
    int32_t q = (int32_t)((uint32_t)e.R8(mrow + 0x178) << 8);
    int32_t arc = (int32_t)((0u - a) << 1);
    if (rng) {
        q = (int32_t)((int64_t)q / (int32_t)rng);
        arc = (int32_t)((int64_t)arc / (int32_t)rng);
    }
    const uint32_t seed = u + 0x20;
    d[4] = (uint32_t)m;
    if (alt > 0 && e.H(0x1330, 0, 0, 0, 0, 0, 2, seed)) d[4] = (uint32_t)alt;
    d[10] = e.R8(mrow + 0x9a) + clc::G(e, 0xD70B8 + (e.Lcg(seed) % 5u) * 4);
    const uint32_t mi = e.HB(0xb30f0, (uint32_t)(uintptr_t)d);
    if (!mi) return 1;
    e.H(0xc3b8, e.R(mi + 0x2c), rng);
    e.H(0xc2fe, mi, (uint32_t)q);
    e.H(0xc4a2, mi, rng);
    e.H(0xc2c8, mi, rng);
    e.H(0xc7e30, 0, 0, 0, 0, 0, mi, a, 0);
    e.H(0xc7fb0, mi);
    const uint32_t pa = e.R(mi + 0x54);
    const uint32_t mo = pa ? e.R(pa + 0x30) : 0;
    if (mo) { e.W(mo + 0x10, 0); e.W(mo + 0x14, 0); e.W(mo + 0x18, (uint32_t)arc << 11); }
    return 1;
}
