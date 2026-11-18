// 1.13c D2Client+0x1E80: destructor of the locked container, stdcall(obj). Deletes the critical section at obj+0x50 (a host no-op:
// one thread here), then the destructor body 0x1FB0(obj); returns what that leaves in EAX.
#include "d2c_seam.h"
extern "C" uint32_t d2c_1e80(const D2cSeam* s, const uint8_t* obj) { return (uint32_t)(*s)(0x1fb0, 0, 0, 0, 0, 0, (uintptr_t)obj); }
