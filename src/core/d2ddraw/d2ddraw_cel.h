// src/core/d2ddraw/d2ddraw_cel.h -- the seams shared by the 1.13c D2DDraw cel drawers (0x6A90, 0x6C00, 0x6D10,
// 0x6E20, 0x6F60). Decompile-derived. The D2CMP imports go through their thunks (0x5F76..0x5FA6); the module
// globals are read once per call, as retail reads them.
#pragma once
#include <cstdint>

struct D2DDrawCelEnv {
    uint32_t (*cmp10005)(const uint8_t* ctx, uint32_t a, uint32_t b);   // thunk 0x5F7C: the cel is drawable
    int32_t  (*cmp10060)(uint32_t h);                                   // thunk 0x5F82: cel width
    int32_t  (*cmp10000)(uint32_t h);                                   // thunk 0x5FA6: cel height
    int32_t  (*cmp10002)(uint32_t h);                                   // thunk 0x5F76: cel x offset
    int32_t  (*cmp10069)(uint32_t h);                                   // thunk 0x5F88: cel y offset
    uint32_t (*cmp10072)(const uint8_t* ctx, int32_t x, int32_t y, int32_t ox, int32_t oy, uint8_t* surf,
                         int32_t sh, int32_t sw, uint32_t colour, uint32_t zero);                    // thunk 0x5F8E: shadow
    uint32_t (*cmp10015)(const uint8_t* ctx, int32_t x, int32_t y, int32_t ox, int32_t oy, uint8_t* surf,
                         int32_t sh, int32_t sw, uint32_t colour, uint32_t blend, uint32_t extra);   // thunk 0x5F9A: cel
    uint32_t (*cmp10080)(const uint8_t* ctx, int32_t x, int32_t y, int32_t l, int32_t r, uint8_t* surf,
                         int32_t sh, int32_t sw, uint32_t zero, uint32_t blend, int32_t skip, int32_t rows,
                         uint32_t zero2);                                                            // thunk 0x5F94: clipped cel
    uint32_t (*blend)(uint32_t mode);                                   // 0x6A10 (DDrawTabLookup), its out word dropped
    void     (*fatal)(uint32_t line);                                   // Fog 10265(line) -> Fog 10024 -> _exit(-1)
    int32_t  screenW, screenH, originX, originY;                        // DAT 0x101C8, 0x101D8, 0x101E8, 0x101EC
    uint8_t* surface;                                                   // DAT 0xF8A8
    const uint8_t* mode;                                                // DAT 0x101D0 (the dword at +0x14 is the shadow-colour flag)
    const uint8_t* table;                                               // DAT 0xF784, the colour table
};

extern "C" {
void     d2ddraw_cel_draw_shadow_6a90(const D2DDrawCelEnv* e, const uint8_t* ctx, int32_t x, int32_t y);
uint32_t d2ddraw_cel_draw_color_6c00(const D2DDrawCelEnv* e, const uint8_t* ctx, int32_t x, int32_t y,
                                     uint8_t pal, uint32_t mode, uint32_t extraIdx);
uint32_t d2ddraw_cel_draw_6d10(const D2DDrawCelEnv* e, const uint8_t* ctx, int32_t x, int32_t y,
                               uint8_t pal, uint32_t mode, uint32_t extra);
uint32_t d2ddraw_clip_rect_to_screen_6e20(const D2DDrawCelEnv* e, const int32_t* clip, int32_t y, uint32_t h,
                                          int32_t x, int32_t* out);
uint32_t d2ddraw_cel_draw_clipped_6f60(const D2DDrawCelEnv* e, const uint8_t* ctx, int32_t x, int32_t y,
                                       const int32_t* clip, uint32_t mode);
}
