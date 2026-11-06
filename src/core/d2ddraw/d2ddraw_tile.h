// src/core/d2ddraw/d2ddraw_tile.h -- the seams shared by the 1.13c D2DDraw tile drawers TileDrawLit (0x7320) and
// ShadowTile (0x7570). Decompile-derived. A tile (ECX) carries the record count at +0x50 and the 0x14-byte record
// list at +0x54 (int16 x at +0, int16 y at +2, the raster word at +0x10); both are re-read for every record.
#pragma once
#include <cstdint>

struct D2DDrawTileEnv {
    uint32_t (*cmp10106)(const uint8_t* tile, uint32_t a, uint32_t b);       // thunk 0x5F70, called (tile, 1, 0)
    void     (*drawLit)(uint32_t word, int32_t x, int32_t y, const uint32_t* quad);   // [[0x101DC]+0x10]: ECX, EDX, 2 stack
    void     (*drawShadowAlpha)(uint32_t word, int32_t x, int32_t y, uint32_t a);     // [[0x101DC]+0x14]: ECX, EDX, 2 stack
    void     (*drawShadow)(uint32_t word, int32_t x, int32_t y);                       // [[0x101DC]+0x04]: ECX, EDX, 1 stack
    int32_t  screenW, screenH;                                                         // DAT 0x101C8, 0x101D8
    const uint8_t* mode;                                                               // DAT 0x101D0 (dword +0x14: alpha shadows)
};

extern "C" {
uint32_t d2ddraw_tile_draw_lit(const D2DDrawTileEnv* e, const uint8_t* tile, int32_t x, int32_t y,
                               const uint8_t* light, uint32_t half);
uint32_t d2ddraw_shadow_tile(const D2DDrawTileEnv* e, const uint8_t* tile, int32_t x, int32_t y,
                             uint32_t unused, uint32_t half);
}

namespace d2ddraw_tile {
inline int32_t rd32(const uint8_t* p) { int32_t v; __builtin_memcpy(&v, p, 4); return v; }
inline int32_t rd16(const uint8_t* p) { int16_t v; __builtin_memcpy(&v, p, 2); return v; }
inline const uint8_t* list(const uint8_t* tile) { return (const uint8_t*)(uintptr_t)(uint32_t)rd32(tile + 0x54); }
// the clip window: x in [left, right), y in [-0x20, sh + 0x20); half 1 keeps the left screen half, half 2 the right
inline void window(const D2DDrawTileEnv* e, uint32_t half, int32_t& left, int32_t& right) {
    left = -0x20; right = e->screenW;
    if (half == 1) right = e->screenW - e->screenW / 2;
    else if (half == 2) left = e->screenW / 2 - 0x20;
}
}
