// 1.13c D2Client+0x66A20 -- forwards to vtable slot +0x20 of the owned child (this[2]); see cl_vthunk.h.
#include "cl_vthunk.h"
extern "C" uint32_t d2c_66a20(uint32_t self, const cl::Env& e) { return clc::vforward(e, self, 0x20); }
