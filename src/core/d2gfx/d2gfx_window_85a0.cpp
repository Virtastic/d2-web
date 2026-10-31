#include <cstdint>

// 1.13c D2Gfx+0x085A0 (ordinal 10072): stdcall(windowed, res) -- create the main window and open the renderer. Win32 and the table are host seams;
// the fatal paths (null table 0x8e / window already made 0x8f: Fog 10265, 10024, _exit(-1)) are reported through `fatal`, which does not return.
// Order: a "Diablo II" window already exists -> message box, return 0. Else store windowed (+0x1125C) and res (+0x11260); style 0xCB0000 windowed,
// else 0x80080000 (0x86080000 when mode == 5). The 0x7FD0 size is framed by AdjustWindowRectEx(style, 0, 0x40000) and *extra (+0x1D580) = framed
// height - client height, taken BEFORE the placement below. Windowed: centred on the desktop client rect, each origin clamped to 0. Mode 4: origin
// 0, size = screen metrics floored at 800x600. Screen width < 800 -> ChangeDisplaySettingsA(800x600x8). CreateWindowExA failure -> 0 (error via
// 0x7AA0). Success: show/update/focus, GdiSetBatchLimit(1), 0x80A0, slot +0x0C; on slot failure set +0x1D664 = 1, 0x8070, destroy the window.
// Then 0xAEE0, SetWindowPos(0xFFFFFFFE,0,0,w,h,2); returns the slot result.
struct D2GfxWin { uint32_t windowed, res, extra, failed, hwnd, table, mode; };
struct D2GfxWinHost {
    uint32_t (*exists)();                                   // FindWindowA("Diablo II", 0) != 0
    void (*message)();                                      // the single-instance MessageBoxA (0x30)
    void (*fatal)(uint32_t id);                             // Fog 10265(id), 10024, _exit(-1)
    void (*frame)(uint32_t style, uint32_t res, int32_t* w, int32_t* h, int32_t* baseH);  // 0x7FD0 + AdjustWindowRectEx
    void (*desktop)(int32_t* right, int32_t* bottom);       // GetClientRect(GetDesktopWindow())
    int32_t (*metric)(int32_t index);                       // GetSystemMetrics
    void (*changeDisplay)();                                // ChangeDisplaySettingsA(800x600x8, 0)
    uint32_t (*create)(uint32_t style, int32_t x, int32_t y, int32_t w, int32_t h);  // CreateWindowExA(0x40008, ...)
    void (*createFailed)();                                 // 0x7AA0(GetLastError())
    void (*prepare)();                                      // Show/Update/SetFocus, GdiSetBatchLimit(1), 0x80A0
    uint32_t (*slot0c)();
    void (*teardown)();                                     // 0x8070
    void (*destroy)();                                      // DestroyWindow
    void (*restore)();                                      // 0xAEE0
    void (*setPos)(int32_t w, int32_t h);
};
extern "C" uint32_t d2gfx_sub_85a0(uint32_t windowed, uint32_t res, D2GfxWin* g, const D2GfxWinHost* h) {
    if (h->exists()) { h->message(); return 0; }
    if (g->table == 0) { h->fatal(0x8e); return 0; }
    if (g->hwnd != 0) { h->fatal(0x8f); return 0; }
    g->windowed = windowed; g->res = res;
    uint32_t style = g->windowed == 1 ? 0xCB0000u : (g->mode == 5 ? 0x86080000u : 0x80080000u);
    int32_t w, ht, base;
    h->frame(style, res, &w, &ht, &base);
    g->extra = (uint32_t)(ht - base);
    int32_t x = 0, y = 0;
    if (g->windowed == 1) {
        int32_t r, b; h->desktop(&r, &b);
        x = (r - w) >> 1; if (x < 0) x = 0;
        y = (b - ht) >> 1; if (y < 0) y = 0;
    } else if (g->mode == 4) {
        w = h->metric(0); if (w < 0x321) w = 800;
        ht = h->metric(1); if (ht < 0x259) ht = 600;
    }
    if (h->metric(0) < 800) h->changeDisplay();
    if (!(g->hwnd = h->create(style, x, y, w, ht))) { h->createFailed(); return 0; }
    h->prepare();
    uint32_t rv = h->slot0c();
    if (rv == 0) { g->failed = 1; h->teardown(); if (g->hwnd) { h->destroy(); g->hwnd = 0; } }
    h->restore();
    h->setPos(w, ht);
    return rv;
}
