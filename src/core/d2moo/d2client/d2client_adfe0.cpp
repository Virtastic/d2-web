// 1.13c D2Client+0xADFE0: server hero-ready packet. Sets the globals 0x11C3A0 = 1 and 0x11C3A8 = 0; with no hero (0x11BBFC) or a zero
// 0xC1F0(hero) retail logs error 0x527 and exits (the path falls into int3: no return). Otherwise 0xC1DE and 0xC1EA chain from that
// result, 0x6D1C0 runs, and a hero reloaded from the global whose unit type is 0 and whose info pointer (+0x14) is set has the four
// dwords at info+0x150..0x15C cleared. Returns the reloaded hero.
#include "cl_genv.h"
using namespace cl;
extern "C" uint32_t d2c13_adfe0(const Env& e, const GEnv& g) {
    e.W(g.G(0x11c3a0), 1);
    e.W(g.G(0x11c3a8), 0);
    uint32_t pl = e.self;
    const uint32_t x = pl ? e.H(0xc1f0, pl) : 0;
    if (!x) { Fatal(e, g, 0x527); return 0; }
    const uint32_t y = e.H(0xc1de, x);
    e.H(0xc1ea, y);
    e.H(0x6d1c0);
    pl = e.self;
    if (pl && !e.R(pl)) {
        const uint32_t info = e.R(pl + 0x14);
        if (info) for (uint32_t o = 0x150; o <= 0x15c; o += 4) e.W(info + o, 0);
    }
    return pl;
}
