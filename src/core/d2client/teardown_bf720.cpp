// 1.13c D2Client+0xBF720 UI teardown; env-seam style. The window handle comes from
// thunk 0xD0B4 (D2Gfx 10048); 0x3CC60 / 0x3CCC0 take EAX = 2 / 4 and two stack words (window, tag; ret 8). The four objects
// are released through vtable slot 0 (ECX = object, stack 1); slot 0x2C of 0xFAD68 (ECX) is saved into 0x11BBB0 first.
#include <cstdint>
#include <emscripten.h>
struct D2cBF720G {  // globals, RVA = 0x6fb.. - 0x6fab0000
    int32_t aad74;      // 0xFAD74
    int32_t cc434;      // 0x11C434
    int32_t d349c;      // 0x12349C
    int32_t d3498;      // 0x123498
    void* aad68;        // 0xFAD68 (vtbl slot 0x2c returns int)
    int32_t aad60;      // 0xFAD60
    void* aad64;        // 0xFAD64
    void* aad6c;        // 0xFAD6C
    void* aad70;        // 0xFAD70
    int32_t aad5c;      // 0xFAD5C
    int32_t cbbb0;      // 0x11BBB0
    int32_t cbbbc;      // 0x11BBBC
    int32_t cbe78;      // 0x11BE78
    int32_t cbc34;      // 0x11BC34
};
struct D2cBF720Env {
    uint32_t (*hwnd)();                                      // thunk 0xD0B4 (D2Gfx 10048)
    void (*ecc60)(uint32_t mode, uint32_t w, uint32_t tag);  // D2Client+0x3CC60 (EAX = 2), tag 0x6fba51ec
    void (*eccc0)(uint32_t mode, uint32_t w, uint32_t tag);  // D2Client+0x3CCC0 (EAX = 4), tag 0x6fba51bc
    void (*f53d20)();                                        // D2Client+0xA3D20
    int32_t (*slot2c)(void* obj);                            // (*vtbl+0x2c)(ECX = obj)
    void (*del)(void* obj, int32_t flags);                   // (*vtbl+0)(ECX = obj, 1)
    uint32_t tagA, tagB;
};
extern "C" {
// 1.13c D2Client+0xBF720
EMSCRIPTEN_KEEPALIVE void d2c_bf720(const D2cBF720Env* e, D2cBF720G* g) {
    if (g->aad74) {
        g->cc434 = 1;
        if (!g->d349c) {
            g->d349c = 1;
            if (g->d3498) { e->ecc60(2, e->hwnd(), e->tagA); g->d3498 = 0; }
            e->eccc0(4, e->hwnd(), e->tagB);
        }
        g->aad74 = 0;
    }
    if (g->aad68) g->cbbb0 = e->slot2c(g->aad68);
    if (g->aad60) { e->f53d20(); g->aad60 = 0; }
    if (g->aad64) { e->del(g->aad64, 1); g->aad64 = 0; }
    if (g->aad68) { e->del(g->aad68, 1); g->aad68 = 0; }
    if (g->aad6c) { e->del(g->aad6c, 1); g->aad6c = 0; }
    if (g->aad70) { e->del(g->aad70, 1); g->aad70 = 0; }
    if (g->aad5c == 0) g->cbbbc = 0;
    else if (g->aad5c > 0 && g->aad5c < 5) { g->cbe78 = 0; if (g->cbc34 == 0xd) g->cbc34 = 0xc; }
}
}
