// 1.13c D2Client+0x3EF10: thiscall tail jump to vtable slot +0x18 of the object at this+8 (ECX = the object, the stack words pass through).
#include "d2c_vcall.h"
extern "C" uint32_t d2c_3ef10(uint32_t self, uint32_t w, const cl::Env& e) { return d2cVcall(e, self, 0x18, w); }
