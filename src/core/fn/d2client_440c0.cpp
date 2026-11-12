// 1.13c D2Client+0x440C0 probe: r[0]=st (24), r[1]=lg (512), r[2]=n0, r[3]=n1, r[5]=n2 (the fifth arg rides past the result slot r[4]).
// st dwords: 0 g3b8 (11C3B8), 1 g2e0 (11C2E0), 2 head selector (1: CCC4 returns n0, 0: none), 3..5 scripted C1DE kinds (last repeats).
// The descriptor patches the +0x7C links per tuple (n0 -> n1, n1 -> n2); the probe leaves them as given. Node pointers are logged as 0xB1<blob><offset>; blobs are n0=2, n1=3, n2=4.
#include <emscripten.h>
#include <cstdint>
#include "../d2client/client_host.h"
#include "hookenv.h"
extern "C" void d2c_440c0(D2cHost, uint32_t, uint32_t);
namespace {
HookEnv* g_env; uint32_t g_n[3];
uint32_t norm(uint32_t p) {
    for (uint32_t k = 0; k < 3; ++k) if (p >= g_n[k] && p < g_n[k] + 128) return 0xB1000000u | ((2 + k) << 16) | (p - g_n[k]);
    return p;
}
uint32_t host(D2cCall* c) {
    switch (c->rva) {
    case 0x18370: case 0xC1DE: case 0x53550: c->s[0] = norm(c->s[0]); break;
    default: break;
    }
    return g_env->h(c->rva, c->eax, c->ecx, c->edx, c->ebx, c->esi, c->s[0], c->s[1], c->s[2], c->s[3]);
}
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_440c0(uint32_t* r) {
    const uint32_t* st = (const uint32_t*)(uintptr_t)r[0];
    g_n[0] = r[2]; g_n[1] = r[3]; g_n[2] = r[5];
    const uint32_t sc[] = { 0xCCC4, st[2] == 1 ? r[2] : 0u, 0xC1DE, st[3], 0xC1DE, st[4], 0xC1DE, st[5], 0, 0 };
    HookEnv env((uint32_t*)(uintptr_t)r[1], sc); g_env = &env;
    d2c_440c0(host, st[0], st[1]);
    r[4] = 0;
}
