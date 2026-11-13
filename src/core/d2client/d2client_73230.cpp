// 1.13c D2Client+0x73230 -- skill level change replay: from two skill values of the unit's skill handle it derives a new level
// (1..3) and stores it; a rise starts the skill's cast sounds/effects (0x72A50, 0x6B300 x3), a fall undoes the extra levels
// with 0x727F0. ECX = unit, EDX = skill. Returns the EAX the function leaves.
#include "cl_env.h"
extern "C" uint32_t d2c_73230(uint32_t unit, int32_t skill, const cl::Env& e) {
    if (skill < 0) return 0;
    const uint32_t dt = e.dt;
    if (skill >= (int32_t)e.R(dt + 0xc4)) return dt;
    const uint32_t row = (uint32_t)skill * 0x3c + e.R(dt + 0xbc);
    if (!row) return dt;
    const uint32_t p = e.H(0xc1c0, unit, (uint32_t)skill);
    if (!p) return 0;
    const int32_t a = (int32_t)e.H(0xc4b4, p, 0x84, 0);
    const int32_t b = (int32_t)e.H(0xc4b4, p, 0x85, 0);
    auto W = [&](uint32_t o) { const int32_t v = e.R16s(row + o); return v < 0 ? -1 : v; };
    const int32_t S0 = W(0xa), S1 = W(2), S2 = W(4), S3 = W(6), S4 = W(8);
    const int32_t cur = (int32_t)e.H(0xc4b4, p, 0x163, 0);
    const int32_t q = b / 3;
    const int32_t n = (a >= q ? 3 : 2) - (a >= 2 * q ? 0 : 1);
    uint32_t v = e.H(0xc562, p, 0x163, (uint32_t)n, 0);
    if (n == cur) return v;
    if (n < cur) {
        for (int32_t k = cur - n; k > 0; k--) {
            e.H(0x727f0, (uint32_t)S1, 0, 0, 0, 0, unit);
            v = e.H(0x727f0, (uint32_t)S2, 0, 0, 0, 0, unit);
        }
        return v;
    }
    e.H(0x72a50, 0, 0, 0, 0, 0, 0, unit, (uint32_t)skill);
    uint32_t eax = v;
    if (S3 != -1 && n >= 3) eax = e.H(0x6b300, unit, 7, 0, 0, 0, (uint32_t)S3);
    if (S4 != -1 && n >= 2) eax = e.H(0x6b300, unit, 7, 0, 0, 0, (uint32_t)S4);
    eax = (uint32_t)S0;
    if (S0 == -1 || S1 == -1 || S2 == -1) return eax;
    return e.H(0x6b300, unit, 4, 0, (uint32_t)S1, (uint32_t)S2, (uint32_t)S0);
}
