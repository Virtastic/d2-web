// 1.13c D2Client+0x781D0 probe: r[0] = EDX, r[1] = EBX (text blob), r[2] = the width the 0xD35A hook returns;
// blobs g (the four globals), t, lg = r[14], r[16], r[18].
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"
struct D2c781D0Env { int32_t (*textWidth)(uint32_t); void (*drawText)(uint32_t, int32_t, int32_t, uint32_t, uint32_t); };
extern "C" void d2c_port_781d0(int32_t, uint32_t, int32_t, int32_t, int32_t, int32_t, const D2c781D0Env*);
namespace {
HookEnv* g;
int32_t textWidth(uint32_t t) { return (int32_t)g->h(0xd35a, 0, t); }
void drawText(uint32_t t, int32_t x, int32_t y, uint32_t z0, uint32_t z1) { g->h(0xd37e, 0, t, (uint32_t)x, 0, 0, (uint32_t)y, z0, z1); }
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_781d0(uint32_t* r) {
    const uint32_t sc[4] = { 0xd35a, r[2], 0, 0 };
    HookEnv he((uint32_t*)(uintptr_t)r[18], sc); g = &he;
    const int32_t* gl = (const int32_t*)(uintptr_t)r[14];
    D2c781D0Env env = { textWidth, drawText };
    d2c_port_781d0((int32_t)r[0], r[1], gl[0], gl[1], gl[2], gl[3], &env);
    r[4] = 0;
}
