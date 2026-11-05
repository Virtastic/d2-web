// 1.13c D2Client+0x7C730 probe: blobs g (both globals), obj, vt, lg = r[14], r[16], r[18], r[20].
// The virtual slot is blob vt's stubptr recorder (rva 0xF00000 + 2 * 0x10000).
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"
struct D2c7C730Env { void (*closeWindow)(); void (*destroyObject)(uint32_t, uint32_t); };
extern "C" void d2c_port_7c730(uint32_t*, uint32_t*, const D2c7C730Env*);
namespace {
HookEnv* g;
void closeWindow() { g->h(0xa3d20); }
void destroyObject(uint32_t obj, uint32_t flag) { g->h(0xF20000, 0, obj, 0, 0, 0, flag); }
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_7c730(uint32_t* r) {
    HookEnv he((uint32_t*)(uintptr_t)r[20], nullptr); g = &he;
    D2c7C730Env env = { closeWindow, destroyObject };
    uint32_t* gl = (uint32_t*)(uintptr_t)r[14];
    d2c_port_7c730(gl, gl + 1, &env);
    r[4] = 0;
}
