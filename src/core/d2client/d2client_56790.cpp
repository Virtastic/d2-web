// 1.13c D2Client+0x56790 -- skill do: a monster charge sequence keyed on the unit's frame. Near the target (or once
// the target record says so) it winds down into the row's first mode; at the row's start frame the path is given the
// formula speed and set moving; at the end frame the unit drops into the row's second mode.
#include "cl_skdo.h"
extern "C" uint32_t d2c_56790(uint32_t u, int32_t sk, uint32_t lvl, const cl::Env& e) {
    const uint32_t r = clsk::row(e, sk);
    if (!r) return 0;
    const uint32_t p = e.H(0xc310, u);
    if (!p) return 0;
    int32_t x = 0, y = 0;
    e.H(0xc5ec, u, (uint32_t)(uintptr_t)&x, (uint32_t)(uintptr_t)&y);
    if (x > (int32_t)e.R(r + 0x154)) return 0;
    const uint32_t path = e.R(u + 0x2c);
    if (!path) return 0;
    const uint32_t t = e.H(0xc2da, path);
    const bool near = t && (int32_t)e.H(0xcc70, t, u) <= 1;
    if (near || (e.H(0xc628, p) & 2)) {
        e.H(0xc60a, p, 0);
        e.W(u + 0x48, e.R(r + 0x148) << 8);
        e.H(0xcb56, u, e.R(r + 0x14c));
        return 1;
    }
    if (x == (int32_t)e.R(r + 0x150)) {
        int32_t s = (int32_t)(e.H(0xc316, u, e.R(r + 0x138), (uint32_t)sk, lvl) << 8);
        if (s <= 0x100) s = 0x100;
        const uint32_t a = e.H(0xc1c6, u, 0x43, 0);
        const uint32_t q = e.H(0x14a0, 0, 0, 0, 0, 0, a, 100, (uint32_t)s);
        e.H(0xc328, path, q, 0x6FB85BB8u, 0x466);
        e.H(0xc38e, path, 1);
        e.H(0xc376, path, u, 0);
        e.H(0xc60a, p, 1);
        return 1;
    }
    if (x == (int32_t)e.R(r + 0x154)) {
        e.W(u + 0x48, e.R(r + 0x158) << 8);
        e.H(0xcb56, u, e.R(r + 0x15c));
    }
    return 1;
}
