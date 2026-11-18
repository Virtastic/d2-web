// 1.13c D2Client+0xba880 -- client missile: timed fade-out arm. ECX = unit; tail-jumps to 0xB17C0(ECX = u) on every
// path and returns its value. owner = 0xA60D0(EAX = u); 0 goes straight to the tail. t = 0xC4BA(u); when t == 0:
// 0xC322(u), 0xB5380(owner), t = (seed step on u +0x20) % 35 + 10. Then 0xC394(u, t - 1).
#include "cl_env.h"
extern "C" uint32_t d2c_ba880(uint32_t u, const cl::Env& e) {
    uint32_t own = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (own) {
        uint32_t t = e.H(0xc4ba, u);
        if (!t) {
            e.H(0xc322, u);
            e.H(0xb5380, own);
            t = e.Lcg(u + 0x20) % 35u + 10u;
        }
        e.H(0xc394, u, t - 1);
    }
    return e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
