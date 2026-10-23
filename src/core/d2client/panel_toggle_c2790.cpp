// D2Client+0xC2790: panel open/close/toggle (fastcall ECX=panel, EDX=mode, stack=arg3); env-seam style.
// Ordinals 10265/10024/10227/10006 unverified.
#include <cstdint>
#include <emscripten.h>
struct D2cC2790Env {
    int32_t* panel;      // DAT_6fbaad80[0x26]
    int32_t* g_ad94;     // DAT_6fbaad94
    int32_t  g_c9854;    // DAT_6fbc9854
    int32_t* g_cbbfc;    // *DAT_6fbcbbfc (null = pointer unset)
    int32_t* g_cc418;    // DAT_6fbcc418
    int32_t* g_cc414;    // DAT_6fbcc414
    int32_t  w;          // DAT_6fb8bc48
    int32_t  h;          // DAT_6fba9e14
    int32_t  cursorX;    // DAT_6fbcb828
    const int32_t* blk1; // 7 words: DAT_6fbaad88,dd0,dbc,dc0,e10,e14,dd8
    const int32_t* blk2; // 2 words: DAT_6fbaad84,ad90
    int32_t (*ord10227)();
    int32_t (*f71a80)();
    int32_t (*f55440)();
    void (*f71c40)(uint32_t);
    void (*f1eb50)();
    void (*f721f0)();
    void (*ord10006)();
    void (*f3a4d0)(int32_t a, int32_t b, int32_t c);
    void (*f61b0)();
    void (*f3bbf0)();
    void (*f1f80)();
    void (*fatal)(uint32_t code);  // Ordinal_10265/10024/_exit
    int32_t* clr;        // DAT_6fbcc3dc,3e0,3f8,3fc
};
extern "C" {
// 1.13c D2Client+0xC2790
EMSCRIPTEN_KEEPALIVE uint32_t d2c_c2790(const D2cC2790Env* e, uint32_t p, uint32_t mode, int32_t a3) {
    if (p > 0x25) { e->fatal(0x13ff); return 0; }
    if (mode > 2) { e->fatal(0x1400); return 0; }
    int32_t old = e->panel[p];
    int32_t o = e->ord10227();
    if ((((o == 0) || e->g_c9854 == 0) && (p == 0x23 || p == 0x24 || p == 0x25)) || e->f71a80() == 0) return 0;
    uint32_t u = 0;
    if (mode == 0) {
        if (!e->g_cbbfc || (*e->g_cbbfc == 0 && e->f55440() == 0)) e->panel[p] = 1;
    } else if (mode == 1) {
        e->panel[p] = 0;
    } else {
        if (!e->g_cbbfc) e->panel[p] = e->panel[p] == 0;
        else if (*e->g_cbbfc == 0 && e->f55440() == 0) e->panel[p] = e->panel[p] == 0;
        else if (p == 5) *e->g_ad94 = (*e->g_ad94 == 0);
    }
    int32_t now = e->panel[p];
    if (old == 0) { if (now) { e->f71c40(u); if (p == 5) e->f1eb50(); } }
    else if (now == 0) e->f721f0();
    int32_t cx = e->cursorX, W = e->w, H = e->h;
    if (p - 1 > 0x24) return 1;
    int32_t q = (W + ((W >> 31) & 3)) >> 2, hq = (H + ((H >> 31) & 3)) >> 2;
    int32_t a, b;
    switch (p) {
    case 1: e->f3bbf0(); /* fallthrough */
    case 4: {
        const int32_t* k = e->blk1;
        bool idle = !k[0] && !k[1] && !k[2] && !k[3] && !k[4] && !k[5] && !k[6];
        if (idle) {
            if (now == 0) {
                *e->g_cc418 = 0; *e->g_cc414 = 0; e->ord10006(); e->f3a4d0(H, 0, 0);
                if (a3 && !(W / 2 <= cx)) e->f61b0();
                return 1;
            }
            *e->g_cc418 = -q; *e->g_cc414 = 1; e->ord10006(); e->f3a4d0(H - hq, 0, -hq);
            if (a3 && !(cx <= q)) e->f61b0();
            return 1;
        }
        if (now != 0) goto L_aba;
        *e->g_cc418 = q; *e->g_cc414 = 2; e->ord10006();
        a = H + hq; b = hq; break;
    }
    case 2: case 0xf: case 0x10: case 0x14: case 0x16: case 0x24: case 0x25:
        if (!e->blk2[0] && !e->blk2[1]) {
            if (now == 0) {
                *e->g_cc418 = 0; *e->g_cc414 = 0; e->ord10006(); e->f3a4d0(H, 0, 0);
                if (a3 && !(cx <= W / 2)) e->f61b0();
                return 1;
            }
            *e->g_cc418 = q; *e->g_cc414 = 2; e->ord10006(); e->f3a4d0(H + hq, 0, hq);
            if (a3 && !(W - q <= cx)) e->f61b0();
            return 1;
        }
        if (now != 0) goto L_aba;
    L_c5f:
        *e->g_cc418 = -q; *e->g_cc414 = 1; e->ord10006();
        a = H - hq; b = -hq; break;
    case 0xb: if (now == 0) e->f1f80(); return 1;
    case 0xc: case 0x19: case 0x1a: case 0x1b: case 0x1c: case 0x1d: case 0x20:
        *e->g_cc418 = 0;
        if (now != 0) goto L_aba;
        *e->g_cc414 = 0; e->ord10006(); a = H; b = 0; break;
    case 0xe:
        if (now != 0) goto L_c5f;
        *e->g_cc418 = 0; *e->g_cc414 = 0; e->ord10006(); a = H; b = 0; break;
    default: return 1;
    }
    e->f3a4d0(a, 0, b);
    return 1;
L_aba:
    *e->g_cc418 = 0; *e->g_cc414 = 3; e->ord10006(); e->f3a4d0(H, 0, 0);
    e->clr[0] = e->clr[1] = e->clr[2] = e->clr[3] = 0;
    return 1;
}
}
