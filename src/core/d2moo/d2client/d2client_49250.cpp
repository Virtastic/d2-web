// 1.13c D2Client+0x49250 -- speech-menu entry: refused (0) while a reply is pending; an open dialog (0x119767) is
// dropped, the four-word routine run with zeros and EBX = 1, and the follow-up routine called. Returns 1.
#include "cl_tbl.h"
extern "C" uint32_t d2c_49250(const cl::Env& e) {
    if (clt::G8(e, 0x119661)) return 0;
    if (clt::G(e, 0x119767)) {
        e.H(0xa3d20);
        e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, 1);
        clt::S(e, 0x119767, 0);
        e.H(0x48b20);
    }
    return 1;
}
