// 1.13c D2Client+0xC6450, written from the disassembly. EDI = unit, plain ret. s = 0xC838(ECX = unit); if s and 0xC628(s) has bit 0, 0xC60A(s, that | 2).
// v = 0x225D0(EAX = 0x9E3, ECX = unit); zero returns 0. Else walk the list at DAT_6fbcbf30 ({object, next} nodes) for an object whose +8 equals v and
// tail-call 0x88550(ECX = object), returning its result; no match returns v.
#include "cl_mb2.h"
// 1.13c D2Client+0xC6450
extern "C" uint32_t d2c_c6450(uint32_t u, const clm::Env& e) {
    const uint32_t s = e.H(0xc838, 0, u);
    if (s) {
        const uint32_t f = e.H(0xc628, 0, 0, 0, 0, 0, s);
        if (f & 1) e.H(0xc60a, 0, 0, 0, 0, 0, s, f | 2);
    }
    const uint32_t v = e.H(0x225d0, 0x9e3, u);
    if (!v) return 0;
    for (uint32_t n = e.G(0x11bf30); n; n = e.R(n + 4)) {
        const uint32_t o = e.R(n);
        if (e.R(o + 8) == v) return e.H(0x88550, 0, o);
    }
    return v;
}
