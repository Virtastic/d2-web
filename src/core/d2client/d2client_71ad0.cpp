// 1.13c D2Client+0x71AD0 -- table entry: three calls (the second with ECX = 1), then 1 when either of the two words
// at 0x11C01C / 0x11C020 is set, else 0.
#include "cl_tbl.h"
extern "C" uint32_t d2c_71ad0(const cl::Env& e) {
    e.H(0x14b00);
    e.H(0x71a70, 0, 0, 0, 0, 0, 0, 1);
    e.H(0x5f020);
    return clt::G(e, 0x11C01C) || clt::G(e, 0x11C020) ? 1 : 0;
}
