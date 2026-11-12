// 1.13c D2Client+0x511B0 -- table predicate: 1 for a unit of type 1, else 0.
#include "cl_tbl.h"
extern "C" uint32_t d2c_511b0(uint32_t u, const cl::Env& e) { return u && e.R(u) == 1 ? 1 : 0; }
