// D2CMP Codec.cpp: step the DCC cell walker to the next cell of the next non-empty frame.
#include "d2cmp_dcc.h"

// 1.13c D2CMP+0x13300
// Retail: ECX = walker, EAX = box. Returns 1 with the walker on a cell, 0 past the last frame.
// Cells are 4x4 on the box grid; a frame's first column/row is 4 - (origin % 4) wide when the
// frame extends past it, and a trailing cell absorbs a remainder of up to 5.
int32_t d2cmp_13300(DccIter* p, const DccBox* box) {
    int32_t cx = p->cx + p->cw, w = p->w;
    p->cx = cx;
    if (cx < w) {
        p->off += p->cw;
        p->cell++;
        p->cw = (w <= cx + 5) ? w - cx : 4;
        return 1;
    }
    int32_t cy = p->cy + p->ch, h = p->h;
    p->cy = cy;
    if (!(h <= cy)) {
        int32_t rx = p->x % 4;
        p->cx = 0;
        p->cw = (-rx + 5 < w) ? -rx + 4 : w;
        int32_t o = p->rowOff + p->stride * p->ch;
        p->off = o; p->rowOff = o;
        int32_t c = p->cellRow + box->cellsW;
        p->cell = c; p->cellRow = c;
        p->ch = (h <= cy + 5) ? h - cy : 4;
        return 1;
    }
    int32_t i = p->frame + 1;
    p->frame = i;
    while (i < box->nframes) {
        const DccFrame& f = box->frames[i];
        if (f.w != 0 && f.h != 0) {
            p->w = f.w;
            p->h = f.h;
            p->x = f.xoff - box->xmin;
            int32_t y = (f.yoff - box->ymin) - (f.bottomUp == 0 ? f.h : 0);
            p->y = y;
            p->pix = f.pix;
            p->stride = f.stride;
            int32_t c = p->x / 4 + (y / 4) * box->cellsW;
            p->cellRow = c; p->cell = c;
            p->cx = 0; p->cy = 0; p->rowOff = 0; p->off = 0;
            int32_t rx = p->x % 4, ry = y % 4;
            p->cw = (-rx + 5 < p->w) ? -rx + 4 : p->w;
            p->ch = (-ry + 5 < p->h) ? -ry + 4 : p->h;
            return 1;
        }
        i++;
        p->frame = i;
    }
    return 0;
}
