// 1.13c D2Client+0xb9470 -- client missile: scattered burst around the unit. EBX = unit, stack chance, count, spread,
// class (ret 0x10), no return. class outside [0, sgptDataTables +0xB6C) returns. When 0xC406(u) != 0 and
// 0x1380(ECX = u +0x20, ESI = chance) != 0 it returns. Request word 0 = 0x20, 1 = 0xA60D0(EAX = u), 2 = u, 11/12 =
// 0xC388/0xC3FA(u), 4 = class; base x/y = the unit path +0xC/+0x10 (static-path types) or 0xC3BE/0xC2EC(path).
// count times: dx, dy = rnd(2*spread) - spread each from the inline seed step on u +0x20 (power-of-two masks, else
// unsigned remainder; 0 when 2*spread <= 0), pushed away from the centre by spread (sign of the offset), word 7 = x +dx,
// word 8 = y +dy; 0xB30F0(ECX = block) per shot.
#include "cl_env.h"
static uint32_t d2c_b9470_rnd(uint32_t u, int32_t n, const cl::Env& e) {
    if (n <= 0) return 0;
    uint32_t lo = e.Lcg(u + 0x20);
    uint32_t m = (uint32_t)n;
    return (m & (m - 1)) ? lo % m : lo & (m - 1);
}
extern "C" void d2c_b9470(uint32_t u, uint32_t chance, int32_t count, int32_t spread, int32_t cls, const cl::Env& e) {
    if (cls < 0 || cls >= (int32_t)e.R(e.dt + 0xb6c)) return;
    if (e.H(0xc406, u) && e.H(0x1380, 0, 0, 0, 0, 0, 0, u + 0x20, 0, 0, chance)) return;
    uint32_t q[23] = {};
    q[0] = 0x20;
    q[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    q[2] = u;
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    q[4] = (uint32_t)cls;
    uint32_t x0 = e.PD(u, 0xc3be, 0xc);
    uint32_t y0 = e.PD(u, 0xc2ec, 0x10);
    if (count <= 0) return;
    int32_t span = (int32_t)((uint32_t)spread * 2u);
    for (int32_t i = count; i; --i) {
        int32_t dx = (int32_t)(d2c_b9470_rnd(u, span, e) - (uint32_t)spread);
        int32_t dy = (int32_t)(d2c_b9470_rnd(u, span, e) - (uint32_t)spread);
        dx = (int32_t)((uint32_t)dx + (dx < 0 ? 0u - (uint32_t)spread : (uint32_t)spread));
        dy = (int32_t)((uint32_t)dy + (dy < 0 ? 0u - (uint32_t)spread : (uint32_t)spread));
        q[7] = x0 + (uint32_t)dx;
        q[8] = y0 + (uint32_t)dy;
        e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    }
}
