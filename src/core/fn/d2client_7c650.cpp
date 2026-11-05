// 1.13c D2Client+0x7C650 probe: blobs st (the global), o (the object), lg (hook log) = r[14], r[16], r[18].
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"
struct D2c7C650Env { void (*freeList)(uint32_t); void (*fogFree)(uint32_t, uint32_t, uint32_t, uint32_t); };
extern "C" void d2c_port_7c650(uint32_t*, const D2c7C650Env*);
namespace {
HookEnv* g;
void freeList(uint32_t eax) { g->h(0x7b3d0, eax); }
void fogFree(uint32_t p, uint32_t src, uint32_t line, uint32_t z) { g->h(0xbf86, 0, p, 0xB2000000u | src, 0, 0, line, z); }
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_7c650(uint32_t* r) {
    HookEnv he((uint32_t*)(uintptr_t)r[18], nullptr); g = &he;
    D2c7C650Env env = { freeList, fogFree };
    d2c_port_7c650((uint32_t*)(uintptr_t)r[14], &env);
    r[4] = 0;
}
