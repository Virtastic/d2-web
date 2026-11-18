// 1.13c D2Client+0x56C40 -- skill do: a repeating step driven by the unit's skill-target record. Looks up the entry
// for the record's two fields; none: the record's counter is reset. Otherwise, when the entry is accepted and the
// counter (less one) is still positive, the counter is stored back, the row's word at +0x148 is applied and the low
// byte of the unit's word at +0x48 restarts at 0x100.
#include "cl_skdo.h"
extern "C" uint32_t d2c_56c40(uint32_t u, int32_t sk, uint32_t, const cl::Env& e) {
    const uint32_t r = clsk::row(e, sk);
    if (!r) return 0;
    const uint32_t p = e.H(0xc310, u);
    if (!p) return 0;
    const uint32_t z = e.H(0xcb9e, p);
    const uint32_t y = e.H(0xcb6e, p);
    const uint32_t t = e.H(0xa4e20, z, 0, 0, 0, 0, y & 0x7f, y, (z << 9) + clc::kBase + 0x10A608);
    if (!t) { e.H(0xcb50, p, 0); return 0; }
    if (!e.H(0xc6e2, u, t, 0)) return 0;
    const int32_t n = (int32_t)e.H(0xcb68, p) - 1;
    if (n <= 0) return 0;
    e.H(0xcb50, p, (uint32_t)n);
    e.H(0xcb56, u, e.R(r + 0x148));
    e.W(u + 0x48, (e.R(u + 0x48) & 0xffffff00u) + 0x100);
    return 1;
}
