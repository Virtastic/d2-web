// D2CMP Codec.cpp: bounding box of one DCC direction's frames.
#include "d2cmp_dcc.h"

// 1.13c D2CMP+0x13220
// Retail: ECX = header, EDX = direction, EAX = frames, EDI = box. dir >= header+8 -> fatal 0x373.
// Top edge: yoff - h for a top-down frame (bottomUp == 0), yoff for a bottom-up one.
// No frames leaves min 0x7FFFFFFF / max 0x80000000, so w and h wrap to 1 (32-bit arithmetic).
void d2cmp_13220(const int32_t* hdr, int32_t dir, DccFrame* frames, DccBox* box, D2CmpFatalFn fatal) {
    if (!(dir < hdr[2])) { fatal(0x373); return; }
    box->hdr = hdr;
    box->frames = frames;
    box->nframes = hdr[3];
    box->dir = dir;
    int32_t x0 = 0x7fffffff, x1 = (int32_t)0x80000000u, y0 = 0x7fffffff, y1 = (int32_t)0x80000000u;
    for (int32_t i = 0; i < hdr[3]; i++) {
        const DccFrame& f = frames[i];
        int32_t y = f.yoff - (f.bottomUp == 0 ? f.h : 0);
        if (f.xoff < x0) x0 = f.xoff;
        if (x1 < f.xoff + f.w) x1 = f.xoff + f.w;
        if (y < y0) y0 = y;
        if (y1 < y + f.h) y1 = y + f.h;
    }
    box->h = (int32_t)((uint32_t)y1 - (uint32_t)y0);
    box->xmin = x0;
    box->ymin = y0;
    box->w = (int32_t)((uint32_t)x1 - (uint32_t)x0);
}
