// 1.13c D2Client+0x599E0 -- creates the 0x140x100 box (callback 0x598A0, slot 0x1177F0) and its two labelled buttons (strings 0xFCB, 0xD48).
// A pre-existing box (0x13D), a failed create (0x529), an empty slot after the buttons (0x4F2) or a negative window field +0x50 (0x4F8)
// exits through 0xBF92/0xBF80/0x3664 (the hook returns); success sets window +0x44 = -1. gl+0 = DWORD slot 0x1177F0.
#include "cl_env.h"
extern "C" void d2c_599e0(const cl::Env& e) {
    uint32_t code;
    if (e.R(e.gl)) {
        code = 0x13d;
    } else {
        const uint32_t cb = 0xB20598A0u;
        const uint32_t a[11] = {0x140, 0x64, cb, cb, 0, 1, 1, 0, 0, 0, 1};
        const uint32_t win = e.H(0xa3e40, a[0], a[1], a[2], a[3], a[4]);
        if (e.blk) e.blk(e.ctx, 0xa3e40, (uint32_t)(uintptr_t)a, 11);
        e.W(e.gl, win);
        if (!win) {
            code = 0x529;
        } else {
            e.W(win + 0x5c, 1);
            uint32_t s = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0xfcb);
            const uint32_t b1[6] = {s, 0xf, 0, 1, 0, 0};
            e.H(0xa36f0, b1[0], b1[1], b1[2], b1[3], b1[4], 0, e.R(e.gl));
            if (e.blk) e.blk(e.ctx, 0xa36f0, (uint32_t)(uintptr_t)b1, 6);
            s = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0xd48);
            const uint32_t b2[6] = {s, 0xf, 0, 1, cb, 1};
            e.H(0xa36f0, b2[0], b2[1], b2[2], b2[3], b2[4], 0, e.R(e.gl));
            if (e.blk) e.blk(e.ctx, 0xa36f0, (uint32_t)(uintptr_t)b2, 6);
            const uint32_t w2 = e.R(e.gl);
            if (!w2) {
                code = 0x4f2;
            } else if ((int32_t)e.R(w2 + 0x50) < 0) {
                code = 0x4f8;
            } else {
                e.W(w2 + 0x44, 0xffffffffu);
                return;
            }
        }
    }
    const uint32_t m = e.H(0xbf92, code);
    e.H(0xbf80, 0xB20D359Fu, m);
    e.H(0x3664, 0xffffffffu);
}
