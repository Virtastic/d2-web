// 1.13c D2Client+0x40A70 -- skill do: the two-step presentation on the unit itself, fed the row's graded pair
// (first value in EAX of the seven-word routine, second in EBX of the four-word one).
#include "cl_skdo.h"
extern "C" uint32_t d2c_40a70(uint32_t u, int32_t sk, uint32_t, const cl::Env& e) {
    const uint32_t r = clsk::row(e, sk);
    if (!r) return 0;
    int32_t a, b;
    clsk::gradePair(e, u, r, a, b);
    e.H(0x6b300, u, 2, 0, 0, 0, (uint32_t)a);
    e.H(0x88a70, u, 0, 0, 0, 0, 0, 0, 0, (uint32_t)b);
    return 1;
}
