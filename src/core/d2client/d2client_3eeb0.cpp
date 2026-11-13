// 1.13c D2Client+0x3EEB0: thiscall forwarder (ret 4), calls vtable slot +0x30 of the object at this+8 as a call; the stack word is not passed on.
#include "d2c_vcall.h"
extern "C" uint32_t d2c_3eeb0(uint32_t self, uint32_t, const cl::Env& e) { return d2cVcall(e, self, 0x30, 0); }
