// 1.13c D2Client+0xBB210 -- spawn a client missile at a unit's position: EAX = unit, six stack words (ret 0x18).
// y = 10750(path) / path+0x10 and x = 10867(path) / path+0xc (static-path unit types 2, 4, 5), 0 without a path;
// then 0xB6D40 with EAX = p2, ESI = unit, stack (x, y, p1, p3, p4, p5, p6). Returns what 0xB6D40 returns.
#include <cstdint>
#include "cl_env.h"
extern "C" uint32_t d2c_bb210(uint32_t u, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t p5, uint32_t p6,
                              const cl::Env& e) {
    uint32_t y = e.PD(u, 0xc2ec, 0x10);
    uint32_t x = e.PD(u, 0xc3be, 0xc);
    uint32_t w[7] = { x, y, p1, p3, p4, p5, p6 };
    e.Xs(p5, p6);
    uint32_t r = e.H(0xb6d40, x, y, p1, p3, p4, p2, 0, 0, 0, u);
    if (e.blk) e.blk(e.ctx, 0xb6d40, (uint32_t)(uintptr_t)w, 7);
    return r;
}
