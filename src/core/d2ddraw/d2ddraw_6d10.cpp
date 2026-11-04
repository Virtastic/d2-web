// src/core/d2ddraw/d2ddraw_6d10.cpp -- 1.13c D2DDraw.dll CelDraw. Decompile-derived.
// Gate the cel against the screen, then hand it to the cel rasteriser (ordinal 10015) with the
// colour-table entry at byte 0x0C. The tab index is read before the lookup writes it, so the
// colour index is always 0. The 0x118 colour branch is dead (its local is set to 1 and never written).
#include <cstdint>
#include <cstring>

struct D2DDraw6D10Env {
    uint32_t (*valid)(const uint8_t* ctx);                                   // ordinal 10005(ctx)
    int32_t  (*celTop)(uint32_t h);                                          // ordinal 10000
    int32_t  (*celLeft)(uint32_t h);                                         // ordinal 10002
    int32_t  (*celWidth)(uint32_t h);                                        // ordinal 10060
    int32_t  (*celHeight)(uint32_t h);                                       // ordinal 10069
    uint32_t (*tabLookup)(int32_t idx, uint32_t* name);                      // tab lookup at 0x06A10
    void     (*rasterCel)(const uint8_t* ctx, int32_t x, int32_t y, int32_t ox, int32_t oy,
                          uint8_t* buf, int32_t h, int32_t w, uint32_t colour, uint32_t tab,
                          uint32_t ebp);                                     // ordinal 10015
    int32_t  screenW, screenH, originX, originY;                             // DAT_6f8d01c8/01d8/01e8/01ec
    uint8_t* surface;                                                        // DAT_6f8cf8a8
    const uint8_t* table;                                                    // DAT_6f8cf784, byte-addressed
};

static inline uint32_t d2ddraw_6d10_rd32(const uint8_t* base, uint32_t off) {
    uint32_t v; memcpy(&v, base + off, 4); return v;
}

// 1.13c D2DDraw+0x06D10
// reg: ecx = ctx (cel context; cel handle is the dword at +0x3C), edx = x, edi = yBias, ebp = mode word.
// Returns 1 when the cel went to the rasteriser, 0 when a gate rejected it.
uint32_t d2ddraw_cel_draw(const D2DDraw6D10Env* e, const uint8_t* ctx, int32_t x, int32_t yBias, uint32_t ebp) {
    if (e->valid(ctx) == 0) return 0;
    uint32_t h = 0;
    memcpy(&h, ctx + 0x3C, 4);
    int32_t w = e->celWidth(h);
    int32_t top = e->celTop(h);
    int32_t left = e->celLeft(h);
    if (!(left + x < e->screenW && -1 < w + left + x)) return 0;
    int32_t ch = e->celHeight(h);
    if (!(-1 < ch + top && (ch + top) - yBias < e->screenH)) return 0;
    uint32_t name = 0;
    uint32_t tab = e->tabLookup(e->screenH, &name);
    uint32_t colour = d2ddraw_6d10_rd32(e->table, 0x0C);
    e->rasterCel(ctx, x, top, e->originX, e->originY, e->surface, e->screenH, e->screenW, colour, tab, ebp);
    return 1;
}
