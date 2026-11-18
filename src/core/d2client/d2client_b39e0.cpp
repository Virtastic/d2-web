// 1.13c D2Client+0xb39e0 -- client missile: scatter landing spots; ESI = missile unit, stack (a0, x, y), ret 0xc. Exit EAX is the
// last call's result on the spawn path and stale otherwise (not modelled).
#include "cl_env.h"
extern "C" void d2c_b39e0(uint32_t u, uint32_t a0, uint32_t x, uint32_t y, const cl::Env& e) {
    if (!u || !a0) return;
    uint32_t g = e.H(0xc1f0, u);
    if (!g) return;
    uint32_t seed = u + 0x20;
    uint32_t go = 1;
    for (uint32_t i = 0;;) {
        if (i >= 4) { if (go) return; break; }
        int32_t dx = 0x11 - (int32_t)(e.Lcg(seed) % 0x22);
        int32_t dy = 0x11 - (int32_t)(e.Lcg(seed) % 0x22);
        if (dx < -1 || dx > 1 || dy < -1) {
            uint32_t ny = (uint32_t)dy + y, nx = (uint32_t)dx + x;
            if ((e.H(0xc4a8, g, nx, ny, 4) & 0xffff) == 0) { x = nx; y = ny; go = 0; }
        }
        i++;
        if (!go) break;
    }
    uint32_t q[23] = {};
    q[0] = 1; q[1] = a0;
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    q[4] = 0x267; q[5] = x; q[6] = y;
    e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    q[4] = 0x268;
    e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
}
