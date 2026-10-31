// D2Client batch b55-cli-a. Globals the comments reads come in structs of pointers (each member names its RVA); callees go through D2cHost.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
typedef int32_t i32; typedef uint32_t u32;
struct B55PanelG {
    i32 *a /*1AADDC*/, *b /*1AADE4*/, *c /*1AADF0*/, *d /*1AADE8*/, *e /*1AADB0*/, *f /*1AADE0*/, *g /*1AADEC*/, *h /*1AAD8C*/,
        *i /*1AADFC*/, *j /*1CC474*/, *k /*1A5384*/, *l /*1AAD84*/, *m /*1AAD90*/, *out /*1CC088*/;
};
struct B55PanelG2 {
    i32 *a /*1AADDC*/, *b /*1AADE4*/, *d /*1AADE8*/, *e /*1AADB0*/, *c /*1AADF0*/, *n /*1AADF4*/, *f /*1AADE0*/, *h /*1AAD8C*/,
        *o /*1AADC0*/, *p /*1AADD8*/, *q /*1AAE10*/, *r /*1AAD88*/, *s /*1AADBC*/, *t /*1AADD0*/, *g /*1AADEC*/, *out /*1CC08C*/;
};
struct B55TextG { i32 *betaA /*18F0D8*/; uint8_t *betaB /*1C3AB6 (u8)*/; };
struct B55RefreshG { i32 *ready /*1CC3B8*/, *player /*1CBBFC*/, *mode /*1CC394*/, *v /*1CC21C*/, *flag /*1C9854*/; };
extern "C" {
// 1.13c D2Client+0x637B0
EMSCRIPTEN_KEEPALIVE i32 d2c_637b0(B55PanelG* p) {
    i32 r = 0;
    if (*p->a || *p->b || *p->c || *p->d || *p->e || *p->f || *p->g || *p->h) r = 1;
    if ((*p->i || *p->j) && *p->k != 2) r = 1;
    if (*p->l == 0 && *p->m == 0) { *p->out = 0; return r; }
    *p->out = 1;
    return 1;
}
// 1.13c D2Client+0x63880
EMSCRIPTEN_KEEPALIVE i32 d2c_63880(B55PanelG2* p) {
    if (!*p->a && !*p->b && !*p->d && !*p->e && !*p->c && !*p->n && !*p->f && !*p->h && !*p->o && !*p->p &&
        !*p->q && !*p->r && !*p->s && !*p->t && !*p->g) {
        *p->out = *p->q;
        return 0;
    }
    *p->out = 1;
    return 1;
}
// 1.13c D2Client+0x64250  (ECX = param_1)
EMSCRIPTEN_KEEPALIVE void d2c_64250(D2cHost h, B55TextG* g, const uint8_t* param1, u32 strDiablo, u32 strText) {
    D2cCall c = {}; c.rva = 0x80000000u | 10040; h(&c);
    i32 m = 0; for (int k = 0; k < 4; k++) m |= (i32)param1[0x124 + k] << (8 * k);
    int a, b;
    if (m == 0) { a = 1; b = 0; } else if (m == 1) { a = 0; b = 1; } else if (m == 2) { a = 1; b = 1; } else return;
    *g->betaA = a; *g->betaB = (uint8_t)b;
    D2cCall d = {}; d.rva = 0x80000000u | 426; d.s[0] = strDiablo; d.s[1] = strText; d.s[2] = 0; d.s[3] = (u32)b; h(&d);
}
}
