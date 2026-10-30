// Ground item labels (Alt held): the placement and colour rules of the label pass, derived from the Ghidra decompile.
// Prior art: none (D2MOO has no D2Client module). The browser walks the ground items and measures their text;
// everything the retail pass decides afterwards -- viewport rejection, the slide that stacks overlapping plates,
// the hover pick, the colour index -- is decided here, and the shell reads the slots back.
#include <cstdint>
#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

namespace {
constexpr int kMaxSlots = 0x20;        // retail's slot table; a 33rd label is a fatal exit there, never reached (the walk stops at 0x20)
struct Slot { int32_t l, t, r, b; int32_t mode, ci; int32_t cursorInside; };
Slot g_slots[kMaxSlots];
int32_t g_uiPin = -1;   // a recorded stream's pinned panel layout (the `uimode` event), -1 when the shell's own panels decide
int32_t g_n, g_minX, g_maxX, g_scrW, g_scrH, g_ui, g_curX, g_curY, g_hoverUnit;
bool g_hoverDone;

// 1.13c D2Client+0x58DE0: clamp the plate to the viewport, then slide it past the first earlier plate it overlaps.
// Returns 0 when it moved (the caller tries the next mode), 1 when it is clear.
int32_t place(int32_t& l, int32_t& t, int32_t& r, int32_t& b, int32_t mode) {
    const int32_t lim = g_scrH - 0x30;
    if (l < g_minX) { r = g_minX + (r - l); l = 6; }
    if (t < 6) { b += 6 - t; t = 6; }
    if (g_maxX <= r) { const int32_t d = r - g_maxX + 1; r -= d; l -= d; }
    if (lim <= b) { const int32_t d = b - lim; b -= d; t -= d; }
    for (int32_t i = 0; i < g_n; ++i) {
        const Slot& s = g_slots[i];
        if (s.r < l || r < s.l || s.b < t || b < s.t) continue;
        int32_t d;
        if (mode < 7) {
            if ((b - t) / 2 != 0 && t <= lim) d = s.t - b - 3; else d = s.b - t + 5;
            t += d; b += d;
        } else {
            if ((r - l) / 2 != 0 && g_maxX <= l) { d = s.l - r - 1; } else { d = s.r - l + 1; }
            l += d; r += d;
        }
        return 0;
    }
    return 1;
}
}  // namespace

extern "C" {
// 1.13c D2Client+0x58FB0 (start): ui = the side-panel layout (0 none, 1 left panel, 2 right panel), hoverUnit = the ground
// item the cursor picks (0 none), curX/curY the cursor. Returns 0 when the pass does not run: its caller (D2Client+0xC39E0)
// skips it in UI mode 3.
EMSCRIPTEN_KEEPALIVE int32_t d2_showitems_begin(int32_t scrW, int32_t scrH, int32_t ui, int32_t curX, int32_t curY, int32_t hoverUnit) {
    if (g_uiPin >= 0) ui = g_uiPin;
    g_n = 0;
    if (ui == 3) return 0; g_scrW = scrW; g_scrH = scrH; g_ui = ui; g_curX = curX; g_curY = curY; g_hoverUnit = hoverUnit; g_hoverDone = false;
    g_minX = 0; g_maxX = scrW;
    if (ui == 1) g_maxX = scrW / 2 - 0x20; else if (ui == 2) g_minX = scrW / 2;
    return 1;
}
// Pin the panel layout the pass reads (D2Client global 0xBCC414), as a recorded stream's `uimode` event does; -1 unpins.
EMSCRIPTEN_KEEPALIVE void d2_showitems_pin_ui(int32_t ui) { g_uiPin = ui; }
// 1.13c D2Client+0x58FB0 (per unit). cx,y = the projected feet position, textW the Font16 width of the widest line,
// lines the line count, quality the item quality, grey = socketed-or-ethereal condition met, gold = quest item that
// is not Wirt's leg. Returns the slot index, or -1 when the plate is off screen / cannot be placed.
EMSCRIPTEN_KEEPALIVE int32_t d2_showitems_add(int32_t unit, int32_t cx, int32_t y, int32_t textW, int32_t lines, int32_t quality, int32_t grey, int32_t gold) {
    if (g_n >= kMaxSlots) return -1;
    // The +-W/4 panel shift belongs to the perspective branch only (D2Gfx perspective transform); the flat branch takes the
    // projected x as it is: with the right-side layout every plate clamps at W/2-0x20, measured.
    if (!(g_minX <= cx && cx <= g_maxX && -9 < y && y <= g_scrH - 0x10)) return -1;
    const int32_t w = textW + 8, h = lines * 16;
    int32_t l = cx - (w >> 1), r = l + w, t = y - h + 4, b = y;
    bool ok = false;
    for (int32_t mode = 0; mode < 12 && !ok; ++mode) ok = place(l, t, r, b, mode) != 0;
    if (!(r <= g_maxX && g_minX <= l && ok)) return -1;
    Slot& s = g_slots[g_n];
    s = { l, t, r, b, 1, 0, 0 };
    bool hovered = false;
    if (!g_hoverDone) {
        const bool inside = !(g_curX < l || r < g_curX || g_curY < t || b + 4 < g_curY);
        if (inside) { s.mode = 5; s.ci = 0; s.cursorInside = 1; g_hoverDone = hovered = true; }
        else if (unit == g_hoverUnit) { s.mode = 5; s.ci = 0; g_hoverDone = hovered = true; }
    }
    if (!hovered) {
        switch (quality) {
            case 4: s.ci = 3; break; case 5: s.ci = 2; break; case 6: s.ci = 9; break;
            case 7: s.ci = 4; break; case 8: s.ci = 8; break; case 9: s.ci = 10; break;
            default: s.ci = grey ? 5 : 0;
        }
        if (gold) s.ci = 4;
    }
    return g_n++;
}
EMSCRIPTEN_KEEPALIVE int32_t d2_showitems_count() { return g_n; }
// field: 0 l, 1 t, 2 r, 3 b, 4 draw mode (1 plain, 5 highlighted), 5 text colour index, 6 cursor inside the plate
EMSCRIPTEN_KEEPALIVE int32_t d2_showitems_slot(int32_t i, int32_t field) {
    if (i < 0 || i >= g_n) return 0;
    const Slot& s = g_slots[i];
    switch (field) { case 0: return s.l; case 1: return s.t; case 2: return s.r; case 3: return s.b; case 4: return s.mode; case 5: return s.ci; default: return s.cursorInside; }
}
// The order the client's room chain presents ground items in (measured on a recorded Alt pass: nearest to the hero first by
// |dx|+|dy| in subtiles, equal distances in arrival (unit id) order); the shell sorts by this key.
EMSCRIPTEN_KEEPALIVE int32_t d2_showitems_order_key(int32_t ix, int32_t iy, int32_t px, int32_t py) {
    const int32_t dx = ix - px, dy = iy - py;
    return (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
}
}
