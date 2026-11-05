// 1.13c D2Client+0x825B0 probe: r[0] = EAX (unit), r[1] = ECX (view), r[2] = mode, r[3] = mask, r[5] = 0xCA54 return, r[6] = 0xC51A return; blobs v, lg = r[14], r[16].
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"
struct D2c825B0Env { uint32_t (*unitKind)(uint32_t); void (*ca2a)(uint32_t, uint32_t); void (*f69a90)(uint32_t, uint32_t); uint32_t (*c51a)(uint32_t, uint32_t); void (*f81ff0)(uint32_t, uint32_t, uint32_t); void (*ca18)(uint32_t, uint32_t); };
extern "C" void d2c_port_825b0(uint32_t, uint32_t, uint32_t, uint32_t, const D2c825B0Env*);
namespace {
HookEnv* g;
uint32_t unitKind(uint32_t u) { return g->h(0xca54, 0, 0, 0, 0, 0, u); }
void ca2a(uint32_t b, uint32_t u) { g->h(0xca2a, 0, 0, 0, 0, 0, b, u); }
void f69a90(uint32_t m, uint32_t v) { g->h(0x69a90, m, v); }
uint32_t c51a(uint32_t u, uint32_t z) { return g->h(0xc51a, 0, 0, 0, 0, 0, u, z); }
void f81ff0(uint32_t one, uint32_t u, uint32_t v) { g->h(0x81ff0, 0, 0, 0, 0, u, one); g->edi(0x81ff0, v); }
void ca18(uint32_t b, uint32_t u) { g->h(0xca18, 0, 0, 0, 0, 0, b, u); }
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_825b0(uint32_t* r) {
    const uint32_t sc[6] = { 0xca54, r[5], 0xc51a, r[6], 0, 0 };
    HookEnv he((uint32_t*)(uintptr_t)r[16], sc); g = &he;
    D2c825B0Env env = { unitKind, ca2a, f69a90, c51a, f81ff0, ca18 };
    d2c_port_825b0(r[0], r[1], r[2], r[3], &env);
    r[4] = 0;
}
