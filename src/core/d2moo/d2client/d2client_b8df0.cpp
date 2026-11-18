// 1.13c D2Client+0xb8df0 -- client missile: periodic scatter child. EAX = period, EBX = unit, stack radius, class (ret 8);
// returns the 0xB30F0 result or 0. period <= 0, 0xC4D2(u) % period != 0, owner 0xA60D0(EAX = u) == 0 or room 0xC1F0(u) == 0
// return 0. Seed 0xC3B2(ECX = u +0x20, EDX = 0xC4D2(u) + 0x1630(ECX = u)); request word 0 = 0x2003, 1 = owner, 11/12 =
// 0xC388 / 0xC3FA(u), 4 = class; dx = (radius-1) - 0x1380(ECX = u +0x20, ESI = 2*(radius-1)), dy the same with the second
// draw; 5/6 = 0x1630(u)+dx / 0x1660(u)+dy; 0xC4A8(room, x, y, 4) != 0 returns 0, else 0xB30F0(ECX = block).
#include "cl_env.h"
extern "C" uint32_t d2c_b8df0(int32_t period, uint32_t u, int32_t radius, uint32_t cls, const cl::Env& e) {
    if (period <= 0) return 0;
    if ((int32_t)e.H(0xc4d2, u) % period != 0) return 0;
    uint32_t own = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!own) return 0;
    uint32_t room = e.H(0xc1f0, u);
    if (!room) return 0;
    uint32_t t = e.H(0xc4d2, u);
    uint32_t x1 = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
    e.H(0xc3b2, 0, 0, 0, 0, 0, 0, u + 0x20, t + x1);
    uint32_t q[23] = {};
    q[0] = 0x2003;
    q[1] = own;
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    q[4] = cls;
    int32_t r1 = radius - 1;
    uint32_t span = (uint32_t)(2 * r1);
    int32_t dx = r1 - (int32_t)e.H(0x1380, 0, 0, 0, 0, 0, 0, u + 0x20, 0, 0, span);
    int32_t dy = r1 - (int32_t)e.H(0x1380, 0, 0, 0, 0, 0, 0, u + 0x20, 0, 0, span);
    uint32_t x = e.H(0x1630, 0, 0, 0, 0, 0, 0, u) + dx;
    q[5] = x;
    uint32_t y = e.H(0x1660, 0, 0, 0, 0, 0, 0, u) + dy;
    q[6] = y;
    if ((uint16_t)e.H(0xc4a8, room, x, y, 4)) return 0;
    return e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
}
