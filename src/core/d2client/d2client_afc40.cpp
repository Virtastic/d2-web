// 1.13c D2Client+0xAFC40 -- table entry: in modes 0 and 1 (0x11C394) a nonzero first check returns 0. The poll's
// answer 0 opens panel 6 (EAX = 6), raises the flag at 0xFAE20, clears the word at 0x11C39C and returns 1; any other
// answer returns whether it is 2.
#include "cl_tbl.h"
extern "C" uint32_t d2c_afc40(const cl::Env& e) {
    const uint32_t m = clt::G(e, 0x11C394);
    if ((m == 0 || m == 1) && e.H(0xd0a2)) return 0;
    const uint32_t r = e.H(0xd228);
    if (r) return r == 2;
    e.H(0x43550, 0, 0, 0, 0, 0, 6);
    clt::S(e, 0xFAE20, 1);
    clt::S(e, 0x11C39C, 0);
    return 1;
}
