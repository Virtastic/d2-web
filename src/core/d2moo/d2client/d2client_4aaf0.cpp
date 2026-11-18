// 1.13c D2Client+0x4AAF0 -- speech-menu entry: clears the two panel words, runs the four-word routine with zeros and
// EBX = 1, drops the dialog and clears the handle at 0x11975B. In menu state 0xC: state 5 runs the routine taking the
// id in EDI, then (behind the build check, line 0x139B) clears the pending word at 0xFADA0 and, when it was set, runs
// the two follow-ups; any other state runs the plain follow-up. Outside menu state 0xC (asserting state 6 at line
// 0xD55 and menu state 8 at line 0xD56) the menu state becomes 3 and the last routine runs. Returns 1.
#include "cl_tbl.h"
extern "C" uint32_t d2c_4aaf0(const cl::Env& e) {
    clt::S(e, 0x11C2F8, 0);
    clt::S(e, 0x11C2F4, 0);
    e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, 1);
    e.H(0xa3d20);
    const uint32_t st = clt::G(e, 0x119663);
    clt::S(e, 0x11975B, 0);
    if (st == 0xc) {
        if (clt::G(e, 0x119729) != 5) {
            e.H(0x48b20);
            return 1;
        }
        e.H(0x47b20);
        e.Edj(0x47b20, clt::G(e, 0x11971D));
        const uint32_t pend = clt::G(e, 0xFADA0);
        e.H(0xbf8c);
        if (clt::G(e, 0xDD5D0) != 0x26) clt::assertAt(e, 0x139b);
        clt::S(e, 0xFADA0, 0);
        if (pend) {
            e.H(0xbe2e0);
            e.H(0x492f0);
        }
        return 1;
    }
    if (clt::G(e, 0x119729) == 6) clt::assertAt(e, 0xd55);
    else if (st == 8) clt::assertAt(e, 0xd56);
    clt::S(e, 0x119663, 3);
    e.H(0x8f630);
    return 1;
}
