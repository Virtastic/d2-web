// 1.13c D2Game+0xF1490, injectable callees. Registers: ECX = pGame (passed on to 0xE07A0), ESI = unit, EBX = out id (may be 0);
// stack words: 0 unused, 1 = skill id for 0x9E810, 2 = out level pointer; ret 0xC. 0x9E810 is EAX = the 10392 result masked
// to 0 when it is the unit, stack word 0 = unit, word 1 = skill id, ret 8. 0xACD0 (D2Common 11129): 2 words.
#include <cstdint>
struct GF1490Env {
    void (*e07a0)(uint32_t eax, uint32_t ecx);
    uint32_t (*o10392)(uint32_t a);
    uint32_t (*f9e810)(uint32_t eax, uint32_t s0, uint32_t s1);
    uint32_t (*o11129)(uint32_t s0, uint32_t s1);
};
// 1.13c D2Game+0xF1490
uint32_t g_f1490(uint32_t ecx, uint32_t esi, uint32_t ebx, uint32_t w0, uint32_t w1, uint32_t out2, const GF1490Env& env)
{
    (void)w0;
    env.e07a0(esi, ecx);
    uint32_t a = env.o10392(*(uint32_t*)(uintptr_t)(esi + 0x2C));
    if (a == esi) a = 0;
    const uint32_t p = env.f9e810(a, esi, w1);
    if (p == 0) return 0;
    env.o11129(esi, p);
    if (ebx != 0) {
        *(uint32_t*)(uintptr_t)ebx = *(uint32_t*)(uintptr_t)p;
        *(uint32_t*)(uintptr_t)out2 = *(uint32_t*)(uintptr_t)(p + 0xC);
    }
    return 1;
}
