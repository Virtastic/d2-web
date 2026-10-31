// 1.13c D2DDraw+0x08c40 -- cooperative level, palette creation, then display open. Decompile-derived.
#include <cstdint>

struct D2DDrawCoopHost {
    uint32_t* hwnd;                                                  // +0xFDC4
    uint32_t* entries;                                               // +0xFDC8, 0x100 PALETTEENTRY dwords
    uint32_t  (*setCoop)(uint32_t hwnd, uint32_t flags);             // IDirectDraw vtbl+0x50
    uint32_t  (*createPalette)(uint32_t caps, uint32_t* entries);    // vtbl+0x14, result to +0x10230
    void      (*log)(uint32_t hr, const char* msg);                  // 0x7690
    uint32_t  (*openDisplay)(void);                                  // 0x85A0
};

// 1.13c D2DDraw+0x08c40
// Stores hwnd, SetCooperativeLevel(hwnd, 0x11 = EXCLUSIVE|FULLSCREEN); zeroes the 256 palette entries,
// CreatePalette(DDPCAPS_8BIT 4, entries). Either failure is logged and returns 0; otherwise 0x85A0's result.
extern "C" uint32_t d2ddraw_coop_and_palette(const D2DDrawCoopHost* h, uint32_t hwnd) {
    *h->hwnd = hwnd;
    uint32_t hr = h->setCoop(hwnd, 0x11);
    if (hr) { h->log(hr, "SetCooperativeLevel failed!"); return 0; }
    for (int32_t i = 0; i < 0x100; ++i) h->entries[i] = 0;
    hr = h->createPalette(4, h->entries);
    if (hr) { h->log(hr, "CreatePalette failed!"); return 0; }
    return h->openDisplay();
}
