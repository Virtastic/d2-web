#include <cstdint>
#include <cstdlib>
#include "d2gfx_blit.h"

extern "C" void d2gfx_sub_9bf0(const uint8_t*, int32_t, int32_t, const D2GfxBlitState*);
extern "C" void d2gfx_sub_a890(const uint8_t*, int32_t, int32_t, const uint8_t*, const D2GfxBlitState*);
extern "C" void d2gfx_sub_8a40(const uint8_t*, uint8_t*, int32_t, int32_t, int32_t, int32_t, int32_t, uint8_t, const D2GfxLitState*);
extern "C" void d2gfx_sub_9e90(const uint8_t*, int32_t, int32_t, uint8_t*, int32_t, int32_t, int32_t, int32_t, int32_t, uint8_t, const D2GfxLitState*);

// 1.13c D2Gfx+0xABF0: tile draw dispatcher. lit = DAT_6fa9d660, noLight = DAT_6fa90be8,
// r = the four-int light rect (param_4). Palette (ESI) explicit.
extern "C" void d2gfx_sub_abf0(const uint8_t* src, int32_t x, int32_t y, const int32_t* r, int32_t lit, int32_t noLight,
                               const uint8_t* palette, const D2GfxBlitState* st, const D2GfxLitState* ls) {
    if (lit != 0) {
        if ((r[0] & 0xf8) == 0xf8) d2gfx_sub_9bf0(src, x, y, st);
        else d2gfx_sub_a890(src, x, y, palette, st);
        return;
    }
    int32_t a = r[0], b = r[1], da = r[3] - a, db = r[2] - b;
    uint32_t a5 = (uint32_t)a << 5;
    int32_t m = abs(b - a) + abs(da) + abs(db);
    if (noLight == 0 && m > 9) {
        if (y >= st->yTop && y < st->yBot && x >= st->xLeft && x < st->xRight) {
            if (!st->base) abort();
            uint8_t* dst = st->base + st->rowOff[y] + x;
            if (y >= st->inY0 && y < st->inY1 && x >= st->inX0 && x < st->inX1)
                d2gfx_sub_8a40(src, dst, (int32_t)a5, da, b << 5, db, 0x20, 8, ls);
            else
                d2gfx_sub_9e90(src, x, y, dst, (int32_t)a5, da, b << 5, db, 0x20, 8, ls);
            return;
        }
    } else if ((a5 & 0xffffff00) == 0x1f00) {
        d2gfx_sub_9bf0(src, x, y, st);
        return;
    } else {
        d2gfx_sub_a890(src, x, y, palette, st);
    }
}
