// 1.13c D2Client+0x77D30 probe: blobs tb (table window), ix (index byte), lg (hook log) = r[14], r[16], r[18].
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"
struct D2c77D30Env { void (*closeFile)(uint32_t); void (*fogFree)(uint32_t, uint32_t, uint32_t, uint32_t); };
extern "C" void d2c_port_77d30(uint8_t*, const uint8_t*, const D2c77D30Env*);
namespace {
HookEnv* g;
void closeFile(uint32_t h) { g->h(0xc18a, 0, 0, 0, 0, 0, h); }
void fogFree(uint32_t p, uint32_t src, uint32_t line, uint32_t z) { g->h(0xbf86, 0, p, 0xB2000000u | src, 0, 0, line, z); }
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_77d30(uint32_t* r) {
    HookEnv he((uint32_t*)(uintptr_t)r[18], nullptr); g = &he;
    D2c77D30Env env = { closeFile, fogFree };
    d2c_port_77d30((uint8_t*)(uintptr_t)r[14], (const uint8_t*)(uintptr_t)r[16], &env);
    r[4] = 0;
}
