// 1.13c D2Client+0x9d670 FN replay probe
#include <emscripten.h>
#include <cstdint>
#include <cstring>
typedef uint32_t (*Find4D630)(uint32_t, uint32_t);
extern "C" uint32_t d2_client_9d670(uint32_t, uint32_t, uint32_t, Find4D630);
static uint32_t* g_lg; static uint32_t g_hit, g_sel;
static uint32_t look(uint32_t a, uint32_t b) { uint32_t* w = g_lg + 1 + g_lg[0]++ * 10; for (int i = 0; i < 10; ++i) w[i] = 0; w[0] = 0x9D630; w[2] = a; w[3] = b; return g_sel ? g_hit : 0; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_9d670(uint32_t* r) {
    const uint8_t* g = (const uint8_t*)(uintptr_t)r[2]; uint32_t unit; std::memcpy(&unit, g, 4); std::memcpy(&g_hit, g + 4, 4); std::memcpy(&g_sel, g + 8, 4);
    g_lg = (uint32_t*)(uintptr_t)r[3]; g_lg[0] = 0;
    r[4] = d2_client_9d670(r[0], r[1], unit, look);
}
