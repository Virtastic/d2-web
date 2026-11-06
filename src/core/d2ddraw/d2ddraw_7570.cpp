// src/core/d2ddraw/d2ddraw_7570.cpp -- 1.13c D2DDraw.dll ShadowTile. Decompile-derived.
// ECX = tile, EDX = x, stack y, an unused word, half (ret 0xC). Every record whose screen point lies in the clip window
// sets the result and is drawn: with the mode block's +0x14 flag (re-read per record) through raster slot +0x14 at
// alpha 0xC0, without it through slot +0x04. Returns 1 when any record was in the window, else 0.
#include "d2ddraw_tile.h"

// 1.13c D2DDraw+0x07570
extern "C" uint32_t d2ddraw_shadow_tile(const D2DDrawTileEnv* e, const uint8_t* tile, int32_t x, int32_t y,
                                        uint32_t, uint32_t half) {
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
        drew = 1;
        if (rd32(e->mode + 0x14) != 0) e->drawShadowAlpha((uint32_t)rd32(list(tile) + i * 0x14 + 0x10), xx, yy, 0xC0);
        else e->drawShadow((uint32_t)rd32(list(tile) + i * 0x14 + 0x10), xx, yy);
    }
    return drew;
}
