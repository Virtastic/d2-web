// 1.13c D2Client+0xa5480 FN replay probe
#include <emscripten.h>
#include <cstdint>
#include <cstring>
extern "C" uint32_t d2_client_a5480(const uint32_t*, uint32_t (*)(uint32_t));
static uint32_t* g_lg;
static uint32_t ord(uint32_t unit) { uint32_t* w = g_lg + 1 + g_lg[0]++ * 10; for (int i = 0; i < 10; ++i) w[i] = 0; w[0] = 0xC1F0; w[6] = unit; return 0x1234; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_a5480(uint32_t* r) {
    g_lg = (uint32_t*)(uintptr_t)r[1]; g_lg[0] = 0;
    r[4] = d2_client_a5480((const uint32_t*)(uintptr_t)r[0], ord);
}
