// 1.13c D2Client+0x42C10 probe: regs eax ecx; r[0]=lg (eax), r[1]=W (ecx, the scripted D35A width),
// r[2]=id, r[3]=x. D1EC returns 0x4444; D35A returns W. The Unicode ctor and strcpy are iat pins (not logged).
#include <emscripten.h>
#include <cstdint>
#include "../d2client/client_host.h"
#include "hookenv.h"
extern "C" int32_t d2c_42c10(D2cHost, uint32_t, int32_t);
namespace {
HookEnv* g_env;
uint32_t host(D2cCall* c) {
    if (c->rva == 0xCEB00 || c->rva == 0xCEAB8) return 0;
    return g_env->h(c->rva, c->eax, c->ecx, c->edx, c->ebx, c->esi, c->s[0], c->s[1], c->s[2], c->s[3]);
}
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_42c10(uint32_t* r) {
    const uint32_t sc[] = { 0xD1EC, 0x4444, 0xD35A, r[1], 0, 0 };
    HookEnv env((uint32_t*)(uintptr_t)r[0], sc); g_env = &env;
    r[4] = (uint32_t)d2c_42c10(host, r[2], (int32_t)r[3]);
}
