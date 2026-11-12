// 1.13c D2Client+0x15820 -- skill do: a chained missile. While the target record's counter is positive the handler
// takes the record's unit (by kind and id, else the nearest one the finder answers), points the unit at it, sends a
// request (kind 0x21) toward the resolved point, lowers the counter and, with hops left, finds the next unit near the
// last one and stores it on the path and in the record; a stored hop ends through the two closing routines.
#include "cl_skdo.h"
static uint32_t find(const cl::Env& e, uint32_t u, uint32_t v, uint32_t id) {
    uint32_t X[6] = { 0, 0xffffffffu, 0, id, id, 0 };
    e.Xs(0xB2000000u | 0xA09F0, (uint32_t)(uintptr_t)X);
    e.H(0xa1860, u, 0, 0, v, 0xa283);
    return X[0] ? X[0] : X[2];
}
extern "C" uint32_t d2c_15820(uint32_t u, int32_t sk, uint32_t lvl, const cl::Env& e) {
    const uint32_t r = clsk::row(e, sk);
    if (!r) return 0;
    const uint32_t p = e.H(0xc310, u);
    if (!p) return 0;
    const int32_t n = (int32_t)e.H(0xcb68, p);
    if (n <= 0) return 0;
    const uint32_t v = e.H(0xc316, u, e.R(r + 0x64), (uint32_t)sk, lvl);
    const uint32_t k = e.H(0xcb9e, p);
    const uint32_t g = e.H(0xcb6e, p);
    uint32_t o = e.H(0xa4e20, g, 0, 0, 0, 0, k & 0x7f, k, 0xB2000000u | (0x10A608u + (g << 9)));
    if (!o) o = find(e, u, v, k);
    if (o) e.H(0xc62e, u, o);
    int32_t X = 0, Y = 0;
    e.Ed((uint32_t)(uintptr_t)&X);
    if (!e.H(0xa1340, (uint32_t)(uintptr_t)&Y, 0, 0, 0, 0, 0, 0, 0, u)) return 0;
    int32_t m = e.R16s(r + 0xea);
    if (e.H(0xd012, 0, 0, 0, 0, 0, 0, u) != 1) {
        const int32_t alt = e.R16s(r + 0xec);
        if (alt >= 0) m = alt;
    }
    if (!clsk::mis(e, m)) return 0;
    uint32_t d[23] = {0};
    d[0] = 0x21; d[1] = u;
    d[5] = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
    d[6] = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
    d[11] = (uint32_t)sk; d[12] = lvl; d[8] = (uint32_t)Y; d[7] = (uint32_t)X; d[4] = (uint32_t)m;
    e.HB(0xb30f0, (uint32_t)(uintptr_t)d);
    e.H(0xcb50, p, (uint32_t)(n - 1));
    if (n - 1 > 0) {
        const uint32_t o2 = find(e, u, v, o ? e.R(o + 0xc) : 0xffffffffu);
        if (!o2) return 0;
        const uint32_t y = e.H(0x1660, 0, 0, 0, 0, 0, 0, o2);
        const uint32_t x = e.H(0x1630, 0, 0, 0, 0, 0, 0, o2);
        e.H(0xc478, e.R(u + 0x2c), x, y, 0);
        e.H(0xcb7a, p, e.R(o2));
        e.H(0xcb8c, p, e.R(o2 + 0xc));
        e.H(0x2e90, 0, 0, 0, 0, 0, (uint32_t)sk);
        e.H(0xa05f0);
    }
    return 1;
}
