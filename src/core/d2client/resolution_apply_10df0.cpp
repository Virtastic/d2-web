#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x10DF0: apply video mode (ESI = mode). Pure part: the dimension globals.
struct D2ClientRes { uint32_t w, h, wide, hudBottom, hudRight, hudW; };
static D2ClientRes g_res = {640, 480, 0, 440, 640, 640};
static_assert(sizeof(D2ClientRes) == 24, "layout");

// cur = D2Gfx ordinal 10031 result. Returns 1 if the rebuild tail (10176, 0xEFF90, 0x108D0(1), 0x73090) must run.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_10df0(uint32_t mode, uint32_t cur) {
    if (mode == cur) return 0;
    if (mode == 0) { g_res.w = 0x280; g_res.h = 0x1e0; g_res.wide = 0; }
    else if (mode == 2) { g_res.w = 800; g_res.h = 600; g_res.wide = 1; }
    g_res.hudBottom = g_res.h - 0x28;   // DAT 0x...9e18
    g_res.hudRight = g_res.w;           // DAT 0x...bc50
    g_res.hudW = g_res.w;               // DAT 0x...9e14
    return 1;
}
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_10df0_get(uint32_t i) { return i < 6 ? (&g_res.w)[i] : 0; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_10df0_set(uint32_t i, uint32_t v) { if (i < 6) (&g_res.w)[i] = v; }
