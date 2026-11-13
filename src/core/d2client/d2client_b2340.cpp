// 1.13c D2Client+0xB2340 (ecx = unit): n = 0xC406(unit); the first row of the descriptor table whose limit is below n (the last row when none is) gives a modulus and a
// value. When n is a multiple of the modulus and the unit has a 0xA60D0 record, a 23-dword request {1, record, ., ., 0x15C, x, y, ..., 0xC388(unit), 0xC3FA(unit)} goes to
// 0xB30F0 (ECX = the block); a non-null result gets 0xC448(result, value) and 0xC394(result, n). Always ends by tail-calling 0xB17C0 with ECX = unit.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB2340
extern "C" uint32_t d2c_b2340(uint32_t u, const cl::Env& e) {
    static const int32_t kTab[6][3] = { { 0x12c, 0x19, 1 }, { 0xfa, 0xa, 3 }, { 0x64, 8, 4 }, { 0x4b, 6, 4 }, { 0x1e, 4, 3 }, { 0, 0x64, 0 } };   // retail rdata 0xD3688, plus the row after it
    const int32_t n = (int32_t)e.H(0xc406, u);
    const uint32_t rec = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    int i = 0;
    while (i < 5 && !(n > kTab[i][0])) ++i;
    if (n % kTab[i][1] == 0) {
        const uint32_t y = e.PD(u, 0xc2ec, 0x10), x = e.PD(u, 0xc3be, 0xc);
        const uint32_t k = e.H(0xc3fa, u), g = e.H(0xc388, u);
        if (rec) {
            uint32_t q[23] = {};
            q[0] = 1; q[1] = rec; q[4] = 0x15c; q[5] = x; q[6] = y; q[11] = g; q[12] = k;
            const uint32_t nu = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
            if (nu) { e.H(0xc448, nu, (uint32_t)kTab[i][2]); e.H(0xc394, nu, (uint32_t)n); }
        }
    }
    return e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
