// 1.13c D2Client+0xad800 FN replay probe
#include <emscripten.h>
#include <cstdint>
#include <cstring>
struct Env { void* (*unit_lookup)(const void*, int32_t); void (*unit_refresh)(void); void (*unit_free)(int32_t); void (*msg_47970)(const void*); void (*msg_82F50)(const void*); void (*input_1e160)(const void*, const void*); };
extern "C" void d2_client_ad800(const void*, const Env*);
static uint32_t* g_lg; static uint32_t g_hit, g_sel;
static void* look(const void*, int32_t id) { uint32_t* w = g_lg + 1 + g_lg[0]++ * 10; for (int i = 0; i < 10; ++i) w[i] = 0; w[0] = 0x9D630; w[3] = (uint32_t)id; return g_sel ? (void*)(uintptr_t)g_hit : nullptr; }
static void refresh() { uint32_t* w = g_lg + 1 + g_lg[0]++ * 10; for (int i = 0; i < 10; ++i) w[i] = 0; w[0] = 0x772E0; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_ad800(uint32_t* r) {
    const uint8_t* rec = (const uint8_t*)(uintptr_t)r[0];
    g_lg = (uint32_t*)(uintptr_t)r[1]; g_lg[0] = 0; std::memcpy(&g_hit, rec + 16, 4); std::memcpy(&g_sel, rec + 20, 4);
    Env e; std::memset(&e, 0, sizeof e); e.unit_lookup = look; e.unit_refresh = refresh;
    d2_client_ad800(rec, &e); r[4] = 0;
}
