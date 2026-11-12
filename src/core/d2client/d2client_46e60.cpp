// 1.13c D2Client+0x46E60 -- speech-menu continuation: while a reply is pending, asks for the next sound (BX = the
// current one); a real one (not 0xFFFF) with the player present is stored, played on the player and the continuation
// re-armed. Otherwise the pending flag and continuation are cleared, ECX is kept as the argument and the menu closes.
#include "cl_tbl.h"
extern "C" uint32_t d2c_46e60(uint32_t ecx, const cl::Env& e) {
    if (clt::G8(e, 0x119661) == 1) {
        const uint32_t s = e.H(0x45ff0, 0, 0, 0, 0, 0, 0, 0, 0, clt::G16(e, 0x119673));
        if ((s & 0xffff) != 0xffff && e.H(0x46150)) {
            clt::S16(e, 0x119673, s);
            const uint32_t pl = e.H(0x46150);
            e.H(0x7d3c0, pl, s);
            clt::S(e, 0x11BF94, 0x6FAF6E60u);
            clt::S(e, 0x11BF98, 0);
            return 1;
        }
    }
    clt::S8(e, 0x119661, 0);
    clt::S(e, 0x11BF94, 0);
    clt::S(e, 0x11BF98, ecx);
    e.H(0x4a270);
    return 1;
}
