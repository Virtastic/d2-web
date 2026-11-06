// src/core/d2ddraw/d2ddraw_7320.cpp -- 1.13c D2DDraw.dll TileDrawLit. Decompile-derived.
// ECX = tile, EDX = x, stack y, light, half (ret 0xC). For each record whose screen point lies in the clip window the
// raster slot +0x10 draws it with the lit quad {b0, b1, b1, b0}: b0, b1 the light bytes at idx*4 and idx*4+4, idx the
// record's own x >> 5. Returns 1 when any record drew, else 0 (also when 10106 refuses the tile).
#include "d2ddraw_tile.h"

// 1.13c D2DDraw+0x07320
extern "C" uint32_t d2ddraw_tile_draw_lit(const D2DDrawTileEnv* e, const uint8_t* tile, int32_t x, int32_t y,
                                          const uint8_t* light, uint32_t half) {
    using namespace d2ddraw_tile;
    uint32_t drew = 0;
    if (e->cmp10106(tile, 1, 0) == 0) return 0;
    const int32_t bottom = e->screenH + 0x20;
    int32_t left, right; window(e, half, left, right);
    if (rd32(tile + 0x50) <= 0) return 0;
    for (int32_t i = 0; i < rd32(tile + 0x50); ++i) {
        const uint8_t* r = list(tile) + i * 0x14;
        const int32_t xx = rd16(r) + x, yy = rd16(r + 2) + y;
        if (xx < left || xx >= right || yy < -0x20 || yy >= bottom) continue;
        r = list(tile) + i * 0x14;
        const int32_t idx = rd16(r) >> 5;
        const uint32_t b0 = light[idx * 4], b1 = light[idx * 4 + 4];
        const uint32_t quad[4] = { b0, b1, b1, b0 };
        e->drawLit((uint32_t)rd32(r + 0x10), xx, yy, quad);
        drew = 1;
    }
    return drew;
}
