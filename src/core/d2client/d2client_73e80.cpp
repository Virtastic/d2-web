// 1.13c D2Client+0x73E80 -- skill do: launches the owner slot's missile and stores the row's formula (+0x138) on it.
#include "cl_skdo.h"
extern "C" uint32_t d2c_73e80(uint32_t u, int32_t sk, uint32_t lvl, const cl::Env& e) {
    const uint32_t r = clsk::row(e, sk);
    if (!r) return 0;
    const int32_t a = (int32_t)e.H(0x40550, u, 0, 0, 0, 0, (uint32_t)sk);
    if (!clsk::mis(e, a)) return 0;
    clsk::acted(e, u);
    const uint32_t c = e.H(0xc316, u, e.R(r + 0x138), (uint32_t)sk, lvl);
    const uint32_t m = clsk::launch3(e, u, a, sk, lvl);
    if (!m) return 0;
    e.H(0xc448, m, c);
    return 1;
}
