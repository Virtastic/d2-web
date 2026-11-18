// 1.13c D2Client+0x43920 probe: r[0]=st (12 bytes), r[1]=lg (1024).
// st dwords: 0 g39c (11C39C), 1 the opcode the D210 hookw writes into the buffer, 2 loop count n.
// D210 returns 0x10.. (n-1 values) then -1; D24C returns 0x20.. (n-1 values) then -1 (the last value repeats).
// Ordinal_10020/10008 (D210/D24C) are 2-stack-word callees; the buffer is a stack local (0xB3000000 in the log).
// Runs the game body d2c_43920 (the one d2net_local_drain calls) with every callee bound to the capture's hooks.
#include <emscripten.h>
#include <cstdint>
#include "../d2client/d2client_43920.h"
#include "hookenv.h"
namespace {
HookEnv* g_env;
uint32_t P(const void* p) { return (uint32_t)(uintptr_t)p; }
uint32_t takeSystem(uint8_t* b, uint32_t len) { return g_env->h(0xD210, 0, 0, 0, 0, 0, P(b), len); }
uint32_t takeGame(uint8_t* b, uint32_t len) { return g_env->h(0xD24C, 0, 0, 0, 0, 0, P(b), len); }
void acd70(const uint8_t* p) { g_env->h(0xACD70, 0, 0, 0, 0, P(p)); }
void acbf0(const uint8_t* p) { g_env->h(0xACBF0, P(p)); }
void ace20(const uint8_t* p, int32_t n) { g_env->h(0xACE20, P(p), 0, 0, 0, 0, (uint32_t)n); }
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
    const D2c43920Env e = { takeSystem, takeGame, acd70, acbf0, ace20, (int32_t*)st };
    d2c_43920(&e);
    r[4] = 0;
}
