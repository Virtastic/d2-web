// Cursor-following panel tick of D2Client. Callees through D2cHost (client_host.h) in retail order.
#include "client_host.h"
#include <emscripten.h>

struct D2c16910G {   // the globals the function reads (guest values)
    uint32_t c4f0, b854, b858, b820, b824, b828, b82c, b830;
    uint32_t b838[7];   // table at 0x6FBCB838
};

// 1.13c D2Client+0x16910. g.c4f0 == 0: return. obj = DAT_6FBCB834 (objTok its guest address; 0 = null).
// A (obj null or obj[0] != 4) and g.b854 < 7 (signed): x = clamp(b824, 0, b820-1); y = clamp(b828+b82c, b82c, b830-b82c-1);
//   rec18 zeroed, rec[0] = (int32)b858 >> 8, rec[10] = b838[b854]; 0xD096(rec, y, x, -1, 5, 0), then 0x167D0().
// B (obj[0] == 4): rec18 zeroed, rec[13]=0, rec[16]=0, rec[0]=0, byte 5 of rec = 1; 0x6E2C0(EAX=-1; obj, rec, &outA),
//   zero -> return; 0xC0D0(rec, 0, 1) zero -> return; w = rec[15]; zero -> fatal (0xBF92(0x2B3), 0xBF80(0x6FB8359F, r),
//   0x3664(-1)); else h = 0xC0E2(w); 0xC0D6(w, b824 - h/2) -> k; 0x6B5A0(b82c + b828 - k/2) with EBX = obj.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_16910(D2cHost h, const D2c16910G* g, uint32_t objTok, const uint32_t* obj,
                                                    uint32_t* rec18, uint32_t recTok, uint32_t outATok) {
    if (g->c4f0 == 0) return;
    if (obj == nullptr || obj[0] != 4) {
        int32_t idx = (int32_t)g->b854;
        if (idx >= 7) return;
        int32_t x = (int32_t)g->b824;
        if (x < 0) x = 0; else if (x > (int32_t)g->b820 - 1) x = (int32_t)g->b820 - 1;
        int32_t sum = (int32_t)(g->b828 + g->b82c), y = sum;
        if (y < (int32_t)g->b82c) y = (int32_t)g->b82c;
        else { int32_t m = (int32_t)g->b830 - (int32_t)g->b82c - 1; if (y > m) y = m; }
        for (int i = 0; i < 18; ++i) rec18[i] = 0;
        rec18[10] = g->b838[idx];   // negative idx reads below the table in retail; callers keep it 0..6
        rec18[0] = (uint32_t)((int32_t)g->b858 >> 8);
        D2cCall c = { 0xD096, 0, 0, 0, 0, 0, { recTok, (uint32_t)y, (uint32_t)x, 0xFFFFFFFFu } };
        h(&c);   // trailing stack words 5, 0
        D2cCall d = { 0x167D0, 0, 0, 0, 0, 0, { 0, 0, 0, 0 } };
        h(&d);
        return;
    }
    for (int i = 0; i < 18; ++i) rec18[i] = 0;
    rec18[13] = 0; rec18[16] = 0; rec18[0] = 0;
    ((uint8_t*)rec18)[5] = 1;
    D2cCall a = { 0x6E2C0, 0xFFFFFFFFu, 0, 0, 0, 0, { objTok, recTok, outATok, 0 } };
    if (!h(&a)) return;
    D2cCall b = { 0xC0D0, 0, 0, 0, 0, 0, { recTok, 0, 1, 0 } };
    if (!h(&b)) return;
    uint32_t w = rec18[15];
    if (w == 0) {
        D2cCall e1 = { 0xBF92, 0, 0, 0, 0, 0, { 0x2B3, 0, 0, 0 } };
        uint32_t r = h(&e1);
        D2cCall e2 = { 0xBF80, 0, 0, 0, 0, 0, { 0x6FB8359Fu, r, 0, 0 } };
        h(&e2);
        D2cCall e3 = { 0x3664, 0, 0, 0, 0, 0, { 0xFFFFFFFFu, 0, 0, 0 } };
        h(&e3);
        return;
    }
    D2cCall f = { 0xC0E2, 0, 0, 0, 0, 0, { w, 0, 0, 0 } };
    uint32_t hw = h(&f);
    D2cCall k = { 0xC0D6, 0, 0, 0, 0, 0, { w, g->b824 - (hw >> 1), 0, 0 } };
    uint32_t kk = h(&k);
    D2cCall p = { 0x6B5A0, 0, 0, 0, objTok, 0, { g->b82c - (kk >> 1) + g->b828, 0, 0, 0 } };
    h(&p);
}
