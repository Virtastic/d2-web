// 1.13c D2Client+0xA9A20 (297 B, ret 0x10): ambient/light colour of a unit.
// EAX = flag, ECX = this (only its top byte survives, as the residue of the push-ecx slot), ESI = unit; stack: oa (dword out), r, g, b (byte outs).
// flag != 0: m = C1DE(unit). m == 8: alpha = 524D0(&r,&g,&b). m outside 0x6B..0x6C: rgb = bytes at 0x119614..16, alpha = dword 0x119610.
// m in 0x6B..0x6C: alpha 0 unless dword 0x11C2E4 != 0 and dword 0xF2130 == -1, then rgb = (0xFF,0x40,0x30), alpha = 0xA0.
// Any nonzero channel ends it; otherwise (and for flag == 0) C59E(unit,&hi,&r,&g,&b) gives alpha = hi; still all zero: C5B6(DAT 0x11C3B8,&r,&g,&b) and
// alpha = C5A4(DAT 0x11C3B8).
#include <emscripten.h>
#include "d2client_a9_env.h"
struct D2cA9a20G { uint32_t c2e4; int32_t a2130; uint32_t c9610; uint8_t rgb[4]; uint32_t c3b8; };   // RVAs 0x11C2E4, 0xF2130, 0x119610, 0x119614 (3 bytes), 0x11C3B8
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_a9a20(uint32_t flag, uint32_t thiz, uint32_t unit, uint32_t* oa, uint8_t* orr, uint8_t* og, uint8_t* ob, const D2cA9a20G* g, const D2cA9Env* e) {
    uint8_t r = 0, gg = 0, b = 0, hi = (uint8_t)(thiz >> 24);
    uint32_t eax = flag;
    bool store = false;
    if (flag) {
        const int32_t m = (int32_t)e->call(e->ctx, 0xC1DE, 0, 0, 0, 0, unit, 0, 0, 0, 0);
        if (m == 8) eax = e->call(e->ctx, 0x524D0, 0, 0, 0, 0, d2c_hp(&r), d2c_hp(&gg), d2c_hp(&b), 0, 0);
        else if (m > 0x6a && m <= 0x6c) {
            if (g->c2e4 != 0 && g->a2130 == -1) { r = 0xff; gg = 0x40; b = 0x30; eax = 0xa0; } else eax = 0;
        } else { r = g->rgb[0]; gg = g->rgb[1]; b = g->rgb[2]; eax = g->c9610; }
        store = r || gg || b;
    }
    if (!store) {
        e->call(e->ctx, 0xC59E, 0, 0, 0, 0, unit, d2c_hp(&hi), d2c_hp(&r), d2c_hp(&gg), d2c_hp(&b));
        eax = hi;
        if (!(r || gg || b)) {
            e->call(e->ctx, 0xC5B6, 0, 0, 0, 0, g->c3b8, d2c_hp(&r), d2c_hp(&gg), d2c_hp(&b), 0);
            eax = e->call(e->ctx, 0xC5A4, 0, 0, 0, 0, g->c3b8, 0, 0, 0, 0);
        }
    }
    *oa = eax; *orr = r; *og = gg; *ob = b;
}
