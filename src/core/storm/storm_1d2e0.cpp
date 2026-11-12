// src/core/storm/storm_1d2e0.cpp -- 1.13c Storm+0x1D2E0: subtract from a region every visible, non-transparent descendant
// window rect of a window (in the base window's client coordinates). The user32 / gdi32 calls go through g_storm_imp_hook keyed
// by the retail IAT slot (the FN probe in src/core/fn/storm_walker.cpp records them); with no hook they answer 0.
#include <cstdint>

extern "C" uint32_t (*g_storm_imp_hook)(uint32_t slot, uint32_t w0, uint32_t w1, uint32_t w2);
extern "C" uint32_t g_storm_imp_w3;   // fourth stack word of CreateRectRgn / CombineRgn

namespace {
uint32_t g_lastError;              // 0x6FC45388
uint32_t* g_err = &g_lastError;
inline uint32_t Imp(uint32_t slot, uint32_t a = 0, uint32_t b = 0, uint32_t c = 0) {
    return g_storm_imp_hook ? g_storm_imp_hook(slot, a, b, c) : 0;
}
inline uint32_t Imp4(uint32_t slot, uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
    g_storm_imp_w3 = d;
    uint32_t v = Imp(slot, a, b, c);
    g_storm_imp_w3 = 0;
    return v;
}
inline uint32_t P(const void* p) { return (uint32_t)(uintptr_t)p; }
}  // namespace

// 1.13c Storm+0x1D2E0 (fastcall ECX base, EDX region, [esp+4] window)
void Storm_Sub1D2E0(void* base, void* rgn, void* wnd) {
    if (base == nullptr || rgn == nullptr || wnd == nullptr) {
        *g_err = 0x57;   // SetLastError(0x57) has no host effect here
        return;
    }
    const uint32_t b = P(base), g = P(rgn), w = P(wnd);
    if (Imp(0x43374, w) == 0) return;                         // IsWindowVisible
    const uint32_t ex = Imp(0x4330c, w, 0xFFFFFFECu);         // GetWindowLongA(w, GWL_EXSTYLE)
    if (w != b && (ex & 0x20) == 0) {
        volatile int32_t rc[4] = {0, 0, 0, 0};
        Imp(0x432b0, w, P((const void*)rc));                  // GetWindowRect
        Imp(0x432b4, b, P((const void*)rc));                  // ScreenToClient (left, top)
        Imp(0x432b4, b, P((const void*)(rc + 2)));            // ScreenToClient (right, bottom)
        if (Imp(0x43078, g, P((const void*)rc)) != 0) {       // RectInRegion
            const uint32_t n = Imp4(0x4307c, (uint32_t)rc[0], (uint32_t)rc[1], (uint32_t)rc[2], (uint32_t)rc[3]);   // CreateRectRgn
            Imp4(0x43074, g, g, n, 4);                        // CombineRgn(rgn, rgn, n, RGN_DIFF)
            Imp(0x43024, n);                                  // DeleteObject
        }
    }
    for (uint32_t c = Imp(0x432a0, w); c != 0; c = Imp(0x43358, c, 2))   // GetTopWindow / GetWindow(GW_HWNDNEXT)
        Storm_Sub1D2E0(base, rgn, (void*)(uintptr_t)c);
}

// FN probe entry: errWord is the pinned last-error word.
extern "C" void storm_1d2e0_fnrun(uintptr_t base, uintptr_t rgn, uintptr_t wnd, uint32_t* errWord) {
    g_err = errWord;
    Storm_Sub1D2E0((void*)base, (void*)rgn, (void*)wnd);
    g_err = &g_lastError;
}
