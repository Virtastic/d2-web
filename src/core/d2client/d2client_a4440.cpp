// 1.13c D2Client+0xA4440 -- key-down message: the panel block (missing: assertion, line 0x2EE) offers it to each listener through slot +0x18
// and stops at the first nonzero answer. Otherwise the message code (msg+8) is tested after the 0xA3B50 check (EAX = the panel block):
// when that answers nonzero nothing more happens; code 0x0D runs 0xA42E0, code 0x1B runs 0xA4380, any other code with the panel word
// at +0x18 equal to 1 is marked handled and passed to the shared tail.
#include "cl_walk.h"
extern "C" uint32_t d2c_a4440(uint32_t msg, const cl::Env& e) {
    const uint32_t p = clw::panel(e, 0x2ee);
    if (!p) return 0;
    uint32_t a = 0;
    if (clw::walk(e, p, 0x18, msg, a, true)) return a;
    const uint32_t code = e.R(msg + 8);
    const uint32_t c = e.H(0xa3b50,0, 0, 0, 0, 0, p);
    if (c) return c;
    if (code == 0xd) return e.H(0xa42e0, msg);
    if (code == 0x1b) return e.H(0xa4380, msg);
    if (e.R(p + 0x18) == 1) return pa4::fin(e, msg);
    return 1;
}
