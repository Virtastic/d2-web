// 1.13c D2DDraw+0x06330 -- screen clear through a colour-fill Blt. Decompile-derived.
#include <cstdint>

struct D2DDrawClearHost {
    void*    surface;                                  // +0x10220 primary surface (null: no-op)
    uint32_t (*blt)(void* surface, uint32_t flags, const uint32_t* fx);  // vtbl+0x14, dst/src rects null
    void     (*log)(uint32_t hr, const char* msg);     // 0x7690
    uint8_t* buf; int32_t width; int32_t height;       // +0xF8A8, +0x101C8, +0x101D8
};

// 1.13c D2DDraw+0x06330
// Blt(null, null, null, DDBLT_COLORFILL 0x400, &fx) with fx = {dwSize 100, zeros}. Success returns.
// HRESULT 0x887601AE clears the software buffer instead: (height - (skipPanel ? 0x2F : 0)) * width bytes.
// Any other failure is logged as "Blit (screen clear) failed!".
extern "C" void d2ddraw_screen_clear(const D2DDrawClearHost* h, uint32_t skipPanel) {
    uint32_t fx[25] = {100};
    if (!h->surface) return;
    uint32_t hr = h->blt(h->surface, 0x400, fx);
    if (!hr) return;
    if (hr == 0x887601AEu) {
        int32_t rows = skipPanel ? h->height - 0x2F : h->height;
        uint32_t n = (uint32_t)(rows * h->width);
        for (uint32_t i = 0; i < n; ++i) h->buf[i] = 0;
        return;
    }
    h->log(hr, "Blit (screen clear) failed!");
}
