// 1.13c D2Client+0x40DE0 -- skill start function (ECX = unit, EDX = skill id, one stack word): the shared routine
// with the skill id on the stack and ESI = the unit; its answer is returned.
#include "cl_pa4.h"
extern "C" uint32_t d2c_40de0(uint32_t ecx, uint32_t edx, const cl::Env& e) { return e.H(0x40ca0, edx, 0, 0, 0, 0, 0, 0, 0, 0, ecx); }
