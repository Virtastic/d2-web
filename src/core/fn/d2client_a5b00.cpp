// 1.13c D2Client+0xa5b00 FN replay probe
#include <emscripten.h>
#include <cstdint>
#include <cstring>
struct Env { uint32_t (*ord10044)(); void (*insert)(void*, uint32_t, uint32_t, uint32_t); void* r[8]; };
extern "C" void d2_client_a5b00(uint32_t, uint32_t, const Env*);
extern "C" uintptr_t d2_client_slots_10a608();
static uint32_t* g_lg;
static void ins(void* base, uint32_t slot, uint32_t value, uint32_t owner) {
    uint32_t* w = g_lg + 1 + g_lg[0]++ * 10; for (int i = 0; i < 10; ++i) w[i] = 0;
    w[0] = 0xA6EF0; w[2] = 0xB2000000u | (uint32_t)(0x10a608 + ((uintptr_t)base - d2_client_slots_10a608()));
    w[3] = slot; w[6] = value; w[7] = owner;
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_a5b00(uint32_t* r) {
    g_lg = (uint32_t*)(uintptr_t)r[2]; g_lg[0] = 0;
    Env e; std::memset(&e, 0, sizeof e); e.insert = ins;
    d2_client_a5b00(r[0], r[1], &e); r[4] = 0;
}
