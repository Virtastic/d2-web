// src/core/d2win/d2win_sub_17d70.cpp -- 1.13c D2Win+0x17D70 (232 B), from the cached decompile.
// Mouse-wheel dispatch over the control list. Retail globals/callees are injected.
#include <cstdint>
#include <cstdlib>
#include <emscripten.h>

struct D2Win17D70Ctx {
    uint32_t mask0, mask2;                        // gdwBitMasks[0], [2]
    int32_t* focus;                               // DAT_6f9a9e24
    int32_t* head;                                // DAT_6f9014b8 (else head2 below)
    int32_t* head2;                               // DAT_6f9014a0
    void (*sub17800)(const uint8_t* ev);          // cursor clamp
    int32_t* (*sub170F0)(int32_t*, int32_t*);     // nearest-control pick
    uint32_t (*subB0D0)(int32_t* best, int32_t delta, const uint8_t* ev);   // EAX = delta, ECX = best, stack = ev
    uint32_t (*subF610)(int32_t* ctl, int32_t delta);                       // EAX = ctl, ESI = delta
    void (*fatal)(uint32_t code);
    void (*ord511)(const uint8_t* ev);
};

static int32_t* nxt(int32_t* c) { return (int32_t*)(intptr_t)c[0xf]; }

// 1.13c D2Win+0x17D70: ev is the wheel event; its signed word at +0x0A divided by 120 (truncating) is the notch count.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_17D70_env(const D2Win17D70Ctx* gp, const uint8_t* ev) {
    const D2Win17D70Ctx& g = *gp;
    const int32_t wheel = (int16_t)(ev[0xa] | ev[0xb] << 8);
    const int32_t delta = wheel / 120;
    g.sub17800(ev);
    if (g.focus) {
        if (g.focus[0] != 0xb) { g.fatal(0x62); std::exit(-1); }
        uint32_t f = (uint32_t)g.focus[2];
        if ((g.mask2 & f) && (g.mask0 & f)) return;
    }
    int32_t* best = nullptr;
    int32_t* n = g.head ? g.head : g.head2;
    if (n) {
        for (int32_t* c = n; c; c = nxt(c))
            if (c[0] == 5 && (g.mask2 & (uint32_t)c[2]) && (g.mask0 & (uint32_t)c[2]) && c[0x12] > 0)
                best = g.sub170F0(best, c);
        if (best && g.subB0D0(best, delta, ev)) goto done;
    }
    while (n && (n[0] != 6 || !g.subF610(n, delta))) n = nxt(n);
done:
    g.ord511(ev);
}
