// 1.13c D2Client+0x50800: stdcall, one mouse message. Left-button release over the panel-close buttons: Storm ordinal 10031 (thunk 0xD0AE,
// the UI mode) == 2 with 0xFADA4 set returns untouched; otherwise 0x16300(msg) records the pointer, then mode 2 releases the pressed button
// 0x11C30C through 0x4FE70 / 0xC2790(4, 0), anything else through 0x4FEC0(EAX=[0x11C414], ECX=x, EDI=y): the open panels are closed and swept
// (as 0x5C3D0) and the HUD offset 0x11C414 / 0x11C418 moves through 0xD1B0 / 0x8A4D0. Every path that handled the message sets +0x18 = 1,
// +0x1C = 0 and calls 0xBE84; a miss clears 0x11C30C and +0x18.
#include <cstdint>
#include "cl_env.h"
namespace { constexpr uint32_t G(uint32_t rva) { return 0x6FAB0000u + rva; } constexpr uint32_t kT = 0xB2000000u; }
extern "C" void d2c_50800(uint32_t m, const cl::Env& e) {
    const uint32_t x = e.R(m + 0xC) & 0xFFFF, y = e.R(m + 0xE) & 0xFFFF;
    if (e.H(0xD0AE) == 2 && e.R(G(0xFADA4)) != 0) return;
    e.H(0x16300, m);
    const bool mode2 = e.H(0xD0AE) == 2;
    const uint32_t pressed = e.R(G(0x11C30C));
    auto handled = [&] { e.W(m + 0x1C, 0); e.W(m + 0x18, 1); e.H(0xBE84, m); };
    if (mode2) {
        if (pressed && e.H(0x4FE70)) {
            e.W(G(0x11C30C), 0);
            e.H(0xC2790, 0, 0, 0, 0, 0, 0, 4, 0);
            e.W(m + 0x18, 1); e.W(m + 0x1C, 0); e.H(0xBE84, m);
            return;
        }
    } else if (pressed) {
        const uint32_t r = e.H(0x4FEC0, 0, 0, 0, 0, 0, e.R(G(0x11C414)), x);
        e.Edj(0x4FEC0, y);
        if (r) {
            const uint32_t oldc = e.R(G(0xFAD9C));
            e.W(G(0x11C30C), 0);
            e.H(0xBF8C);
            auto fatal = [&] { const uint32_t s = e.H(0xBF92, 0x139B); e.H(0xBF80, kT | 0xD359F, s); e.H(0x3664, 0xFFFFFFFFu); };
            if (e.R(G(0xDD5D0)) != 0x26) { fatal(); return; }
            e.W(G(0xFAD9C), 0);
            if (oldc) { const uint32_t hw = e.H(0xD0B4); e.H(0x3CC60, hw, kT | 0xD0664, 0, 0, 0, 2); }
            const uint32_t old90 = e.R(G(0xFAD90));
            e.H(0xBF8C);
            if (e.R(G(0xDD5D0)) != 0x26) { fatal(); return; }
            if (e.R(G(0x113A68)) == 0) {
                for (uint32_t i = 0; (int32_t)i < (int32_t)e.R(G(0xDD5D0)); ++i) {
                    if (e.R(G(0xFAD80) + 4 * i) != 1) continue;
                    const uint32_t k = e.R(e.R(G(0xDD538) + 4 * i) + 0x10);
                    if (k > 4 || k == 2 || k == 3) { handled(); return; }
                    if (k == 1) e.H(0xC2790, 0, 0, 0, 0, 0, 0, i, 1);
                    else if (k == 4 && e.R(G(0x119721))) { const uint32_t edi = e.R(G(0x11971D)); e.H(0x47B20); e.Edj(0x47B20, edi); }
                }
                const uint32_t u = e.R(G(0x11BBFC));
                uint32_t sw;
                if (!u || (e.R(u) == 0 && e.H(0xA5440) == 0)) { sw = 1; e.W(G(0xFAD90), 1); } else sw = e.R(G(0xFAD90));
                if (old90 == 0) {
                    if (sw) { const uint32_t hw = e.H(0xD0B4); e.H(0x3CCC0, hw, kT | 0xD0604, 0, 0, 0, 2); }
                } else if (sw == 0) {
                    const uint32_t hw = e.H(0xD0B4); e.H(0x3CC60, hw, kT | 0xD0604, 0, 0, 0, 2);
                    e.H(0xBE2E0); e.H(0x77DA0);
                }
                const uint32_t w = e.R(G(0xDBC48));
                const uint32_t q = (uint32_t)(((int32_t)w + (int32_t)(((int32_t)w >> 31) & 3)) >> 2);
                uint32_t ecx, sp;
                if (!e.R(G(0xFAD88)) && !e.R(G(0xFADD0)) && !e.R(G(0xFADBC)) && !e.R(G(0xFADC0)) && !e.R(G(0xFAE10)) && !e.R(G(0xFAE14)) && !e.R(G(0xFADD8))) {
                    if (!sw) { e.W(G(0x11C418), 0); e.W(G(0x11C414), 0); e.H(0xD1B0, 0, 0, 0, 0, 0, 0, 0); ecx = e.R(G(0xF9E14)); sp = 0; }
                    else {
                        e.W(G(0x11C418), 0u - q); e.W(G(0x11C414), 1); e.H(0xD1B0, 0, 0, 0, 0, 0, 0, 0u - q);
                        const uint32_t a = e.R(G(0xF9E14)); const uint32_t t = (uint32_t)(((int32_t)a + (int32_t)(((int32_t)a >> 31) & 3)) >> 2);
                        ecx = a - t; sp = 0u - t;
                    }
                } else if (sw) {
                    e.W(G(0x11C418), 0); e.W(G(0x11C414), 3); e.H(0xD1B0, 0, 0, 0, 0, 0, 0, 0);
                    e.H(0x8A4D0, 0, 0, 0, 0, 0, e.R(G(0xF9E18)), e.R(G(0xF9E14)), 0, 0, e.R(G(0x11C3C4)));
                    e.W(G(0x11C3DC), 0); e.W(G(0x11C3E0), 0); e.W(G(0x11C3F8), 0); e.W(G(0x11C3FC), 0);
                    handled();
                    return;
                } else {
                    e.W(G(0x11C418), q); e.W(G(0x11C414), 2); e.H(0xD1B0, 0, 0, 0, 0, 0, 0, q);
                    const uint32_t a = e.R(G(0xF9E14)); const uint32_t t = (uint32_t)(((int32_t)a + (int32_t)(((int32_t)a >> 31) & 3)) >> 2);
                    ecx = a + t; sp = t;
                }
                e.H(0x8A4D0, sp, 0, 0, 0, 0, e.R(G(0xF9E18)), ecx, 0, 0, e.R(G(0x11C3C4)));
            }
            handled();
            return;
        }
    }
    e.W(G(0x11C30C), 0);
    e.W(m + 0x18, 0);
}
