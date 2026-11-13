// 1.13c D2Client+0x65DE0 -- calls the panel toggle (0x65760) with mode 2.
#include "cl_cmd.h"
extern "C" uint32_t d2c_65de0(uint32_t, const cl::Env& e) { return e.H(0x65760, 2, 0, 0, 0, 0, 0); }
