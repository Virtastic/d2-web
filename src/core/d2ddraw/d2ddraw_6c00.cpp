// src/core/d2ddraw/d2ddraw_6c00.cpp -- 1.13c D2DDraw.dll CelDrawColor. Decompile-derived.
// Gate the coloured cel, then the cel rasteriser (ordinal 10015). The top getter (ordinal 10000)
// is called by the retail code but its result is unused, so the port does not call it.
#include <cstdint>
#include <cstring>

struct D2DDraw6C00Env {
    uint32_t (*valid)(const uint8_t* ctx);                                   // ordinal 10005(ctx)
    int32_t  (*celLeft)(uint32_t h);                                         // ordinal 10002
    int32_t  (*celWidth)(uint32_t h);                                        // ordinal 10060
    int32_t  (*celHeight)(uint32_t h);                                       // ordinal 10069
    uint32_t (*tabLookup)(int32_t idx, uint32_t* name);                      // tab lookup at 0x06A10
    void     (*rasterCel)(const uint8_t* ctx, int32_t x, int32_t y, int32_t ox, int32_t oy,
                          uint8_t* buf, int32_t h, int32_t w, uint32_t colour, uint32_t tab,
                          uint32_t extra);                                   // ordinal 10015
    int32_t  screenW, screenH, originX, originY;
    uint8_t* surface;
    const uint8_t* table;                                                    // byte-addressed
};

static inline uint32_t d2ddraw_6c00_rd32(const uint8_t* base, uint32_t off) {
    uint32_t v; memcpy(&v, base + off, 4); return v;
}

// 1.13c D2DDraw+0x06C00
// reg: ecx = ctx, edx = x, esi = y argument, ebx = palette index, edi = yBias, ebp = mode.
// Mode 7 selects the 0x118 colour; a nonzero index adds the 0xD0-based entry.
uint32_t d2ddraw_cel_draw_color(const D2DDraw6C00Env* e, const uint8_t* ctx, int32_t x, int32_t esi,
                                uint32_t ebx, int32_t yBias, uint32_t ebp) {
    if (e->valid(ctx) == 0) return 0;
    uint32_t h = 0;
    memcpy(&h, ctx + 0x3C, 4);
    int32_t w = e->celWidth(h);
    int32_t left = e->celLeft(h);
    if (!(left + x < e->screenW && -1 < w + left + x)) return 0;
    int32_t ch = e->celHeight(h);
    if (!(-1 < ch && ch - yBias < e->screenH)) return 0;
    uint32_t name = 0;
    uint32_t tab = e->tabLookup(e->screenH, &name);
    uint32_t colour = d2ddraw_6c00_rd32(e->table, 0x0C);
    if (ebp == 7) colour = d2ddraw_6c00_rd32(e->table, 0x118);
    uint32_t extra = 0;
    if (ebx != 0) extra = d2ddraw_6c00_rd32(e->table, 0xD0 + ebx * 4);
    e->rasterCel(ctx, esi, 0, e->originX, e->originY, e->surface, e->screenH, e->screenW, colour, tab, extra);
    return 1;
}
