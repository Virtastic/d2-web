// src/core/d2ddraw/d2ddraw_6f60.cpp -- 1.13c D2DDraw.dll CelDrawClipped. Decompile-derived (disassembly 0x6F60).
// fastcall: ECX = cel context, EDX = x, stack y, clip rect, blend mode (ret 0xC). D2CMP 10005 refusing the cel
// returns 0; DDrawClipRectToScreen (0x6E20) missing returns 1; else D2CMP 10080 draws the clipped rows and its
// result is returned.
#include <cstring>
#include "d2ddraw_cel.h"

// 1.13c D2DDraw+0x06F60
extern "C" uint32_t d2ddraw_cel_draw_clipped_6f60(const D2DDrawCelEnv* e, const uint8_t* ctx, int32_t x, int32_t y,
                                                  const int32_t* clip, uint32_t mode) {
    int32_t out[4] = {0, 0, 0, 0};
    if (e->cmp10005(ctx, 0, 1) == 0) return 0;
    uint32_t h; memcpy(&h, ctx + 0x3C, 4);
    if (d2ddraw_clip_rect_to_screen_6e20(e, clip, y, h, x, out) != 0) return 1;
    uint32_t blend = e->blend(mode);
    return e->cmp10080(ctx, x, y, out[0], out[1], e->surface, e->screenH, e->screenW, 0, blend, out[2], out[3], 0);
}
