// 1.13c D2Client+0xC9000 -- skill do: a rate-limited effect. When the frame counter has passed the stored deadline,
// the deadline moves five frames on and the effect routine runs (EAX = 0, ECX = unit, stack word 1). Always succeeds.
#include "cl_skdo.h"
extern "C" uint32_t d2c_c9000(uint32_t u, int32_t, uint32_t, const cl::Env& e) {
    const uint32_t now = clc::G(e, 0x1197F8);
    if (now > clc::G(e, 0x12345C)) {
        clc::S(e, 0x12345C, now + 5);
        e.H(0x4c500, 1, 0, 0, 0, 0, 0, u);
    }
    return 1;
}
