// src/core/d2ddraw/d2ddraw_6f60.cpp -- 1.13c D2DDraw.dll CelDrawClipped. Decompile-derived.
// Clips the cel rectangle first (clip rect at 0x06E20). Only when that clip reports the rect
// as visible does the cel go to the clipped rasteriser (ordinal 10080).
#include <cstdint>
#include <cstring>

struct D2DDraw6F60Env {
    uint32_t (*valid)(const uint8_t* ctx, uint32_t a, uint32_t b);           // ordinal 10005(ctx, 0, 1)
    uint32_t (*clipRect)(int32_t ecx, uint32_t obj, const int32_t* rect, int32_t* out,
                         int32_t esi, int32_t edi);                          // clip rect at 0x06E20
    uint32_t (*tabLookup)(int32_t idx, uint32_t* name);                      // tab lookup at 0x06A10
    void     (*rasterClipped)(const uint8_t* ctx, const int32_t* rect, int32_t zero, int32_t edi,
                              int32_t esi, uint8_t* buf, int32_t h, int32_t w, uint32_t zero2,
                              uint32_t tab, uint32_t ebp, uint32_t zero3, uint32_t zero4);  // ordinal 10080
    int32_t  screenW, screenH;                                               // DAT_6f8d01c8/01d8
    uint8_t* surface;                                                        // DAT_6f8cf8a8
};

// 1.13c D2DDraw+0x06F60
// reg: ecx = ctx, edx = rect pointer, esi, edi, ebp pass straight through.
void d2ddraw_cel_draw_clipped(const D2DDraw6F60Env* e, const uint8_t* ctx, const int32_t* rect,
                              int32_t esi, int32_t edi, uint32_t ebp) {
    if (e->valid(ctx, 0, 1) == 0) return;
    uint32_t h = 0;
    memcpy(&h, ctx + 0x3C, 4);
    int32_t out[4] = {0, 0, 0, 0};
    if (e->clipRect(0, h, rect, out, esi, edi) != 0) return;
    uint32_t name = 0;
    uint32_t tab = e->tabLookup(0, &name);
    e->rasterClipped(ctx, rect, 0, edi, esi, e->surface, e->screenH, e->screenW, 0, tab, ebp, 0, 0);
}
