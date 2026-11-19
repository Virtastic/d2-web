// 1.13c D2Client+0x45cc0 probe; drives the client-tree body d2c_45cc0 (src/core/d2client/d2client_batch_14fd0.cpp).
// r[0] = n (EAX), r[1] = tbl (blob t, r[14], 60 bytes: three 0x14-byte entries {key, a, b, want,
// callback}), r[2] = key, r[3] = p. Blob lg (r[16], 512 bytes) is the hook log. A table callback is a module address (0x6FAF5D90):
// the body calls a function pointer, so the probe's table copy carries the probe's own callback, which records the module rva.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "hookenv.h"
extern "C" uint32_t d2c_45cc0(int32_t* tbl, int32_t count, int32_t key, int32_t p2, int32_t (*o10174)(int32_t, int32_t, int32_t));
namespace {
HookEnv* g_env;
int32_t o10174(int32_t p, int32_t a, int32_t b) { return (int32_t)g_env->h(0xC58C, 0, 0, 0, 0, 0, (uint32_t)p, (uint32_t)a, (uint32_t)b, 0); }
uint32_t cb45d90() { return g_env->h(0x45D90); }
const uint32_t kNone[] = { 0, 0 };
// per p: the C58C queue (the entry's hit result) and the callback queue (its EAX)
const uint32_t kP201[] = { 0xC58C, 0, 0xC58C, 1, 0x45D90, 7, 0x45D90, 9, 0, 0 };
const uint32_t kP202[] = { 0xC58C, 1, 0xC58C, 0, 0, 0 };
const uint32_t kP204[] = { 0xC58C, 1, 0x45D90, 0, 0, 0 };
const uint32_t* pick(uint32_t p) {
    switch (p) {
    case 0x201: return kP201;
    case 0x202: return kP202;
    case 0x204: return kP204;
    default: return kNone;
    }
}
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_45cc0(uint32_t* r) {
    const uint8_t* tbl = (const uint8_t*)(uintptr_t)r[1];
    uint32_t* lg = (uint32_t*)(uintptr_t)r[16];
    HookEnv env(lg, pick(r[3]));
    env.cap = 12;
    g_env = &env;
    const int32_t n = (int32_t)r[0];
    int32_t ent[64 * 5] = {};
    const int32_t m = n < 0 ? 0 : n > 64 ? 64 : n;
    std::memcpy(ent, tbl, (size_t)m * 0x14);
    for (int32_t i = 0; i < m; ++i)
        if (ent[i * 5 + 4] == (int32_t)0x6FAF5D90) ent[i * 5 + 4] = (int32_t)(intptr_t)&cb45d90;
    r[4] = d2c_45cc0(ent, m, (int32_t)r[2], (int32_t)r[3], o10174);
}
