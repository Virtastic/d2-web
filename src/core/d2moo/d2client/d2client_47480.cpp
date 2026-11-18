// 1.13c D2Client+0x47480 -- speech-menu entry: in state 5 drops the dialog and runs the state-5 close; in menu state
// 4 drops the dialog and runs the two-word routine with (0, 0); otherwise clears the two panel words. Returns 1.
#include "cl_tbl.h"
extern "C" uint32_t d2c_47480(const cl::Env& e) {
    if (clt::G(e, 0x119729) == 5) {
        clt::closeDlg(e, 0x119757);
        e.H(0x46c00);
        return 1;
    }
    if (clt::G(e, 0x119663) == 4) {
        clt::closeDlg(e, 0x119757);
        e.H(0x46ee0, 0, 0);
        return 1;
    }
    clt::S(e, 0x11C2F8, 0);
    clt::S(e, 0x11C2F4, 0);
    return 1;
}
