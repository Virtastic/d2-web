// 1.13c D2Client+0x8AE00 --
// Particle-field draw/step. Ordinals and 8AC40 are host callbacks; globals are passed in.
#include <emscripten.h>
#include <cstdint>

struct D2c8AE00 {
    uint32_t seed[2];           // *PTR_DAT_6fb91304 (lo, hi)
    uint32_t inited;            // DAT_6fbcbf24
    uint32_t last_tick;         // DAT_6fbbb598
    uint8_t  pal[8];            // DAT_6fbbeae0
    int32_t  x[256];            // DAT_6fbbeaf0
    int32_t  y[256];            // DAT_6fbbb5a0
    int32_t  dx[256];           // DAT_6fbbeef0
    int32_t  col[256];          // DAT_6fbbb440
};
static_assert(sizeof(D2c8AE00) == 16 + 8 + 4 * 1024, "layout");

static uint32_t rnd(uint32_t* s) {
    uint64_t v = (uint64_t)s[0] * 0x6ac690c5u + s[1];
    s[0] = (uint32_t)v; s[1] = (uint32_t)(v >> 32);
    return (uint32_t)v;
}

extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8ae00(D2c8AE00* g, uint32_t now,
                     void (*o10936)(), void (*o10995)(), void (*o10614)(),
                     uint8_t (*o10190)(int32_t, int32_t, int32_t),
                     void (*o10010)(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t),
                     void (*sub8ac40)(int32_t idx)) {
    if (!g->inited) {
        o10936(); o10995(); o10614();
        int32_t i = 0;
        for (int32_t acc = 0; acc < 0x400; acc += 0x80, i++) {
            int32_t base = acc / 7 + 0x80, c[3];
            for (int k = 0; k < 3; k++) {
                c[k] = (int32_t)(rnd(g->seed) & 0x3f) - 0x20 + base;
                if (c[k] > 0xff) c[k] = 0xff;
            }
            g->pal[i] = o10190(c[0], c[1], c[2]);
        }
        for (int32_t n = 0; n < 0x100; n++) sub8ac40(n);
        g->inited = 1;
    }
    for (int i = 0; i < 0x100; i++) o10010(g->x[i], g->y[i], g->x[i], g->y[i], g->col[i], 0xff);
    uint32_t t = g->last_tick;
    if (now - g->last_tick > 0x28) {
        for (int i = 0; i < 0x100; i++) {
            int32_t v = g->x[i] + g->dx[i];
            g->x[i] = v;
            if (v < 0) sub8ac40(i);
        }
        t = now;
    }
    g->last_tick = t;
}
