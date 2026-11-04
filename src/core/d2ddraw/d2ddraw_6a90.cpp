// src/core/d2ddraw/d2ddraw_6a90.cpp -- 1.13c D2DDraw.dll CelDrawShadow. Decompile-derived.
// Gate the shadow cel, then hand it to the shadow rasteriser (ordinal 10072). The colour is the
// first table dword only when the mode flag at DAT_6f8d01d0+0x14 is set.
#include <cstdint>
#include <cstring>

struct D2DDraw6A90Env {
    uint32_t (*valid)(const uint8_t* ctx, uint32_t a, uint32_t b);           // ordinal 10005(ctx, 0, 1)
    int32_t  (*celTop)(uint32_t h);                                          // ordinal 10000
    int32_t  (*celLeft)(uint32_t h);                                         // ordinal 10002
    int32_t  (*celWidth)(uint32_t h);                                        // ordinal 10060
    int32_t  (*celHeight)(uint32_t h);                                       // ordinal 10069
    void     (*rasterShadow)(uint32_t ebx, uint32_t ebp, int32_t zero, int32_t ox, int32_t oy,
                             uint8_t* buf, int32_t h, int32_t w, uint32_t colour, uint32_t extra);  // ordinal 10072
    int32_t  screenW, screenH, originX, originY;                             // DAT_6f8d01c8/01d8/01e8/01ec
    uint8_t* surface;                                                        // DAT_6f8cf8a8
    const uint8_t* table;                                                    // DAT_6f8cf784, byte-addressed
    int32_t  colourFlag;                                                     // dword at DAT_6f8d01d0+0x14
};

// 1.13c D2DDraw+0x06A90
// reg: ecx = ctx, edx = x, ebx = index, ebp = mode word. Returns nothing; the gate just skips the draw.
void d2ddraw_cel_draw_shadow(const D2DDraw6A90Env* e, const uint8_t* ctx, int32_t x, uint32_t ebx, uint32_t ebp) {
    if (e->valid(ctx, 0, 1) == 0) return;
    uint32_t h = 0;
    memcpy(&h, ctx + 0x3C, 4);
    int32_t w = e->celWidth(h);
    int32_t top = e->celTop(h);
    int32_t left = e->celLeft(h);
    int32_t xx = (x - top) + left;
    if (!(xx < e->screenW && -1 < w + top + xx)) return;
    int32_t ch = e->celHeight(h);
    if (!(-1 < ch && ch - (top >> 1) < e->screenH)) return;
    uint32_t colour = 0;
    if (e->colourFlag != 0) memcpy(&colour, e->table, 4);
    e->rasterShadow(ebx, ebp, 0, e->originX, e->originY, e->surface, e->screenH, e->screenW, colour, 0);
}
