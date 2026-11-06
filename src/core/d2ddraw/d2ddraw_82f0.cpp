// 1.13c D2DDraw+0x082F0 -- present the render surface (BltFast, then Flip in flip-chain mode) and push a dirty palette.
// Decompile-derived, checked against the disassembly. The module globals arrive as pointers so a caller (or a probe)
// owns them; the three COM methods and the 0x7690 error logger are seams. Retail's string arguments are 0xCC04
// "BltFast failed!" and 0xCBF0 "SetEntries failed!"; the palette entries are the 0xFDCC block.
#include <cstdint>

struct D2DDrawPresentEnv {
    const uint32_t* flipMode;      // DAT 0x10234
    const uint32_t* render;        // DAT 0x10220, the surface drawn into
    const uint32_t* back;          // DAT 0x1021C, the flip chain's back buffer
    const uint32_t* primary;       // DAT 0x10218
    const int32_t*  screenW;       // DAT 0x101C8
    const int32_t*  screenH;       // DAT 0x101D8
    uint32_t*       paletteDirty;  // DAT 0x10240
    const uint32_t* palette;       // DAT 0x10230, the IDirectDrawPalette
    uint32_t        entries;       // &DAT 0xFDCC, handed to SetEntries as it is
    uint32_t (*bltFast)(uint32_t surf, uint32_t x, uint32_t y, uint32_t src, const int32_t* rect, uint32_t flags);  // vtbl+0x1C
    uint32_t (*flip)(uint32_t surf, uint32_t over, uint32_t flags);                                                  // vtbl+0x2C
    uint32_t (*setEntries)(uint32_t pal, uint32_t flags, uint32_t start, uint32_t count, uint32_t entries);         // vtbl+0x18
    void     (*log)(uint32_t hr, uint32_t msg);   // 0x7690 (ECX = hr, stack = message); msg 0 = BltFast, 1 = SetEntries
};

// 1.13c D2DDraw+0x082F0
// Mode 0: the render surface (when set) is BltFast'ed onto the primary. Mode != 0: when render and back are both set and
// differ, render is BltFast'ed onto back; the primary then Flips (0, DDFLIP_WAIT 1), its result ignored. The rect is
// (0, 0, screenW, screenH) and the flags DDBLTFAST_WAIT 0x10. A BltFast failure logs and returns 0. Then a dirty palette
// (with a palette object) is cleared and SetEntries(0, 1, 0xFF, entries) is called; a failure logs and returns 0. Else 1.
extern "C" uint32_t d2ddraw_present_82f0(const D2DDrawPresentEnv* e) {
    const uint32_t render = *e->render;
    uint32_t hr = 0, dst = 0;
    if (*e->flipMode) {
        const uint32_t back = *e->back;
        if (render && back && render != back) dst = back;
    } else if (render) {
        dst = *e->primary;
    }
    if (dst) {
        const int32_t rect[4] = {0, 0, *e->screenW, *e->screenH};
        hr = e->bltFast(dst, 0, 0, render, rect, 0x10);
        if (hr) { e->log(hr, 0); return 0; }
    }
    if (*e->flipMode) e->flip(*e->primary, 0, 1);
    if (*e->paletteDirty && *e->palette) {
        *e->paletteDirty = 0;
        hr = e->setEntries(*e->palette, 0, 1, 0xFF, e->entries);
        if (hr) { e->log(hr, 1); return 0; }
    }
    return 1;
}
