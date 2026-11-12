// 1.13c D2Client+0xAFF70 -- table entry: the three-word routine with (ECX, EDX, 0); its answer is returned.
#include "cl_tbl.h"
extern "C" uint32_t d2c_aff70(uint32_t ecx, uint32_t edx, const cl::Env& e) { return e.H(0xc30a, ecx, edx, 0); }
