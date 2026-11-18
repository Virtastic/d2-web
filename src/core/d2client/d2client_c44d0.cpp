// 1.13c D2Client+0xC44D0 -- skill do: launches the row's missile (+0xEC) at the unit's own position. A row without a
// usable missile still succeeds.
#include "cl_skdo.h"
extern "C" uint32_t d2c_c44d0(uint32_t u, int32_t sk, uint32_t, const cl::Env& e) {
    const uint32_t r = clsk::row(e, sk);
    if (!r) return 0;
    const int32_t m = e.R16s(r + 0xec);
    if (!clsk::mis(e, m)) return 1;
    const uint32_t a = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
    const uint32_t b = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
    return e.H(0xaff10, (uint32_t)m, b, a, (uint32_t)sk, 1, 0, 0, u) != 0;
}
