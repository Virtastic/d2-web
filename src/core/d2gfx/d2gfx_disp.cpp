#include <cstdlib>
// 1.13c D2Gfx+0xB050 / 0xB0B0 / 0xBA30: wrappers that forward to a slot of the loaded renderer table.
// The slot is a parameter (the table is not ported): slot(ecx, edx, stack words...) and its eax passes through.
#include <cstdint>
typedef uint32_t (*D2GfxSlot4)(uint32_t ecx, uint32_t edx, uint32_t, uint32_t, uint32_t, uint32_t);
typedef uint32_t (*D2GfxSlot5)(uint32_t ecx, uint32_t edx, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
extern "C" uint32_t d2gfx_sub_b050(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, D2GfxSlot4 slot) { return slot(a0, a1, a2, a3, a4, a5); }
extern "C" uint32_t d2gfx_sub_b0b0(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6, D2GfxSlot5 slot) { return slot(a0, a1, a2, a3, a4, a5, a6); }
// D2FillArea: signed right < left or bottom < top forwards nothing; eax is not written on the first reject (entered value), is the bottom on the second.
extern "C" uint32_t d2gfx_D2FillArea(uint32_t eaxIn, int32_t left, int32_t top, int32_t right, int32_t bottom, uint32_t c, uint32_t x, D2GfxSlot4 slot) {
    if (right < left) return eaxIn;
    if (bottom < top) return (uint32_t)bottom;
    return slot((uint32_t)left, (uint32_t)top, (uint32_t)right, (uint32_t)bottom, c, x);
}
// 1.13c D2Gfx+0xB080 (DrawCelContext): six words, slot +0x84 gets ecx = a0, edx = a1, a2..a5 on the stack.
extern "C" uint32_t d2gfx_sub_b080(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, D2GfxSlot4 slot) { return slot(a0, a1, a2, a3, a4, a5); }
// 1.13c D2Gfx+0xADC0: the one word goes to slot +0x28 in ecx, nothing on the stack; eax passes through.
extern "C" uint32_t d2gfx_sub_adc0(uint32_t a0, uint32_t (*slot)(uint32_t ecx)) { return slot(a0); }
// 1.13c D2Gfx+0xAE10 (retail: eax = source): the DLL's table copy (+0x14788, 0x120 bytes), its self pointer (+0x148A8), and 49 x 0x100 bytes
// gathered through source dwords 3..51 into +0x11678. The state is the DLL's, so it is exposed to the probe, not returned.
static uint32_t g_ae10_copy[0x48], g_ae10_self;
static uint8_t g_ae10_pal[49 * 0x100];
extern "C" void d2gfx_sub_ae10(const uint32_t* src) {
    for (int i = 0; i < 0x48; ++i) g_ae10_copy[i] = src[i];
    if ((uintptr_t)g_ae10_copy > 0xFFFFFFFFu) abort();   // the word stored below is 32-bit
    g_ae10_self = (uint32_t)(uintptr_t)g_ae10_copy;
    for (int i = 0; i < 49; ++i) { const uint8_t* p = (const uint8_t*)(uintptr_t)src[3 + i]; for (int k = 0; k < 0x100; ++k) g_ae10_pal[i * 0x100 + k] = p[k]; }
}
extern "C" const uint32_t* d2gfx_ae10_copy() { return g_ae10_copy; }
extern "C" const uint8_t* d2gfx_ae10_pal() { return g_ae10_pal; }
// 1.13c D2Gfx+0xB100 (stdcall src): slot +0x74 gets ecx = src, then sub_AE10 runs on src.
extern "C" void d2gfx_sub_b100(uint32_t src, void (*slot)(uint32_t ecx)) { slot(src); d2gfx_sub_ae10((const uint32_t*)(uintptr_t)src); }
// 1.13c D2Gfx+0x7CC0: unless the windowed flag (+0x1125C) is 1, show(hwnd, cmd) for each nonzero hwnd (+0) of the four 0x30-byte records (cmd at +0xC) in order.
extern "C" void d2gfx_sub_7cc0(uint32_t windowed, const uint8_t* table, void (*show)(uint32_t hwnd, uint32_t cmd)) {
    if (windowed == 1) return;
    for (int r = 0; r < 4; ++r) { const uint32_t* rec = (const uint32_t*)(table + r * 0x30); if (rec[0]) show(rec[0], rec[3]); }
}
// 1.13c D2Gfx+0xB9F0 (sub_B9F0): byte-for-byte D2FillArea's wrapper, forwarding through table slot +0xBC instead of +0xB8.
extern "C" uint32_t d2gfx_sub_b9f0(uint32_t eaxIn, int32_t left, int32_t top, int32_t right, int32_t bottom, uint32_t c, uint32_t x, D2GfxSlot4 slot) { return d2gfx_D2FillArea(eaxIn, left, top, right, bottom, c, x, slot); }
// 1.13c D2Gfx+0xB900: remember the current word (+0x10BD8) at +0x10BDC, then tail-call slot +0x54 with ecx = the word.
extern "C" uint32_t d2gfx_sub_b900(uint32_t cur, uint32_t* last, uint32_t (*slot)(uint32_t ecx)) { *last = cur; return slot(cur); }
// 1.13c D2Gfx+0xB9A0 (stdcall a0): slot +0xC4 with ecx = a0.
extern "C" uint32_t d2gfx_sub_b9a0(uint32_t a0, uint32_t (*slot)(uint32_t ecx)) { return slot(a0); }
// 1.13c D2Gfx+0xBA70 (stdcall a0, a1, a2): slot +0xB4 with ecx = a0, dl = low byte of a1, stack a2.
extern "C" uint32_t d2gfx_sub_ba70(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t (*slot)(uint32_t ecx, uint8_t dl, uint32_t a2)) { return slot(a0, (uint8_t)a1, a2); }
// 1.13c D2Gfx+0x7F50: 1 iff the renderer table (+0x11268) exists and the flag (+0x1125C) is 0.
extern "C" uint32_t d2gfx_sub_7f50(uint32_t table, uint32_t flag) { return table != 0 && flag == 0; }
// 1.13c D2Gfx+0xB1E0 (stdcall a0): a non-zero a0 replaces the stored word (+0x10BEC); slot +0x58 gets ecx = the stored word.
extern "C" uint32_t d2gfx_sub_b1e0(uint32_t a0, uint32_t* stored, uint32_t (*slot)(uint32_t ecx)) { if (a0) *stored = a0; return slot(*stored); }
// 1.13c D2Gfx+0x7E90 (D2GetWindowHandle, slot +0x38) / +0x7E20 (WINDOW_DecodeSmacker, slot +0x40): stdcall(a0, a1, a2); non-null table -> slot(ecx = a0, edx = a1, a2). (Null table: log + _exit, not modelled.)
typedef uint32_t (*D2GfxSlot1)(uint32_t ecx, uint32_t edx, uint32_t);
extern "C" uint32_t d2gfx_sub_7e90(uint32_t a0, uint32_t a1, uint32_t a2, D2GfxSlot1 slot) { return slot(a0, a1, a2); }
extern "C" uint32_t d2gfx_sub_7e20(uint32_t a0, uint32_t a1, uint32_t a2, D2GfxSlot1 slot) { return slot(a0, a1, a2); }
// 1.13c D2Gfx+0xBB20 (slot +0xA0, six words) and +0xBB50 (slot +0x9C, five words): ecx = a0, edx = a1, the rest on the stack.
typedef uint32_t (*D2GfxSlot3)(uint32_t ecx, uint32_t edx, uint32_t, uint32_t, uint32_t);
extern "C" uint32_t d2gfx_sub_bb20(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, D2GfxSlot4 slot) { return slot(a0, a1, a2, a3, a4, a5); }
extern "C" uint32_t d2gfx_sub_bb50(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, D2GfxSlot3 slot) { return slot(a0, a1, a2, a3, a4); }
extern "C" uint32_t d2gfx_sub_aff0(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, D2GfxSlot3 slot) { return slot(a0, a1, a2, a3, a4); }  // 1.13c D2Gfx+0xAFF0, slot +0x98
// 1.13c D2Gfx+0x0B7A0 (D2CreateMainWindow, slot +0x64): stdcall(a0..a4); mode >= 4 and +0x10BE0 != 0 -> slot(ecx = a0, edx = a1, a2, a3, a4). (Else log + _exit, not modelled.)
extern "C" uint32_t d2gfx_sub_b7a0(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, D2GfxSlot3 slot) { return slot(a0, a1, a2, a3, a4); }
// 1.13c D2Gfx+0x7ED0 (D2PrintImage): tail-jump to slot +0x34, ecx and edx untouched.
extern "C" uint32_t d2gfx_sub_7ed0(uint32_t ecx, uint32_t edx, uint32_t (*slot)(uint32_t, uint32_t)) { return slot(ecx, edx); }
// 1.13c D2Gfx+0xBA90 (slot +0xB0) / +0xBAB0 (+0xAC) / +0xBAD0 (+0xA8) / +0xAF90 (+0xD0): stdcall(a0, a1); slot(ecx = a0, dl = low byte of a1), no stack word; eax passes through.
typedef uint32_t (*D2GfxSlotCD)(uint32_t ecx, uint8_t dl);
extern "C" uint32_t d2gfx_slot_cd(uint32_t a0, uint32_t a1, D2GfxSlotCD slot) { return slot(a0, (uint8_t)a1); }
// 1.13c D2Gfx+0xB220: the word at +0x10BE8 becomes 1 if it was 0, else 0.
extern "C" void d2gfx_sub_b220(uint32_t* w) { *w = (*w == 0); }
// 1.13c D2Gfx+0x7DA0 (slot +0x48) / +0x7DE0 (slot +0x44): stdcall(a0); slot(ecx = a0).
extern "C" uint32_t d2gfx_sub_7da0(uint32_t a0, uint32_t (*slot)(uint32_t ecx)) { return slot(a0); }
extern "C" uint32_t d2gfx_sub_7de0(uint32_t a0, uint32_t (*slot)(uint32_t ecx)) { return slot(a0); }
// 1.13c D2Gfx+0x7E60 (D2GetResolution): tail-jump to slot +0x3C, ecx and edx untouched.
extern "C" uint32_t d2gfx_sub_7e60(uint32_t ecx, uint32_t edx, uint32_t (*slot)(uint32_t, uint32_t)) { return slot(ecx, edx); }
// 1.13c D2Gfx+0x7F70: stdcall(a0, a1); slot +0x50 (ecx = a0, edx = a1), no stack word.
extern "C" uint32_t d2gfx_sub_7f70(uint32_t a0, uint32_t a1, uint32_t (*slot)(uint32_t, uint32_t)) { return slot(a0, a1); }
// 1.13c D2Gfx+0x7F10: stdcall(a0); *flag (+0x1125C) = (a0 == 0); slot +0x2C (ecx = a0, edx = flag), no stack word. Null table: log + _exit, not modelled.
extern "C" uint32_t d2gfx_sub_7f10(uint32_t a0, uint32_t* flag, uint32_t (*slot)(uint32_t, uint32_t)) { *flag = (a0 == 0); return slot(a0, *flag); }
// 1.13c D2Gfx+0x7FD0 (ordinal 10025): stdcall(mode, *w, *h); mode > 3 logs + _exit(-1) (not modelled).
extern "C" void d2gfx_sub_7fd0(uint32_t mode, uint32_t* w, uint32_t* h) {
    static const uint16_t dim[4][2] = {{640, 480}, {800, 600}, {800, 600}, {1344, 700}};
    if (mode > 3) { *w = 0; *h = 0; return; }
    *w = dim[mode][0]; *h = dim[mode][1];
}
// 1.13c D2Gfx+0xB6E0 (ordinal 10037): with mode (+0x11258) 4..7 and +0x10BE0 set, tail-jump slot +0x6C, ecx and edx untouched; else retail logs and _exit(-1) (not modelled).
extern "C" uint32_t d2gfx_sub_b6e0(uint32_t ecx, uint32_t edx, uint32_t (*slot)(uint32_t, uint32_t)) { return slot(ecx, edx); }
// 1.13c D2Gfx+0xB180 (ordinal): tail-jump to slot +0x30, ecx and edx untouched.
extern "C" uint32_t d2gfx_sub_b180(uint32_t ecx, uint32_t edx, uint32_t (*slot)(uint32_t, uint32_t)) { return slot(ecx, edx); }
// 1.13c D2Gfx+0xB1A0 (ordinal): tail-jump to slot +0x4c, ecx and edx untouched.
extern "C" uint32_t d2gfx_sub_b1a0(uint32_t ecx, uint32_t edx, uint32_t (*slot)(uint32_t, uint32_t)) { return slot(ecx, edx); }
// 1.13c D2Gfx+0xB1D0 (ordinal): tail-jump to slot +0x5c, ecx and edx untouched.
extern "C" uint32_t d2gfx_sub_b1d0(uint32_t ecx, uint32_t edx, uint32_t (*slot)(uint32_t, uint32_t)) { return slot(ecx, edx); }
// 1.13c D2Gfx+0xB0E0 (ordinal 10038): stdcall(a0, a1, a2); slot +0x78 gets ecx = a0, edx = a1, a2 on the stack.
extern "C" uint32_t d2gfx_sub_b0e0(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t (*slot)(uint32_t, uint32_t, uint32_t)) { return slot(a0, a1, a2); }
// 1.13c D2Gfx+0x7C80 (sub_7C80): nine words, slot +0x7C gets ecx = a0, edx = a1, a2..a8 on the stack.
typedef uint32_t (*D2GfxSlot7)(uint32_t ecx, uint32_t edx, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
extern "C" uint32_t d2gfx_sub_7c80(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7, uint32_t a8, D2GfxSlot7 slot) { return slot(a0, a1, a2, a3, a4, a5, a6, a7, a8); }
// 1.13c D2Gfx+0x82D0 (D2ShowWindow, ordinal 10026, teardown): slot +0x10 result != 0; the Win32 tail (ShowCursor, DestroyWindow, ChangeDisplaySettingsA) has no browser analogue. (Null table: log + _exit, not modelled.)
extern "C" uint32_t d2gfx_sub_82d0(uint32_t (*slot)(uint32_t, uint32_t)) { return slot(0, 0) != 0; }
// 1.13c D2Gfx+0x7D00: unless the windowed flag (+0x1125C) is 1, for edge 0..3 appbar(edge) (SHAppBarMessage ABM_GETAUTOHIDEBAR) -> hwnd stored in record[edge] (+0); a nonzero hwnd is then hidden. Retail also sets WINDOWPLACEMENT.length = 0x2C at record+4 and fills it (GetWindowPlacement, not modelled) which 7CC0 replays; the browser has no shell bars, so appbar is a parameter.
extern "C" void d2gfx_sub_7d00(uint32_t windowed, uint32_t* table, uint32_t (*appbar)(uint32_t edge), void (*hide)(uint32_t hwnd)) {
    if (windowed == 1) return;
    for (uint32_t e = 0; e < 4; ++e) { uint32_t h = appbar(e); table[e * 12] = h; if (h) { table[e * 12 + 1] = 0x2C; hide(h); } }
}
// 1.13c D2Gfx+0xADD0: table slots +0x1C then +0x20, no arguments; returns 1.
extern "C" uint32_t d2gfx_sub_add0(uint32_t (*s1c)(), uint32_t (*s20)()) { s1c(); s20(); return 1; }
// 1.13c D2Gfx+0xAFB0 (slot +0x94): stdcall(a0..a3); slot(ecx = a0, edx = a1, a2, a3) on the stack, eax passes through.
extern "C" uint32_t d2gfx_sub_afb0(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t (*slot)(uint32_t, uint32_t, uint32_t, uint32_t)) { return slot(a0, a1, a2, a3); }
// 1.13c D2Gfx+0xBBA0 (slot +0xC8): cdecl(a0, a1, a2, ...); slot(ecx = a0, edx = a1, a2, &fourth argument) -- the slot is handed the address of the caller's remaining argument words.
extern "C" uint32_t d2gfx_sub_bba0(uint32_t a0, uint32_t a1, uint32_t a2, const uint32_t* rest, uint32_t (*slot)(uint32_t, uint32_t, uint32_t, const uint32_t*)) { return slot(a0, a1, a2, rest); }
// 1.13c D2Gfx+0xB800: renderer mode (+0x11258) 4..7 and +0x10BE0 != 0 -> store a0 at +0x1124C, a1 at +0x11248, slot +0x60(ecx = a0, edx = a1). (Else log + _exit, not modelled.)
extern "C" uint32_t d2gfx_sub_b800(uint32_t a0, uint32_t a1, uint32_t* w1124c, uint32_t* w11248, uint32_t (*slot)(uint32_t, uint32_t)) { *w1124c = a0; *w11248 = a1; return slot(a0, a1); }
// 1.13c D2Gfx+0x84C0 (WINDOW_GetState, no arguments): state == want or a null window leaves the state untouched. Pausing
// minimises the window (ShowWindow 6), shows the auto-hide bars (0x7CC0), drains `while (ShowCursor(1) < 0)`, tells the
// surface (slot +0x14: ecx = hwnd, edx = res, stack = the new state) and sets the state. Unpausing returns while the window
// is iconic, restores the placement (0x7D00), drains `while (ShowCursor(0) >= 0)`, tells the surface, then runs the palette
// slot (+0x70: ecx = palette, edx = the interface pointer) and the contrast/gamma tail (0xAEE0). The table is not ported:
// its two slots and the three Win32 imports are parameters. The null-table fatal path (Fog 10265/10024, _exit(-1)) is not modelled.
extern "C" uint32_t d2gfx_sub_84c0(uint32_t* state, uint32_t want, uint32_t hwnd, uint32_t res, uint32_t table, uint32_t palette,
        uint32_t (*isIconic)(uint32_t hwnd), uint32_t (*showWindow)(uint32_t hwnd, uint32_t cmd), int32_t (*showCursor)(int32_t show),
        uint32_t (*updatePlacement)(), uint32_t (*showAll)(),
        uint32_t (*stateSlot)(uint32_t hwnd, uint32_t res, uint32_t state), uint32_t (*paletteSlot)(uint32_t ecx, uint32_t edx),
        uint32_t (*contrastGamma)()) {
    if (*state == want || hwnd == 0) return *state;
    if (want) { showWindow(hwnd, 6); showAll(); while (showCursor(1) < 0) {} }
    else { if (isIconic(hwnd)) return *state; updatePlacement(); while (showCursor(0) >= 0) {} }
    *state = want; stateSlot(hwnd, res, *state);
    if (*state == 0) { paletteSlot(palette, table); contrastGamma(); }
    return *state;
}
// 1.13c D2Gfx+0x88B0: sub_84C0, then return the word at +0x1D66C. With the window handle (+0x11264) null, 84C0 changes nothing (the Win32 branch is not modelled).
extern "C" uint32_t d2gfx_sub_88b0(uint32_t cur) { return cur; }

// 1.13c D2Gfx+0xAEE0: contrast/gamma restore. load = Fog#423 SRegLoadValue("Diablo II", key, 0, &out), leaves out untouched when the key is absent.
// Contrast preset 100 (0 when renderer mode +0x11258 == 4) -> slot +0x30(ecx = 0xB, edx = contrast); Gamma preset 0x9B -> *gamma (+0x10BEC) -> slot +0x58(ecx = gamma). Returns the slot +0x58 eax.
extern "C" uint32_t d2gfx_sub_aee0(uint32_t mode, void (*load)(const char*, const char*, uint32_t, uint32_t*), uint32_t (*slot30)(uint32_t, uint32_t), uint32_t (*slot58)(uint32_t), uint32_t* gamma) {
    uint32_t c = mode == 4 ? 0 : 100, g = 0x9b;
    load("Diablo II", "Contrast", 0, &c); slot30(0xb, c);
    load("Diablo II", "Gamma", 0, &g); *gamma = g; return slot58(g);
}
// 1.13c D2Gfx+0xBAF0: stdcall(a0..a4) -> renderer slot +0xA4 (ecx = a0, edx = a1, stack a2, a3, a4).
extern "C" uint32_t d2gfx_sub_baf0(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t (*slot)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)) { return slot(a0, a1, a2, a3, a4); }
// 1.13c D2Gfx+0xB020: stdcall(a0..a5); a4 == 0 -> nothing (eax = 0); else slot +0x8C (ecx = a0, edx = a1, stack a2..a5).
extern "C" uint32_t d2gfx_sub_b020(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t (*slot)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)) { return a4 ? slot(a0, a1, a2, a3, a4, a5) : 0; }
// 1.13c D2Gfx+0x80D0: table present: [+0x11260] := a0; windowed (+0x1125C == 1) re-sizes the Win32 window (not modelled, no browser window); returns 1.
extern "C" uint32_t d2gfx_sub_80d0(uint32_t a0, uint32_t* w11260) { *w11260 = a0; return 1; }
// 1.13c D2Gfx+0xB120: src != 0 -> copy 256 dwords to the palette table (+0x11278); slot +0x70 (ecx = src; null passes the table address).
extern "C" void d2gfx_sub_b120(const uint32_t* src, uint32_t* pal, void (*slot)(uint32_t ecx)) { if (src) for (int i = 0; i < 0x100; ++i) pal[i] = src[i]; slot(src ? (uint32_t)(uintptr_t)src : (uint32_t)(uintptr_t)pal); }
// 1.13c D2Gfx+0xB3A0 (the tail of D2GFX_Release, ordinal 10001): FreeLibrary(+0x1126C) -- a bad handle just fails, so the BOOL is 0;
// then ordinal 10233 (thunk 0x7842), then the module word (+0x1126C) and the renderer-table word (+0x11268) are zeroed.
// The null-module-word assert (Fog#10265 message + Fog#10024 log) followed by _exit(-1) is not modelled: the probe never pins zero.
extern "C" uint32_t d2gfx_sub_b3a0(uint32_t* table, uint32_t* module, void (*after)()) { after(); *module = 0; *table = 0; return 0; }
// 1.13c D2Gfx+0xB4B0 (ordinal 10084, renderer shutdown): slot +8 of the table; UnregisterClassA; (error box when +0x1D664 != 0, not modelled);
// then sub_B3A0: FreeLibrary(module), ordinal 10233, table ptr and module words cleared. (Null module: log + _exit, not modelled.)
extern "C" void d2gfx_sub_b4b0(uint32_t* table, uint32_t* module, void (*slot)(), void (*after)()) { slot(); after(); *module = 0; *table = 0; }
// 1.13c D2Gfx+0xBB80: stdcall(a0..a3) -> slot +0xCC (ecx = a0, edx = a1, a2, a3); eax passes through.
extern "C" uint32_t d2gfx_sub_bb80(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t (*slot)(uint32_t, uint32_t, uint32_t, uint32_t)) { return slot(a0, a1, a2, a3); }
// 1.13c D2Gfx+0x7F00: tail jmp to slot +0xD4; ecx, edx and the stack pass through.
extern "C" uint32_t d2gfx_sub_7f00(uint32_t ecx, uint32_t edx, uint32_t (*slot)(uint32_t, uint32_t)) { return slot(ecx, edx); }
// 1.13c D2Gfx+0xADF0: stdcall(a0..a3), edx as entered -> slot +0x18 (ecx = a0, dl = low byte of a1, edx otherwise as entered, a2, a3).
extern "C" uint32_t d2gfx_sub_adf0(uint32_t edx, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t (*slot)(uint32_t, uint32_t, uint32_t, uint32_t)) { return slot(a0, (edx & ~0xffu) | (a1 & 0xffu), a2, a3); }

// 1.13c D2Gfx+0x1D470 (export ordinal 10087, `?rInitialWindowRect@@3UtagRECT@@A`): a tagRECT, 16 bytes
// (left, top, right, bottom; each a 32-bit LONG), the main window's screen rectangle at startup. The .data
// image is 16 zero bytes and nothing writes it at load; it is written only inside Win32 window branches --
// D2Gfx+0x80D0 at 0x826F (sub_7FD0 into right/bottom, then GetWindowRect over the whole rect) and D2Win's
// own centring routine (store at D2Win RVA 0x17625 through the IAT slot at D2Win RVA 0x1B0C8). No 1.13c code
// reads it. A browser has no OS window, so it is session state here; the writers' branch is the one
// already declares unmodelled.
extern "C" int32_t d2gfx_rInitialWindowRect[4];
extern "C" const int32_t* d2gfx_rInitialWindowRect_get(void) { return d2gfx_rInitialWindowRect; }
