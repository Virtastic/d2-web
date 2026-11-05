// 1.13c D2Game+0xDCD80, injectable callees. EAX = stat id, EDI = unit, stack words a0..a4 (ret 0x14). 0xA42A is
// (unit, stat, 0), three stack words. 0xDBED0 takes EAX = a4, ECX = a2, EDX = esi and stack (unit, ebx, a3), ret 0xC.
// a1 == -1 skips the second stat read. Under 8 (in 1/256 units) the result is a4 clamped at 0 from below.
#include <cstdint>
struct GDCD80Env {
    int32_t  (*a42a)(uint32_t unit, int32_t stat, uint32_t layer);
    uint32_t (*dbed0)(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2);
};
// 1.13c D2Game+0xDCD80
uint32_t g_dcd80(int32_t eax, uint32_t unit, int32_t a0, int32_t a1, int32_t a2, int32_t a3, int32_t a4, const GDCD80Env& env)
{
    const int32_t esi = (int32_t)((uint32_t)env.a42a(unit, eax, 0) << 8);
    if (esi < 8) return (uint32_t)(a4 < 0 ? 0 : a4);
    const int32_t ebx = (int32_t)((uint32_t)env.a42a(unit, a0, 0) << 8);
    if (a1 != -1) {
        const int32_t x = env.a42a(unit, a1, 0);
        a2 += x;
        a3 += x;
    }
    return env.dbed0((uint32_t)a4, (uint32_t)a2, (uint32_t)esi, unit, (uint32_t)ebx, (uint32_t)a3);
}
