// 1.13c D2Client+0x48650 -- speech-menu entry: refused (0) while a reply is pending; an open dialog (0x119767) is
// dropped and the menu closed. Returns 1.
#include "cl_tbl.h"
extern "C" uint32_t d2c_48650(const cl::Env& e) {
    if (clt::G8(e, 0x119661)) return 0;
    if (clt::G(e, 0x119767)) {
        e.H(0xa3d20);
        clt::S(e, 0x119767, 0);
        e.H(0x48150);
    }
    return 1;
}
