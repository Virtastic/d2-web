// 1.13c D2Client+0x481C0 -- speech-menu entry: drops the dialog; without the player the menu just closes. Otherwise
// plays the current sound on the player, appends it to the nine-word history, and either ends the menu state (no
// follow-up sound) or marks the reply pending; then sends the nine-byte message {0x31, id, sound}. Returns 1.
#include "cl_tbl.h"
extern "C" uint32_t d2c_481c0(const cl::Env& e) {
    clt::closeDlg(e, 0x119773);
    if (!e.H(0x46150)) {
        e.H(0x48150);
        return 1;
    }
    const uint32_t w = clt::G16(e, 0x11966F);
    const uint32_t pl = e.H(0x46150);
    e.H(0x7d3c0, pl, w);
    const uint32_t n = clt::G16(e, 0x1197A6);
    if (n < 9) {
        clt::S16(e, 0x119792 + 2 * n, w);
        clt::S16(e, 0x1197A6, n + 1);
    }
    if (clt::G16(e, 0x119671) == 0xffff) clt::S(e, 0x119663, 0);
    else clt::replyPending(e, 0x6FAFB230u);
    clt::send31(e);
    return 1;
}
