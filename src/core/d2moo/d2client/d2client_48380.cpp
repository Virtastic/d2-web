// 1.13c D2Client+0x48380 -- speech-menu entry: drops the dialog whose handle is at 0x119763 and closes the menu.
// Returns 1.
#include "cl_tbl.h"
extern "C" uint32_t d2c_48380(const cl::Env& e) {
    clt::closeDlg(e, 0x119763);
    e.H(0x48150);
    return 1;
}
