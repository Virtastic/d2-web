// ECX = room, EDX = x, EAX = y, EDI = radius; returns 1
// when the point (x, y) +- radius is not outside the room rectangle D2Common 10561 fills ({x0, y0, w, h}).
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xA0750
extern "C" uint32_t d2c_a0750(uint32_t room, int32_t x, int32_t y, int32_t r, const cl::Env& e) {
    int32_t L[4] = { 0, 0, 0, 0 };
    e.H(0xc5b0, room, (uint32_t)(uintptr_t)L);                      // 10561 (room, &rect)
    auto add = [](int32_t p, int32_t q) { return (int32_t)((uint32_t)p + (uint32_t)q); };   // retail lea/add/sub: wrapping
    auto sub = [](int32_t p, int32_t q) { return (int32_t)((uint32_t)p - (uint32_t)q); };
    if (add(x, r) < L[0] && sub(x, r) > add(L[2], L[0])) return 0;
    if (add(y, r) >= L[1]) return 1;
    return sub(y, r) > add(L[3], L[1]) ? 0 : 1;
}
