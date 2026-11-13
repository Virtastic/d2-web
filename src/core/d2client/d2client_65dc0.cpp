// 1.13c D2Client+0x65DC0 -- calls the panel toggle (0x65760) with mode 4.
#include "cl_cmd.h"
extern "C" uint32_t d2c_65dc0(uint32_t, const cl::Env& e) { return e.H(0x65760, 4, 0, 0, 0, 0, 0); }
