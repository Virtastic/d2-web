// 1.13c D2Client+0xC4290 -- table predicate: 1 for a unit of type 3 and class 0x1D4, else 0.
#include "cl_tbl.h"
extern "C" uint32_t d2c_c4290(uint32_t u, const cl::Env& e) { return u && e.R(u) == 3 && e.R(u + 4) == 0x1d4 ? 1 : 0; }
