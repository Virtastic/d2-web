// 1.13c D2Client+0xB17C0 (ecx = unit): the per-unit animation advance. Retail address-space seams only (cl_env.h): every
// callee goes through e.H with the registers and stack words the retail call site passes.
// Module globals ride in e.gl (one pinned block): +0 DAT 0x11C394 (screen mode), +4 0x11BBFC (the local player),
// +8 0x119824 (the step rate), +0x10 the 0xDD858 table (8 bytes per state, first dword read).
#include <cstdint>
#include "cl_env.h"

// 1.13c D2Client+0xB17C0
extern "C" void d2c_b17c0(uint32_t u, const cl::Env& e) {
    if (!u) return;
    int32_t idx = (int32_t)e.R(u + 4);
    if (idx < 0 || (int32_t)e.R(e.dt + 0xb6c) <= idx) return;
    const uint32_t ms = e.R(e.dt + 0xb64) + (uint32_t)idx * 0x1a4;
    if (!ms) return;
    const uint32_t g = e.gl;
    uint32_t local = 0;
    const uint32_t mode0 = e.R(g);
    if (mode0 <= 8) {
        static const uint8_t kSel[9] = { 0, 0, 1, 1, 1, 1, 0, 1, 0 };   // the byte table at 0xB1AD8
        if (!kSel[mode0] && e.H(0xa60d0, 0, 0, 0, 0, 0, u) == e.R(g + 4)) local = 1;
    }
    uint32_t mode = 0;
    for (;;) {
        uint32_t edi = e.H(0xc39a, u);
        int32_t cnt = (int32_t)e.H(0xc406, u);
        const bool run = cnt <= (int32_t)edi;
        if (run && (int32_t)e.H(0xc4d2, u) > (int32_t)e.R8(ms + 0x135)) e.W(u + 0xc8, e.R(u + 0xc8) & 0xfffbffffu);
        uint32_t pd = e.R(u + 0x54);
        if (pd) {
            uint32_t fp = e.R(pd + 0x30);
            if (fp) { uint32_t fl = e.R(fp); if ((fl & 1) && !(fl & 8)) { e.H(0xb1420, 1, 0, 0, 0, 0, 0, u); return; } }
        }
        if (run) {
            bool step = true;   // 618AF: the c3a6 call
            if (e.R8(ms + 0x137) != 0) {
                if (e.R8(ms + 0x180) != 0) {
                    uint32_t b2 = e.R8(ms + 0x182);
                    int32_t c = (int32_t)e.H(0xc406, u);
                    int32_t lim = (int32_t)(e.R8(ms + 0x178) - b2 + 1);
                    if (c > lim) { e.H(0xc418, u, e.R8(ms + 0x181), b2); step = false; }
                }
            } else if (e.H(0xc46c, u) != 0) { step = false; }
            if (step) e.H(0xc3a6, u);
        }
        mode = e.R(u + 0x2c);
        if (e.H(0xc496, mode)) e.H(0xc3e2, u, e.R(g + 8));
        int32_t c1 = (int32_t)e.H(0xc406, u);
        e.H(0xc2c8, u, (uint32_t)(c1 - 1));
        if ((int32_t)e.H(0xc406, u) <= 0) { e.H(0xb1420, 0, 0, 0, 0, 0, 0, u); return; }
        if (!run) return;
        if (e.R8(ms + 0x12c) != 0) {
            uint32_t pl = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
            bool go = !pl;
            if (pl) { uint32_t o = e.H(0xc1f0, pl); if (o && e.H(0xc208, o)) go = true; }
            if (go) {
                uint32_t k = e.R8(ms + 0x12c);
                if ((int32_t)e.H(0xc406, u) >= (int32_t)k) {
                    e.H(0xc2c8, u, k);
                    if (e.R8(ms + 0x137) == 0) {
                        int32_t d = (int32_t)e.R8(ms + 0x178) - (int32_t)e.R8(ms + 0x12c);
                        if (d > 0) e.W(u + 0x44, (uint32_t)d << 8);
                    }
                }
            }
        }
        const uint32_t st = e.R(u + 0x10);
        if (!st) return;
        const uint32_t fl = e.H(0xc4c6, mode);
        if (fl & 5) { e.H(0xb1420, 1, 0, 0, 0, 0, 0, u); return; }
        if (st == 6) return;
        uint32_t blk[3];
        blk[0] = u; blk[2] = ms;
        blk[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
        if (e.R8(ms + 0x185) && (fl & 0xffff)) {
            uint32_t arr = 0;
            const uint32_t n = e.H(0xc304, mode, (uint32_t)(uintptr_t)&arr);
            const uint32_t owner = e.H(0xc1f0, u);
            for (uint32_t i = 0; (int32_t)i < (int32_t)n; i++) {
                uint32_t xy = e.R(arr + i * 4);
                uint32_t t = e.H(0xc4c0, u);
                uint32_t tbl = e.R(g + 0x10 + st * 8);
                e.Xs(t);
                uint32_t r = e.H(0xc370, owner, xy & 0xffff, xy >> 16, tbl, (uint32_t)(uintptr_t)blk);
                if (r && !e.H(0xa5440)) { e.H(0xb1420, 0, 0, 0, 0, 0, r, u); return; }
            }
        }
        if (!local) return;
        if (e.H(0xc4d2, u) != 1) return;
        if (!e.H(0xc496, mode)) return;
        local = 0;
    }
}
