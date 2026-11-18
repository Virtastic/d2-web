// 1.13c D2Client+0x71FB0 -- state callback (ECX = unit): the one-word unit routine; its answer is returned.
#include "cl_pa4.h"
extern "C" uint32_t d2c_71fb0(uint32_t ecx, const cl::Env& e) { return e.H(0xc784, ecx); }
