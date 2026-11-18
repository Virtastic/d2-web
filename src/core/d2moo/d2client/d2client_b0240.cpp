// 1.13c D2Client+0xB0240 (ecx = unit): 1 when the unit is type 3 (a missile) whose class row (stride 0x1A4, +4 flags byte) shares a bit with
// byte 0x10 of the gdwBitMasks table, else 0.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB0240
extern "C" uint32_t d2c_b0240(uint32_t u, const cl::Env& e) {
    if (!u || e.R(u) != 3) return 0;
    uint32_t row = e.UnitRow(u);
    if (!row) return 0;
    return (e.R8(e.bm + 0x10) & e.R8(row + 4)) != 0;
}
