// 1.13c D2Client+0x1ED0: destructor wrapper, stdcall(obj). Runs the destructor body 0x1FB0(obj) and returns what it leaves in EAX.
#include "d2c_seam.h"
extern "C" uint32_t d2c_1ed0(const D2cSeam* s, const uint8_t* obj) { return (uint32_t)(*s)(0x1fb0, 0, 0, 0, 0, 0, (uintptr_t)obj); }
