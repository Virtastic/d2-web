// 1.13c D2Client+0x67930 -- method (ECX = the wrapper, one stack word): the routine taking the stack word in EAX and the
// inner object (+8) in ECX.
#include "cl_tbl.h"
extern "C" uint32_t d2c_67930(uint32_t self, uint32_t a, const cl::Env& e) { return e.H(0x67080, 0, 0, 0, 0, 0, a, e.R(self + 8)); }
