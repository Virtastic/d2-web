// 1.13c D2Client+0x57060 -- bind a panel's control and report whether it took (ECX = panel, stack word unused, ret 4).
// Panel +0x2C is the control handle. A null handle is retail's fatal path (string lookup 0x946, message, exit(-1)); the vector never reaches it.
// Else: Ordinal 10392(handle) zero returns 0; Ordinal 10511(panel) zero returns 0; the handle is read again (zero returns 0), then
// 10168(h,5), 10647(h,2), 10334(h,panel,0); 10800(h) below 1 (signed) retries with 10647(h,1), 10334(h,panel,0), 10800(h), still below 1
// returns 0. Success: 10137(result of 10511, 1), return 1.
#include "cl_env.h"
extern "C" uint32_t d2c_57060(uint32_t p, const cl::Env& e) {
    e.H(0x2a0b0, 0, 0, 0, 0, 0, p);
    uint32_t f = e.R(p + 0x2c);
    if (f == 0) { uint32_t m = e.H(0xbf92, 0x946); e.H(0xbf80, 0xB2000000u | 0xD359F, m); e.H(0x3664, 0xffffffffu); return 0; }
    if (e.H(0xc2da, f) == 0) return 0;
    uint32_t b = e.H(0xc310, p);
    if (b == 0) return 0;
    f = e.R(p + 0x2c);
    if (f == 0) return 0;
    e.H(0xc3b8, f, 5); e.H(0xc38e, f, 2); e.H(0xc376, f, p, 0);
    if ((int32_t)e.H(0xcc6a, f) < 1) {
        e.H(0xc38e, f, 1); e.H(0xc376, f, p, 0);
        if ((int32_t)e.H(0xcc6a, f) < 1) return 0;
    }
    e.H(0xc60a, b, 1);
    return 1;
}
