// D2Client+0x6AA30: place-and-draw a unit overlay; env-seam style (ordinals are callbacks, globals are fields).
#include <cstdint>
#include <emscripten.h>
struct D2c6AA30Env {
    uint32_t* unit;                 // ESI (may be null); [0x31] bit5 = skip, [0x15] -> room/path ptr
    int32_t sel;                    // stack word uStack_80 (0 = no extra offset)
    int32_t base;                   // stack word pcStack_84 (initial y base)
    int32_t g9960, gC418, g995C;    // DAT 6fbc9960 / 6fbcc418 / 6fbc995c
    int32_t gCC414, g8BC48;         // DAT 6fbcc414 (1/2 = side), 6fb8bc48 (screen width)
    int32_t (*ord10494)();
    int32_t (*cfg10688)(int32_t** cfg);   // returns nonzero + cfg ptr (fatal if 0 in retail)
    int32_t (*ord10651)();
    int32_t (*ord11142)();
    int32_t (*onScreen)(int32_t x);       // FUN_6fb1a3d0
    int32_t (*prep)(uint32_t* unit, int32_t* desc);  // FUN_6fb1e2c0, desc = 18 words
    void (*ord10011)(int32_t* desc, int32_t x, int32_t y);
};
extern "C" {
// 1.13c D2Client+0x6AA30
EMSCRIPTEN_KEEPALIVE int32_t d2c_6aa30(D2c6AA30Env* e) {
    uint32_t* u = e->unit;
    if ((u && ((u[0x31] >> 5) & 1)) || e->ord10494()) return 0;
    int32_t x = 0;
    int32_t* cfg = nullptr;
    if (!e->cfg10688(&cfg)) return 0;
    if (!*(char*)((char*)cfg + 0x150) || !*(char*)((char*)cfg + 0x118 + e->sel)) return 0;
    x = cfg[0x148 / 4];
    int32_t a = 0, b = 0;
    if (u && u[0x15] && *(int32_t*)(u[0x15] + 0x30)) {
        int32_t p = *(int32_t*)(u[0x15] + 0x30);
        a = *(int32_t*)(p + 4) >> 11;
        b = *(int32_t*)(p + 8) >> 11;
    }
    x = (a - (e->g9960 - e->gC418)) + e->ord10651() + x;
    int32_t y = e->base + (b - (e->g995C - 8)) + e->ord11142();
    if (e->sel) {
        int32_t q = (e->g8BC48 + ((e->g8BC48 >> 31) & 3)) >> 2;  // width/4, trunc toward 0
        if (e->gCC414 == 1) x -= q; else if (e->gCC414 == 2) x += q;
        y += 0x10;
    }
    if (!e->onScreen(x)) return 0;
    int32_t d[18] = {0};
    d[8] = (d[8] & ~0xff00) | 0x100 | (e->sel & 0xff);  // low half of word 0 (uStack_80 low16) = 0x100|sel byte
    if (!e->prep(u, d)) return 0;
    e->ord10011(d, x, y);
    return 1;
}
}
