// D2DDraw UtilDiamond: four DrawLine8 edges between a rect's edge midpoints. Decompile-derived.
#include <cstdint>

// 1.13c D2DDraw+0x064E0 (UtilDiamond): rect {l,t,r,b}; r < l is fatal 0x12D, b < t fatal 0x12E (retail logs and
// exits; the seam returns here); an empty width or height draws nothing; else four DrawLine8 (0x6130) edges.
extern "C" void d2ddraw_util_diamond_64e0(void (*drawLine)(const int32_t*, const int32_t*, uint8_t),
                                          void (*fatal)(uint32_t), const int32_t* rc, uint8_t color) {
    int32_t left = rc[0], top = rc[1], right = rc[2], bottom = rc[3];
    if (right < left) { fatal(0x12D); return; }
    if (bottom < top) { fatal(0x12E); return; }
    if (right - left == 0 || bottom - top == 0) return;
    int32_t midY = top + (int32_t)((uint32_t)(bottom - top) >> 1);
    int32_t midX = left + (int32_t)((uint32_t)(right - left) >> 1);
    int32_t pR[2] = { right, midY }, pB[2] = { midX, bottom }, pL[2] = { left, midY }, pT[2] = { midX, top };
    drawLine(pR, pT, color);
    drawLine(pB, pR, color);
    drawLine(pL, pB, color);
    drawLine(pT, pL, color);
}
