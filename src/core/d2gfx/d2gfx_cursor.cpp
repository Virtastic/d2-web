#include <cstdint>
// 1.13c D2Gfx+0x8290. *shown = +0x10CF8. ShowCursor's OS counter is not modelled (wasm has none); the flag is the observable.
extern "C" void d2gfx_sub_8290(uint32_t windowed, uint32_t mode, uint32_t hide, uint32_t* shown) {
    if (!windowed || mode == 4 || mode == 5 || mode == 6) return;
    *shown = !hide;   // 0x80A0 hides (flag 0), 0x8070 shows (flag 1); both no-op when already there
}
// 1.13c D2Gfx+0x8070 (show) / +0x80A0 (hide): flag-gated ShowCursor loops; flag = +0x10CF8.
extern "C" void d2gfx_sub_8070(uint32_t* shown) { if (!*shown) *shown = 1; }
extern "C" void d2gfx_sub_80A0(uint32_t* shown) { if (*shown) *shown = 0; }
