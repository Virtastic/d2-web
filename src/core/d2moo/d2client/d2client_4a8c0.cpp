// 1.13c D2Client+0x4A8C0: opens the 0x9663=1 pop-up state for the unit passed in (stdcall, one argument), clears the pop-up scratch
// globals twice, drops the busy bit on the 46150 object, walks the 0x26 control slots (mode 1 calls 0xC2790, mode 4 calls 0x47B20, other
// modes leave through 0x48150), re-derives the 0xAADA0 flag from the player, then runs the deferred 0x88A70 call. Callees go through D2cHost
// in retail's call order. aw = window 0x6FBA46C4.. (12 bytes), tb = window 0x6FBA37BD.., plpp = the dword at 0x6FBCBBFC, cpp = window 0x6FBCC2F4..0x6FBCC400,
// wpp = window 0x6FBC9650.., fpp = window 0x6FBAAD80.. (38 dwords), tpp = window 0x6FB8D538.. (38 pointers then the count),
// g3a68 = the dword at 0x6FBC3A68.
#include <emscripten.h>
#include "d2client_hw.h"

extern "C" EMSCRIPTEN_KEEPALIVE void d2h_4a8c0(D2cHost h, uint32_t arg, uint8_t* awp, uint8_t* tbp, uint8_t* plpp, uint8_t* cpp, uint8_t* wpp, uint8_t* fpp, uint8_t* tpp, const uint32_t* g3a68) {
    Win aw = { awp, 0x6fba46c4 }, cc = { cpp, 0x6fbcc2f4 }, pw = { plpp, 0x6fbcbbfc }, w = { wpp, 0x6fbc9650 }, fw = { fpp, 0x6fbaad80 }, tw = { tpp, 0x6fb8d538 };
    auto clr = [&]() {
        aw.wr(0x6fba46c8, 0x10); aw.wr(0x6fba46c4, 0x10);
        cc.wr(0x6fbcc3dc, 0); cc.wr(0x6fbcc3e0, 0); cc.wr(0x6fbcc3f8, 0); cc.wr(0x6fbcc3fc, 0); cc.wr(0x6fbcc2f8, 0); cc.wr(0x6fbcc2f4, 0);
    };
    auto fatal = [&](uint32_t key) {   // Ordinal_10265(key), Ordinal_10024(fmt, str), _exit(-1): never returns in retail
        uint32_t s = hc(h, 0xbf92, 0, 0, 0, 0, 0, key);
        hc(h, 0xbf80, 0, 0, 0, 0, 0, 0x6fb8359f, s); hc(h, 0x3664, 0, 0, 0, 0, 0, 0xffffffffu);
    };
    clr();
    if (arg == 0 || hc(h, 0x46150) != 0) { hc(h, 0x48150); return; }
    const uint32_t pl = pw.rd(0x6fbcbbfc);
    w.wr(0x6fbc9663, 1); w.wr(0x6fbc9729, 0);
    w.wr(0x6fbc966b, hc(h, 0xc1c6, 0, 0, 0, 0, 0, pl, 0xe, 0));
    hc(h, 0xbe350, 0, 0, 0, 0, 0, 0, 0);
    clr();
    if (hc(h, 0x46150) != 0) { fatal(0xdc9); return; }
    hc(h, 0x46070, arg);
    uint8_t* c = (uint8_t*)(uintptr_t)hc(h, 0x46150);
    if (c) st(c, 0xc4, ld(c, 0xc4) & 0xfffffffdu);
    const uint32_t old = fw.rd(0x6fbaada0);
    hc(h, 0xbf8c);
    if (tw.rd(0x6fb8d5d0) != 0x26) { fatal(0x139b); return; }
    uint32_t edx = fw.rd(0x6fbaada0);
    if (edx == 0) {
        if (*g3a68 != 0) { hc(h, 0x48150); return; }
        for (uint32_t i = 0; (int32_t)i < (int32_t)tw.rd(0x6fb8d5d0); ++i) {
            if (fw.rd(0x6fbaad80 + i * 4) != 1) continue;
            uint32_t mode = ld((const void*)(uintptr_t)tw.rd(0x6fb8d538 + i * 4), 0x20);
            if (mode == 0) continue;
            if (mode == 1) { hc(h, 0xc2790, 0, i, 1, 0, 0, 0); continue; }
            if (mode == 4) { if (w.rd(0x6fbc9721) != 0) hc(h, 0x47b20); continue; }
            hc(h, 0x48150); return;
        }
        edx = fw.rd(0x6fbaada0);
    }
    if (pl == 0) { edx = (edx == 0); fw.wr(0x6fbaada0, edx); }
    else if (ld((const void*)(uintptr_t)pl, 0) == 0 && hc(h, 0xa5440, pl) == 0) { edx = (edx == 0); fw.wr(0x6fbaada0, edx); }
    if (old != 0 && edx == 0) { hc(h, 0xbe2e0); hc(h, 0x492f0); }
    hc(h, 0x48930);
    uint32_t idx = hc(h, 0x45c00);
    uint8_t* flag = tbp + idx * 0x16;
    if (*flag != 0) {
        uint32_t cur = hc(h, 0x46150);
        uint32_t r = hc(h, 0x9cce0, cur, 0, 0, 0, 0, 1);
        if (r != 0) { hc(h, 0x22eb0, arg); hc(h, 0x88a70, 0, 0, 0, r, 0, pw.rd(0x6fbcbbfc), 0, 1, 0); }
        *flag = 0;
    }
}
