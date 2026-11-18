// 1.13c D2Client+0x669E0 -- forwards to vtable slot +0x2C of the owned child (this[2]); see cl_vthunk.h.
#include "cl_vthunk.h"
extern "C" uint32_t d2c_669e0(uint32_t self, const cl::Env& e) { return clc::vforward(e, self, 0x2c); }
