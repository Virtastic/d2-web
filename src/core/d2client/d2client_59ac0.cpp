// 1.13c D2Client+0x59AC0 -- creates the 0x140x100 box (callbacks 0x59910 / 0x59980, slot 0x1177E8) with three labelled buttons
// (strings 0xFCA, 0xD49, 0xD48). Returns 0 when the box already exists, else 1. A failed create (0x529), an empty slot after the
// buttons (0x4F2) or a negative window field +0x50 (0x4F8) exits through 0xBF92/0xBF80/0x3664 (the hook returns, then 1 comes back
// after the +0x44 store). gl+0 = DWORD slot 0x1177E8.
#include "cl_env.h"
extern "C" uint32_t d2c_59ac0(const cl::Env& e) {
    if (e.R(e.gl)) return 0;
    uint32_t code;
    const uint32_t c1 = 0xB2059910u, c2 = 0xB2059980u;
    const uint32_t a[11] = {0x140, 0x64, c1, c1, 0, 1, 1, 0, 0, 0, 1};
    const uint32_t win = e.H(0xa3e40, a[0], a[1], a[2], a[3], a[4]);
    if (e.blk) e.blk(e.ctx, 0xa3e40, (uint32_t)(uintptr_t)a, 11);
    e.W(e.gl, win);
    if (!win) {
        code = 0x529;
    } else {
        e.W(win + 0x5c, 1);
        const uint32_t ids[3] = {0xfca, 0xd49, 0xd48}, cbs[3] = {0, c2, c1}, fl[3] = {0, 1, 1};
        for (int i = 0; i < 3; i++) {
            const uint32_t s = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, ids[i]);
            const uint32_t b[6] = {s, 0xf, 0, 1, cbs[i], fl[i]};
            e.H(0xa36f0, b[0], b[1], b[2], b[3], b[4], 0, e.R(e.gl));
            if (e.blk) e.blk(e.ctx, 0xa36f0, (uint32_t)(uintptr_t)b, 6);
        }
        const uint32_t w2 = e.R(e.gl);
        if (!w2) {
            code = 0x4f2;
        } else if ((int32_t)e.R(w2 + 0x50) < 0) {
            code = 0x4f8;
        } else {
            e.W(w2 + 0x44, 0xffffffffu);
            return 1;
        }
    }
    const uint32_t m = e.H(0xbf92, code);
    e.H(0xbf80, 0xB20D359Fu, m);
    e.H(0x3664, 0xffffffffu);
    return 1;
}
