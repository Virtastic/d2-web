// src/core/d2ddraw/d2ddraw_6a90.cpp -- 1.13c D2DDraw.dll CelDrawShadow. Decompile-derived (disassembly 0x6A90).
// fastcall: ECX = cel context, EDX = x, one stack word y (ret 4). The cel is dropped when D2CMP 10005 refuses it
// or its shadow box (x offset less the height, half the height above y) misses the screen; otherwise D2CMP 10072
// draws it. The shadow colour is the table's first dword when the mode flag (+0x14) is set, else 0.
#include <cstring>
#include "d2ddraw_cel.h"

// 1.13c D2DDraw+0x06A90
extern "C" void d2ddraw_cel_draw_shadow_6a90(const D2DDrawCelEnv* e, const uint8_t* ctx, int32_t x, int32_t y) {
    if (e->cmp10005(ctx, 0, 1) == 0) return;
    uint32_t h; memcpy(&h, ctx + 0x3C, 4);
    int32_t w = e->cmp10060(h);
    int32_t t = e->cmp10000(h);
    int32_t xx = x - t + e->cmp10002(h);
    if (xx >= e->screenW) return;
    if ((int32_t)((uint32_t)w + (uint32_t)t + (uint32_t)xx) < 0) return;
    int32_t v = (int32_t)((uint32_t)e->cmp10069(h) + (uint32_t)y);
    if (v < 0) return;
    if (v - (t >> 1) >= e->screenH) return;
    uint32_t flag; memcpy(&flag, e->mode + 0x14, 4);
    uint32_t colour = 0;
    if (flag != 0) memcpy(&colour, e->table, 4);
    e->cmp10072(ctx, x, y, e->originX, e->originY, e->surface, e->screenH, e->screenW, colour, 0);
}
