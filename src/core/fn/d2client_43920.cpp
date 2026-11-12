// 1.13c D2Client+0x43920 probe: r[0]=st (12 bytes), r[1]=lg (1024).
// st dwords: 0 g39c (11C39C), 1 the opcode the D210 hookw writes into the buffer, 2 loop count n.
// D210 returns 0x10.. (n-1 values) then -1; D24C returns 0x20.. (n-1 values) then -1 (the last value repeats).
// Ordinal_10020/10008 (D210/D24C) are 2-stack-word callees; the buffer is a stack local (0xB3000000 in the log).
#include <emscripten.h>
#include <cstdint>
#include "../d2client/client_host.h"
#include "hookenv.h"
extern "C" void d2c_43920_run(D2cHost, uint8_t*, int32_t*);
namespace {
HookEnv* g_env;
uint32_t host(D2cCall* c) {
    return g_env->h(c->rva, c->eax, c->ecx, c->edx, c->ebx, c->esi, c->s[0], c->s[1], c->s[2], c->s[3]);
}
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_43920(uint32_t* r) {
    uint32_t* st = (uint32_t*)(uintptr_t)r[0];
    const uint32_t n = st[2];
    uint32_t sc[128] = {}; uint32_t k = 0;
    for (uint32_t i = 0; i + 1 < n; ++i) { sc[k++] = 0xD210; sc[k++] = 0x10 + i; }
    sc[k++] = 0xD210; sc[k++] = 0xFFFFFFFFu;
    for (uint32_t i = 0; i + 1 < n; ++i) { sc[k++] = 0xD24C; sc[k++] = 0x20 + i; }
    sc[k++] = 0xD24C; sc[k++] = 0xFFFFFFFFu;
    sc[k++] = (1u << 24) | 0xD210; sc[k++] = st[1];   // hookw RVA 0 0x0: the opcode dword written at s0 (the buffer)
    sc[k++] = 0; sc[k++] = 0;
    HookEnv env((uint32_t*)(uintptr_t)r[1], sc); g_env = &env;
    uint8_t buf[512] = {};
    d2c_43920_run(host, buf, (int32_t*)st);
    r[4] = 0;
}
