// 1.13c D2Client+0xB2F90 (ecx = unit, void): step the frame counter (+0x44, 24.8 fixed) by the row bytes +0x181 /
// +0x182: at the last frame roll a random advance (ordinal at 0x1380 over the unit's +0x20 seed), at frame count
// otherwise step back 3 or 2 frames (floor 0). Missing row runs B1720; tail B17C0.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB2F90
extern "C" void d2c_b2f90(uint32_t u, const cl::Env& e) {
    uint32_t row = u ? e.UnitRow(u) : 0;
    if (!row) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    int32_t lo = e.R(row + 0x181) & 0xff;
    int32_t f = (int32_t)e.R(u + 0x44) >> 8;
    if (f == lo - 1) {
        int32_t hi = e.R(row + 0x182) & 0xff;
        int32_t r = (int32_t)e.H(0x1380, 0, 0, 0, 0, 0, 0, u + 0x20, 0, 0, (uint32_t)(hi - lo));
        e.W(u + 0x44, (uint32_t)(r + f) << 8);
    } else if ((int32_t)e.H(0xc406, u) == lo) {
        int32_t v = lo - 3;
        if (v < 0) v = 0;
        e.W(u + 0x44, (uint32_t)v << 8);
    } else if ((int32_t)e.H(0xc406, u) < (int32_t)(e.R(row + 0x181) & 0xff)) {
        f -= 2;
        if (f < 0) f = 0;
        e.W(u + 0x44, (uint32_t)f << 8);
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
