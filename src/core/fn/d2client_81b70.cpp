// 1.13c D2Client+0x81b70 FN replay probe
#include <emscripten.h>
#include <cstdint>
#include <cstring>
struct Env { uint32_t (*fillArea)(int32_t, int32_t, int32_t, int32_t, uint32_t, uint32_t); const char* (*sstrChr)(const char*, int32_t); void (*fogFree)(void*, const char*, int32_t, int32_t); uint32_t (*weaponClassId)(void*); };
extern "C" void d2_client_81b70(void*, uint8_t, uint16_t*, uint16_t*, const Env*);
static uint32_t* g_lg; static uint32_t g_hv;
static uint32_t wcid(void* u) { uint32_t* w = g_lg + 1 + g_lg[0]++ * 10; for (int i = 0; i < 10; ++i) w[i] = 0; w[0] = 0xCA5A; w[6] = (uint32_t)(uintptr_t)u; return g_hv; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_81b70(uint32_t* r) {
    uint8_t* g = (uint8_t*)(uintptr_t)r[2]; g_lg = (uint32_t*)(uintptr_t)r[3]; g_lg[0] = 0; std::memcpy(&g_hv, g + 8, 4);
    Env e; std::memset(&e, 0, sizeof e); e.weaponClassId = wcid;
    d2_client_81b70((void*)(uintptr_t)r[0], (uint8_t)r[1], (uint16_t*)g, (uint16_t*)(g + 4), &e); r[4] = 0;
}
