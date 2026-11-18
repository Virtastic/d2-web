// 1.13c D2Client+0x473B0 -- speech-menu entry: the dialog's selected line (the dword at +0x44, -1 without a dialog)
// must reach the first gossip line (2 when the current entry of the 0x16-byte table at 0xF37BC has its flag byte set,
// else 3) and the player must be present; else returns 0. The line past that base picks the sound through the lookup
// (table at 0x113B10, asserted non-null at line 0x710); it is stored, played on the player, the reply marked pending
// and the dialog dropped. Returns 1.
#include "cl_tbl.h"
extern "C" uint32_t d2c_473b0(const cl::Env& e) {
    const uint32_t dlg = clt::G(e, 0x119767);
    const int32_t sel = dlg ? (int32_t)e.R(dlg + 0x44) : -1;
    const int32_t base = e.R8(0x6FBA37BCu + clt::G(e, 0x119679) * 0x16) ? 2 : 3;
    if (sel < base) return 0;
    if (!e.H(0x46150)) return 0;
    const uint32_t tbl = clt::G(e, 0x113B10);
    if (!tbl) clt::assertAt(e, 0x710);
    const uint32_t s = e.H(0xcdf6, tbl, (uint32_t)(sel - base));
    clt::S16(e, 0x119673, s);
    const uint32_t pl = e.H(0x46150);
    e.H(0x7d3c0, pl, s);
    clt::replyPending(e, 0x6FAF6E60u);
    e.H(0xa3d20);
    clt::S(e, 0x119767, 0);
    return 1;
}
