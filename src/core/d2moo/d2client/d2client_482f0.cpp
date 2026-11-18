// 1.13c D2Client+0x482F0 -- speech-menu entry: an open dialog is dropped and the nine-byte message {0x31, id, sound}
// sent; then the menu closes. Returns 1.
#include "cl_tbl.h"
extern "C" uint32_t d2c_482f0(const cl::Env& e) {
    if (clt::G(e, 0x119773)) {
        e.H(0xa3d20);
        clt::S(e, 0x119773, 0);
        clt::send31(e);
    }
    e.H(0x48150);
    return 1;
}
