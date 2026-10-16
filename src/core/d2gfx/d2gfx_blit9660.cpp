// 1.13c D2Gfx+0x9660: roof block drawer -- 99C0's run-length walk, each pixel lit by the interpolated light map then shaded.
#include <cstdint>
#include <cstdlib>
#include "d2gfx_blit.h"

// fastcall(ecx = art, edx = x, [y], [light a,b,c,d], [alpha]); st = 99C0's globals plus lmap (0x6FA95450, 32x32x32) and lev (0x6FA91678, 256x256)
// the light cell is NOT advanced by the left clip (retail quirk, as 9E90)
extern "C" void d2gfx_sub_9660(const uint8_t* src, int32_t x, int32_t y, const int32_t* light, uint8_t alpha, const D2GfxBlitState* st, const uint8_t* lmap, const uint8_t* lev) {
    const uint8_t* shade;
    if (alpha >= 0xC0) shade = st->shade[0]; else if (alpha >= 0x80) shade = st->shade[1]; else if (alpha >= 0x40) shade = st->shade[2]; else return;
    int32_t sx = light[3] - light[0], sy = light[2] - light[1];
    int32_t a = light[0] << 5, b = light[1] << 5;
    if (y < st->yTop || y >= st->yBot || x < st->xLeft || x >= st->xRight) return;
    if (!st->base) abort();                                       // retail: error box, then _exit(-1)
    uint8_t* row = st->base + st->rowOff[y] + x;
    uint8_t* d = row;
    auto cell = [&]() { return lmap + ((a >> 8) * 32 + (b >> 8)) * 32; };
    const uint8_t* l = cell();
    if (y >= st->inY0 && y < st->inY1 && x >= st->inX0 && x < st->inX1) {
        for (int rows = 32; rows;) {
            uint8_t dx = src[0], run = src[1]; src += 2;
            if (!dx && !run) { --rows; row += st->pitch; d = row; b += sy; a += sx; l = cell(); continue; }
            d += dx; l += dx;
            for (const uint8_t* e = src + run; src < e; ++src, ++d, ++l) *d = shade[((uint32_t)lev[((uint32_t)*l << 8) + *src] << 8) + *d];
        }
        return;
    }
    int32_t yy = y;
    for (int rows = 0; rows < 32;) {
        uint8_t dx = src[0], run = src[1]; src += 2;
        if (!dx && !run) { ++rows; ++yy; row += st->pitch; d = row; b += sy; a += sx; l = cell(); continue; }
        if (yy >= st->c2Bottom) return;
        const uint8_t* lp = l + dx;
        d += dx;
        if (yy >= st->c2Top) {
            int32_t xr = (int32_t)(d - row) + x, left = st->c2Left - xr, right = st->c2Right - xr;
            if (left < 0) left = 0;
            if (right < 0) right = 0;
            int32_t n = (right < run ? right : run) - left;
            for (int32_t i = 0; i < n; ++i) { uint8_t* o = d + left + i; *o = shade[((uint32_t)lev[((uint32_t)lp[i] << 8) + src[left + i]] << 8) + *o]; }
        }
        src += run; d += run; l = lp + run;
    }
}
