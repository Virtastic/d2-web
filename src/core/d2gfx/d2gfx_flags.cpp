#include <cstdint>

// D2Gfx module globals (retail image base 0x6FA80000); +0x10BDC is 0x100 at the capture point (D2Gfx+0x0B8D0 ran), +0x10BE4 is 1, the rest zero.
static uint32_t g_bdc = 0x100, g_be0, g_be4 = 1, g_bf0, g_bf4;  // +0x10BDC, +0x10BE0, +0x10BE4, +0x10BF0, +0x10BF4

extern "C" void d2gfx_sub_b1c0() { g_bf0 = 1; }                    // 1.13c D2Gfx+0x0B1C0
extern "C" uint32_t d2gfx_sub_b8d0(uint32_t edx, uint32_t (*slot)(uint32_t, uint32_t)) { g_bdc = 0x100; return slot(0x100, edx); }  // 1.13c D2Gfx+0x0B8D0: +0x10BDC = 0x100, ecx = 0x100, tail-jump to slot +0x54
extern "C" uint32_t d2gfx_sub_b8f0() { return g_bdc; }             // 1.13c D2Gfx+0x0B8F0
extern "C" uint32_t d2gfx_sub_b290() { return g_be4 ? g_be0 : 0; } // 1.13c D2Gfx+0x0B290
extern "C" void d2gfx_sub_b240(uint32_t v) { g_bf4 = v; }          // 1.13c D2Gfx+0x0B240
extern "C" uint32_t d2gfx_sub_b310() { return g_be4; }             // 1.13c D2Gfx+0x0B310
extern "C" void d2gfx_sub_b260() { g_bf4 = g_bf4 == 0; }           // 1.13c D2Gfx+0x0B260
extern "C" void d2gfx_sub_b2b0() { g_be0 = g_be0 == 0; }           // 1.13c D2Gfx+0x0B2B0
static uint32_t g_windowed;                                        // +0x1125C, zero at the capture point
static uint32_t g_renderer_mode;                                   // +0x11258, zero until a back end loads
extern "C" uint32_t d2gfx_sub_b2d0() { return 3 < g_renderer_mode; } // 1.13c D2Gfx+0x0B2D0
extern "C" uint32_t d2gfx_sub_b280(uint32_t v) { return g_be0 = v; } // 1.13c D2Gfx+0x0B280 (void; eax still holds v)
extern "C" uint32_t d2gfx_sub_b2e0(uint32_t v) { return g_be4 = 3 < g_renderer_mode ? v : 0; } // 1.13c D2Gfx+0x0B2E0
extern "C" uint32_t d2gfx_sub_b330() { return g_windowed; }        // 1.13c D2Gfx+0x0B330 (+0x1125C)
extern "C" void d2gfx_save(uint32_t* s) { s[0] = g_be0; s[1] = g_be4; }  // probes restore shared state
extern "C" void d2gfx_restore(const uint32_t* s) { g_be0 = s[0]; g_be4 = s[1]; }
