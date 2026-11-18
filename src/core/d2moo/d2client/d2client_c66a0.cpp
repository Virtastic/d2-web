// 1.13c D2Client+0xC66A0, written from the disassembly. EAX = skill index, stack (unit, a1, px, py), ret 0x10: find a spot near (*px, *py) for the unit.
// A skill index outside the skills table (0xBA0 count / 0xB98 base, stride 0x23C) or an absent row returns 0. W / Hh are the unit path's width / height
// (types 2, 4, 5: path +0xC / +0x10; else 0xC3BE / 0xC2EC of the path, 0 without one). n = 0xC316(unit, row+0x64, idx, a1); d = max(0xC4D8(unit, *px, *py), 1).
// d > n scales the point toward the path origin: *px = (*px - W) * n / d + W, *py = (*py - Hh) * n / d + Hh. c30 = clamp(*py - Hh, -2, 2),
// c14 = clamp(W - *px, -2, 2). Three passes try the point (*px, *py), (*px + c30, *py + c14), (*px - c30, *py - c14): 0xC676(0x1400(EAX = 0xC1F0(unit), EBX = x),
// &{x, y}, 0xC4C0(unit), 0x1C09, 1) rewrites the point and must be non-zero, 0xC2E6(r, x, y, 0xCD30(unit), 0x1C09) zero, 0xCD42(W, Hh, x, y, r, 0x804) non-zero and
// 0xC4D8(unit, x, y) <= n: then *px, *py := x, y and 1. After the three passes a type-0 unit gets 0x23200(unit, 0x13); returns 0.
#include "cl_mb2.h"
static int32_t clamp2(int32_t v) { return v > -2 ? (v >= 2 ? 2 : v) : -2; }
// 1.13c D2Client+0xC66A0
extern "C" uint32_t d2c_c66a0(uint32_t idx, uint32_t u, uint32_t a1, uint32_t px, uint32_t py, const clm::Env& e) {
    if ((int32_t)idx < 0) return 0;
    const uint32_t row = e.SkillRow((int32_t)idx);
    if (!row) return 0;
    const int32_t t = (int32_t)e.R(u);
    const bool box = t == 2 || (t > 3 && t <= 5);
    const uint32_t path = e.R(u + 0x2c);
    const uint32_t w = box ? e.R(path + 0xc) : (path ? e.H(0xc3be, 0, 0, 0, 0, 0, path) : 0u);
    const uint32_t hh = box ? e.R(path + 0x10) : (path ? e.H(0xc2ec, 0, 0, 0, 0, 0, path) : 0u);
    const int32_t n = (int32_t)e.H(0xc316, 0, 0, 0, 0, 0, u, e.R(row + 0x64), idx, a1);
    int32_t d = (int32_t)e.H(0xc4d8, 0, 0, 0, 0, 0, u, e.R(px), e.R(py));
    if (d < 1) d = 1;
    if (d > n) {
        const int32_t sy = (int32_t)(e.R(py) - hh);
        const int32_t sx = (int32_t)(e.R(px) - w);
        e.W(px, (uint32_t)((int32_t)((uint32_t)sx * (uint32_t)n) / d) + w);
        e.W(py, (uint32_t)((int32_t)((uint32_t)sy * (uint32_t)n) / d) + hh);
    }
    const int32_t c30 = clamp2((int32_t)(e.R(py) - hh));
    const int32_t c14 = clamp2((int32_t)(w - e.R(px)));
    uint32_t xs = e.R(px), ys = e.R(py);
    for (int iter = 3;;) {
        const uint32_t f = e.H(0xc1f0, 0, 0, 0, 0, 0, u);
        const uint32_t s = e.H(0x1400, f, 0, 0, xs);
        const uint32_t k = e.H(0xc4c0, 0, 0, 0, 0, 0, u);
        uint32_t pt[2] = { xs, ys };
        const uint32_t r = e.H(0xc676, 0, 0, 0, 0, 0, s, (uint32_t)(uintptr_t)pt, k, 0x1c09, 1);
        const uint32_t ox = pt[0], oy = pt[1];
        if (r) {
            const uint32_t cd = e.H(0xcd30, 0, 0, 0, 0, 0, u);
            if (!e.H(0xc2e6, 0, 0, 0, 0, 0, r, ox, oy, cd, 0x1c09) &&
                e.H(0xcd42, 0, 0, 0, 0, 0, w, hh, ox, oy, r, 0x804) &&
                (int32_t)e.H(0xc4d8, 0, 0, 0, 0, 0, u, ox, oy) <= n) {
                e.W(px, ox); e.W(py, oy);
                return 1;
            }
        }
        --iter;
        if (iter == 2) { xs = e.R(px) + (uint32_t)c30; ys = e.R(py) + (uint32_t)c14; }
        else if (iter == 1) { xs = e.R(px) - (uint32_t)c30; ys = e.R(py) - (uint32_t)c14; }
        else if (iter == 0) {
            if (e.R(u) == 0) e.H(0x23200, 0, 0, 0, 0, 0, u, 0x13);
            return 0;
        }
    }
}
