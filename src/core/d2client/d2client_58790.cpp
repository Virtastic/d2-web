// 1.13c D2Client+0x58790 -- skill do: marks the unit, then asks for an override missile (the query may also rewrite
// the level through its out-pointer). An override other than 0 and 0x1F is launched plainly when it is a Missiles
// row. Otherwise the row's own missile (+0xEA) goes through the missile check and the row's flag byte picks the
// launch routine.
#include "cl_skdo.h"
extern "C" uint32_t d2c_58790(uint32_t u, int32_t sk, uint32_t lvl, const cl::Env& e) {
    const uint32_t r = clsk::row(e, sk);
    if (!r) return 0;
    clsk::acted(e, u);
    uint32_t lv = lvl;
    int32_t m = (int32_t)e.H(0xc754, u, (uint32_t)(uintptr_t)&lv);
    if (m != 0 && m != 0x1f) {
        if (!clsk::mis(e, m) || !(e.R(e.dt + 0xb64) + (uint32_t)m * 0x1a4)) return 0;
        clsk::launch3(e, u, m, sk, lv);
        return 1;
    }
    m = e.R16s(r + 0xea);
    if (!clsk::mis(e, m)) return 0;
    if (!e.H(0x1580, 0, 0, 0, 0, 0, (uint32_t)m)) return 0;
    if (e.R8(e.bm + 4) & e.R8(r + 4)) clsk::launch8(e, u, m, sk, lv); else clsk::launch3(e, u, m, sk, lv);
    return 1;
}
