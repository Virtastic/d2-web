// 1.13c D2Game+0xF1420, injectable callees. Registers: EAX = pGame, ESI = unit, EBX = out id (may be 0); stack word 0 = out level
// pointer; ret 4. 0xE07A0 is EAX = unit, ECX = pGame, plain ret. 0xE09D0 is EAX = unit, stack word 0 = pGame, ret 4.
// Thunks: 0xA598 (D2Common 10392, 1 word) and 0xACD0 (D2Common 11129, 2 words). Always returns 1.
#include <cstdint>
struct GF1420Env {
    void (*e07a0)(uint32_t eax, uint32_t ecx);
    uint32_t (*o10392)(uint32_t a);
    uint32_t (*e09d0)(uint32_t eax, uint32_t s0);
    uint32_t (*o11129)(uint32_t s0, uint32_t s1);
};
// 1.13c D2Game+0xF1420
uint32_t g_f1420(uint32_t eax, uint32_t esi, uint32_t ebx, uint32_t out2, const GF1420Env& env)
{
    env.e07a0(esi, eax);
    const uint32_t r = env.o10392(*(uint32_t*)(uintptr_t)(esi + 0x2C));
    if (r != esi && r != 0) return 1;
    const uint32_t p = env.e09d0(esi, eax);
    env.o11129(esi, p);
    if (ebx == 0) return 1;
    *(uint32_t*)(uintptr_t)ebx = p ? *(uint32_t*)(uintptr_t)p : 6;
    *(uint32_t*)(uintptr_t)out2 = p ? *(uint32_t*)(uintptr_t)(p + 0xC) : 0xFFFFFFFFu;
    return 1;
}
