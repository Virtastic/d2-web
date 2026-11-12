// 1.13c D2Client+0x42C70 probe: r[0]=st (28 bytes), r[1]=pl (player struct, gptr 11B980), r[2]=lg, r[3]=ttl (title string).
// st dwords: 0 cc390 (byte), 1 g991c (byte), 2 g9854, 3 g8bc48, 4 width1 (D35A), 5 width2 (D35A), 6 titleNull (D414 returns 0 when nonzero, else ttl).
// Unicode ctor and toUnicode are iat pins (not logged).
#include <emscripten.h>
#include <cstdint>
#include "../d2client/client_host.h"
#include "hookenv.h"
extern "C" void d2c_42c70(D2cHost, const uint8_t*, uint32_t, uint32_t, uint32_t, int32_t, const char*);
namespace {
HookEnv* g_env;
uint32_t host(D2cCall* c) {
    if (c->rva == 0xCEB00 || c->rva == 0xCEAC4) return 0;
    return g_env->h(c->rva, c->eax, c->ecx, c->edx, c->ebx, c->esi, c->s[0], c->s[1], c->s[2], c->s[3]);
}
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_42c70(uint32_t* r) {
    const uint32_t* st = (const uint32_t*)(uintptr_t)r[0];
    const uint8_t* pl = (const uint8_t*)(uintptr_t)r[1];
    const uint32_t ttl = r[3];
    const uint32_t sc[] = { 0xD35A, st[4], 0xD35A, st[5], 0xD414, st[6] ? 0u : ttl, 0, 0 };
    HookEnv env((uint32_t*)(uintptr_t)r[2], sc); g_env = &env;
    d2c_42c70(host, pl, st[0] & 0xFFu, st[1] & 0xFFu, st[2], (int32_t)st[3], (const char*)(uintptr_t)ttl);
    r[4] = 0;
}
