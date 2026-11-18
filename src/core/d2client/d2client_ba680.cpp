// 1.13c D2Client+0xBA680 (ecx = unit): with row 0x17B's window test passing, spawns a 0x17E child (request type 0x20) at a random offset around the room, kicks its motion block and clears flag 2, then tails to 0xB17C0.
#include "cl_env.h"
extern "C" uint32_t d2c_ba680(uint32_t u, const cl::Env& e) {
    int32_t t = (int32_t)e.H(0xc406, u);
    uint32_t row = e.Row(0x17b);
    e.H(0xb55c0, 0x17c, 0, 0, 0, 0, 0xa);
    int32_t hi = e.R16s(row + 0x96);
    int32_t lo = hi - (int32_t)e.R(row + 0x38);
    if (t < lo && t > hi - (int32_t)e.R(row + 0x3c)) {
        uint32_t room = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
        if (room) {
            uint32_t sp = u + 0x20;
            uint32_t r1 = e.H(0x1330, 0, 0, 0, 0, 0, 0xa, sp);
            uint32_t a = e.H(0x1630, 0, 0, 0, 0, 0, 0, room);
            uint32_t q[23] = {};
            q[0] = 0x20; q[1] = room; q[2] = u; q[7] = r1 + a - 5;
            uint32_t r2 = e.H(0x1330, 0, 0, 0, 0, 0, 0xa, sp);
            uint32_t b = e.H(0x1660, 0, 0, 0, 0, 0, 0, room);
            q[8] = r2 + b - 5;
            q[11] = e.H(0xc388, u);
            q[12] = e.H(0xc3fa, u);
            q[4] = 0x17e;
            uint32_t ch = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
            if (ch) {
                uint32_t r3 = e.H(0x1330, 0, 0, 0, 0, 0, 3, sp);
                uint32_t k = (uint32_t)(-3 - (int32_t)r3);
                uint32_t pa = e.R(ch + 0x54);
                if (pa) { uint32_t mo = e.R(pa + 0x30); if (mo) { e.W(mo + 0x1c, 0); e.W(mo + 0x20, 0); e.W(mo + 0x24, k << 11); } }
                e.H(0xc7c80, 0, 0, 0, 0, 0, ch, 0xf);
                pa = e.R(ch + 0x54);
                if (pa) { uint32_t mo = e.R(pa + 0x30); if (mo) e.W(mo, e.R(mo) & 0xfffffffdu); }
            }
        }
    }
    return e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
