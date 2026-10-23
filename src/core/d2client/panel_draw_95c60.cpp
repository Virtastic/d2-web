// D2Client+0x95C60: panel background pieces + buy/sell button row + tab strip; env-seam style.
// Draw desc is an 18-dword block: d[0]=frame/type, d[13]=cel, d[16]=0 (same layout as 0x36660).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct D2c95c60Env {
    int32_t* flag_bc30;                  // DAT 0x4CBC30 (cleared when f3D300 != 0)
    int32_t x0, y0, yAdj;                // DAT 0x4CB9A0, 0x4CB9A4, 0x48BC4C
    uint8_t* panelCel;                   // DAT 0x4C97C0
    uint8_t* tabCel;                     // DAT 0x4CBC6C
    uint8_t** buysellCel;                // DAT 0x4CB9C4 (lazy)
    int32_t flag_c9721;                  // DAT 0x4C9721
    int32_t id_c971d;                    // DAT 0x4C971D (read as dword)
    uint8_t** unitHash;                  // DAT 0x4BA808 [128] chains; next=[0x39], id=[3], type=[0]
    int32_t flag_c97a8;                  // DAT 0x4C97A8
    int32_t nBtn, btnPage;               // DAT 0x4911C8, 0x4911C4
    const int32_t* btnX;                 // 0x4911D0 table, index (i+page*4)
    const uint32_t* btnRec;              // 0x4BB23C, 5 dwords/rec: [0]+ushort@+10 = frame
    int32_t nTab;                        // DAT 0x4911C0
    const uint8_t* tabRec;               // 0x491182, 0x12 bytes/rec: dword@0 flag, dword@4 shown, dword@-6 y
    int32_t (*f3D300)();
    void (*f458A0)(uint32_t* unit);
    void (*fatalNoUnit)();               // Ordinal_10265(0x5a)+Ordinal_10024(0x4B359F); _exit(-1)
    void (*fmtPath)(char* out);
    uint8_t* (*f2B420)(int32_t zero);
    void (*ord10041)(int32_t* desc, int32_t x, int32_t y, int32_t a, int32_t b, int32_t c);
    void (*ord10184)();
    void (*ord10003)();
    void (*ord10028)();
    void (*ord10150)(int32_t y, int32_t mode, int32_t z);
};
extern "C" {
// 1.13c D2Client+0x95C60
EMSCRIPTEN_KEEPALIVE void d2c_95c60(D2c95c60Env* e) {
    if (*e->flag_bc30 == 1 && e->f3D300()) *e->flag_bc30 = 0;
    int32_t d[18] = {0};
    d[13] = (int32_t)(intptr_t)e->panelCel;
    int32_t yb = e->y0 + e->yAdj;
    // ponytail: frames 0,1 (call 2) not shown in decompile; 0,1,2,3 inferred from the 2x2 piece layout.
    for (int32_t k = 0; k < 4; k++) {
        d[0] = k;
        e->ord10041(d, e->x0 + (k & 1) * 0x100, yb + (k < 2 ? -0xe0 : -0x30), -1, 5, 0);
    }
    if (e->flag_c9721) {
        for (uint32_t* u = (uint32_t*)e->unitHash[e->id_c971d & 0x7f]; u; u = (uint32_t*)(uintptr_t)u[0x39]) {
            if ((int32_t)u[3] == e->id_c971d) {
                if (u[0] != 1) e->fatalNoUnit();
                e->f458A0(u);
                break;
            }
        }
    }
    if (!*e->buysellCel) {
        char path[260]; e->fmtPath(path);
        *e->buysellCel = e->f2B420(0);
    }
    d[13] = (int32_t)(intptr_t)*e->buysellCel;
    for (int32_t i = 0; i < e->nBtn; i++) {
        const uint32_t* r = e->btnRec + i * 5;
        uint16_t w; memcpy(&w, (const uint8_t*)r + 10, 2);
        d[0] = (int32_t)(w + r[0]);
        e->ord10041(d, e->btnX[i + e->btnPage * 4] - 1 + e->x0, yb - 0x3f, -1, 5, 0);
    }
    e->ord10184();
    for (int32_t j = 0; j < e->nTab; j++) {
        const uint8_t* t = e->tabRec + j * 0x12;
        uint32_t a, b; memcpy(&a, t, 4); memcpy(&b, t + 4, 4);
        int32_t m = a ? 4 : 0;
        if (b) {
            int32_t y; memcpy(&y, t - 6, 4);
            e->ord10003(); e->ord10028();
            d[13] = (int32_t)(intptr_t)e->tabCel;
            d[0] = m == 4 ? j : j + 1;
            e->ord10041(d, j * 0x50 + e->x0, yb - 0x1c1, -1, 5, 0);
            if (!e->flag_c97a8) e->ord10150(y + yb - 0x1e0, m, 0);
        }
    }
    e->ord10184();
}
}
