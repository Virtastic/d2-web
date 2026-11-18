// 1.13c D2Client+0x461D0 -- speech-menu entry: drops the open dialog and clears its handle. In state 5 it hands over
// to the state-5 routine; otherwise (after the extra call when the word at 0x11BC34 is 5) the menu state becomes 3
// and the two panel words are cleared. Returns 1.
#include "cl_tbl.h"
extern "C" uint32_t d2c_461d0(const cl::Env& e) {
    e.H(0xa3d20);
    const uint32_t st = clt::G(e, 0x119729);
    clt::S(e, 0x119757, 0);
    if (st == 5) {
        e.H(0x49440);
        return 1;
    }
    if (clt::G(e, 0x11BC34) == 5) e.H(0x8d300);
    clt::S(e, 0x119663, 3);
    clt::S(e, 0x11C2F8, 0);
    clt::S(e, 0x11C2F4, 0);
    return 1;
}
