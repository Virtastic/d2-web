// 1.13c D2Client+0x71B60 -- table entry: when the first check answers 0 and the begin call (1, 0, 0, 0) succeeds, the
// frame sequence runs (the word at 0x11C010 goes to its middle call). Then the closing call; returns whether the
// word at 0x11C018 is set.
#include "cl_tbl.h"
extern "C" uint32_t d2c_71b60(const cl::Env& e) {
    e.H(0x14b00);
    if (!e.H(0xd0a2) && e.H(0xd0a8, 1, 0, 0, 0)) {
        e.H(0xd090, 0);
        e.H(0xd3a2);
        e.H(0x71280, 1);
        e.H(0x717b0);
        e.H(0x71220, clt::G(e, 0x11C010));
        e.H(0x71740);
        e.H(0x16a90);
        e.H(0xd08a);
    }
    e.H(0x5f020);
    return clt::G(e, 0x11C018) != 0;
}
