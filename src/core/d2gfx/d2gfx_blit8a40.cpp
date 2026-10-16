// 1.13c D2Gfx+0x8A40 and +0x9E90: light-shade blits of a run-length sprite, one row per
// end marker. dst[i] = tab[src[i] + lmap[cell + i] * 256]; the cell advances per row from two fixed-point coordinates.
#include <cstdint>
#include "d2gfx_blit.h"

static inline const uint8_t* cell(const D2GfxLitState* st, int32_t a, int32_t b, uint8_t sh) {
    return st->lmap + (((uint32_t)(a >> (sh & 31)) * 32u + (uint32_t)(b >> (sh & 31))) * 32u);
}

// eax = src; stack: dst, a, aStep, b, bStep, rows, sh (ret 0x1c). A run of more than 32 pixels is skipped, not drawn.
extern "C" void d2gfx_sub_8a40(const uint8_t* src, uint8_t* dst, int32_t a, int32_t aStep, int32_t b, int32_t bStep, int32_t rows, uint8_t sh, const D2GfxLitState* st) {
    const uint8_t* m = cell(st, a, b, sh); uint8_t* row = dst;
    for (;;) {
        if (rows == 0) return;
        uint32_t skip = src[0], cnt = src[1]; src += 2;
        if (!skip && !cnt) { dst = row += st->pitch; a += aStep; b += bStep; --rows; m = cell(st, a, b, sh); continue; }
        dst += skip; m += skip;
        if (cnt <= 0x20) for (uint32_t i = 0; i < cnt; ++i) dst[i] = st->tab[src[i] + m[i] * 256u];
        dst += cnt; src += cnt; m += cnt;
    }
}

// eax = src; stack: x0, y, dst, a, aStep, b, bStep, rows, sh (ret 0x24). Rows from y; a run is clipped to columns [c2Left,c2Right) relative to
// x0 once y >= c2Top, and the whole sprite ends at y >= c2Bottom. Retail does not advance the light map by the left clip.
extern "C" void d2gfx_sub_9e90(const uint8_t* src, int32_t x0, int32_t y, uint8_t* dst, int32_t a, int32_t aStep, int32_t b, int32_t bStep, int32_t rows, uint8_t sh, const D2GfxLitState* st) {
    if (rows <= 0) return;
    const uint8_t* m = cell(st, a, b, sh); uint8_t* row = dst; int32_t done = 0;
    do {
        int32_t skip = src[0], cnt = src[1]; src += 2;
        if (!skip && !cnt) { ++done; dst = row += st->pitch; a += aStep; b += bStep; ++y; m = cell(st, a, b, sh); continue; }
        if (st->c2Bottom <= y) return;
        dst += skip; m += skip;
        if (st->c2Top <= y) {
            int32_t off = -(int32_t)((dst - row) + x0);
            int32_t lo = off + st->c2Left; if (lo < 1) lo = 0;
            int32_t hi = off + st->c2Right; if (hi < 1) hi = 0; if (cnt <= hi) hi = cnt;
            if (hi - lo > 0 && hi - lo <= 0x20) for (int32_t k = 0; k < hi - lo; ++k) dst[lo + k] = st->tab[src[lo + k] + m[k] * 256u];
        }
        src += cnt; dst += cnt; m += cnt;
    } while (done < rows);
}
