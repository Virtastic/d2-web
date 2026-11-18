// 1.13c D2Client+0x669D0 -- forwards to vtable slot +0x30 of the owned child (this[2]); see cl_vthunk.h.
#include "cl_vthunk.h"
extern "C" uint32_t d2c_669d0(uint32_t self, const cl::Env& e) { return clc::vforward(e, self, 0x30); }
