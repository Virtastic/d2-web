// D2DDraw utility primitives over the 8-bit surface: point and diamond markers, built on the
// FillRect8 and DrawLine8 helpers (same arithmetic as the retail wrappers).
#include <cstdint>

static void d2ddraw_fatal_default(uint32_t) {}
extern "C" { void (*d2ddraw_fatal)(uint32_t code) = d2ddraw_fatal_default; }   // the host sets the fatal-error handler; the default returns
extern "C" void d2ddraw_fill_rect(int32_t* r, uint8_t color);
extern "C" void d2ddraw_draw_line(const int32_t* p1, const int32_t* p0, uint8_t color);

// 1.13c D2DDraw+0x063C0 (UtilPoint): size 0 draws nothing; else a square of side 2*(size-1)+1 centred on pt.
extern "C" void d2ddraw_util_point(const int32_t* pt, uint8_t size, uint8_t color) {
    if (size == 0) return;
    uint32_t e = (uint32_t)(uint8_t)(size - 1);
    int32_t r[4] = { (int32_t)((uint32_t)pt[0] - e), (int32_t)((uint32_t)pt[1] - e),
                     (int32_t)((uint32_t)pt[0] + 1 + e), (int32_t)((uint32_t)pt[1] + 1 + e) };
    d2ddraw_fill_rect(r, color);
}

// 1.13c D2DDraw+0x064E0 (UtilDiamond): rect {l,t,r,b}; inverted rect is fatal (0x12D / 0x12E);
// an empty width or height draws nothing; else four DrawLine8 edges between the edge midpoints.
extern "C" void d2ddraw_util_diamond(const int32_t* rc, uint8_t color) {
    int32_t left = rc[0], top = rc[1], right = rc[2], bottom = rc[3];
    if (right < left) { d2ddraw_fatal(0x12D); return; }
    if (bottom < top) { d2ddraw_fatal(0x12E); return; }
    if (right - left == 0 || bottom - top == 0) return;
    int32_t midY = top + (int32_t)((uint32_t)(bottom - top) >> 1);
    int32_t midX = left + (int32_t)((uint32_t)(right - left) >> 1);
    int32_t pR[2] = { right, midY }, pB[2] = { midX, bottom }, pL[2] = { left, midY }, pT[2] = { midX, top };
    d2ddraw_draw_line(pR, pT, color);
    d2ddraw_draw_line(pB, pR, color);
    d2ddraw_draw_line(pL, pB, color);
    d2ddraw_draw_line(pT, pL, color);
}
