// 1.13c D2Game+0xCE090: ECX = game, EDX = unit, w0 = packet, w1 = length. 0xCD630 validates (writes w0/w1 back); on success 0x78430(game, 0, 2, w0, w1, 0), return 0.
#include <cstdint>
struct GCd630Env {
    uint32_t (*cd630)(uint32_t eax, uint32_t ecx, uint32_t* ebx, uint32_t s0, uint32_t s1, uint32_t* s2);
    void (*f78430)(uint32_t eax, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4);
};
uint32_t g_ce090(uint32_t ecx, uint32_t edx, uint32_t pkt, uint32_t len, const GCd630Env& env) {
    uint32_t w0 = pkt, w1 = len;
    uint32_t r = env.cd630(w0, w1, &w1, ecx, edx, &w0);
    if (r) return r;
    env.f78430(ecx, 0, 2, w0, w1, 0);
    return 0;
}
