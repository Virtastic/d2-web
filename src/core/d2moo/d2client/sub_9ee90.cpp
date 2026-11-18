// D2Client+0x9EE90: per-room-tile list walk (blink/fade timers, cel draw submit); env-seam style.
// Retail 32-bit layout: every pointer is a uint32_t guest address read through the env.
#include <cstdint>
#include <emscripten.h>
struct D2c9EE90Env {
    uint32_t (*rd32)(uint32_t a);
    uint8_t (*rd8)(uint32_t a);
    void (*wr32)(uint32_t a, uint32_t v);
    void (*wr8)(uint32_t a, uint8_t v);
    int32_t (*ord10013)();
    uint32_t (*getTick)();                           // GetTickCount
    uint32_t (*ord10265)(uint32_t id);
    void (*ord10024)(uint32_t fmtAddr, uint32_t a);
    void (*exitProc)(int32_t code);                   // _exit
    void (*f58230)(uint32_t src, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a); // D2Client+0x58230
    int32_t (*ord10001)(uint32_t cel, int32_t x, int32_t y, const uint32_t* colors, uint32_t g);
    int32_t (*ord10058)(uint32_t cel, int32_t x, int32_t y, const uint32_t* colors, uint32_t g, uint32_t alpha);
    void (*f166d0)(uint32_t p);                       // D2Client+0x166D0
};
extern "C" {
// 1.13c D2Client+0x9EE90
EMSCRIPTEN_KEEPALIVE void d2c_9ee90(const D2c9EE90Env* e, uint32_t ctx) {
    const uint32_t G_44 = 0x6fbcc414, G_MODE = 0x6fba17c8, G_CNT = 0x6fb80a70, G_TAB0 = 0x6fb8ed18, G_TAB1 = 0x6fb8eef8;
    int32_t n = (int32_t)e->rd32(ctx + 0xeab0);
    int32_t left = n * n;
    if (!left) return;
    uint32_t ent = e->rd32(ctx + 0xeaa8) + 0x14;
    do {
        left--;
        uint32_t head = e->rd32(ent);
        if (head) {
            uint32_t g44 = e->rd32(G_44);
            int32_t o10013 = e->ord10013();
            uint32_t tick = e->getTick();
            bool fade = (int32_t)e->rd32(G_MODE) > 3;
            for (uint32_t node = head; node; node = e->rd32(node + 0x10)) {
                uint32_t d = e->rd32(node + 0xc);
                if (e->rd32(d + 0x1c) == 0xf) {
                    e->ord10024(0x6fb8359f, e->ord10265(0xf9));
                    e->exitProc(-1);
                    return;
                }
                uint32_t fl = e->rd32(d + 0x24);
                if (fl & 2) {
                    uint8_t lo = e->rd8(d + 0x29), hi = e->rd8(d + 0x2a);
                    if (fade && tick < e->rd32(d + 0x2c)) {
                        int32_t v = ((int32_t)hi - (int32_t)lo) * (int32_t)((tick - e->rd32(d + 0x2c)) + 500);
                        e->wr8(d + 0x28, (uint8_t)(v / 500 + lo));
                    } else {
                        e->wr32(d + 0x24, fl & ~2u);
                        e->wr8(d + 0x28, hi);
                    }
                    uint32_t f5 = e->rd32(d + 0x14);
                    e->wr32(d + 0x14, e->rd8(d + 0x28) == 0 ? f5 | 0x400 : f5 & 0xfffffbffu);
                }
                if ((e->rd32(d + 0x24) & 4) && e->rd32(d + 0x2c) <= tick) {
                    e->wr32(d + 0x24, e->rd32(d + 0x24) & ~4u);
                    e->wr32(d + 0x14, e->rd32(d + 0x14) | 8);
                }
                uint32_t f5 = e->rd32(d + 0x14);
                if ((f5 & 0x408) == 0 && (f5 & 0x1c000)) {
                    uint32_t idx = e->rd32(e->rd32(d + 0x18));
                    uint32_t cnt = e->rd32(G_CNT + idx * 4);
                    uint32_t tab = (e->rd8(d + 0x24) & 1) ? G_TAB1 : G_TAB0;
                    uint32_t col[8];
                    for (uint32_t i = 0; i < cnt; i++) {
                        uint8_t r, g, b, a;
                        e->f58230(e->rd32(node + 4) + e->rd32(tab + idx * 0x30 + i * 8) * 8, &r, &g, &b, &a);
                        col[i] = r | (uint32_t)g << 8 | (uint32_t)b << 16 | (uint32_t)a << 24;
                    }
                    int32_t x, y;
                    if (o10013 == 0) {
                        x = (int32_t)(e->rd32(ctx + 4) - e->rd32(ctx + 0x24) + e->rd32(d));
                        y = (int32_t)(e->rd32(d + 4) - e->rd32(ctx + 0x28) + e->rd32(ctx + 8));
                    } else {
                        x = (int32_t)e->rd32(node + 4);
                        y = (int32_t)e->rd32(node + 8);
                    }
                    uint8_t al = e->rd8(d + 0x28);
                    int32_t r = al == 0xff ? e->ord10001(e->rd32(d + 0x18), x, y, col, g44)
                                           : e->ord10058(e->rd32(d + 0x18), x, y, col, g44, al);
                    if (r) e->wr32(d + 0x14, e->rd32(d + 0x14) | 0x20000);
                }
            }
        }
        // second walk over entry[+4] list: only dead stack stores of a tile lookup, then f166d0(node->data)
        for (uint32_t node = e->rd32(ent + 4); node; node = e->rd32(node + 0x10)) e->f166d0(e->rd32(node + 0xc));
        ent += 0x24;
    } while (left);
}
}
