// 1.13c D2Client+0x5BF20 -- mouse-move over the 0x2F-wide strip (stack = message, ret 4). `now` is GetTickCount (one pinned value).
// g = pinned globals, one dword each: +0 [119854] gate, +4 [11C284] mode, +8 [11C28C] strip x, +0xC [11C298], +0x10 [11C294],
// +0x14 [11C2A0] hover start, +0x18 [11BBFC] player, +0x1C [11C4D4] list head (nodes: +4 type, +8, +0xC owner id, +0x20, +0x30 next).
// Gate clear: nothing. Else message +0x18 = 0, [11C298] = 0; outside the strip (mode 2, x not in [28C, 28C+0x2E], y not in [0x18, 0x3C],
// signed) clears [11C294] and [11C2A0] and returns. Inside: [11C2A0] = now when 0; with a player whose Ordinal 11017(player+0x60)
// is 0, the first list node of type 7 owned by player+0xC with +0x20 == 0 and +8 != -1 sets [11C298] = 1 and, once now - [11C2A0] >= 0x2BC
// (unsigned), [11C294] = 1. Finally 0x5BCC0(0) nonzero sets [11C294] = 1.
#include "cl_env.h"
extern "C" void d2c_5bf20(uint32_t msg, const cl::Env& e, uint32_t g, uint32_t now) {
    if (e.R(g) == 0) return;
    int32_t x = (int32_t)(e.R(msg + 0xc) & 0xffff), y = (int32_t)((e.R(msg + 0xc) >> 16) & 0xffff);
    e.W(msg + 0x18, 0); e.W(g + 0xc, 0);
    int32_t sx = (int32_t)e.R(g + 8);
    if ((int32_t)e.R(g + 4) == 2 || x < sx || x > sx + 0x2e || y < 0x18 || y > 0x3c) {
        e.W(g + 0x10, 0); e.W(g + 0x14, 0);
        return;
    }
    if (e.R(g + 0x14) == 0) e.W(g + 0x14, now);
    uint32_t pl = e.R(g + 0x18);
    if (pl != 0 && e.H(0xc202, e.R(pl + 0x60)) == 0) {
        pl = e.R(g + 0x18);
        uint32_t n = e.R(g + 0x1c);
        if (pl != 0 && n != 0) {
            uint32_t id = e.R(pl + 0xc);
            for (; n != 0; n = e.R(n + 0x30)) {
                if (e.R(n + 4) == 7 && e.R(n + 0xc) == id && e.R(n + 0x20) == 0) {
                    if (e.R(n + 8) != 0xffffffffu) {
                        e.W(g + 0xc, 1);
                        if (now - e.R(g + 0x14) >= 0x2bcu) e.W(g + 0x10, 1);
                    }
                    break;
                }
            }
        }
    }
    if (e.H(0x5bcc0, 0) != 0) e.W(g + 0x10, 1);
}
