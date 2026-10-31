// 1.13c D2Client+0xa5b60 FN replay probe
#include <emscripten.h>
#include <cstdint>
#include <cstring>
typedef uintptr_t U;
struct D2ClientA5Env { void (*fatal)(uint32_t); void (*o0)(); void (*o1)(); void (*o2)(); U (*fn_9d630)(uint32_t, uint32_t); void* rest[40]; };
extern "C" uint16_t d2_client_a5b60(U rec, uint32_t arg, const D2ClientA5Env*);
static uint32_t* g_lg; static uint32_t g_hit, g_sel;
static U look(uint32_t a, uint32_t b) { uint32_t* w = g_lg + 1 + g_lg[0]++ * 10; for (int i = 0; i < 10; ++i) w[i] = 0; w[0] = 0x9D630; w[2] = a; w[3] = b; return g_sel ? (U)g_hit : 0; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_a5b60(uint32_t* r) {
    g_lg = (uint32_t*)(uintptr_t)r[2]; g_lg[0] = 0; g_hit = 0; g_sel = 0;
    if (r[0]) { const uint8_t* rec = (const uint8_t*)(uintptr_t)r[0]; std::memcpy(&g_hit, rec + 16, 4); std::memcpy(&g_sel, rec + 20, 4); }
    D2ClientA5Env e; std::memset(&e, 0, sizeof e); e.fn_9d630 = look;
    r[4] = d2_client_a5b60((U)r[0], r[1], &e);
}
