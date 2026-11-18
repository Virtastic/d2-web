// 1.13c D2Client+0x72E40 -- skill step replay: scales a run of ids over the two word ranges (+2..+4, +6..+8) of the skill row by
// a level pair; the id at index (D-1)*n/F gets the 0x69130 / 0x6B300 treatment, every other id the 0x69660 call. ECX = unit,
// EDX = skill. Returns the EAX the function leaves.
#include "cl_env.h"
extern "C" uint32_t d2c_72e40(uint32_t unit, int32_t skill, const cl::Env& e) {
    if (skill < 0) return 0;
    const uint32_t dt = e.dt;
    if (skill >= (int32_t)e.R(dt + 0xc4)) return dt;
    const uint32_t row = (uint32_t)skill * 0x3c + e.R(dt + 0xbc);
    if (!row) return dt;
    const uint32_t p = e.H(0xc1c0, unit, (uint32_t)skill);
    if (!p) return 0;
    int32_t D = (int32_t)e.H(0xc4b4, p, 0xa9, 0);
    const int32_t b = (int32_t)e.H(0xc4b4, p, 0x15e, 0);
    const uint32_t c = e.H(0xc4b4, p, 0x15f, 0);
    if (b < 0 || b >= (int32_t)e.R(dt + 0xba0)) return c;
    const uint32_t desc = (uint32_t)b * 0x23c + e.R(dt + 0xb98);
    if (!desc) return c;
    int32_t F = (int32_t)e.H(0xc316, unit, e.R(desc + 0x13c), (uint32_t)b, c);
    if (F <= 0) return (uint32_t)F;
    int32_t lo = e.R16s(row + 2), hi = e.R16s(row + 4);
    int32_t n = hi - lo + 1;
    if (F >= n) F = n;
    if (D >= n) D = n;
    uint32_t eax = 0;
    for (int part = 0; part < 2; part++) {
        if (part) { lo = e.R16s(row + 6); hi = e.R16s(row + 8); n = hi - lo + 1; }
        if (lo < 0 || hi < lo) { if (part) return (uint32_t)hi; continue; }
        eax = (uint32_t)((int32_t)((uint32_t)(D - 1) * (uint32_t)n) / F);
        const int32_t g = (int32_t)eax;
        int32_t s = lo;
        for (int32_t i = 0; i < n; i++, s++) {
            if (i == g) {
                eax = e.H(0x69130, 0, 0, 0, 0, 0, unit, (uint32_t)s);
                if (!eax) eax = e.H(0x6b300, unit, 3, 0, 0, 0, (uint32_t)s);
            } else {
                eax = e.H(0x69660, 0, 0, 0, 0, 0, eax, 0, (uint32_t)s, 0, 0);
                e.Edj(0x69660, unit);
            }
        }
    }
    return eax;
}
