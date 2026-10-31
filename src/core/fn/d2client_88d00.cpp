// 1.13c D2Client+0x88d00 FN replay probe
#include <emscripten.h>
#include <cstdint>
#include <cstring>
extern "C" void d2_client_88d00(uint32_t*, void (*)(void));
static uint32_t* g_lg;
static void rec886f0() { uint32_t* w = g_lg + 1 + g_lg[0]++ * 10; for (int i = 0; i < 10; ++i) w[i] = 0; w[0] = 0x886F0; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_88d00(uint32_t* r) {
    uint32_t* g = (uint32_t*)(uintptr_t)r[0]; g_lg = (uint32_t*)(uintptr_t)r[1]; g_lg[0] = 0;
    d2_client_88d00(g, rec886f0);
    r[4] = *g;
}
