// 1.13c D2Client+0x7BF60 probe: r[0] = EDI (block), r[1..3] = the widths the 0xD35A hook returns in call order; blobs o, n, s, lg = r[14], r[16], r[18], r[20].
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"
struct D2c7BF60Env { int32_t (*textWidth)(uint32_t); };
extern "C" void d2c_port_7bf60(uint8_t*, const D2c7BF60Env*);
namespace {
HookEnv* g;
int32_t textWidth(uint32_t t) { return (int32_t)g->h(0xd35a, 0, t); }
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_7bf60(uint32_t* r) {
    const uint32_t sc[8] = { 0xd35a, r[1], 0xd35a, r[2], 0xd35a, r[3], 0, 0 };
    HookEnv he((uint32_t*)(uintptr_t)r[20], sc); g = &he;
    D2c7BF60Env env = { textWidth };
    d2c_port_7bf60((uint8_t*)(uintptr_t)r[0], &env);
    r[4] = 0;
}
