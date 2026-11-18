// 1.13c D2Client+0x7AC90 -- table entry: the one-word routine with ECX = 0x18, EDX = 1 and stack word 0. Returns 1.
#include "cl_tbl.h"
extern "C" uint32_t d2c_7ac90(const cl::Env& e) {
    e.H(0xc2790, 0, 0, 0, 0, 0, 0, 0x18, 1);
    return 1;
}
