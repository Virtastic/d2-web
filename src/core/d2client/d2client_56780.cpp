// 1.13c D2Client+0x56780 -- skill do: clears the unit's word at +0x3C and reports success.
#include "cl_skdo.h"
extern "C" uint32_t d2c_56780(uint32_t u, int32_t, uint32_t, const cl::Env& e) { e.W(u + 0x3c, 0); return 1; }
