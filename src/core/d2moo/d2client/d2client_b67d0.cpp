// 1.13c D2Client+0xb67d0 -- client missile: chain to the nearest other target. ECX = missile unit, EDX = the unit it
// came from; always returns 1. Needs a room and EDX != 0; Missiles row +0x36 > 0 first fires 0x6B300(EDX, 2, 0..0,
// EAX = that value). With D2Common 0xC322 > 1 hops left, the range is row +0x6C, or (<= 0) D2Common
// 0xC316(room, skill +0x64, skill, level) clamped to >= 1 when the 0x15B0 skill row exists. 0xA1A50(EAX = 0x88583,
// ECX = guid of EDX, EDX = range; room, x, y) picks the target; a type-0x21 request flies from the unit to it, and the
// child gets D2Common 0xC448(hops - 1) and 0xC3A0(EDX).
#include "cl_env.h"
extern "C" uint32_t d2c_b67d0(uint32_t u, uint32_t src, const cl::Env& e) {
    if (!u) return 1;
    int32_t cid = (int32_t)e.R(u + 4);
    if (cid < 0) return 1;
    uint32_t row = e.Row(cid);
    if (!row) return 1;
    uint32_t room = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!room || !src) return 1;
    int32_t s36 = e.R16s(row + 0x36);
    if (s36 > 0) e.H(0x6b300, src, 2, 0, 0, 0, (uint32_t)s36);
    int32_t hops = (int32_t)e.H(0xc322, u);
    if (hops <= 1) return 1;
    uint32_t x = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
    uint32_t y = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
    int32_t rng = (int32_t)e.R(row + 0x6c);
    if (rng <= 0) {
        uint32_t sk = e.H(0xc388, u);
        uint32_t lv = e.H(0xc3fa, u);
        uint32_t srow = e.H(0x15b0, 0, 0, 0, 0, 0, sk);
        if (!srow) return 1;
        rng = (int32_t)e.H(0xc316, room, e.R(srow + 0x64), sk, lv);
        if (rng < 1) rng = 1;
    }
    uint32_t t = e.H(0xa1a50, room, x, y, 0, 0, 0x88583, e.R(src + 0xc), (uint32_t)rng);
    if (!t || t == src) return 1;
    uint32_t q[23] = {};
    q[0] = 0x21; q[1] = room; q[3] = t; q[4] = e.R(u + 4);
    q[5] = x; q[6] = y;
    q[7] = e.H(0x1630, 0, 0, 0, 0, 0, 0, t);
    q[8] = e.H(0x1660, 0, 0, 0, 0, 0, 0, t);
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    if (m) {
        e.H(0xc448, m, (uint32_t)(hops - 1));
        e.H(0xc3a0, m, src);
    }
    return 1;
}
