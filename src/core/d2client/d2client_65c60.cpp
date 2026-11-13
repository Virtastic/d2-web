// 1.13c D2Client+0x65C60 -- calls the panel toggle (0x65760) with mode 0.
#include "cl_cmd.h"
extern "C" uint32_t d2c_65c60(uint32_t, const cl::Env& e) { return e.H(0x65760, 0, 0, 0, 0, 0, 0); }
