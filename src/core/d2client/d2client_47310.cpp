// 1.13c D2Client+0x47310 -- speech-menu entry: with the player present plays sound 0x152 on it, marks the reply
// pending, drops the dialog and clears its handle. Returns 1.
#include "cl_tbl.h"
extern "C" uint32_t d2c_47310(const cl::Env& e) {
    if (e.H(0x46150)) {
        const uint32_t pl = e.H(0x46150);
        e.H(0x7d3c0, pl, 0x152);
        clt::replyPending(e, 0x6FAF6E60u);
        e.H(0xa3d20);
        clt::S(e, 0x119767, 0);
    }
    return 1;
}
