// 1.13c D2Client+0x15660 -- skill do: a missile from the unit toward a unit found near the local player. The owner
// slot must be a missile id and the local player must exist. The finder gets the player's position, the row's range
// formula and a filter seeded with the player's id; its first answer, else its third, is the target. A request
// (kind 0x20) is sent at the target's position and the made missile gets the row's second formula value.
#include "cl_skdo.h"
extern "C" uint32_t d2c_15660(uint32_t u, int32_t sk, uint32_t lvl, const cl::Env& e) {
    const uint32_t r = clsk::row(e, sk);
    if (!r) return 0;
    const int32_t a = (int32_t)e.H(0x40550, u, 0, 0, 0, 0, (uint32_t)sk);
    if (!clsk::mis(e, a)) return 0;
    const uint32_t t = e.H(0xa5f70);
    if (!t) return 0;
    const uint32_t v1 = e.H(0xc316, u, e.R(r + 0x138), (uint32_t)sk, lvl);
    const uint32_t id = e.R(t + 0xc);
    const uint32_t y = e.H(0x1660, 0, 0, 0, 0, 0, 0, t);
    const uint32_t x = e.H(0x1630, 0, 0, 0, 0, 0, 0, t);
    uint32_t X[6] = { 0, 0xffffffffu, 0, id, id, 0 };
    e.Xs(0xB2000000u | 0xA09F0, (uint32_t)(uintptr_t)X);
    e.H(0xa1860, u, x, y, v1, 0xa283);
    uint32_t o = X[0];
    if (!o) o = X[2];
    if (!o) return 0;
    const uint32_t v2 = e.H(0xc316, u, e.R(r + 0x13c), (uint32_t)sk, lvl);
    uint32_t d[23] = {0};
    d[0] = 0x20; d[1] = u; d[2] = t; d[11] = (uint32_t)sk; d[12] = lvl; d[4] = (uint32_t)a;
    d[7] = e.H(0x1630, 0, 0, 0, 0, 0, 0, o);
    d[8] = e.H(0x1660, 0, 0, 0, 0, 0, 0, o);
    const uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)d);
    if (m) e.H(0xc448, m, v2);
    return 1;
}
