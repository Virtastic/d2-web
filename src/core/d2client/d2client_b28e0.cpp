// 1.13c D2Client+0xB28E0 (eax = unit, void): without a unit row run B1720. A row whose +0x5c is positive sets the
// unit's +0x54/+0x30 block to (0, 0, row[0x58]<<11) and (0, 0, (-row[0x5c])<<11), then calls ordinals 10401 and
// 10405... (the two C4A2/C2C8 setters) with the quotient row[0x58]/row[0x5c].
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB28E0
extern "C" void d2c_b28e0(uint32_t u, const cl::Env& e) {
    uint32_t row = u ? e.UnitRow(u) : 0;
    if (!row) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    int32_t d = (int32_t)e.R(row + 0x5c);
    if (d <= 0) return;
    uint32_t b = e.R(u + 0x54);
    if (b) { uint32_t c = e.R(b + 0x30); if (c) { e.W(c + 4, 0); e.W(c + 8, 0); e.W(c + 0xc, e.R(row + 0x58) << 11); } }
    b = e.R(u + 0x54);
    if (b) { uint32_t c = e.R(b + 0x30); if (c) { e.W(c + 0x10, 0); e.W(c + 0x14, 0); e.W(c + 0x18, (0u - (uint32_t)d) << 11); } }
    int32_t q = (int32_t)e.R(row + 0x58) / d;
    e.H(0xc4a2, u, (uint32_t)q);
    e.H(0xc2c8, u, (uint32_t)q);
}
