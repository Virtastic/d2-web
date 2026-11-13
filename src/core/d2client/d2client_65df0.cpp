// 1.13c D2Client+0x65DF0 -- calls the screen refresh (0x656B0) with 1.
#include "cl_cmd.h"
extern "C" uint32_t d2c_65df0(uint32_t, const cl::Env& e) { return e.H(0x656b0, 1); }
