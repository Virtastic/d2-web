// 1.13c D2Client+0xA2230: ECX = unit, EDX = edi, one stack argument (slot), ret 4. Marks the unit dirty (+0xC4 |= 0x40), then
// either forwards 0xC754's result to 0xA15C0, or (no 0xC748 flag) runs 0x2A0B0 and 0xB2DA on the unit path. Always returns 1.
#include "cl_env.h"
extern "C" uint32_t d2cl_a2230(uint32_t unit, uint32_t edi, uint32_t slot, const cl::Env& e) {
    if (unit) e.W(unit + 0xc4, e.R(unit + 0xc4) | 0x40);
    if (e.H(0xc748, unit)) {
        uint32_t sl = slot;
        const int32_t r = (int32_t)e.H(0xc754, unit, (uint32_t)(uintptr_t)&sl);
        if (r == -1) return 1;
        e.H(0xa15c0, (uint32_t)r, edi, sl, 0, 0, unit);
        return 1;
    }
    e.H(0x2a0b0, 0, 0, 0, 0, 0, unit);
    const uint32_t p = e.R(unit + 0x2c);
    if (!p) __builtin_trap();   // retail pushes 0x946 and exits (fatal path; never captured)
    e.H(0xc2da, p);
    return 1;
}
