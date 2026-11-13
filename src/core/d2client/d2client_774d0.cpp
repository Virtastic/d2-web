// 1.13c D2Client+0x774D0: stdcall, one mouse message. Left-button handler of the stash / hireling inventory strip: with the pointer in the
// button rectangle it closes the open panel (0xBF8C, count check, 0x3CC60 / 0x74F90 / 0xBE2E0 when 0xFADD8 was set, then the 0xD1B0 / 0x8A4D0
// HUD offset); outside it the 0x74E80 / 0x9CE80 / 0x9E5D0 chain, else the list walk over the 5-entry slots of every node (list 0x11BFD8, next at
// +0xCD) paired with the panel list 0x11BC14 (next at +0x80). The message is marked handled (+0x18 = 1, +0x1C = 0) and passed to 0xBE84
// (Storm ordinal 511) on every path but the first (pointer right of half the screen width: +0x18 := 0, no call).
#include <cstdint>
#include "cl_env.h"
namespace { constexpr uint32_t G(uint32_t rva) { return 0x6FAB0000u + rva; } constexpr uint32_t kT = 0xB2000000u; }
extern "C" void d2c_774d0(uint32_t m, const cl::Env& e) {
    const uint32_t head = e.R(G(0x11BFD8));
    const uint32_t unit = e.R(G(0x11BBFC));
    uint32_t b = e.R(G(0x11B9A0)), x = e.R(G(0x11B828)), y = e.R(G(0x11B824)), pn = e.R(G(0x11BC14));
    const uint32_t old = e.R(G(0xFADD8));
    e.W(G(0x11BFF4), 0);
    const int32_t w = (int32_t)e.R(G(0xDBC48));
    if ((int32_t)x >= (w - (w >> 31)) >> 1) { e.W(m + 0x18, 0); return; }
    auto clr = [&] { e.W(G(0x11C3DC), 0); e.W(G(0x11C3E0), 0); e.W(G(0x11C3F8), 0); e.W(G(0x11C3FC), 0); };
    auto done = [&](bool c) { if (c) clr(); e.W(m + 0x18, 1); e.W(m + 0x1C, 0); e.H(0xBE84, m); };
    const int32_t sx = (int32_t)x, sy = (int32_t)y, sb = (int32_t)b;
    if (sx >= sb + 0x116 && sx <= sb + 0x136 && sy >= 0x185 - (int32_t)e.R(G(0x11B9A4)) && sy <= 0x1A5 - (int32_t)e.R(G(0x11B9A4))) {
        e.H(0xBF8C);
        if (e.R(G(0xDD5D0)) != 0x26) {
            const uint32_t s = e.H(0xBF92, 0x139B);
            e.H(0xBF80, kT | 0xD359F, s);
            e.H(0x3664, 0xFFFFFFFFu);
            return;
        }
        e.W(G(0xFADD8), 0);
        if (old) { const uint32_t hw = e.H(0xD0B4); e.H(0x3CC60, hw, kT | 0xD087C, 0, 0, 0, 2); e.H(0x74F90); e.H(0xBE2E0); }
        uint32_t ecx, sp;
        if (!e.R(G(0xFAD84)) && !e.R(G(0xFAD90))) {
            e.W(G(0x11C418), 0); e.W(G(0x11C414), 0); e.H(0xD1B0, 0, 0, 0, 0, 0, 0, 0);
            ecx = e.R(G(0xF9E14)); sp = 0;
        } else {
            const uint32_t ww = e.R(G(0xDBC48));
            const uint32_t q = (uint32_t)(((int32_t)ww + (int32_t)((ww >> 31) & 3)) >> 2);
            e.W(G(0x11C418), 0u - q); e.W(G(0x11C414), 1); e.H(0xD1B0, 0, 0, 0, 0, 0, 0, 0u - q);
            const uint32_t a = e.R(G(0xF9E14)); const uint32_t t = (uint32_t)(((int32_t)a + (int32_t)((a >> 31) & 3)) >> 2);
            ecx = a - t; sp = 0u - t;
        }
        e.H(0x8A4D0, sp, 0, 0, 0, 0, e.R(G(0xF9E18)), ecx, 0, 0, e.R(G(0x11C3C4)));
        done(true);
        return;
    }
    if (e.H(0x74E80, 0x23, 0xB, 0, 0, 0, 0xE3, 0, 0x35) != 0 && (e.H(0x9CE80, 0, 0, 0, 0, 0, pn) & 0xFFFF) != 0xFFFF) {
        e.H(0x9E5D0);
        done(true);
        return;
    }
    if (e.R(G(0x11BFFC)) == 0) {
        for (uint32_t n = head; n; n = e.R(n + 0xCD))
            for (uint32_t i = 0; i < 5; ++i) { const uint32_t p = n + 0x1C + 0x20 * i; if (e.R(p)) { e.W(p - 0xC, e.R(p - 0xC) - 1); e.W(p, 0); } }
        done(true);
        return;
    }
    uint32_t pnode = pn ? e.R(pn + 0x80) : 0, n = head;
    if (pn && pnode && n) {
        for (;;) {
            for (uint32_t i = 0; i < 5; ++i) {
                const uint32_t p = n + 0x20 * i;
                const int32_t xo = (int32_t)(e.R(p + 4) + b);
                const int32_t yo = (int32_t)(e.R(p + 8) - e.R(G(0x11B9A4)));
                if (e.R(p) == 0) {
                    if ((int32_t)x < xo || (int32_t)x > xo + 0x14 || (int32_t)y < yo - 0x14 || (int32_t)y > yo) continue;
                    const uint32_t kind = e.R(p + 0x14);
                    bool go = true;
                    if (kind == 1) {
                        if (!pn || (e.R(pn + 0x20) & 0xFFFF) < 9) { done(false); return; }
                        const uint32_t r = e.H(0xC1F0, unit);
                        if (r == 0 || e.H(0xC208, r) == 0) go = false;
                    }
                    if (go) { if (e.H(0x773A0, 0, 0, 0, 0, 0, kind, pnode) != 0) e.W(p + 0x10, e.R(p + 0x10) + 2); }
                    x = e.R(G(0x11B828)); b = e.R(G(0x11B9A0)); y = e.R(G(0x11B824)); pn = e.R(G(0x11BC14));
                } else {
                    if ((int32_t)x < xo || (int32_t)x > xo + 0x35 || (int32_t)y < yo - 0x14 || (int32_t)y > yo) continue;
                    e.H(0x77330, 0, 0, 0, 0, 0, i, n, pnode);
                    done(true);
                    return;
                }
            }
            n = e.R(n + 0xCD); pnode = e.R(pnode + 0x80);
            if (!pnode || !n) break;
        }
    }
    e.W(G(0x11BFFC), 0);
    done(true);
}
