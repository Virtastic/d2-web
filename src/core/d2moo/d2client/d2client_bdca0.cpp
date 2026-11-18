// 1.13c D2Client+0xBDCA0 -- the stats panel's mouse-button handler: stack msg (x u16 +0xC, y u16 +0xE; +0x18 / +0x1C written),
// callee pops 4. gl block: +0x00 [0x11B9A0] panel x, +0x04 [0x11B9A4] panel y, +0x08 [0xDBC48] width, +0x0C [0xDBC4C] height,
// +0x10 [0x11BBC4], +0x14 [0xFAD84], +0x18 [0xFAD88], +0x20 [0xFAD90], +0x24 [0xDD5D0], +0x28 [0x11C414], +0x2C [0x11C418],
// +0x30 [0xF9E14], +0x34 [0xF9E18], +0x38 [0x11C3C4], +0x3C [0x11BBFC] the player, +0x40 [0x11C3DC] [0x11C3E0], +0x48 [0x11C3F8]
// [0x11C3FC], +0x50 the 4 stat-button rows at 0xDD810 (14 bytes: x dword, y dword, a dword cleared on each test, stat id word).
// 0x16300(msg) first, [0x11BBC4] := 0. The close box (x0+0x80..x0+0xA0, y0+H-0x5C..y0+H-0x3C) closes the panel: 0xBF8C, the panel
// id [0xDD5D0] must be 0x26 (else Fog #10265(0x139B), #10024, exit), [0xFAD88] := 0 and, when it was set, 0xBF720, 0x3CC60(EAX = 2,
// 0xD0B4(table 0xD0628)), 0xBE2E0. The view then recentres: with neither [0xFAD84] nor [0xFAD90] set, offset 0 (0xD1B0(ECX = 0),
// 0x8A4D0(EAX = [0xF9E18], ECX = [0xF9E14], EDX = 0, ESI = [0x11C3C4], 0)); else offset -(W/4) and ([0xF9E14] - q, -q) with q its /4.
// Inside the panel (x0..W/2-1, y0..y0+H-0x31): with stat points (0xC250(pl, 4, 0)) a hit on a row (y0+H+y-0x1F6 < y < ...-0x1E0,
// x0+x < x < x0+x+0x28) spends one point, or all of #10973 stat 4 when shift is down (GetKeyState(VK_SHIFT), IAT 0xCF104), in
// packets 0x3A [count-1, id] of at most 0x20 by 0x143E0(&pkt, EBX = 3). Every in-panel click clears the four drag globals and sets
// msg+0x18 = 1, msg+0x1C = 0, then 0xBE84(msg). Elsewhere 0xBCB30(ESI = msg) == 0 clears msg+0x18.
#include <cstdint>
#include "cl_env.h"
namespace { constexpr uint32_t kMod = 0xB2000000u; }
extern "C" void d2c_bdca0(uint32_t msg, const cl::Env& e) {
    const uint32_t g = e.gl;
    const int32_t x = (int32_t)(e.R(msg + 0xc) & 0xffff), y = (int32_t)(e.R(msg + 0xc) >> 16);
    e.H(0x16300, msg);
    const int32_t x0 = (int32_t)e.R(g), y0 = (int32_t)e.R(g + 4), H = (int32_t)e.R(g + 0xc);
    e.W(g + 0x10, 0);
    if (x >= x0 + 0x80 && x <= x0 + 0xa0 && y >= y0 + H - 0x5c && y <= y0 + H - 0x3c) {
        uint32_t was = e.R(g + 0x18);
        e.H(0xbf8c);
        if (e.R(g + 0x24) != 0x26) {
            uint32_t m = e.H(0xbf92, 0x139b);
            e.H(0xbf80, kMod | 0xd359fu, m);
            e.H(0x3664, 0xffffffffu);
        }
        e.W(g + 0x18, 0);
        if (was) {
            e.H(0xbf720);
            uint32_t t = e.H(0xd0b4, kMod | 0xd0628u);
            e.H(0x3cc60, t, 0, 0, 0, 0, 2);
            e.H(0xbe2e0);
        }
        if (!e.R(g + 0x14) && !e.R(g + 0x20)) {
            e.W(g + 0x2c, 0); e.W(g + 0x28, 0);
            e.H(0xd1b0, 0, 0, 0, 0, 0, 0, 0);
            e.H(0x8a4d0, 0, 0, 0, 0, 0, e.R(g + 0x34), e.R(g + 0x30), 0, 0, e.R(g + 0x38));
        } else {
            int32_t v = -((int32_t)e.R(g + 8) / 4);
            e.W(g + 0x2c, (uint32_t)v); e.W(g + 0x28, 1);
            e.H(0xd1b0, 0, 0, 0, 0, 0, 0, (uint32_t)v);
            int32_t c = (int32_t)e.R(g + 0x30), q = c / 4;
            e.H(0x8a4d0, (uint32_t)-q, 0, 0, 0, 0, e.R(g + 0x34), (uint32_t)(c - q), 0, 0, e.R(g + 0x38));
        }
        return;
    }
    if (x >= x0 && x <= (int32_t)e.R(g + 8) / 2 - 1 && y >= y0 && y <= y0 + H - 0x31) {
        uint32_t pl = e.R(g + 0x3c);
        if (e.H(0xc250, pl, 4, 0)) {
            for (uint32_t i = 0; i < 4; i++) {
                uint32_t r = g + 0x50 + 0xe * i;
                int32_t c = (int32_t)(e.R(r + 4) + (uint32_t)y0 + (uint32_t)H);
                e.W(r + 8, 0);
                if (y <= c - 0x1f6 || y >= c - 0x1e0) continue;
                int32_t cx = (int32_t)(e.R(r) + (uint32_t)x0);
                if (x <= cx || x >= cx + 0x28) continue;
                int32_t n = (int32_t)e.H(0xc1c6, pl, 4, 0);
                if ((int16_t)e.H(0xcf104, 0x10) >= 0) n = 1;
                uint16_t id = (uint16_t)e.R(r + 0xc);
                while (n != 0) {
                    int32_t k = n < 0x20 ? n : 0x20;
                    n -= k;
                    uint32_t pkt = 0x3au | (uint32_t)(uint16_t)(id | (uint16_t)(((k - 1) & 0xff) << 8)) << 8;
                    e.H(0x143e0, (uint32_t)(uintptr_t)&pkt, 0, 0, 0, 0, 0, 0, 0, 3);
                    if (e.blk) e.blk(e.ctx, 0x143e0, (uint32_t)(uintptr_t)&pkt, 1);
                }
                break;
            }
        }
        e.W(g + 0x40, 0); e.W(g + 0x44, 0); e.W(g + 0x48, 0); e.W(g + 0x4c, 0);
        e.W(msg + 0x18, 1); e.W(msg + 0x1c, 0);
        e.H(0xbe84, msg);
        return;
    }
    if (!e.H(0xbcb30, 0, 0, 0, 0, 0, 0, 0, 0, 0, msg)) e.W(msg + 0x18, 0);
}
