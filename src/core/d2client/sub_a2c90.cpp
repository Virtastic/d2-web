// D2Client+0xA2C90: per-record effect/sound dispatch; env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2cA2C90Env {
    const uint8_t* (*rec)(int32_t idx);                  // sgptDataTables+0xb98 table, 0x23c stride; null if idx<0 or >= count(+0xba0)
    int32_t handlerCount;                                // DAT_6fb8ec50
    int32_t bitMask4;                                    // gdwBitMasks[4]
    void* (*handler)(int32_t i);            // DAT_6fb8ea48[i] (null if none)
    void (*f51340)(int32_t* out);                        // +0x51340 (out: [0]=local_8,[1]=local_4 as 2 dwords)
    int32_t (*f1630)(int32_t* u);                        // +0x1630
    int32_t (*f1660)(int32_t* u);                        // +0x1660
    void (*f526b0)(int32_t a, int32_t b, int32_t c, int32_t d, int32_t e, int32_t f);  // +0xA26B0
    void (*f69a80)(int32_t a, int32_t b, int32_t c, int32_t d, int32_t e, int32_t f, int32_t g, int32_t h, int32_t i); // +0xB9A80
    int32_t (*ord10327)(int32_t* u, int32_t z);          // Ordinal_10327
    void (*f58630)();                                    // +0xA8630
    void (*f38a70)(int32_t a, int32_t b, int32_t c, int32_t d); // +0x88A70
    int32_t (*f55f70)();                                 // +0xA5F70
    void (*f1b300)(int32_t* u, int32_t a, int32_t b, int32_t c, int32_t d); // +0x6B300
    int32_t (*ord10786)(int32_t* p, int32_t a, int32_t retaddr, int32_t* u); // Ordinal_10786
    int32_t* gCbbfc;                                     // DAT_6fbcbbfc (value)
    int32_t* g985c;                                      // DAT_6fbc985c
    int32_t g97f8;                                       // DAT_6fbc97f8
};
static int16_t s16(const uint8_t* p, int o) { return (int16_t)(p[o] | (p[o + 1] << 8)); }
static int32_t s32(const uint8_t* p, int o) { return (int32_t)(p[o] | (p[o+1] << 8) | (p[o+2] << 16) | ((uint32_t)p[o+3] << 24)); }
extern "C" {
// 1.13c D2Client+0xA2C90
EMSCRIPTEN_KEEPALIVE int32_t d2c_a2c90(const D2cA2C90Env* e, int32_t* u, int32_t idx, int32_t p3, int32_t p4, int32_t retaddr) {
    const uint8_t* r = (idx < 0) ? nullptr : e->rec(idx);
    if (!r || s16(r, 0xf4) < 0 || e->handlerCount <= s16(r, 0xf4)) return 0;
    int32_t l4 = 0, l8 = 0, l10 = 0, l14 = 0, a4 = 0, a2 = 0;
    if (s16(r, 0xe8) > 0) {
        if (p4) {
            int32_t o[2] = {0, 0};
            e->f51340(o); l8 = o[0]; l4 = o[1]; a4 = l4; a2 = l8;
            if (l4 && l8) {
                a4 = l4 - e->f1630(u);
                a2 = l8 - e->f1660(u);
                l10 = l4 + a4;
                l14 = l8 + a2;
            }
        }
        if ((e->bitMask4 & r[4] & 0xff) == 0) e->f526b0(s16(r, 0xe8), idx, p3, a4, a2, l14);
        else e->f69a80(l14, s16(r, 0xe8), a4, a2, l10, l14, idx, p3, 0);
        if (u) u[0x31] |= 0x40;
    }
    int32_t h = s16(r, 0xf4);
    if (p4 && s16(r, 0x16c) > 1) h = s16(r, 0x16c);
    if (h < 0 || e->handlerCount <= h || !e->handler(h)) { if (u) u[0x31] |= 0x40; }
    else ((void(*)(int32_t))e->handler(h))(p3);
    int32_t pv = (int32_t)(intptr_t)u;  // piVar4: unit, or local_4 after f58630
    bool useL4 = false;
    if (e->ord10327(u, 0)) { e->f58630(); useL4 = true; }
    if (!l14) return 0;
    int32_t p = useL4 ? l4 : pv;
    if (s16(r, 0x100) > 0) e->f38a70(p, 0, 0, 0);
    if (s16(r, 0x10a) > 0) { int32_t t = e->f55f70(); if (t) e->f38a70(t, 0, 0, 0); }
    if (s16(r, 0x108) > 0) { int32_t t = e->f55f70(); if (t) e->f1b300((int32_t*)(intptr_t)t, 2, 0, 0, 0); }
    int32_t n = e->ord10786((int32_t*)(intptr_t)p, s32(r, 400), retaddr, u);
    if (n > 0 && (int32_t)(intptr_t)e->gCbbfc == p) {
        if (n > 0xc) n = 0xc;
        if (r[0x10] == 0x12) {
            if ((((int32_t*)(intptr_t)p)[0xe] & 0xffffff00u) == 0) { *e->g985c = e->g97f8 + n; return 1; }
        } else *e->g985c = e->g97f8 + n;
    }
    return 1;
}
}
