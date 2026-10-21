// D2Client+0x36660: build the buy/sell button desc and hand it to Ordinal_10041; env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2c36660Env {
    uint8_t* buysellCel;                                  // DAT 0x4CB9C4 (lazy-loaded cel)
    int32_t flagCC448;                                    // DAT 0x4CC448
    void (*fmtPath)(char* out);                           // wsprintfA("%s\\ui\\panel\\buysellbtn","DATA\\GLOBAL")
    uint8_t* (*f2B420)(int32_t zero);                     // D2Client+0x2B420
    int32_t (*ord10031)();                                // Ordinal_10031
    void (*ord10041)(int32_t* desc, int32_t x, int32_t y, int32_t a, int32_t b, int32_t c);  // Ordinal_10041
};
extern "C" {
// 1.13c D2Client+0x36660
EMSCRIPTEN_KEEPALIVE uint8_t* d2c_36660(D2c36660Env* e) {
    int32_t d[18] = {0};
    char path[260];
    if (!e->buysellCel) { e->fmtPath(path); e->buysellCel = e->f2B420(0); }
    d[16] = 0;
    d[0] = (e->flagCC448 != 0) + 10;
    d[13] = (int32_t)(intptr_t)e->buysellCel;
    e->ord10041(d, e->ord10031() == 2 ? 0x2ad : 0x20d, 0x38, -1, 5, 0);
    return e->buysellCel;
}
}
