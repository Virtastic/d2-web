// 1.13c D2Client+0x483B0: the per-frame pump of the pop-up controls. Callees go through D2cHost in
// retail's call order. w = the window 0x6FBC9650..0x6FBC97C0, pl = the dword at 0x6FBCBBFC (the player unit, or 0), cc = window 0x6FBCC394..,
// dd = window 0x6FBD2300.., g834/g8c9c = the dwords at 0x6FBCB834 / 0x6FBA8C9C, cur = the object 0x46150 returns (host-scripted), tick = GetTickCount.
#include <emscripten.h>
#include "d2client_hw.h"

extern "C" EMSCRIPTEN_KEEPALIVE void d2h_483b0(D2cHost h, uint8_t* wp, uint8_t* pl, uint32_t* g834, uint8_t* ccp, uint8_t* ddp, uint32_t* g8c9c, uint32_t tick) {
    Win w = { wp, 0x6fbc9650 }, cc = { ccp, 0x6fbcc394 }, dd = { ddp, 0x6fbd2300 };
    const uint32_t PL = P32(pl);
    uint32_t* c;
    bool run = !pl || ld(pl, 0x10) == 0x11;
    if (!run) {
        c = (uint32_t*)(uintptr_t)hc(h, 0x46150);
        if (c) {
            c = (uint32_t*)(uintptr_t)hc(h, 0x46150);
            if (c && c[4] == 0xc) run = true;
        }
        if (!run) run = hc(h, 0x46150) == 0;
    }
    if (run) { hc(h, 0x467c0); hc(h, 0x48150); }
    uint32_t esi;
    if ((esi = w.rd(0x6fbc9767)) != 0) goto sink;
    if ((esi = w.rd(0x6fbc976b)) != 0) goto sink;
    if ((esi = w.rd(0x6fbc9763)) != 0) { hc(h, 0x42ad0); hc(h, 0xa3840, 0, 0, 0, 0, esi); return; }
    if ((esi = w.rd(0x6fbc975b)) != 0) goto sink;
    if (w.rd(0x6fbc975f)) {
        c = (uint32_t*)(uintptr_t)hc(h, 0x46150);
        if (c) {
            c = (uint32_t*)(uintptr_t)hc(h, 0x46150);
            if (c && c[1] == 0x202 && w.rd(0x6fbc97b4) == 0) {
                if (w.rd(0x6fbc97b8) == 0) {
                    w.wr(0x6fbc97b8, 1);
                    uint32_t r = hc(h, 0x1380, 0, PL + 0x20, 0, 0, 0x1e);
                    w.wr(0x6fbc97bc, (r + 0x78) * 1000u + tick);
                }
                if (w.rd(0x6fbc97bc) > tick) {   // jbe skips: bc <= tick
                    w.wr(0x6fbc97b4, 1);
                    hc(h, 0x88a70, 0, 0, 0, 0x11d0, 0, 0, 0, 0, 0);
                }
            }
        }
        uint32_t d = w.rd(0x6fbc974f);
        if (cc.rd(0x6fbcc394) && d && d < tick) { hc(h, 0x47920); return; }
        if (w.rd(0x6fbc974b) < tick) {
            hc(h, 0x14660);
            *g8c9c = 0; cc.wr(0x6fbcc404, 1); cc.wr(0x6fbcc3a4, 1);
        }
        return;
    }
    if (w.rd(0x6fbc9757)) {
        uint32_t r = hc(h, 0xc202, 0, 0, 0, 0, 0, ld(pl, 0x60));
        if (r == 0 && *g834 == 0) { hc(h, 0xa3840, 0, 0, 0, 0, w.rd(0x6fbc9757)); return; }
        w.wr(0x6fbc9663, 3);
        hc(h, 0x92850);
        dd.wr(0x6fbd2301, tick);
        dd.wr(0x6fbd2305, dd.rd(0x6fbd2305) + 1);
        hc(h, 0xa3d20);
        w.wr(0x6fbc9757, 0);
        return;
    }
    if ((esi = w.rd(0x6fbc976f)) != 0) goto sink;
    if ((esi = w.rd(0x6fbc9773)) != 0) goto sink;
    return;
sink:
    hc(h, 0xa3840, 0, 0, 0, 0, esi);
}
