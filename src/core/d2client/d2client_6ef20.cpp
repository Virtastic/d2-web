// 1.13c D2Client+0x6EF20: blink-phase text draw. ECX, EDX = draw context, arg0 = stack word, now = GetTickCount.
#include "cl_env.h"
extern "C" void d2c_6ef20(uint32_t ecx, uint32_t edx, uint32_t arg0, uint32_t now, const cl::Env& e) {
    const uint32_t B = 0x6FAB0000u;
    if (e.R(B + 0xf9ea4) == 1) {
        uint32_t s = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0xd4c);
        e.H(0xd37e, ecx, arg0, 0, 0, 0, 0, s, edx + 0x83);
    }
    if (now - e.R(B + 0x123450) > (e.R(B + 0xf9ea0) & 0xffffu)) {   // interval is a 16-bit word
        e.W(B + 0x123450, now);
        if (e.R(B + 0xf9ea4)) { e.W(B + 0xf9ea0, (e.R(B + 0xf9ea0) & 0xffff0000u) | 100); e.W(B + 0xf9ea4, 0); }
        else { e.W(B + 0xf9ea0, (e.R(B + 0xf9ea0) & 0xffff0000u) | 500); e.W(B + 0xf9ea4, 1); }
    }
}
