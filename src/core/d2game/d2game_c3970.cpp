// 1.13c D2Game+0xC3970: 0x340C0(EBX = record, ESI = game), then tail call 0x33830(EAX = inner key or -1, ECX = *(game+0x18) + 0x14).
#include <cstdint>
struct GC3970Env { void (*f340c0)(uint32_t ebx, uint32_t esi); uint32_t (*f33830)(uint32_t eax, uint32_t ecx); };
uint32_t g_c3970(uint32_t game, uint32_t rec, const GC3970Env& env) {
    env.f340c0(rec, game);
    uint32_t inner = *(const uint32_t*)(uintptr_t)(rec + 0xC);
    uint32_t sub = *(const uint32_t*)(uintptr_t)(game + 0x18);
    uint32_t key = inner ? *(const uint32_t*)(uintptr_t)(inner + 0xC) : 0xFFFFFFFFu;
    return env.f33830(key, sub + 0x14);
}
