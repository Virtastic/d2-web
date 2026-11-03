// 1.13c D2Client+0x2BB50 -- scatter skill: ECX = unit, EDX = skill index, [esp+4] = A, ret 4.
// Gates: 0xC1F0(unit) non-zero and 0xC208 of it zero, a valid skills row, a class from 0x40550 inside 0xB6C. n = 0xC316(unit, row+0x64, i, A) and
// M = 0xC316(unit, row+0x64, i, 1) must be positive; COUNT = (n*n / (M*M)) / (roll(0x12) + 1). Each of COUNT draws takes two 0x1380(2n+1) rolls
// (offsets dx, dy from the centre n); inside the circle dx*dx + dy*dy < n*n a 0xB30F0 request (kind 0x2001) is made and the new object's path gets a
// 0xC3C4 with a roll(0x40).
#include "cl_mb2.h"
extern "C" uint32_t d2c_2bb50(uint32_t u, int32_t i, uint32_t A, const clm::Env& e) {
    uint32_t a = e.H(0xc1f0, 0, 0, 0, 0, 0, u);
    if (!a) return 0;
    if (e.H(0xc208, 0, 0, 0, 0, 0, a)) return 0;
    uint32_t row = e.SkillRow(i);
    if (!row) return 0;
    int32_t C0 = (int32_t)e.H(0x40550, (uint32_t)i, 0, 0, 0, 0, u);
    if (C0 < 0 || C0 >= (int32_t)e.R(e.dt + 0xb6c)) return 0;
    int32_t n = (int32_t)e.H(0xc316, 0, 0, 0, 0, 0, u, e.R(row + 0x64), (uint32_t)i, A);
    if (n <= 0) return 0;
    int32_t M = (int32_t)e.H(0xc316, 0, 0, 0, 0, 0, u, e.R(row + 0x64), (uint32_t)i, 1);
    if (M <= 0) return 0;
    int32_t n2 = n * n;
    int32_t rnd = (int32_t)e.H(0x1330, 0x12, u + 0x20);
    int32_t count = (n2 / (M * M)) / (rnd + 1);
    uint32_t X0 = e.H(0x1630, 0, u), Y0 = e.H(0x1660, 0, u);
    uint32_t q[23] = {};
    q[0] = 0x2001; q[1] = u; q[4] = (uint32_t)C0; q[11] = (uint32_t)i; q[12] = A;
    for (; count > 0; count--) {
        int32_t dx = (int32_t)e.H(0x1380, 0, u + 0x20, 0, 0, (uint32_t)(2 * n + 1)) - n;
        int32_t dy = (int32_t)e.H(0x1380, 0, u + 0x20, 0, 0, (uint32_t)(2 * n + 1)) - n;
        if ((int32_t)((uint32_t)dy * (uint32_t)dy + (uint32_t)dx * (uint32_t)dx) >= n2) continue;
        q[5] = (uint32_t)dx + X0 + 2; q[6] = (uint32_t)dy + Y0 + 2;
        uint32_t s = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
        if (!s) continue;
        uint32_t r40 = e.H(0x1330, 0x40, u + 0x20);
        e.H(0xc3c4, 0, 0, 0, 0, 0, e.R(s + 0x2c), r40);
    }
    return 1;
}
