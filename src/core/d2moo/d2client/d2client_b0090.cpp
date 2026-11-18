// 1.13c D2Client+0xB0090 (ecx = dx, edx = room, eax = dy; stack r2, x, y; ret 0xC): within r2 and a clear 4-mask line
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB0090
extern "C" uint32_t d2c_b0090(int32_t dx, uint32_t room, int32_t dy, int32_t r2, int32_t x, int32_t y, const cl::Env& e) {
    if (!room) return 0;
    if ((int32_t)((uint32_t)dx * (uint32_t)dx + (uint32_t)dy * (uint32_t)dy) > r2) return 0;
    int32_t from[2] = { x, y };
    int32_t to[2] = { (int32_t)((uint32_t)x + (uint32_t)dx), (int32_t)((uint32_t)y + (uint32_t)dy) };
    // thunk 0xC436 (D2Common ordinal 11076), stack: room, &from, &to, 4  -- the two out-pointers are stack addresses
    uint32_t r = e.H(0xc436, room, (uint32_t)(uintptr_t)from, (uint32_t)(uintptr_t)to, 4);
    return r == 0 ? 1 : 0;
}
