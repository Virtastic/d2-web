// 1.13c D2Client+0x73D70 -- skill do: the owner-slot missile through the plain launch; succeeds when the launch does.
#include "cl_skdo.h"
extern "C" uint32_t d2c_73d70(uint32_t u, int32_t sk, uint32_t lvl, const cl::Env& e) {
    if (!clsk::row(e, sk)) return 0;
    const int32_t a = (int32_t)e.H(0x40550, u, 0, 0, 0, 0, (uint32_t)sk);
    if (!clsk::mis(e, a)) return 0;
    clsk::acted(e, u);
    return clsk::launch3(e, u, a, sk, lvl) != 0;
}
