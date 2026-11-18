// True when the common unit query (unit, 2) answers nonzero, or the hero answers nonzero to the state check for 0x36.
#include "cl_env.h"
// 1.13c D2Client+0x81CB0. EAX = unit.
extern "C" uint32_t d2cl_81cb0(uint32_t unit, const cl::Env& e) {
    if (e.H(0xca12, unit, 2)) return 1;
    uint32_t self = e.R(0x6FAB0000u + 0x11bbfc);
    if (self && e.H(0xc1fc, self, 0x36)) return 1;
    return 0;
}
