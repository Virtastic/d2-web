// 1.13c D2Client+0xB0B70 (ecx = unit): ring of eight side spawns around the unit (0x59 effect, 0x1a1 body) whose
// life is cut by a seeded roll; always returns 1.
#include <cstdint>
#include "cl_env.h"
namespace {
const int32_t kY[8] = {0, 1, 1, 1, 0, -1, -1, -1};
const int32_t kX[8] = {1, 1, 0, -1, -1, -1, 0, 1};
// the unit's LCG (seed lo +0x20, hi +0x24) reduced mod m (m <= 0 leaves the seed alone and gives 0)
uint32_t roll(const cl::Env& e, uint32_t u, int32_t m) {
    if (m <= 0) return 0;
    uint64_t pr = (uint64_t)e.R(u + 0x20) * 0x6ac690c5u + e.R(u + 0x24);
    uint32_t lo = (uint32_t)pr;
    e.W(u + 0x24, (uint32_t)(pr >> 32)); e.W(u + 0x20, lo);
    return ((uint32_t)m & ((uint32_t)m - 1)) == 0 ? (((uint32_t)m - 1) & lo) : lo % (uint32_t)m;
}
}  // namespace
// 1.13c D2Client+0xB0B70
extern "C" uint32_t d2c_b0b70(uint32_t u, const cl::Env& e) {
    uint32_t ty = e.R(u), p = e.R(u + 0x2c);
    bool direct = ty == 2 || ((int32_t)ty > 3 && (int32_t)ty <= 5);
    uint32_t v1, v2;
    if (direct) { v1 = e.R(p + 0xc); v2 = e.R(p + 0x10); }
    else { v1 = p ? e.H(0xc3be, p) : 0; v2 = p ? e.H(0xc2ec, p) : 0; }
    auto spawn = [&](uint32_t a0, uint32_t a1, uint32_t a2) {
        uint32_t k = e.H(0xc3fa, u);
        uint32_t g = e.H(0xc388, u);
        return e.H(0xb0450, a0, a1, a2, g, k, 0, 0, 0, 0, u);
    };
    spawn(0x58, 0, 0);
    for (int i = 0; i < 8; i++) {
        uint32_t y = (uint32_t)kY[i] + v1, x = (uint32_t)kX[i] + v2;
        uint32_t nu = spawn(0x59, 0, 0);
        if (nu) e.H(0xc478, e.R(nu + 0x2c), y, x, 0);
        uint32_t b = spawn(0x1a1, y, x);
        if (!b) continue;
        int32_t life = (int32_t)e.H(0xc430, b);
        int32_t half = (life + (life < 0 ? 1 : 0)) >> 1;
        uint32_t cut = (uint32_t)life - roll(e, u, half);
        e.H(0xc4a2, b, cut);
        uint32_t q = e.R(b + 0x2c);
        int32_t m = (int32_t)e.H(0xc496, q);
        uint32_t r2 = roll(e, u, m);
        e.H(0xc328, q, r2, 0xB20D4010u, 0x1a6b);
        e.H(0xc7c80, 0x1e, 0, 0, 0, 0, b, cut);                                  // eax = unit, ecx = cut, stack 0x1e
    }
    return 1;
}
