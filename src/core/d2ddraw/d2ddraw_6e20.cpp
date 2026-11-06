// src/core/d2ddraw/d2ddraw_6e20.cpp -- 1.13c D2DDraw.dll DDrawClipRectToScreen. Decompile-derived (disassembly 0x6E20).
// Register call: EAX = clip rect {left, right, top, bottom}, ECX = y, EDX = cel handle, stack x, out (ret 8).
// The clip rect is cut to [0, screenW] x [0, screenH]; the cel box (x + x offset, width; y + y offset rising by
// the height) is tested against it. Returns 1 (out untouched) when it misses; else out = {left, right, rows
// skipped below the bottom, rows kept} and 0. A box with no rows left is a fatal error (line 0xAB).
#include "d2ddraw_cel.h"

// 1.13c D2DDraw+0x06E20
extern "C" uint32_t d2ddraw_clip_rect_to_screen_6e20(const D2DDrawCelEnv* e, const int32_t* clip, int32_t y, uint32_t h,
                                                     int32_t x, int32_t* out) {
    int32_t w = e->cmp10060(h);
    int32_t t = e->cmp10000(h);
    int32_t l = clip[0] > 0 ? clip[0] : 0;
    int32_t r = clip[1] < e->screenW ? clip[1] : e->screenW;
    if (l > r) return 1;
    int32_t xx = (int32_t)((uint32_t)x + (uint32_t)e->cmp10002(h));
    if (xx >= r) return 1;
    if ((int32_t)((uint32_t)w + (uint32_t)xx) < l) return 1;
    int32_t top = clip[2] > 0 ? clip[2] : 0;
    int32_t bot = clip[3] < e->screenH ? clip[3] : e->screenH;
    if (top > bot) return 1;
    int32_t yy = (int32_t)((uint32_t)y + (uint32_t)e->cmp10069(h));
    if (yy < top) return 1;
    int32_t up = (int32_t)((uint32_t)yy - (uint32_t)t);
    if (up > bot) return 1;
    out[0] = l;
    out[1] = r;
    out[2] = yy > bot ? yy - bot - 1 : 0;
    out[3] = up < top ? yy - out[2] - top + 1 : t - out[2];
    if (out[3] <= 0) e->fatal(0xAB);
    return 0;
}
