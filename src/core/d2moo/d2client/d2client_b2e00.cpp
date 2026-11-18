// 1.13c D2Client+0xB2E00 (eax = unit, void): as B28E0 with the divisor clamped to at least 1 (row[0x5c]) and the
// numerator to at least that divisor (row[0x58]); always runs, then sets C4A2 / C2C8 to the quotient.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB2E00
extern "C" void d2c_b2e00(uint32_t u, const cl::Env& e) {
    uint32_t row = u ? e.UnitRow(u) : 0;
    if (!row) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    int32_t d = (int32_t)e.R(row + 0x5c);
    if (d < 1) d = 1;
    int32_t a = (int32_t)e.R(row + 0x58);
    if (d > a) a = d;
    uint32_t b = e.R(u + 0x54);
    if (b) { uint32_t c = e.R(b + 0x30); if (c) { e.W(c + 4, 0); e.W(c + 8, 0); e.W(c + 0xc, (uint32_t)a << 11); } }
    b = e.R(u + 0x54);
    if (b) { uint32_t c = e.R(b + 0x30); if (c) { e.W(c + 0x10, 0); e.W(c + 0x14, 0); e.W(c + 0x18, (0u - (uint32_t)d) << 11); } }
    int32_t q = a / d;
    e.H(0xc4a2, u, (uint32_t)q);
    e.H(0xc2c8, u, (uint32_t)q);
}
