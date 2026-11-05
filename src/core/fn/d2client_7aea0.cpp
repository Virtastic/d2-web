// 1.13c D2Client+0x7AEA0 probe: blobs st (head global), obj (object global word at +0), vt, n1, n2, n3, lg = r[14], r[16], ..., r[26].
// The slot-0x30 stub records words 0-5 (REGS h: eax = w4, ecx = w5, s0..s3 = w0..w3).
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"
struct D2c7AEA0Env { void (*slot30)(uint32_t, const uint32_t*); };
extern "C" void d2c_port_7aea0(const uint32_t*, const uint32_t*, const D2c7AEA0Env*);
namespace {
HookEnv* g;
void slot30(uint32_t, const uint32_t* a) { g->h(0xF20030, a[4], a[5], 0, 0, 0, a[0], a[1], a[2], a[3]); }
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_7aea0(uint32_t* r) {
    HookEnv he((uint32_t*)(uintptr_t)r[26], nullptr); g = &he;
    D2c7AEA0Env env = { slot30 };
    const uint32_t objg = r[16];
    d2c_port_7aea0((const uint32_t*)(uintptr_t)r[14], &objg, &env);
    r[4] = 0;
}
