// 1.13c D2Client+0x45e50 probe; drives the client-tree body d2c_45e50 (src/core/d2client/d2client_batch_14fd0.cpp) through its env struct.
// r[0] = the id (the id scan has no effect). Blob g (r[14], 4 bytes) is the dword at 0x11C284, blob b
// (r[16], 4 bytes) the byte at 0x11C29C, blob lg (r[18], 256 bytes) the hook log. The module strings the callee takes are module
// addresses: the host tags them with their rva (0xB2000000 | addr - 0x6FAB0000) before logging.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "hookenv.h"
struct E45E50 { int16_t* tab; int16_t* tabEnd; int32_t flag284; int32_t flag29c;
  void (*f72790)(int32_t, int32_t, int32_t); int32_t (*ord423)(const char*, const char*, int32_t, int32_t*); };
extern "C" void d2c_45e50(E45E50* e, int32_t v);
namespace {
HookEnv* g_env;
void f72790(int32_t a, int32_t b, int32_t c) { g_env->h(0xC2790, 0, (uint32_t)a, (uint32_t)b, 0, 0, (uint32_t)c, 0, 0, 0); }
// the callee's two module strings are logged as module addresses (0x6FB83588 "Diablo II", 0x6FB8566C "PopupHireling")
uint32_t tagStr(const char* s) {
    const uint32_t a = std::strcmp(s, "Diablo II") == 0 ? 0x6FB83588u : std::strcmp(s, "PopupHireling") == 0 ? 0x6FB8566Cu : (uint32_t)(uintptr_t)s;
    return a >= 0x6FAB0000u && a < 0x6FB90000u ? (0xB2000000u | (a - 0x6FAB0000u)) : a;
}
int32_t ord423(const char* a, const char* b, int32_t c, int32_t* out) {
    return (int32_t)g_env->h(0xBE8A, 0, 0, 0, 0, 0, tagStr(a), tagStr(b), (uint32_t)c, (uint32_t)(uintptr_t)out);
}
const uint32_t kNone[] = { 0, 0 };
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_45e50(uint32_t* r) {
    uint32_t* g = (uint32_t*)(uintptr_t)r[14];
    uint8_t* b = (uint8_t*)(uintptr_t)r[16];
    uint32_t* lg = (uint32_t*)(uintptr_t)r[18];
    HookEnv env(lg, kNone);
    env.cap = 6;
    g_env = &env;
    int16_t scan[8] = {};
    E45E50 e{ scan, scan, (int32_t)*g, (int32_t)*b, f72790, ord423 };
    d2c_45e50(&e, (int32_t)r[0]);
    *g = (uint32_t)e.flag284;
    *b = (uint8_t)e.flag29c;
    r[4] = 0;
}
