// 1.13c D2Client+0xBA600 (ecx = unit): with a room, a zero 0xC322 value draws one LCG bit to pick effect 0x1A0 or 0x1A6 (0xB3FC0) and reloads the value from 0xAFF80(0xC4D2(unit)); then 0xC448(unit, value - 1). Tails to 0xB17C0.
#include "cl_env.h"
extern "C" uint32_t d2c_ba600(uint32_t u, const cl::Env& e) {
    uint32_t room = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    uint32_t v = e.H(0xc322, u);
    if (room) {
        if (v == 0) {
            uint32_t l = e.Lcg(u + 0x20);
            e.H(0xb3fc0, (l & 1) ? 0x1a0 : 0x1a6, 0, 0, 0, 0, u, room);
            uint32_t k = e.H(0xc4d2, u);
            v = e.H(0xaff80, 0, 0, 0, 0, 0, k);
        }
        e.H(0xc448, u, v - 1);
    }
    return e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);   // tail jmp: EAX is the callee's
}
