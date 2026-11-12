// 1.13c D2Client+0x42880 probe: r[0]=st (36 bytes), r[1]=lg (hooklog).
// st dwords: 0 g3cc, 1 g898, 2 g890, 3 g88c, 4 msgSlot (in/out), 5 GetSystemMetrics, 6 RegisterWindowMessageA, 7 FindWindowA, 8 SendMessageA answers.
// The four user32 imports are answered from st[5..8] without a log record (iat pins); D0B4 returns 1111,2222,3333,4444 (last repeats).
#include <emscripten.h>
#include <cstdint>
#include "../d2client/client_host.h"
#include "hookenv.h"
extern "C" void d2c_42880(D2cHost, int32_t, int32_t, int32_t, int32_t, int32_t*);
namespace {
HookEnv* g_env; int32_t* g_st;
uint32_t host(D2cCall* c) {
    switch (c->rva) {
    case 0xCF0E8: return (uint32_t)g_st[5];
    case 0xCF0E0: return (uint32_t)g_st[6];
    case 0xCF0EC: return (uint32_t)g_st[7];
    case 0xCF118: return (uint32_t)g_st[8];
    default: return g_env->h(c->rva, c->eax, c->ecx, c->edx, c->ebx, c->esi, c->s[0], c->s[1], c->s[2], c->s[3]);
    }
}
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_42880(uint32_t* r) {
    int32_t* st = (int32_t*)(uintptr_t)r[0]; g_st = st;
    const uint32_t sc[] = { 0xD0B4, 0x1111, 0xD0B4, 0x2222, 0xD0B4, 0x3333, 0xD0B4, 0x4444, 0, 0 };
    HookEnv env((uint32_t*)(uintptr_t)r[1], sc); g_env = &env;
    d2c_42880(host, st[0], st[1], st[2], st[3], &st[4]);
    r[4] = 0;
}
