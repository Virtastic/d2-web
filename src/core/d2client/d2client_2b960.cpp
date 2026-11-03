// 1.13c D2Client+0x2B960 -- ECX = unit, EDX = P2, [esp+4] = Z, ret 4. Walks N steps (0xCB9E of the owner) of a vector (X, Y) from 0xCEF2(unit),
// turning the unit's facing (+0x44 >> 8) one step per iteration and issuing 0x6CC00(unit, P2; x, y, 1, 0) at the stepped position.
// Z selects the side of the vector (Y >= 0 / Y < 0 returns early) and whether the offset runs from the full length or per step.
#include "cl_mb2.h"
extern "C" uint32_t d2c_2b960(uint32_t u, uint32_t P2, uint32_t Z, const clm::Env& e) {
    uint32_t owner = e.H(0xc310, 0, 0, 0, 0, 0, u);
    if (!owner) return 0;
    uint32_t A1 = e.H(0xc1d2, 0, 0, 0, 0, 0, u);
    uint32_t B1 = e.H(0xc1f6, 0, 0, 0, 0, 0, u);
    int32_t XY[2] = { 0, 0 };   // XY[0] = X (stack word 1), XY[1] = Y (stack word 2)
    uint32_t rc = e.H(0xcef2, 0, 0, 0, 0, 0, u, (uint32_t)(uintptr_t)&XY[0], (uint32_t)(uintptr_t)&XY[1]);
    int32_t X = XY[0], Y = XY[1];
    if (Z == 0) { if (Y >= 0) return rc; }
    else if (Y < 0) return rc;
    int32_t ax_ = X < 0 ? -X : X, ay_ = Y < 0 ? -Y : Y;
    int32_t big = ax_ > ay_ ? ax_ : ay_, small = ax_ > ay_ ? ay_ : ax_;
    int32_t den = (small + 2 * big) >> 1;
    if (den == 0) den = 1;
    int32_t qX = X * 15 / den, qY = Y * 15 / den;
    uint32_t d7e = e.H(0xd07e);
    int32_t dir = (int32_t)e.R(u + 0x44) >> 8;
    int32_t n = (int32_t)e.H(0xcb9e, 0, 0, 0, 0, 0, owner);
    if (n <= 0) { e.W(u + 0x44, (uint32_t)dir << 8); return (uint32_t)n; }
    int32_t acc1 = n * qY, acc2 = n * qX, curX = qX, curY = qY, d = dir;
    for (int32_t cnt = n; cnt != 0; cnt--) {
        int32_t ox = 0, oy = 0;
        d--;
        if (d < 0) d += 8;
        e.W(u + 0x44, (uint32_t)d << 8);
        int32_t ax, cx;
        if (d7e) {
            int32_t o[2] = { 0, 0 };
            uint32_t r = e.H(0xc232, 0, 0, 0, 0, 0, u, 0x60, (uint32_t)(uintptr_t)&o[0], (uint32_t)(uintptr_t)&o[1], 0);
            uint32_t r2 = e.H(0xc1ae, 0, 0, 0, 0, 0, u, r);
            e.H(0xd132, 0, 0, 0, 0, 0, r2);
            ox = o[0]; oy = o[1];
            ax = ox + (int32_t)(e.G(0x119960) - e.G(0x11c418));
            cx = oy + (int32_t)e.G(0x11995c) - 8;
        } else { ax = (int32_t)A1; cx = (int32_t)B1; }
        if (Z != 0) { ax -= acc2; cx -= acc1; } else { ax -= curX; cx -= curY; }
        e.H(0x6cc00, 0, u, P2, 0, 0, (uint32_t)ax, (uint32_t)cx, 1, 0);
        curX += qX; curY += qY; acc2 -= qX; acc1 -= qY;
    }
    e.W(u + 0x44, (uint32_t)dir << 8);
    return 0;
}
