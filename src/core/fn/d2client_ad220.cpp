// 1.13c D2Client+0xad220 FN replay probe
#include <emscripten.h>
#include <cstdint>
#include <cstring>
extern "C" void d2_client_ad220(uint32_t, int32_t (*)(uint32_t, int32_t));
static uint32_t* g_lg; static uint32_t g_hit, g_sel;
static int32_t look(uint32_t, int32_t id) { uint32_t* w = g_lg + 1 + g_lg[0]++ * 10; for (int i = 0; i < 10; ++i) w[i] = 0; w[0] = 0x9D630; w[3] = (uint32_t)id; return g_sel ? (int32_t)g_hit : 0; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_ad220(uint32_t* r) {
    const uint8_t* rec = (const uint8_t*)(uintptr_t)r[0];
    g_lg = (uint32_t*)(uintptr_t)r[1]; g_lg[0] = 0; std::memcpy(&g_hit, rec + 16, 4); std::memcpy(&g_sel, rec + 20, 4);
    d2_client_ad220(r[0], look); r[4] = 0;
}
