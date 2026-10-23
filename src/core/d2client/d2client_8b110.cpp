// 1.13c D2Client+0x8B110 (453 B): per-frame world draw pass. Arg in EAX = game-view struct (32-bit layout).
// Seams for ordinal imports and unrecovered callees; globals are passed by value (RVAs in).
#include <cstdint>
#include <emscripten.h>
typedef uintptr_t U;
struct D2Client8B110Env {
    void (*ord_10020)();
    uint32_t (*ord_10008)();
    int32_t (*ord_10013)();
    int32_t (*ord_11080)(uint32_t);
    int32_t (*ord_11168)(uint32_t);
    void (*ord_10474)(int32_t* a, int32_t* b);
    void (*ord_10057)(int32_t x, int32_t y);
    void (*fn_59e30)();
    void (*fn_4edd0)();
    uint32_t (*ord_10331)(uint32_t);
    int32_t (*ord_10826)(uint32_t);
    void (*fn_3ae00)();
    void (*fn_3a6c0)();
    void (*fn_3a140)();
    void (*fn_4ec10)(U self);
    void (*fn_c4050)(U self);          // 0x14050
    void (*fn_2fc70)(U a, uint32_t b);  // 0x7FC70
    void (*fn_4f270)();
    void (*fn_4ee90)(U self);
    void (*fn_3a00)(U self);            // 0x13A00
    void (*ord_10075)(uint32_t a, uint32_t b, uint32_t c, uint32_t d);
    void (*ord_10054)();
    void (*fn_305a0)();
    void (*fn_ec0)();                   // 0x10EC0
};
struct D2Client8B110G {  // by-value globals
    uint32_t cbbfc, cbefc, cbf00, b9dc, cbef8, c97f8, cbf60, cbf64, cbf2c, cbbec;
    uint32_t bitmask0, bitmask1, bitmask3;  // gdwBitMasks[0],[1],[3]
    uint32_t* b9c8;                          // out: DAT_6fbbb9c8
    uint32_t* cbef8p;                        // out: DAT_6fbcbef8
};
// 1.13c D2Client+0x8B110
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_8b110(U self, const D2Client8B110G* g, const D2Client8B110Env* e) {
    auto r32 = [](U p) { return *(uint32_t*)p; };
    if (!r32(self + 0xc) && !r32(self + 0x10)) return;
    e->ord_10020();
    *g->b9c8 = e->ord_10008();
    if (e->ord_10013()) {
        int32_t x = e->ord_11080(g->cbbfc), y = e->ord_11168(g->cbbfc);
        if (g->cbefc) {
            int32_t a, b;
            e->ord_10474(&a, &b);
            x += (int32_t)(g->cbf00 * 0x10000);
            y += (int32_t)(g->b9dc * 0x10000);
        }
        e->ord_10057(x, y);
    }
    if ((int32_t)g->cbef8 < (int32_t)g->c97f8) { *g->cbef8p = g->c97f8; e->fn_59e30(); }
    e->fn_4edd0();
    uint32_t u = e->ord_10331(g->cbbfc);
    if (e->ord_10826(u) == 0x4a) e->fn_3ae00();
    if (e->ord_10826(u) == 0x78) e->fn_3a6c0(); else e->fn_3a140();
    if (g->bitmask0 & r32(self + 0x38))
        for (int32_t n = (int32_t)(r32(self + 0xeab0) * r32(self + 0xeab0)); n; n--) e->fn_4ec10(self);
    e->fn_c4050(self);
    if (r32(g->cbf60 + 0x114)) e->fn_2fc70(g->cbf60, 0x6fbc3904);
    if (r32(g->cbf64 + 0x114)) e->fn_2fc70(g->cbf64, 0x6fbc3890);
    if (r32(self + 0x38) & g->bitmask3) e->fn_4f270();
    e->fn_4ee90(self);
    if (r32(self + 0x38) & g->bitmask1) e->fn_3a00(self);
    if (g->cbf2c) e->ord_10075(0x6fbacde8, 0x6fbb5208, r32(g->cbbec + 0x10), r32(g->cbbec + 0x14));
    e->ord_10054();
    e->fn_305a0();
    e->fn_ec0();
}
