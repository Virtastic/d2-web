// src/core/d2ddraw/d2ddraw_6d10.cpp -- 1.13c D2DDraw.dll CelDraw. Decompile-derived (disassembly 0x6D10).
// fastcall: ECX = cel context, EDX = x, stack y, palette byte, blend mode, extra word (ret 0x10). The cel is dropped
// when D2CMP 10005 refuses it or its box (x + x offset, width; y + y offset less the height) misses the screen.
// The colour is the table dword (pal >> 3) past +0x0C (0 for pal 0xFF), the +0x118 dword for mode 7; the extra
// word passes through. Returns 1 when D2CMP 10015 was called, else 0.
#include <cstring>
#include "d2ddraw_cel.h"

static inline uint32_t rd32(const uint8_t* p, uint32_t off) { uint32_t v; memcpy(&v, p + off, 4); return v; }

// 1.13c D2DDraw+0x06D10
extern "C" uint32_t d2ddraw_cel_draw_6d10(const D2DDrawCelEnv* e, const uint8_t* ctx, int32_t x, int32_t y,
                                          uint8_t pal, uint32_t mode, uint32_t extra) {
    if (e->cmp10005(ctx, 0, 1) == 0) return 0;
    uint32_t h; memcpy(&h, ctx + 0x3C, 4);
    int32_t w = e->cmp10060(h);
    int32_t t = e->cmp10000(h);
    int32_t xx = (int32_t)((uint32_t)e->cmp10002(h) + (uint32_t)x);
    if (xx >= e->screenW) return 0;
    if ((int32_t)((uint32_t)w + (uint32_t)xx) < 0) return 0;
    int32_t v = (int32_t)((uint32_t)e->cmp10069(h) + (uint32_t)y);
    if (v < 0) return 0;
    if (v - t >= e->screenH) return 0;
    uint32_t blend = e->blend(mode);
    uint32_t colour = pal != 0xFF ? rd32(e->table, 0x0C + (uint32_t)(pal >> 3) * 4) : 0;
    if (mode == 7) colour = rd32(e->table, 0x118);
    e->cmp10015(ctx, x, y, e->originX, e->originY, e->surface, e->screenH, e->screenW, colour, blend, extra);
    return 1;
}
