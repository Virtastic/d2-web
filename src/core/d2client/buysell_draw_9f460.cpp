// D2Client+0x9F460: buy/sell panel button draw; env-seam style (Ordinals/globals via Env).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct D2c9F460Env {
    void (*ord10184)();
    int32_t (*ord10003)();
    int32_t (*ord10028)();
    int32_t (*ord10083)();
    void (*ord10014)(int32_t l, int32_t t, int32_t r, int32_t b, int32_t c, int32_t m);
    void (*ord10150)(int32_t x, int32_t y, int32_t z);
    uint32_t (*ord10059)(int32_t id);
    void (*ord10043)(int32_t id, int32_t z);
    void (*cellDraw)(int32_t* cd, int32_t x, int32_t y, int32_t a, int32_t b, int32_t c);  // Ordinal_10041
    uint32_t (*loadBuySellCel)();          // D2Client+0xDB420 (FUN_6fadb420(0))
    void (*f53840)();                      // D2Client+0xA3840
    uint32_t (*rd32)(uint32_t addr);       // reads 32-bit retail memory
    int32_t* gMode;      // 6fbbb210
    uint32_t* gCel0;     // 6fbcbc10
    uint32_t* gCel1;     // 6fbcbc0c
    uint32_t* gBtnCel;   // 6fbcb9c4
    int32_t* gBtnCount;  // 6fb8ecb0
    uint8_t* gBtnTab;    // 6fb8ec62, stride 40
    int32_t* gMouseY;    // 6fbcb828
    int32_t* gMouseX;    // 6fbcb824
    int32_t* gScreenW;   // 6fb8bc4c
    int32_t* gFlag224;   // 6fbbb224
};
extern "C" {
// 1.13c D2Client+0x9F460
EMSCRIPTEN_KEEPALIVE void d2c_9f460(const D2c9F460Env* e) {
    e->ord10184();
    int32_t mode = *e->gMode;
    if (mode == 1 || mode == 0) {
        int32_t cd[18]; memset(cd, 0, sizeof cd);
        cd[0] = 0;  // cd[13] (+0x34) = cel ptr
        cd[13] = (int32_t)(mode == 0 ? *e->gCel0 : *e->gCel1);
        e->cellDraw(cd, 0x69, 0x10e, -1, 5, 0);
        if (*e->gBtnCel == 0) *e->gBtnCel = e->loadBuySellCel();
        cd[13] = (int32_t)*e->gBtnCel;
        int32_t n = *e->gBtnCount;
        for (int32_t i = 0; i < n; i++) {
            const uint8_t* b = e->gBtnTab + i * 40;
            int32_t x, y, fr; uint8_t fl;
            memcpy(&x, b + 8, 4); memcpy(&y, b + 12, 4); memcpy(&fr, b + 32, 4); fl = b[36];
            cd[0] = fr + (fl != 0);
            e->cellDraw(cd, x, y, -1, 5, 0);
        }
        int32_t my = *e->gMouseY, mx = *e->gMouseX;
        if (my < 0x140 && mx < *e->gScreenW - 0x30) {
            for (int32_t i = 0; i < n; i++) {
                const uint8_t* b = e->gBtnTab + i * 40;
                int32_t v[8]; memcpy(v, b, 32);  // x,y,.,.,xmin,xmax,ymin,ymax
                if (v[4] <= my && my <= v[5] && v[6] <= mx && mx <= v[7]) {
                    e->ord10003();
                    int32_t w = e->ord10028();
                    int32_t h = e->ord10083();
                    int32_t h2 = e->ord10083();
                    int32_t t = (v[1] - h2) - 0x2a;
                    int32_t l = v[0] - w / 2;
                    e->ord10014(l + 10, t, l + 0x16 + w, t + h + 6, 0, 2);
                    e->ord10150(v[1] - 0x26, 0, 0);
                    n = *e->gBtnCount;
                }
            }
        }
        if (mode == 1) {
            e->ord10003();
            uint32_t p = e->ord10059(300);
            int32_t y = 0x1e;
            while (p) {
                e->ord10028();
                e->ord10150(y, 4, 0);
                p = e->rd32(p + 4);
                e->ord10043(0x2c3, 0);
                e->ord10043(0x2c4, 0);
                y += 0x14;
            }
        }
    }
    if (*e->gFlag224) e->f53840();
    e->ord10184();
}
}
