// 1.13c D2Client+0xB9EA0 (ecx = unit): with a room whose 0xC1DE(0xC1F0(room)) value is 0x83 or 0x84, runs 0xC346(unit, room) and 0xC3A6(unit); when the 0xC322 value is zero, or an LCG step of the unit seed (+0x20) leaves its low 6 bits clear, it stamps the shared effect timers (e.gl: 0xBB9CC = tick, 0x7D0, 0xBB8, 0x7D0, then 0x11BEFC := 6) and calls 0xC448(unit, 1). One more LCG step: a nonzero remainder mod 10 returns the new seed low / 10; otherwise two further steps give an x and a y offset (remainders mod 60, minus 30) around 0x1630/0x1660 and the return is 0xB3B40(EAX = unit, EBX = y, stack x). Any other room: tails to 0xB1720(ESI = unit).
// e.gl: 5 dwords = the 0x10B9CC quad then the 0x11BEFC word.
#include "cl_env.h"
extern "C" uint32_t d2c_b9ea0(uint32_t u, const cl::Env& e) {
    const uint32_t room = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (room) {
        const uint32_t k = e.H(0xc1f0, room);
        const int32_t c = (int32_t)e.H(0xc1de, k);
        if (c >= 0x83 && c <= 0x84) {
            e.H(0xc346, u, room);
            e.H(0xc3a6, u);
            bool stamp = true;
            if (e.H(0xc322, u)) stamp = (e.Lcg(u + 0x20) & 0x3f) == 0;
            if (stamp) {
                e.W(e.gl, e.Tick(0xC0DE1000u)); e.W(e.gl + 4, 0x7d0); e.W(e.gl + 8, 0xbb8); e.W(e.gl + 12, 0x7d0); e.W(e.gl + 16, 6);
                e.H(0xc448, u, 1);
            }
            const uint32_t l1 = e.Lcg(u + 0x20);
            if (l1 % 10) return l1 / 10;
            const uint32_t l2 = e.Lcg(u + 0x20);
            const uint32_t l3 = e.Lcg(u + 0x20);
            const uint32_t gx = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
            const uint32_t gy = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
            const uint32_t x = gx + (l2 % 0x3c) - 0x1e, y = gy + (l3 % 0x3c) - 0x1e;
            return e.H(0xb3b40, x, 0, 0, 0, 0, u, 0, 0, y);
        }
    }
    e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u);
    return 0;
}
