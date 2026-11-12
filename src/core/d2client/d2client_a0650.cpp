// 1.13c D2Client+0xA0650 -- table entry: the two-word routine with (the unit's path at +0x2C, EDX); its answer is
// returned.
#include "cl_tbl.h"
extern "C" uint32_t d2c_a0650(uint32_t u, uint32_t edx, const cl::Env& e) { return e.H(0xc3c4, e.R(u + 0x2c), edx); }
