// 1.13c D2Client+0x5C7F0: per-frame hireling HUD. void, no args. Globals are read at
// their retail addresses (0x6FAB0000 + rva) through e.R/e.W; the three stack locals x/y/w (0, 0x48, 0xF) are the painters'
// shared out-block and stay real stack memory so each painter is handed the same pointer retail hands it.
#include <cstdint>
#include "cl_env.h"
namespace {
constexpr uint32_t G(uint32_t rva) { return 0x6FAB0000u + rva; }
inline uint32_t P(const volatile void* p) { return (uint32_t)(uintptr_t)p; }
constexpr uint32_t kTag = 0xB2000000u;
}
extern "C" void d2c_5c7f0(const cl::Env& e) {
    volatile uint32_t l[3] = { 0, 0x48, 0xF };   // [esp+0x10] x, [esp+0x14] y, [esp+0x18] w
    const uint32_t unit = e.R(G(0x11BBFC));
    const uint32_t st = e.R(G(0x11C414));
    if (st == 3 || st == 2 || e.R(G(0xFADA4)) || e.R(G(0xFADAC)) || e.R(G(0x11C284)) == 2) {
        e.W(G(0x11C294), 0); e.W(G(0x11C298), 0);
        return;
    }
    const uint32_t x = P(&l[0]), y = P(&l[1]), w = P(&l[2]);
    bool fatal = false;
    if (e.R(G(0x119854)) && !e.R(G(0xFAE0C)) && unit) {
        const uint32_t uid = e.R(unit + 0xC);
        uint32_t n = e.R(G(0x11C4D4));
        while (n && !(e.R(n + 4) == 7 && e.R(n + 0xC) == uid && e.R(n + 0x20) == 0)) n = e.R(n + 0x30);
        if (n && e.R(n + 8) != 0xFFFFFFFFu && e.H(0xBF8C) && e.R(G(0x119854))) {
            if (e.R(G(0xDD5D0)) != 0x26) fatal = true;
            else if (!e.R(G(0x113A68))) {
                bool stop = false;
                uint32_t i = 0;
                do {
                    if (e.R(G(0xFAD80) + 4 * i) == 1) {
                        const uint32_t k = e.R(e.R(G(0xDD538) + 4 * i) + 0x8C);
                        if (k == 1) e.H(0xC2790, 0, 0, 0, 0, 0, 0, i, 1);
                        else if (k == 4) {
                            if (e.R(G(0x119721))) { const uint32_t edi = e.R(G(0x11971D)); e.H(0x47B20); e.Edj(0x47B20, edi); }
                        } else if (k != 0) { stop = true; break; }
                    }
                    ++i;
                } while ((int32_t)i < (int32_t)e.R(G(0xDD5D0)));
                if (!stop) {
                    const uint32_t u = e.R(G(0x11BBFC));
                    bool fire;
                    if (!u || (e.R(u) == 0 && e.H(0xA5440) == 0)) { e.W(G(0xFAE0C), 1); fire = true; }
                    else fire = e.R(G(0xFAE0C)) != 0;
                    if (fire) { const uint32_t hw = e.H(0xD0B4); e.H(0x3CCC0, hw, kTag | 0xD0978, 0, 0, 0, 3); }
                }
            }
        }
    }
    if (!fatal) {
        uint32_t r = e.H(0x5BA90, y, x, 0, 0, 0, unit, 0, 0, w);
        r |= e.H(0x5B930, w, y, x, 0, 0, 0, y, unit);
        if (e.blk) e.blk(e.ctx, 0x5B930, w, 1);
        int32_t cnt = (int32_t)e.R(e.dt + 0xBF0);
        for (int32_t i = 0; i < cnt; ++i) {
            const uint32_t rec = e.R(e.dt + 0xBE8) + (uint32_t)i * 0xE0;
            if (rec) {
                const uint32_t kind = e.R8(rec + 0xE);
                if (kind == 1) { r |= e.H(0x5B890, w, x, 0, 0, 0, 0, 0, unit, 0, y); if (e.blk) e.blk(e.ctx, 0x5B890, x, 1); }
                else if (kind == 2) { r |= e.H(0x5B760, w, y, x, 0, 0, unit); if (e.blk) e.blk(e.ctx, 0x5B760, y, 1); }
                else if (kind == 3) r |= e.H(0x5B6C0, w, x, 0, 0, 0, 0, 0, unit, 0, y);
            }
            cnt = (int32_t)e.R(e.dt + 0xBF0);
        }
        if (r == 0) {
            const uint32_t open = e.R(G(0xFADCC));
            e.H(0xBF8C);
            if (e.R(G(0xDD5D0)) != 0x26) fatal = true;
            else {
                e.W(G(0xFADCC), 0);
                if (open) { const uint32_t hw = e.H(0xD0B4); e.H(0x3CC60, hw, kTag | 0xD0658, 0, 0, 0, 1); }
            }
        }
    }
    if (fatal) {   // panel count != 0x26: the assert path, which never returns in retail
        const uint32_t s = e.H(0xBF92, 0x139B);
        e.H(0xBF80, kTag | 0xD359F, s);
        e.H(0x3664, 0xFFFFFFFFu);
        return;
    }
    if (e.R8(G(0x11C29C)) && !e.R(G(0xFAE10)) && e.H(0xC2790, 0, 0, 0, 0, 0, 0, 0x24, 0)) {
        e.H(0xBEF6, kTag | 0xD3588, kTag | 0xD566C, 0, 1);
        e.W(G(0x11C29C), e.R(G(0x11C29C)) & 0xFFFFFF00u);
    }
    if (e.R(G(0x11C298)) && e.R(G(0x11C294))) {
        volatile uint16_t buf[0x100];
        const uint32_t b = P(&buf[0]);
        e.H(0x12E0, 2, 0, 0, 0, 0, 0x100, b);
        e.H(0xCEAFC, 0, 0, 0, 0, 0, 0, x);              // Unicode::Unicode(&x, 0)
        buf[0] = (uint16_t)l[0];
        const uint32_t sa = e.H(0xD1EC, 0, 0, 0, 0, 0, 0, 0x58A2);
        const uint32_t sb = e.H(0xD1EC, 0, 0, 0, 0, 0, 0, 0x58A3);
        const uint32_t sc = e.H(0xD1EC, 0, 0, 0, 0, 0, 0, 0xF9E);
        const uint32_t v = e.H(0x39F30, 0, 0, 0, 0, 0, 0x36, 0, 1);
        e.H(0xCEAB4, 0x100, b, sb, v);                  // Unicode::sprintf(0x100, buf, fmt, v)
        e.H(0xCEABC, 0, 0, 0, 0, 0, 0, b, sc);          // Unicode::strcat(buf, sc)
        e.H(0xCEABC, 0, 0, 0, 0, 0, 0, b, sa);
        e.H(0xD384, e.R(G(0x11C290)) + 0x24, 0, 0, 0, 0, 0, b, e.R(G(0x11C28C)));
    }
}
