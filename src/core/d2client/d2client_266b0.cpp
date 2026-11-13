// 1.13c D2Client+0x266B0 -- panel callback: asks the two-word routine with (1, 0) and returns its answer.
#include "cl_pa4.h"
extern "C" uint32_t d2c_266b0(const cl::Env& e) { return e.H(0xbe350, 1, 0); }
