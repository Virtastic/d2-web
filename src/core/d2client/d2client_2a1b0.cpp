// 1.13c D2Client+0x2A1B0: walk/target request (ESI unit, AX x; stack y, mode, tx, ty; ret 0x10). x and y are words;
// either zero, unit+0xC4 bit 0x10000, or a dead/neutral mode (type 0: mode 0/0x11, type 1: 0/0xC) leaves it. The
// target word pair goes to unit+0x8C/+0x8E. The near radius is 10 (mode 1), 0 (mode 2), for the player at 0x11BBFC
// ((global 0x119804 + 50) >> 7) + 3/7/5 by its mode 1/3/other, else 15 (type-1 modes 3-5: 5, 6-11: 7). Within the
// radius on both axes -- or, in mode 0 with tx > 0, when #10769-class distance to (x, y) is under 100 and the one to
// (tx, ty) is less -- the D2Common position pair (#C2F8 adjusts the caller's arg slots in place) is checked by
// 0x6E860 and a hit ends it. Otherwise a target from 0x1400(EAX = #C1F0(unit)) or #C2E0(global 0x11C3B8, x, y)
// goes to 0x149E0 (player; AL = 0x5F), 0x18E20 (type 0) or #CF10 (path).
#include "d2client_callee.h"
// 1.13c D2Client+0x2A1B0
extern "C" void d2c_2a1b0(const D2cSvc& v, uint32_t* unit /*ESI*/, uint32_t ax, uint32_t a0, uint32_t mode, uint32_t a2, uint32_t a3,
                          uint32_t player /*[0x11BBFC]*/, uint32_t g9804 /*[0x119804]*/, uint32_t gc3b8 /*[0x11C3B8]*/) {
    const uint32_t x = ax & 0xffff, y = a0 & 0xffff, u = (uint32_t)(uintptr_t)unit;
    if (!x || !y || (unit[0xc4 / 4] & 0x10000)) return;
    const uint32_t t = unit[4];
    if (unit[0] == 0) { if (t == 0 || t == 0x11) return; }
    else if (unit[0] == 1) { if (t == 0 || t == 0xc) return; }
    ((uint16_t*)unit)[0x8c / 2] = (uint16_t)x;
    ((uint16_t*)unit)[0x8e / 2] = (uint16_t)y;
    const uint32_t obj = d2c_call(v, 0xc1f0, 0, 0, 0, 0, 0, u);
    const uint32_t x0 = d2c_call(v, 0x1630, 0, u, 0, 0, 0);
    const uint32_t y0 = d2c_call(v, 0x1660, 0, u, 0, 0, 0);
    int32_t lim;
    if (mode == 1) lim = 10;
    else if (mode == 2) lim = 0;
    else if (u == player) lim = (int32_t)((g9804 + 0x32) >> 7) + (t == 1 ? 3 : t == 3 ? 7 : 5);
    else { lim = 15; if (unit[0] == 1 && t - 3 <= 8) lim = t - 3 <= 2 ? 5 : 7; }
    int32_t dx = (int32_t)(x - x0); if (dx < 0) dx = -dx;
    int32_t dy = (int32_t)(y - y0); if (dy < 0) dy = -dy;
    bool near = dx <= lim && dy <= lim;
    if (!near && mode == 0 && (int32_t)a2 > 0) {
        const int32_t d = (int32_t)d2c_call(v, 0xc6fa, 0, 0, 0, 0, 0, x0, y0, x, y);
        if (d < 100 && (int32_t)d2c_call(v, 0xc6fa, 0, 0, 0, 0, 0, x0, y0, a2, a3) < d) near = true;
    }
    if (near) {
        const uint32_t r1 = d2c_call(v, 0xc1d2, 0, 0, 0, 0, 0, u);
        const uint32_t r2 = d2c_call(v, 0xc1f6, 0, 0, 0, 0, 0, u);
        uint32_t s0 = y, s2 = x;   // the caller's arg0/arg2 slots, adjusted in place by #C2F8
        d2c_call(v, 0xc2f8, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)&s2, (uint32_t)(uintptr_t)&s0);
        if (x == x0 || y == y0) return;
        if (d2c_call(v, 0x6e860, r2, u, r1, 0, 0)) return;
        if (d2c_call(v, 0x6e860, s0, u, s2, 0, 0)) return;
    }
    uint32_t tg = d2c_call(v, 0x1400, obj, 0, 0, 0, 0);
    if (!tg && !(tg = d2c_call(v, 0xc2e0, 0, 0, 0, 0, 0, gc3b8, x, y))) return;
    if (u == player) {
        const uint32_t py = d2c_call(v, 0x1660, 0, u, 0, 0, 0);
        const uint32_t px = d2c_call(v, 0x1630, 0, u, 0, 0, 0);
        d2c_call(v, 0x149e0, (px & 0xffffff00u) | 0x5f, px, py, 0, 0);
        return;
    }
    if (unit[0] == 0) { d2c_call(v, 0x18e20, u, 0, 0, 0, 0, tg, x, y); return; }
    d2c_call(v, 0xcf10, 0, 0, 0, 0, 0, unit[0x2c / 4], u, tg, x, y);
}
