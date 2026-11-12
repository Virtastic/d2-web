// 1.13c D2Client+0x73B50 -- table entry: the three-word routine with (ECX, EDX, 0); its answer is returned.
#include "cl_tbl.h"
extern "C" uint32_t d2c_73b50(uint32_t ecx, uint32_t edx, const cl::Env& e) { return e.H(0xc30a, ecx, edx, 0); }
