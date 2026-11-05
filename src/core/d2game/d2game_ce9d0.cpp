// 1.13c D2Game+0xce9d0, injectable callees. Registers: ECX = game, EDX = unit; stack word 0 = packet pointer, word 1 = length (ret 8).
// 0xCD630 takes EAX = packet, ECX = length, EBX = &word1 and stack game, unit, &word0 (ret 0xC); it rewrites both argument slots.
// Thunk 0xA3AC (1 word) is called twice; 0xA3E2 (unit, 0x148, 0) and 0xA472 (unit, 0x148, value + 1, 0) bump stat 0x148; 0xCE620 takes
// EAX = the second skill result, ECX = unit and stack game, word0, word1, 1 (ret 0x10).
#include <cstdint>
struct GCe9d0Env {
    uint32_t (*cd630)(uint32_t eax, uint32_t ecx, uint32_t* ebx, uint32_t s0, uint32_t s1, uint32_t* s2);
    uint32_t (*skill)(uint32_t unit);   // import thunk 0xA3AC (1 word, ret 4)
    uint32_t (*a3e2)(uint32_t unit, uint32_t id, uint32_t z);
    void (*a472)(uint32_t unit, uint32_t id, uint32_t v, uint32_t z);
    void (*ce620)(uint32_t eax, uint32_t ecx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3);
};
// 1.13c D2Game+0xce9d0
uint32_t g_ce9d0(uint32_t ecx, uint32_t edx, uint32_t pkt, uint32_t len, const GCe9d0Env& env)
{
    uint32_t w0 = pkt, w1 = len;
    uint32_t r = env.cd630(w0, w1, &w1, ecx, edx, &w0);
    if (r != 0) return r;
    if (env.skill(edx) == 0) return 3;
    env.a472(edx, 0x148, env.a3e2(edx, 0x148, 0) + 1, 0);
    uint32_t sk = env.skill(edx);
    env.ce620(sk, edx, ecx, w0, w1, 1);
    return 0;
}
