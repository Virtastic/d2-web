// D2Client+0xC1C40: screen-state change switch (EAX=state, ECX=arg); env-seam style.
// Ordinal_10048 (unverified ordinal name) yields a handle passed to FUN_6faeccc0 (open) / FUN_6faecc60 (close).
#include <cstdint>
#include <emscripten.h>
struct D2cC1C40G {  // globals, RVA = VA-0x6fab0000
    int32_t g_cc434;   // 0x11C434
    int32_t g_d349c;   // 0x12349C
    int32_t g_d3498;   // 0x123498
    int32_t g_aada8;   // 0xFADA8
    int32_t g_cc2f8, g_cc2f4, g_cb9ac, g_77fc, g_77ec, g_cc2d0, g_bb348;
};
struct D2cC1C40Env {
    int32_t (*ord10048)();
    void (*open)(int32_t h, uint32_t tagVA);   // FUN_6faeccc0
    void (*close)(int32_t h, uint32_t tagVA);  // FUN_6faecc60
    void (*f3bb90)(); void (*f1f5b0)(); void (*f272e0)(); void (*f2c770)();
    void (*f2dd0)(int32_t); void (*f6e920)(); void (*f411a0)(); void (*f3cbc0)(); void (*f3cb40)();
    void (*f6fa10)(int32_t ecx);
};
static void sw(const D2cC1C40Env* e, uint32_t t, bool op) { op ? e->open(e->ord10048(), t) : e->close(e->ord10048(), t); }
// shared "enter ingame-ui mode" prefix of states 0xC/0x19/0x1A
static void enter2(const D2cC1C40Env* e, D2cC1C40G* g) {
    g->g_cc434 = 2;
    if (g->g_d349c == 0) {
        g->g_d349c = 1;
        if (g->g_d3498 != 0) { sw(e, 0x6fba51ec, false); g->g_d3498 = 0; }
        sw(e, 0x6fba51bc, true);
    }
}
// shared prefix of states 5/0x17; returns true if it changed flags
static void leave2(const D2cC1C40Env* e, D2cC1C40G* g) {
    g->g_cc434 = 0;
    if (g->g_d349c == 1) {
        g->g_d349c = 0; sw(e, 0x6fba51bc, false);
        if (g->g_d3498 == 0) { sw(e, 0x6fba51ec, true); g->g_d3498 = 1; }
    }
}
extern "C" {
// 1.13c D2Client+0xC1C40
EMSCRIPTEN_KEEPALIVE void d2c_c1c40(const D2cC1C40Env* e, D2cC1C40G* g, uint32_t st, int32_t ecx) {
    static const uint32_t open_tag[0x26] = {  // plain "open tag" states; 0 = not plain
        0x6fb80580,0,0x6fb80628,0x6fb8061c,0x6fb80604,0,0x6fb80640,0x6fb80664,0,0,0x6fb80780,0,0,0,0,0,
        0x6fb80810,0x6fb807bc,0,0x6fb80658,0x6fb80828,0x6fb80864,0,0,0,0,0,0,0,0,0,0,0,0x6fb80924,0x6fb80960,0x6fb80978,0x6fb8099c,0x6fb809c0};
    switch (st) {
    case 1: e->f3bb90(); sw(e, 0x6fb805b0, true); return;
    case 5:
        e->f1f5b0(); leave2(e, g);
        if (g->g_aada8) sw(e, 0x6fb80780, false);
        return;
    case 9: sw(e, 0x6fb80680, true); g->g_cc2f8 = 1; g->g_cc2f4 = 0; return;
    case 10: sw(e, 0x6fb80780, true); g->g_cb9ac = 1; return;
    case 0xb: e->f2dd0(1); e->f6e920(); return;
    case 0xc: enter2(e, g); e->f411a0(); e->f6fa10(ecx); sw(e, 0x6fb8bc60, true); return;
    case 0xf: e->f6fa10(ecx); sw(e, 0x6fb8bcc0, true); return;
    case 0x16: sw(e, 0x6fb8087c, true); e->f272e0(); return;
    case 0x17:
        leave2(e, g); g->g_77fc = 0; g->g_77ec = 0; g->g_cc2d0 = 1; g->g_bb348 = 0;
        e->f6fa10(ecx); sw(e, 0x6fb8bd18, true); return;
    case 0x18: e->f2c770(); return;
    case 0x19: enter2(e, g); e->f3cbc0(); e->f6fa10(ecx); sw(e, 0x6fb8bdb8, true); return;
    case 0x1a: enter2(e, g); e->f3cb40(); e->f6fa10(ecx); sw(e, 0x6fb8be18, true); return;
    case 0x1f: e->f6fa10(ecx); sw(e, 0x6fb8be6c, true); return;
    default:
        if (st < 0x26 && open_tag[st]) sw(e, open_tag[st], true);
        else if (st == 0x25) sw(e, 0x6fb809c0, true);
    }
}
}
