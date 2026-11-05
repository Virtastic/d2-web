// 1.13c D2Client+0x546E0 -- skill-stat scaled value (EAX = base value, EDI = the stat-calc record; stack: unit, id; ret 8).
// Record kinds 2..5 (byte +0x54) look up the skill row u16 +0x56 in the sgptDataTables skill table (+0xBCC, count +0xBD4,
// stride 0x144; out of range or null: 0), ask 0xC1C6 (playerSelf, idx, 0) for the level value and scale: ((v >> row[0x18]) *
// base) >> rec[0x55]. Then >> rec[0x18]. Id 0x7A on a type-4 unit with 0xC1CC (unit, 0x39) set adds 0x32. Shifts are arithmetic.
#include "cl_env.h"
extern "C" uint32_t d2c_546e0(uint32_t base, uint32_t rec, uint32_t unit, uint32_t id, const cl::Env& e) {
    int32_t b = (int32_t)base;
    uint32_t kind = e.R8(rec + 0x54);
    if (kind >= 2 && kind <= 5) {
        uint32_t idx = e.R(rec + 0x56) & 0xffff;
        if ((int32_t)idx >= (int32_t)e.R(e.dt + 0xbd4)) return 0;
        uint32_t row = e.R(e.dt + 0xbcc) + idx * 0x144;
        if (!row) return 0;
        int32_t v = (int32_t)e.H(0xc1c6, e.self, idx, 0);
        v >>= e.R8(row + 0x18) & 31;
        v = (int32_t)((uint32_t)v * (uint32_t)b);
        b = v >> (e.R8(rec + 0x55) & 31);
    }
    b >>= e.R8(rec + 0x18) & 31;
    if (id == 0x7a && unit && e.R(unit) == 4 && e.H(0xc1cc, unit, 0x39)) b += 0x32;
    return (uint32_t)b;
}
