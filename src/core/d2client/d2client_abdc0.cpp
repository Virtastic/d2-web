// 1.13c D2Client+0xABDC0 growQueue, written from the disassembly: EAX = unit; returns the unit's packet-queue header.
// The first call sizes the record stride (0x11BBD8) from the handler table 0xDDE60 (175 x {?, size, handler}): the largest size
// of an entry with a handler, plus 5. The header (unit+0xD8: cap, count, ?, buf) is allocated on demand (Fog 10042, line 0x1266),
// then the buffer grows by 5 records (line 0x126B), the old records copied and the old buffer freed (Fog 10043, line 0x126F).
#include "cl_genv.h"
using namespace cl;
extern "C" uint32_t d2c13_abdc0(uint32_t unit, const Env& e, const GEnv& g) {
    uint32_t st = e.R(g.G(0x11bbd8));
    if (st == 0) {
        for (uint32_t p = 0xdde64; p < 0xde698; p += 0xc)
            if (e.R(g.G(p + 4)) != 0 && (int32_t)e.R(g.G(p)) > (int32_t)st) st = e.R(g.G(p));
        st += 5; e.W(g.G(0x11bbd8), st);
    }
    if (!unit) { Fatal(e, g, 0x8a5); return 0; }
    uint32_t hdr = e.R(unit + 0xd8);
    if (!hdr) {
        hdr = e.H(0xbf7a, 0x1266, 0, 0, 0, 0, 0, 0x10, g.M(0xd4034));
        Zero(hdr, 0x10); e.W(unit + 0xd8, hdr);
    }
    st = e.R(g.G(0x11bbd8));
    const uint32_t cap = e.R(hdr) + 5;
    const uint32_t nb = e.H(0xbf7a, 0x126b, 0, 0, 0, 0, 0, st * cap, g.M(0xd4034));
    Zero(nb, e.R(g.G(0x11bbd8)) * cap);
    const uint32_t old = e.R(hdr + 0xc);
    if (old) {
        Copy(nb, old, e.R(hdr) * e.R(g.G(0x11bbd8)));
        e.H(0xbf86, 0x126f, 0, 0, 0, 0, 0, e.R(hdr + 0xc), g.M(0xd4034));
    }
    e.W(hdr, cap); e.W(hdr + 0xc, nb);
    return hdr;
}
