// 1.13c D2Game+0xC3260 -- should the act 2 quest 0xB reward dialog be offered; injectable callees.
// EAX = game, result in AL (1 = offered). Callees 0xC2460 and 0xC1A00 take EAX = quest node id (node+4), result in AL; plain ret.
#include <cstdint>
struct GC3260Env { uint8_t (*c2460)(uint32_t eax); uint8_t (*c1a00)(uint32_t eax); };
static inline uint32_t g_r32(uint32_t p, uint32_t o) { return *(const uint32_t*)(uintptr_t)(p + o); }
static inline uint8_t g_r8(uint32_t p, uint32_t o) { return *(const uint8_t*)(uintptr_t)(p + o); }
// 1.13c D2Game+0xC3260
uint8_t g_c3260(uint32_t game, const GC3260Env& env)
{
    const uint32_t list = g_r32(game, 0x10f4);
    if (!list) return 1;
    uint32_t n = g_r32(list, 0);
    if (!n) return 1;
    while (g_r32(n, 0) != 0xb) { n = g_r32(n, 0xf4); if (!n) return 1; }
    if (g_r8(g_r32(n, 0x18), 0xd) == 0) return 1;
    if (env.c2460(g_r32(n, 4)) != 0) return 1;
    if (env.c1a00(g_r32(n, 4)) != 0) return 1;
    if (g_r8(n, 9) != 1) return 1;
    if (g_r8(n, 0xc) >= 2) return 1;
    return 0;
}
