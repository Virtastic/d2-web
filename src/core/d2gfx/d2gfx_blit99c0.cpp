// 1.13c D2Gfx+0x99C0: shade-blit one 32-row run-length sprite onto the 8-bit surface.
#include <cstdint>
#include <cstdlib>
#include "d2gfx_blit.h"

// fastcall(ecx = stream, edx = x, [y], [sel]); st = the module globals the function reads (clip rects, surface base/pitch, row table, the 3 shade tables)
extern "C" void d2gfx_sub_99c0(const uint8_t* src, int32_t x, int32_t y, uint8_t sel, const D2GfxBlitState* st) {
    const uint8_t* shade;
    if (sel >= 0xC0) shade = st->shade[0]; else if (sel >= 0x80) shade = st->shade[1]; else if (sel >= 0x40) shade = st->shade[2]; else return;
    if (y < st->yTop || y >= st->yBot || x < st->xLeft || x >= st->xRight) return;
    if (!st->base) abort();                                       // retail: error box, then _exit(-1)
    uint8_t* row = st->base + st->rowOff[y] + x;                  // row start, advanced by pitch per newline
    uint8_t* d = row;
    auto blit = [&](const uint8_t* s, const uint8_t* e, uint8_t* o) { for (; s < e; ++s, ++o) *o = shade[((uint32_t)*o << 8) + *s]; };
    if (y >= st->inY0 && y < st->inY1 && x >= st->inX0 && x < st->inX1) {   // start inside the inner rect: no per-run clipping
        for (int rows = 32; rows;) {
            uint8_t dx = src[0], run = src[1]; src += 2;
            if (!dx && !run) { --rows; row += st->pitch; d = row; continue; }
            d += dx; blit(src, src + run, d); src += run; d += run;
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
            if (n > 0) blit(src + left, src + left + n, d + left);
        }
        src += run; d += run;
    }
}
