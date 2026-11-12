// 1.13c D2Client+0x431B0 probe: regs eax ecx; r[0]=st (8 bytes: 0 stop, 1 active), r[1]=lg (256), r[2]=thread arg (unused).
// Only the stop path is reachable (every tuple has st[0]=1); a tuple with st[0]=0 would loop forever in retail, so the probe does not enter the port for it.
// The critical-section, Sleep and OutputDebugStringA imports are iat pins (not logged).
#include <emscripten.h>
#include <cstdint>
#include "../d2client/client_host.h"
#include "hookenv.h"
extern "C" uint32_t d2c_431b0(D2cHost, volatile uint32_t*, volatile uint32_t*, const uint8_t* const*, const int32_t*);
namespace {
HookEnv* g_env;
uint32_t host(D2cCall* c) {
    switch (c->rva) {
    case 0xCEFBC: case 0xCEFB8: case 0xCEFC0: case 0xCEF98: case 0xCEFA0: return 0;
    default: return g_env->h(c->rva, c->eax, c->ecx, c->edx, c->ebx, c->esi, c->s[0], c->s[1], c->s[2], c->s[3]);
    }
}
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_431b0(uint32_t* r) {
    volatile uint32_t* st = (volatile uint32_t*)(uintptr_t)r[0];
    static const uint32_t none[2] = { 0, 0 };   // no scripted returns: D060/D066 return 0
    HookEnv env((uint32_t*)(uintptr_t)r[1], none); g_env = &env;
    if (st[0] == 0) { r[4] = 0; return; }
    static uint32_t dummyPlayer[0x280 / 4];
    const uint8_t* players[1] = { (const uint8_t*)dummyPlayer };
    int32_t lim = 0;
    r[4] = d2c_431b0(host, st, st + 1, players, &lim);
}
