// 1.13c D2Game+0xCE040: ECX = game, EDX = unit, w0 = packet, w1 = length. 0xCBF50 validates (writes 16-bit w0/w1 back); on success 0x78550(game, unit, 0, 3, w0 & 0xFFFF, w1 & 0xFFFF, 0), return 0.
#include <cstdint>
struct GCe040Env {
    uint32_t (*cbf50)(uint32_t eax, uint32_t esi, uint32_t s0, uint32_t s1, uint32_t* s2, uint32_t* s3);
    void (*f78550)(uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4);
};
uint32_t g_ce040(uint32_t ecx, uint32_t edx, uint32_t pkt, uint32_t len, const GCe040Env& env) {
    uint32_t w0 = pkt, w1 = len;
    uint32_t r = env.cbf50(w0, edx, ecx, w1, &w0, &w1);
    if (r) return r;
    env.f78550(ecx, edx, 0, 3, w0 & 0xFFFF, w1 & 0xFFFF, 0);
    return 0;
}
