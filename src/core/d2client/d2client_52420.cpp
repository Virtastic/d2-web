// 1.13c D2Client+0x52420 -- arms the timed effect: its deadline is the tick getter's answer plus 0x5A and its
// running word is cleared. EAX is the deadline.
#include "cl_pa4.h"
extern "C" uint32_t d2c_52420(const cl::Env& e) {
    const uint32_t r = e.H(0xc09a) + 0x5a;
    pa4::S(e, 0x11c2e8, r);
    pa4::S(e, 0xf2130, 0);
    return r;
}
