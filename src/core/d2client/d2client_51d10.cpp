// 1.13c D2Client+0x51D10 -- cursor click routing (ECX = &px, EAX = &py; returns 0 or 1). Globals by RVA: mode 0x11C414,
// cursor x/y 0x11B828/0x11B824, screen w/h 0xDBC48/0xDBC4C, flags 0x11C2F4/0x11C2F8/0x11C2FC, panel rect 0x11963C..0x119648,
// 0x119638 / 0x11964C, 0x113A68, byte 0x113AFA, pointer 0x11C3C4.
// Mode != 3: the cursor goes to *px/*py; with py at most h-49 the click goes to 0x51860 (EAX = 0, return 1) when 0x11C2FC is
// set and the point lies strictly inside the rect, else by mode: 1 takes 0x51860 if px >= w/2, 2 takes 0x51860 if px < w/2,
// otherwise 0x510F0(px; EBX = py) and return 0. Otherwise (mode 3, or py below the band) with 0x11C2F8 clear: 0x113A68 set
// and 0x7C5C0() zero: byte 0x113AFA = 0, return 1; byte = 0; 0x11C2F4 set and 0x11964C == 5: u = 0xA5B40(ECX = [0x119638],
// EDX = 5), non-null u: 0xCD0C(0xC1F0(u, [u+4])) and *[0x11C3C4] |= 1; 0x11C2F4 = 0. Return 1.
#include "cl_env.h"
namespace {
constexpr uint32_t G(uint32_t rva) { return 0x6FAB0000u + rva; }
}
extern "C" uint32_t d2c_51d10(uint32_t px, uint32_t py, const cl::Env& e) {
    int32_t mode = (int32_t)e.R(G(0x11c414));
    if (mode != 3) {
        e.W(px, e.R(G(0x11b828)));
        int32_t y = (int32_t)e.R(G(0x11b824));
        e.W(py, (uint32_t)y);
        if (y <= (int32_t)e.R(G(0xdbc4c)) - 0x31) {
            int32_t x = (int32_t)e.R(px), half = (int32_t)e.R(G(0xdbc48)) / 2;
            bool in = e.R(G(0x11c2fc)) && x > (int32_t)e.R(G(0x11963c)) && x < (int32_t)e.R(G(0x119644)) &&
                      y > (int32_t)e.R(G(0x119640)) && y < (int32_t)e.R(G(0x119648));
            if (in || (mode == 1 && x >= half) || (mode == 2 && x < half)) { e.H(0x51860); return 1; }
            e.H(0x510f0, px, 0, 0, 0, 0, 0, 0, 0, py);
            return 0;
        }
    }
    if (e.R(G(0x11c2f8))) return 1;
    uint32_t b = e.R(G(0x113af8)) & 0xFF00FFFFu;
    if (e.R(G(0x113a68)) && !e.H(0x7c5c0)) { e.W(G(0x113af8), b); return 1; }
    e.W(G(0x113af8), b);
    if (e.R(G(0x11c2f4)) && e.R(G(0x11964c)) == 5) {
        uint32_t u = e.H(0xa5b40, 0, 0, 0, 0, 0, 0, e.R(G(0x119638)), 5);
        if (u) { e.H(0xcd0c, e.H(0xc1f0, u, e.R(u + 4))); uint32_t p = e.R(G(0x11c3c4)); e.W(p, e.R(p) | 1); }
    }
    e.W(G(0x11c2f4), 0);
    return 1;
}
