// 1.13c D2Client+0x66A80 -- forwards to vtable slot +0x8 of the owned child (this[2]); see cl_vthunk.h.
#include "cl_vthunk.h"
extern "C" uint32_t d2c_66a80(uint32_t self, const cl::Env& e) { return clc::vforward(e, self, 0x8); }
