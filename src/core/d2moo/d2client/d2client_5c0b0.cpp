// 1.13c D2Client+0x5C0B0: stdcall, one mouse message. Hireling-panel hover handler: after the pointer has rested 700 ms inside
// the strip x in [0x11C28C, +0x2E], y in [0x18, 0x3C] (state 0x11C284 != 2, gate 0x119854) the message is marked handled (+0x18 = 1,
// +0x1C = 0), passed to Storm ordinal 511 (thunk 0xBE84) and, when the sweep flag 0xFAE0C is set with 0xFAE10 clear and the viewed
// unit has a type-7 list node, the open panels are swept (as 0x5C3D0) and the HUD offset 0x11C414/0x11C418 moves through
// 0xD1B0 / 0x8A4D0. GetTickCount is the import 0xCEF5C, reached through the seam e.H(0xCEF5C).
#include <cstdint>
#include "cl_env.h"
namespace { constexpr uint32_t G(uint32_t rva) { return 0x6FAB0000u + rva; } constexpr uint32_t kT = 0xB2000000u; }
extern "C" void d2c_5c0b0(uint32_t m, const cl::Env& e) {
    if (!e.R(G(0x119854))) return;
    const bool st2 = e.R(G(0x11C284)) == 2;
    const uint32_t x = e.R(m + 0xC) & 0xFFFF, y = e.R(m + 0xE) & 0xFFFF;
    e.W(m + 0x18, 0);
    const int32_t ox = (int32_t)e.R(G(0x11C28C));
    if (st2 || (int32_t)x < ox || (int32_t)x > ox + 0x2E || (int32_t)y < 0x18 || (int32_t)y > 0x3C) { e.W(G(0x11C2A0), 0); return; }
    if (e.R(G(0x11C2A0)) == 0) e.W(G(0x11C2A0), e.H(0xCEF5C));
    const uint32_t dt = e.H(0xCEF5C) - e.R(G(0x11C2A0));
    if (dt < 0x2BC) { e.W(G(0x11C2A0), 0); return; }
    e.W(m + 0x18, 1); e.W(m + 0x1C, 0);
    e.H(0xBE84, m);
    if (!e.R(G(0xFAE0C)) || e.R(G(0xFAE10))) return;
    const uint32_t u = e.R(G(0x11BBFC));
    if (!u) return;
    uint32_t n = e.R(G(0x11C4D4));
    if (!n) return;
    const uint32_t uid = e.R(u + 0xC);
    while (!(e.R(n + 4) == 7 && e.R(n + 0xC) == uid && e.R(n + 0x20) == 0)) { n = e.R(n + 0x30); if (!n) return; }
    if (e.R(n + 8) == 0xFFFFFFFFu) return;
    if (e.H(0xBF8C) && e.R(G(0x119854))) {
        if (e.R(G(0xDD5D0)) != 0x26) {
            const uint32_t s = e.H(0xBF92, 0x139B);
            e.H(0xBF80, kT | 0xD359F, s);
            e.H(0x3664, 0xFFFFFFFFu);
            return;
        }
        if (!e.R(G(0x113A68))) {
            for (uint32_t i = 0; (int32_t)i < (int32_t)e.R(G(0xDD5D0)); ++i) {
                if (e.R(G(0xFAD80) + 4 * i) != 1) continue;
                const uint32_t k = e.R(e.R(G(0xDD538) + 4 * i) + 0x90);
                if (k > 4 || k == 2 || k == 3) goto out;
                if (k == 1) e.H(0xC2790, 0, 0, 0, 0, 0, 0, i, 1);
                else if (k == 4 && e.R(G(0x119721))) { const uint32_t edi = e.R(G(0x11971D)); e.H(0x47B20); e.Edj(0x47B20, edi); }
            }
            uint32_t sw;
            const uint32_t u2 = e.R(G(0x11BBFC));
            if (!u2 || (e.R(u2) == 0 && e.H(0xA5440) == 0)) { sw = 1; e.W(G(0xFAE10), 1); } else sw = e.R(G(0xFAE10));
            if (sw) { const uint32_t hw = e.H(0xD0B4); e.H(0x3CCC0, hw, kT | 0xD099C, 0, 0, 0, 3); }
            const uint32_t w = e.R(G(0xDBC48));
            const uint32_t q = (uint32_t)(((int32_t)w + (int32_t)(((int32_t)w >> 31) & 3)) >> 2);
            uint32_t ecx, sp;
            if (!e.R(G(0xFAD84)) && !e.R(G(0xFAD90))) {
                if (!sw) { e.W(G(0x11C418), 0); e.W(G(0x11C414), 0); e.H(0xD1B0, 0, 0, 0, 0, 0, 0, 0); ecx = e.R(G(0xF9E14)); sp = 0; }
                else {
                    e.W(G(0x11C418), q); e.W(G(0x11C414), 2); e.H(0xD1B0, 0, 0, 0, 0, 0, 0, q);   // ecx = q
                    const uint32_t a = e.R(G(0xF9E14)); const uint32_t t = (uint32_t)(((int32_t)a + (int32_t)(((int32_t)a >> 31) & 3)) >> 2);
                    ecx = a + t; sp = t;
                }
            } else if (sw) {
                e.W(G(0x11C418), 0); e.W(G(0x11C414), 3); e.H(0xD1B0, 0, 0, 0, 0, 0, 0, 0);
                e.H(0x8A4D0, 0, 0, 0, 0, 0, e.R(G(0xF9E18)), e.R(G(0xF9E14)), 0, 0, e.R(G(0x11C3C4)));
                e.W(G(0x11C3DC), 0); e.W(G(0x11C3E0), 0); e.W(G(0x11C3F8), 0); e.W(G(0x11C3FC), 0);
                goto out;
            } else {
                e.W(G(0x11C418), 0u - q); e.W(G(0x11C414), 1); e.H(0xD1B0, 0, 0, 0, 0, 0, 0, 0u - q);
                const uint32_t a = e.R(G(0xF9E14)); const uint32_t t = (uint32_t)(((int32_t)a + (int32_t)(((int32_t)a >> 31) & 3)) >> 2);
                ecx = a - t; sp = 0u - t;
            }
            e.H(0x8A4D0, sp, 0, 0, 0, 0, e.R(G(0xF9E18)), ecx, 0, 0, e.R(G(0x11C3C4)));
        }
    }
out:
    e.W(G(0x11C2A0), 0);
}
