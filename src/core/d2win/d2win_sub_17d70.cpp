// src/core/d2win/d2win_sub_17d70.cpp -- 1.13c D2Win+0x17D70 (232 B), from the cached decompile.
// Mouse-wheel dispatch over the control list. Retail globals/callees are injected (17800, 170F0 unmapped).
#include <cstdint>
#include <cstdlib>

struct D2Win17D70Ctx {
    uint32_t mask0, mask2;                        // gdwBitMasks[0], [2]
    int32_t* focus;                               // DAT_6f9a9e24
    int32_t* head;                                // DAT_6f9014b8 (else tail below)
    int32_t* head2;                               // DAT_6f9014a0
    void (*sub17800)(int32_t);                    // cursor clamp
    int32_t* (*sub170F0)(int32_t*, int32_t*);     // nearest-control pick
    uint32_t (*subB0D0)(int32_t*, int32_t, uint32_t);
    uint32_t (*subF610)(int32_t*, int32_t);
    void (*fatal)(uint32_t code);
    void (*ord511)();
};

static int32_t* nxt(int32_t* c) { return (int32_t*)(intptr_t)c[0xf]; }

// 1.13c D2Win+0x17D70
void d2win_sub_17D70(const D2Win17D70Ctx& g, int32_t p) {
    g.sub17800(p);
    if (g.focus) {
        if (g.focus[0] != 0xb) { g.fatal(0x62); std::exit(-1); }
        uint32_t f = (uint32_t)g.focus[2];
        if ((g.mask2 & f) && (g.mask0 & f)) return;
    }
    int32_t* best = nullptr;
    int32_t* n = g.head ? g.head : g.head2;
    int32_t* start = n;
    if (n) {
        for (int32_t* c = n; c; c = nxt(c))
            if (c[0] == 5 && (g.mask2 & (uint32_t)c[2]) && (g.mask0 & (uint32_t)c[2]) && c[0x12] > 0)
                best = g.sub170F0(best, c);
        if (best && g.subB0D0(best, p, (uint32_t)p)) goto done;
    }
    while (n && (n[0] != 6 || !g.subF610(n, p))) n = nxt(n);
    (void)start;
done:
    g.ord511();
}
