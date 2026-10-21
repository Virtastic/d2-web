// D2Client+0x48150: reset a block of UI globals, then two callees; env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2c48150G {
    uint8_t  g_bc9663;                 // 0x...bc9663 (RVA 0x2F9663 -> 0)
    uint32_t g_90ea8[6];               // 6fb90ea8..ebc -> 0xffffffff
    uint32_t g_cbc28, g_cbc2c, g_cbc38, g_cbc3c;  // -> 0
};
struct D2c48150Env {
    void (*f47B20)();
    void (*fC2790)(uint32_t, int32_t, int32_t);
};
extern "C" {
// 1.13c D2Client+0x48150
EMSCRIPTEN_KEEPALIVE void d2c_48150(D2c48150G* g, const D2c48150Env* e) {
    g->g_bc9663 = 0;
    for (int i = 0; i < 6; i++) g->g_90ea8[i] = 0xffffffffu;
    g->g_cbc28 = 0; g->g_cbc2c = 0; g->g_cbc38 = 0; g->g_cbc3c = 0;
    e->f47B20();
    e->fC2790(8, 1, 0);
}
}
