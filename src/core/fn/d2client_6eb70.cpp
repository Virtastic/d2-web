// 1.13c D2Client+0x6EB70 probe: r[0] = EAX (in), r[1] = EBX (out), r[2] = star offset the 0xBF02 hook returns (-1 = none); blobs in, out, lg = r[14], r[16], r[18].
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"
struct D2c6EB70Env { uint32_t (*findChar)(uint32_t, uint32_t); void (*copyBounded)(uint32_t, uint32_t, uint32_t); };
extern "C" void d2c_port_6eb70(uint32_t, uint32_t, const D2c6EB70Env*);
namespace {
HookEnv* g; uint32_t gIn, gX;
uint32_t findChar(uint32_t s, uint32_t c) { g->h(0xbf02, 0, 0, 0, 0, 0, s, c); return gX == 0xffffffffu ? 0 : gIn + gX; }
void copyBounded(uint32_t d, uint32_t s, uint32_t m) { g->h(0xbe96, 0, 0, 0, 0, 0, d, s, m); }
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_6eb70(uint32_t* r) {
    HookEnv he((uint32_t*)(uintptr_t)r[18], nullptr); g = &he; gIn = r[0]; gX = r[2];
    D2c6EB70Env env = { findChar, copyBounded };
    d2c_port_6eb70(r[0], r[1], &env);
    r[4] = 0;
}
