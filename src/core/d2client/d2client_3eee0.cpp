// 1.13c D2Client+0x3EEE0: thiscall tail jump to vtable slot +0x24 of the object at this+8 (ECX = the object, the stack words pass through).
#include "d2c_vcall.h"
extern "C" uint32_t d2c_3eee0(uint32_t self, uint32_t w, const cl::Env& e) { return d2cVcall(e, self, 0x24, w); }
