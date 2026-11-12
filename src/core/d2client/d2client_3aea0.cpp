// 1.13c D2Client+0x3AEA0 -- key command: after the build check (the word at 0xDD5D0 must be 0x26, else the assertion
// path: line 0x139B, report, exit(-1)), clear three panel words (0xFADB4, 0x11C2F8, 0x11C2F4).
#include "cl_cmd.h"
extern "C" uint32_t d2c_3aea0(uint32_t, const cl::Env& e) {
    e.H(0xbf8c);
    if (clc::G(e, 0xDD5D0) != 0x26) {
        const uint32_t r = e.H(0xbf92, 0x139b);
        e.H(0xbf80, 0x6FB8359Fu, r, 0x139b);
        e.H(0x3664, 0xFFFFFFFFu);
    }
    clc::S(e, 0xFADB4, 0);
    clc::S(e, 0x11C2F8, 0);
    clc::S(e, 0x11C2F4, 0);
    return 0;
}
