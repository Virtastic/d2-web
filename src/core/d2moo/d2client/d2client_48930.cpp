// 1.13c D2Client+0x48930: opens the 0x9773 pop-up (title = the lower-cased string 0xA5D90 returns, then one
// of five D2Lang lines keyed by the player seed, then line 0x102E). Callees go through D2cHost in retail's call order. wp = window 0x6FBC9650..,
// pl = the player unit (dword at 0x6FBCBBFC), lt = the Fog lower-case table (word[256]) the Unicode import slot 0x6FB7EAD8 points at.
#include <emscripten.h>
#include "d2client_hw.h"

extern "C" EMSCRIPTEN_KEEPALIVE void d2h_48930(D2cHost h, uint8_t* wp, uint8_t* pl, const uint16_t* lt) {
    Win w = { wp, 0x6fbc9650 };
    if (!hc(h, 0x46150)) { hc(h, 0x48150); return; }
    if (w.rd(0x6fbc9773)) {   // retail aborts: _exit(-1)
        uint32_t m = hc(h, 0xbf92, 0, 0, 0, 0, 0, 0xd69);
        hc(h, 0xbf80, 0, 0, 0, 0, 0, 0xB2000000u | 0x6fb8359fu - 0x6fab0000u, m);
        hc(h, 0x3664, 0, 0, 0, 0, 0, 0xffffffffu);
        return;
    }
    hc(h, 0x46880);
    uint32_t x = 0, y = 0;
    uint32_t d = hc(h, 0xa3e40, 0, 0, 0, 0, 0, x, y, 0xB2000000u | 0x482f0, 0xB2000000u | 0x48290);
    w.wr(0x6fbc9773, d);
    if (!d) {
        uint32_t m = hc(h, 0xbf92, 0, 0, 0, 0, 0, 0x529);
        hc(h, 0xbf80, 0, 0, 0, 0, 0, 0xB2000000u | 0x6fb8359fu - 0x6fab0000u, m);
        hc(h, 0x3664, 0, 0, 0, 0, 0, 0xffffffffu);
        return;
    }
    st((void*)(uintptr_t)d, 0x5c, 1);
    uint16_t b[0x50];
    for (int i = 0; i < 0x50; ++i) b[i] = 0;
    hc(h, 0x46150);
    const uint16_t* t = (const uint16_t*)(uintptr_t)hc(h, 0xa5d90);
    int n = 0;
    do { b[n] = t[n]; } while (t[n++]);
    n = 0; while (b[n]) ++n;
    for (int i = 0; i < n; ++i) if (b[i] < 0x100) b[i] = lt[b[i]];
    hc(h, 0xa36f0, 4, d, 0, 0, 0, P32(b), 0x15, 0, 1);
    w.wr(0x6fbc9663, 1);
    uint32_t lo = ld(pl, 0x20), hi = ld(pl, 0x24);
    uint64_t s = (uint64_t)lo * 0x6ac690c5u + hi;
    st(pl, 0x24, (uint32_t)(s >> 32)); st(pl, 0x20, (uint32_t)s);
    static const uint16_t K[5] = { 0xd3a, 0xd3b, 0xd3c, 0xd3d, 0xd3e };
    uint32_t sp = hc(h, 0xd1ec, 0, K[(uint32_t)s % 5]);
    hc(h, 0xa36f0, 0, d, 0, 0, 0, sp, 0xf, 0, 1);
    sp = hc(h, 0xd1ec, 0, 0x102e);
    hc(h, 0xa36f0, 0, d, 0, 0, 0, sp, 0xf, 0, 1);
    hc(h, 0xa3370, 0, d);
}
