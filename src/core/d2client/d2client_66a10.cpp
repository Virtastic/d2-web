// 1.13c D2Client+0x66A10 -- forwards to vtable slot +0x1C of the owned child (this[2]); see cl_vthunk.h.
#include "cl_vthunk.h"
extern "C" uint32_t d2c_66a10(uint32_t self, const cl::Env& e) { return clc::vforward(e, self, 0x1c); }
