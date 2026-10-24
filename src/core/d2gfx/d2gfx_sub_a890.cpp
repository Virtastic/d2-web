#include <cstdint>
#include <cstdlib>
#include "d2gfx_blit.h"

extern "C" void d2gfx_sub_8e30(const uint8_t*, const uint8_t*, uint32_t, uint8_t*, int32_t);
struct D2GfxRleClip { int32_t stride, top, bottom, left, right; const uint8_t* palette; };
void d2gfx_rle_clipped(const uint8_t*, int32_t, int32_t, uint8_t*, int32_t, const D2GfxRleClip&);

// 1.13c D2Gfx+0x0A890: draw one 32-row RLE tile at (x,y) on the 8-bit surface.
// fastcall(ecx = stream, edx = x, [y]); ESI = palette (explicit here); globals gathered in st.
extern "C" void d2gfx_sub_a890(const uint8_t* src, int32_t x, int32_t y, const uint8_t* palette, const D2GfxBlitState* st) {
    if (y < st->yTop || y >= st->yBot || x < st->xLeft || x >= st->xRight) return;
    if (!st->base) abort();                                      // retail: error box, then _exit(-1)
    uint8_t* dst = st->base + st->rowOff[y] + x;
    if (y >= st->inY0 && y < st->inY1 && x >= st->inX0 && x < st->inX1) {
        d2gfx_sub_8e30(src, palette, 0x20, dst, st->pitch);
        return;
    }
    D2GfxRleClip c{st->pitch, st->c2Top, st->c2Bottom, st->c2Left, st->c2Right, palette};
    d2gfx_rle_clipped(src, x, y, dst, 0x20, c);
}
