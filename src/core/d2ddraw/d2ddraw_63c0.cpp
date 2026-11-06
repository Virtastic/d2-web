// D2DDraw UtilPoint: a square marker over FillRect8. Decompile-derived.
#include <cstdint>

// 1.13c D2DDraw+0x063C0 (UtilPoint): size 0 draws nothing; else SetRect(pt - (size-1), pt + 1 + (size-1)) and
// FillRect8 with the colour. Both callees are seams: user32 SetRect and FillRect8 (0x6010).
extern "C" void d2ddraw_util_point_63c0(void (*setRect)(int32_t*, int32_t, int32_t, int32_t, int32_t),
                                        void (*fillRect)(int32_t*, uint8_t), const int32_t* pt, uint8_t size,
                                        uint8_t color) {
    if (size == 0) return;
    uint32_t e = (uint32_t)(uint8_t)(size - 1);
    int32_t r[4];
    setRect(r, (int32_t)((uint32_t)pt[0] - e), (int32_t)((uint32_t)pt[1] - e), (int32_t)((uint32_t)pt[0] + 1 + e),
            (int32_t)((uint32_t)pt[1] + 1 + e));
    fillRect(r, color);
}
