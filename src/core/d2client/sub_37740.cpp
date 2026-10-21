// D2Client+0x37740: per-frame HUD draw gate; env-seam style (globals and callees via Env).
#include <cstdint>
#include <emscripten.h>
struct D2c37740Env {
    const int32_t* busy[6];   // DAT_6fbaad90, ad8c, ad84, adb0, addc, ade4
    int32_t* g454;            // 6fbcc454
    int32_t* gAe08;           // 6fbaae08
    const int32_t* g8bc4c;    // 6fb8bc4c
    const int32_t* g8bc48;    // 6fb8bc48
    const int32_t* gC304;     // 6fbcc304
    const int32_t* gC450;     // 6fbcc450
    const int32_t* gC300;     // 6fbcc300
    const int32_t* gD5d0;     // 6fb8d5d0
    int32_t (*f36450)();
    void (*f373c0)();
    void (*ord10041)(const void* a, int32_t b, int32_t c, int32_t d, int32_t e, int32_t f, int32_t nargs);
    void (*ord10227)();
    void (*ord10265)();
    void (*ord10024)();
    void (*exit_m1)();
    int32_t (*ord10048)();
    void (*fecc60)(int32_t a, int32_t b);
};
extern "C" {
// 1.13c D2Client+0x37740
EMSCRIPTEN_KEEPALIVE void d2c_37740(const D2c37740Env* e) {
    for (int i = 0; i < 6; i++) if (*e->busy[i]) { *e->g454 = 1; return; }
    *e->g454 = 0;
    int32_t old = *e->gAe08;
    if (e->f36450() == 0) {
        int32_t l[18] = {0};
        e->f373c0();
        l[0] = 0; l[16] = *e->gC304; l[17] = 0;   // local_48[0], local_14, local_8
        e->ord10041(l, *e->g8bc48 - 0x4b, 0, 0, 0, 0, 2);
        int32_t a[2] = {*e->gC450 != 0, 0};
        l[4] = *e->gC300;                          // uStack_2c (as seen by decompile)
        e->ord10041(a, *e->g8bc48 - 0x48, *e->g8bc4c - 0xa4, -1, 5, 0, 6);
    } else {
        e->ord10227();
        if (*e->gD5d0 != 0x26) { e->ord10265(); e->ord10024(); e->exit_m1(); return; }
        *e->gAe08 = 0;
        if (old) e->fecc60(e->ord10048(), 0);
    }
}
}
