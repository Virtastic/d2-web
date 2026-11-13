// 1.13c D2Client+0x5C3D0: toggle the hireling panel. cdecl, void. State 0x11C284 (0 closed, 1 open, 2 closing). An open or closing
// panel is shut through 0xC2790(0x23); a closed one is opened (with the sweep of the open-panel array when a hireling is shown)
// and the 0xFAE0C flag change is reported through the D0B4 string handle and 0x3CC60 / 0x3CCC0.
#include <cstdint>
#include "cl_env.h"
namespace { constexpr uint32_t G(uint32_t rva) { return 0x6FAB0000u + rva; } constexpr uint32_t kT = 0xB2000000u; }
extern "C" void d2c_5c3d0(const cl::Env& e) {
    const uint32_t st = e.R(G(0x11C284));
    const bool hasUnit = e.R(G(0x11C394)) != 0;
    if (st != 0) {
        if (st == 1 && hasUnit) {
            e.W(G(0x11C284), 2); e.W(G(0x11C2A0), 0);
            e.H(0xC2790, 0, 0, 0, 0, 0, 0, 0x23, 1);
            return;
        }
        e.W(G(0x11C284), 0);
        e.H(0xC2790, 0, 0, 0, 0, 0, 0, 0x23, 0);
        return;
    }
    const uint32_t old = e.R(G(0xFAE0C));
    bool fatal = false;
    if (!hasUnit) {
        e.W(G(0x11C284), 2); e.W(G(0x11C2A0), 0);
        if (e.H(0xBF8C) && e.R(G(0x119854))) {
            if (e.R(G(0xDD5D0)) != 0x26) fatal = true;
            else {
                e.W(G(0xFAE0C), 0);
                if (old) { const uint32_t hw = e.H(0xD0B4); e.H(0x3CC60, hw, kT | 0xD0978, 0, 0, 0, 3); }
            }
        }
    } else {
        e.W(G(0x11C284), 1);
        if (e.H(0xBF8C) && e.R(G(0x119854))) {
            if (e.R(G(0xDD5D0)) != 0x26) fatal = true;
            else if (!e.R(G(0x113A68))) {
                uint32_t i = 0;
                bool stop = false;
                do {
                    if (e.R(G(0xFAD80) + 4 * i) == 1) {
                        const uint32_t k = e.R(e.R(G(0xDD538) + 4 * i) + 0x8C);
                        if (k > 4) { stop = true; break; }
                        if (k == 1) e.H(0xC2790, 0, 0, 0, 0, 0, 0, i, 1);
                        else if (k == 4) {
                            if (e.R(G(0x119721))) { const uint32_t edi = e.R(G(0x11971D)); e.H(0x47B20); e.Edj(0x47B20, edi); }
                        } else if (k != 0) { stop = true; break; }       // kinds 2 and 3 leave
                    }
                    ++i;
                } while ((int32_t)i < (int32_t)e.R(G(0xDD5D0)));
                if (!stop) {
                    const uint32_t u = e.R(G(0x11BBFC));
                    if (!u || (e.R(u) == 0 && e.H(0xA5440) == 0)) e.W(G(0xFAE0C), 1);
                    const uint32_t now = e.R(G(0xFAE0C));
                    if (old == 0) {
                        if (now) { const uint32_t hw = e.H(0xD0B4); e.H(0x3CCC0, hw, kT | 0xD0978, 0, 0, 0, 3); }
                    } else if (now == 0) {
                        const uint32_t hw = e.H(0xD0B4); e.H(0x3CC60, hw, kT | 0xD0978, 0, 0, 0, 3);
                    }
                }
            }
        }
    }
    if (fatal) {
        const uint32_t m = e.H(0xBF92, 0x139B);
        e.H(0xBF80, kT | 0xD359F, m);
        e.H(0x3664, 0xFFFFFFFFu);
    }
}
