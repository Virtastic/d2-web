// 1.13c D2Client+0xa5420 FN replay probe
#include <emscripten.h>
#include <cstdint>
#include <cstring>
struct Env { uint32_t (*ord10044)(); void (*insert)(void*, uint32_t, uint32_t, uint32_t); void* (*find)(uint32_t, void*, uint32_t, uint32_t); void (*removeAll)(void*); void* r[8]; };
extern "C" void d2_client_a5420(void*, const Env*);
static uint32_t* g_lg;
static void rem(void* esi) { uint32_t* w = g_lg + 1 + g_lg[0]++ * 10; for (int i = 0; i < 10; ++i) w[i] = 0; w[0] = 0xA8630; w[5] = (uint32_t)(uintptr_t)esi; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_a5420(uint32_t* r) {
    g_lg = (uint32_t*)(uintptr_t)r[1]; g_lg[0] = 0;
    Env e; std::memset(&e, 0, sizeof e); e.removeAll = rem;
    d2_client_a5420((void*)(uintptr_t)r[0], &e); r[4] = 0;
}
