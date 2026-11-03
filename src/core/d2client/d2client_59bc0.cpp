// 1.13c D2Client+0x59BC0 -- the click-state pass (no args). gl layout (module slot RVA): +0x00 X 0x11B828, +0x04 Y 0x11B824, +0x08 mode 0x11C2CC,
// +0x0C 0xFAD60, +0x10 originX 0x11B9A0, +0x14 originY 0x11B9A4, +0x18 player 0x11BBFC, +0x1C pending 0xFADDC, +0x20 version 0xDD5D0,
// +0x24 0x11C418, +0x28 0x11C414, +0x2C 0xF9E18, +0x30 0xF9E14, +0x34 0x11C3C4, +0x38 0x12349C, +0x3C 0x123498, +0x40 0x11C434,
// +0x44 0x11BE74, +0x48 box slot 0x1177F0, +0x4C box slot 0x1177E8.
// 1) mode > 2 with 0xFAD60 == 0: three hot rects (X/Y against origin-relative bounds, signed) each draw a string (0xD1EC id, 0xD384 at x, y).
// 2) a player: 0xC1F0(player), then 0xC1DE, then 0xCC5E; a missing unit, type 0x11 or a zero chain result falls to the close path: with 0xFADDC set,
//    send {0x4F,2,0,0,0,0,0} (0x143E0, EBX = 7), the version check (0xDD5D0 == 0x26 else the fatal triple, which returns here),
//    0xFADDC = 0, and when it was set and 0x59830 == 0 the teardown (0xD1B0, 0x8A4D0, 0xBF720, 0xD0B4/0x3CC60, 0xBE2E0, 0x59740, ...).
// 3) otherwise mode 1/2 outside 3..6 with its box slot set calls 0xA3840 (ESI = the slot).
#include "cl_env.h"
extern "C" void d2c_59bc0(const cl::Env& e) {
    const uint32_t g = e.gl;
    const int32_t X = (int32_t)e.R(g), Y = (int32_t)e.R(g + 4);
    if ((int32_t)e.R(g + 8) > 2 && e.R(g + 0xc) == 0) {
        const uint32_t ox = e.R(g + 0x10), oy = e.R(g + 0x14);
        auto in = [&](uint32_t x0, uint32_t x1, uint32_t y0, uint32_t y1) {
            return (int32_t)(ox + x0) <= X && X <= (int32_t)(ox + x1) && (int32_t)(y0 - oy) <= Y && Y <= (int32_t)(y1 - oy);
        };
        auto draw = [&](uint32_t id, uint32_t x, uint32_t y) {
            const uint32_t s = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, id);
            e.H(0xd384, y, 0, 0, 0, 0, 0, s, x);
        };
        if (in(0x111, 0x133, 0xa8, 0xca)) draw(0x1023, ox + 0xef, 0xa7 - oy);
        if (in(0xdd, 0xff, 0x17f, 0x1a1)) draw(0x1023, ox + 0xbb, 0x17e - oy);
        if (in(0x111, 0x133, 0x17f, 0x1a1)) draw(0x1022, ox + 0x107, 0x17e - oy);
    }
    bool close = false;
    const uint32_t u = e.R(g + 0x18);
    if (u) {
        const uint32_t r = e.H(0xc1f0, u);
        const uint32_t u2 = e.R(g + 0x18);
        if (!u2 || e.R(u2 + 0x10) == 0x11) {
            close = true;
        } else if (r) {
            const uint32_t r2 = e.H(0xc1de, r);
            if (!e.H(0xcc5e, r2)) close = true;
        }
    }
    if (!close) {
        const int32_t m = (int32_t)e.R(g + 8);
        if ((m >= 3 && m <= 6) || m == 0) return;
        uint32_t slot;
        if (m == 1) slot = e.R(g + 0x48);
        else if (m == 2) slot = e.R(g + 0x4c);
        else return;
        if (slot) e.H(0xa3840, 0, 0, 0, 0, 0, 0, 0, 0, 0, slot);
        return;
    }
    const uint32_t pend = e.R(g + 0x1c);
    if (!pend) return;
    const uint8_t pk[8] = {0x4f, 2, 0, 0, 0, 0, 0, 0};
    e.H(0x143e0, (uint32_t)(uintptr_t)pk, 0, 0, 0, 0, 0, 0, 0, 7, 0);
    e.H(0xbf8c);
    if (e.R(g + 0x20) != 0x26) {
        const uint32_t m = e.H(0xbf92, 0x139b);
        e.H(0xbf80, 0xB20D359Fu, m);
        e.H(0x3664, 0xffffffffu);
    }
    e.W(g + 0x1c, 0);
    if (!e.H(0x59830)) {
        e.W(g + 0x24, 0);
        e.W(g + 0x28, 0);
        e.H(0xd1b0, 0, 0, 0, 0, 0, 0, 0);
        e.H(0x8a4d0, 0, 0, 0, 0, 0, e.R(g + 0x2c), e.R(g + 0x30), 0, 0, e.R(g + 0x34));
        e.H(0xbf720);
        uint32_t s = e.H(0xd0b4);
        e.H(0x3cc60, s, 0xB20DBD18u, 0, 0, 0, 0xd);
        e.H(0xbe2e0);
        e.H(0x59740, 0);
        e.W(g + 0x40, 1);
        if (!e.R(g + 0x38)) {
            e.W(g + 0x38, 1);
            if (e.R(g + 0x3c)) {
                s = e.H(0xd0b4);
                e.H(0x3cc60, s, 0xB20F51ECu, 0, 0, 0, 2);
                e.W(g + 0x3c, 0);
            }
            s = e.H(0xd0b4);
            e.H(0x3ccc0, s, 0xB20F51BCu, 0, 0, 0, 4);
        }
    }
    e.W(g + 0x44, 0);
    e.H(0x8bb90);
    e.W(g + 8, 0);
}
