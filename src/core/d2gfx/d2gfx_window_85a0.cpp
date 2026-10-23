#include <cstdint>

// 1.13c D2Gfx+0x085A0 (ordinal 10072): stdcall(windowed, res) -- create the main window and open the renderer. Win32 and the table are parameters; the
// fatal paths (null table 0x8e / window already made 0x8f: Fog 10265, 10024, _exit(-1)) are not modelled.
// Order: a "Diablo II" window already exists -> message box, return 0. Else store windowed (+0x1125C) and res (+0x11260); style 0xCB0000 windowed,
// else 0x80080000 (0x86080000 when mode == 5); *extra (+0x1D580) = framed height - client height; CreateWindowExA failure -> 0 (error via 0x7AA0).
// Success: show/update/focus, GdiSetBatchLimit(1), 0x80A0, slot +0x0C; on slot failure set +0x1D664 = 1, 0x8070, destroy the window. Then 0xAEE0,
// SetWindowPos(0xFFFFFFFE,0,0,w,h,2); returns the slot result.
struct D2GfxWin { uint32_t windowed, res, extra, failed, hwnd; };
extern "C" uint32_t d2gfx_sub_85a0(uint32_t windowed, uint32_t res, uint32_t mode, D2GfxWin* g,
        uint32_t (*exists)(), void (*message)(),
        void (*frame)(uint32_t style, uint32_t res, int32_t* x, int32_t* y, int32_t* w, int32_t* h, int32_t* baseH),
        uint32_t (*create)(uint32_t style, int32_t x, int32_t y, int32_t w, int32_t h), void (*createFailed)(),
        void (*prepare)(), uint32_t (*slot0c)(), void (*teardown)(), void (*destroy)(), void (*restore)(), void (*setPos)(int32_t w, int32_t h)) {
    if (exists()) { message(); return 0; }
    g->windowed = windowed; g->res = res;
    uint32_t style = windowed == 1 ? 0xCB0000u : (mode == 5 ? 0x86080000u : 0x80080000u);
    int32_t x = 0, y = 0, w, h, base;
    frame(style, res, &x, &y, &w, &h, &base);
    g->extra = (uint32_t)(h - base);
    if (!(g->hwnd = create(style, x, y, w, h))) { createFailed(); return 0; }
    prepare();
    uint32_t r = slot0c();
    if (r == 0) { g->failed = 1; teardown(); if (g->hwnd) { destroy(); g->hwnd = 0; } }
    restore();
    setPos(w, h);
    return r;
}
