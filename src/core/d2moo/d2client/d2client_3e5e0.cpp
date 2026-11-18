// 1.13c D2Client+0x3E5E0: item-cell draw with a charge/quantity label (the 3E490 sibling). EAX = x, ECX = mode; stack (ret 0x18) = a0, p (int16* item
// id word), a2, a3, y, flag. 0x17F0(EAX = *p) = item record (none: return); cls = byte at record + 7. When the mouse (D2Client+0x11B828 x,
// +0x11B824 y, signed) is inside [x, x+0x30] x [y-0x30, y] a mode other than 0 and 1 folds (4 stays 4, every other value becomes 1: ((mode != 4) - 1 & 3) + 1). The 18-dword
// request (word 0 = cls, word 13 = a0) goes to 0xD0E4(req, x, y, -1, 5, mode; EAX 0, ECX a0, EDX req, EBX x, ESI mode); then 0x3DCF0 (flag 0) or
// 0x3E0C0 (EAX = y, ECX = x, p, a2). With a player ([*(D2Client+0x11BBFC)+0x60] = unit) a count replaces a3: p == 2: u = 0xCA0C(unit), needs
// 0xC76C(u) and 0xC73C(u), count = 0xC1C6(u, 0x46, 0). p == 4: u = 0xCA0C(unit), v = 0xC262(unit, 4), w = 0xC262(unit, 5); v = w when u == v;
// needs v, 0xC1CC(v, 0x2D), 0xC76C(v), 0xC73C(v), count = 0xC1C6(v, 0x46, 0). Finish 0x3D3C0(x, y; EBX = a3 or the count).
#include "cl_env.h"
extern "C" void d2c_3e5e0(const cl::Env& e, uint32_t x, uint32_t mode, uint32_t a0, uint32_t p, uint32_t a2, uint32_t a3, uint32_t y, uint32_t flag) {
    const int32_t key = e.R16s(p);
    const uint32_t rec = e.H(0x17f0, 0, 0, 0, 0, 0, (uint32_t)key);
    if (!rec) return;
    const uint32_t cls = e.R8(rec + 7);
    const int32_t my = (int32_t)e.R(e.gl), mx = (int32_t)e.R(e.gl + 4);
    if (!(mx < (int32_t)x || mx > (int32_t)x + 0x30 || my < (int32_t)y - 0x30 || my > (int32_t)y) && mode != 0 && mode != 1) mode = mode == 4 ? 4 : 1;
    uint32_t req[18] = {};
    req[0] = cls; req[13] = a0;
    const uint32_t rp = (uint32_t)(uintptr_t)req;
    e.H(0xd0e4, rp, x, y, 0xffffffffu, 5, 0, a0, rp, x, mode);
    if (e.blk) e.blk(e.ctx, 0xd0e4, rp, 18);
    e.H(flag ? 0x3e0c0 : 0x3dcf0, p, a2, 0, 0, 0, y, x);
    const uint32_t pl = e.self;
    const uint32_t unit = pl ? e.R(pl + 0x60) : 0;
    if (unit) {
        if (key == 2) {
            const uint32_t u = e.H(0xca0c, unit);
            if (u && e.H(0xc76c, u) && e.H(0xc73c, u)) a3 = e.H(0xc1c6, u, 0x46, 0);
        } else if (key == 4) {
            const uint32_t u = e.H(0xca0c, unit);
            uint32_t v = e.H(0xc262, unit, 4);
            const uint32_t w = e.H(0xc262, unit, 5);
            if (u == v) v = w;
            if (v && e.H(0xc1cc, v, 0x2d) && e.H(0xc76c, v) && e.H(0xc73c, v)) a3 = e.H(0xc1c6, v, 0x46, 0);
        }
    }
    e.H(0x3d3c0, x, y, 0, 0, 0, 0, 0, 0, a3);
}
