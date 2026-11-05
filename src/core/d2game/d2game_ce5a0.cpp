// 1.13c D2Game+0xCE5A0, injectable callees. Registers: ECX = game, EDX = unit; stack word 0 = packet pointer, word 1 = length (ret 8).
// 0xCBF50 takes EAX = packet, ESI = unit and stack game, length, &word0, &word1 (ret 0x10); it rewrites both argument slots.
// Thunk 0xA3AC (1 word) is called twice; 0xA3E2 / 0xA472 bump stat 0x148; 0xCDF30 takes EAX = the second skill result, ECX = unit and
// stack game, word0 & 0xFFFF, word1 & 0xFFFF (ret 0xC).
#include <cstdint>
struct GCe5a0Env {
    uint32_t (*cbf50)(uint32_t eax, uint32_t esi, uint32_t s0, uint32_t s1, uint32_t* s2, uint32_t* s3);
    uint32_t (*skill)(uint32_t unit);   // import thunk 0xA3AC (1 word, ret 4)
    uint32_t (*a3e2)(uint32_t unit, uint32_t id, uint32_t z);
    void (*a472)(uint32_t unit, uint32_t id, uint32_t v, uint32_t z);
    void (*cdf30)(uint32_t eax, uint32_t ecx, uint32_t s0, uint32_t s1, uint32_t s2);
};
// 1.13c D2Game+0xCE5A0
uint32_t g_ce5a0(uint32_t ecx, uint32_t edx, uint32_t pkt, uint32_t len, const GCe5a0Env& env)
{
    uint32_t w0 = pkt, w1 = len;
    uint32_t r = env.cbf50(w0, edx, ecx, w1, &w0, &w1);
    if (r != 0) return r;
    if (env.skill(edx) == 0) return 3;
    env.a472(edx, 0x148, env.a3e2(edx, 0x148, 0) + 1, 0);
    uint32_t sk = env.skill(edx);
    env.cdf30(sk, edx, ecx, w0 & 0xFFFF, w1 & 0xFFFF);
    return 0;
}
