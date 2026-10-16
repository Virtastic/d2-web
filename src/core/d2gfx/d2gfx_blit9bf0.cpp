// 1.13c D2Gfx+0x9BF0: copy one 32-row run-length sprite onto the 8-bit surface (99C0 without the shade).
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "d2gfx_blit.h"

// fastcall(ecx = stream, edx = x, [y]); st = the module globals the function reads (shade tables unused)
extern "C" void d2gfx_sub_9bf0(const uint8_t* src, int32_t x, int32_t y, const D2GfxBlitState* st) {
    if (y < st->yTop || y >= st->yBot || x < st->xLeft || x >= st->xRight) return;
    if (!st->base) abort();                                       // retail: error box, then _exit(-1)
    uint8_t* row = st->base + st->rowOff[y] + x;
    uint8_t* d = row;
    if (y >= st->inY0 && y < st->inY1 && x >= st->inX0 && x < st->inX1) {   // start inside the inner rect: no per-run clipping
        for (int rows = 32; rows;) {
            uint8_t dx = src[0], run = src[1]; src += 2;
            if (!dx && !run) { --rows; row += st->pitch; d = row; continue; }
            d += dx; memcpy(d, src, run); src += run; d += run;
        }
        return;
    }
    int32_t yy = y;
    for (int rows = 0; rows < 32;) {
        uint8_t dx = src[0], run = src[1]; src += 2;
        if (!dx && !run) { ++rows; ++yy; row += st->pitch; d = row; continue; }
        if (yy >= st->c2Bottom) return;
        d += dx;
        if (yy >= st->c2Top) {
            int32_t xr = (int32_t)(d - row) + x, left = st->c2Left - xr, right = st->c2Right - xr;
            if (left < 0) left = 0;
            if (right < 0) right = 0;
            int32_t n = (right < run ? right : run) - left;
            if (n > 0) memcpy(d + left, src + left, n);
        }
        src += run; d += run;
    }
}
