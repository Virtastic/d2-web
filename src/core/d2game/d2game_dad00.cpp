// 1.13c D2Game+0xDAD00, injectable callees. EAX = amount, ECX = ctx (+0xC unit, +0x18 stat record whose +0x48 is the
// absorbed total), EBX = skill row (+0x10, +0x14 stat ids; -1 at +0x10 disables). Plain ret, returns the remaining
// amount. 0x14D0 takes EAX, ECX, EDX in registers (a * c / b); 0xA42A is (unit, stat, layer), three stack words.
#include <cstdint>
struct GDAD00Env {
    int32_t (*a42a)(uint32_t unit, int32_t stat, uint32_t layer);
    int32_t (*f14d0)(int32_t eax, int32_t ecx, int32_t edx);
};
// 1.13c D2Game+0xDAD00
int32_t g_dad00(int32_t amount, uint32_t ctx, uint32_t row, const GDAD00Env& env)
{
    const int32_t s10 = *(const int32_t*)(uintptr_t)(row + 0x10);
    if (s10 == -1) return amount;
    const uint32_t unit = *(const uint32_t*)(uintptr_t)(ctx + 0xC);
    int32_t* acc = (int32_t*)(uintptr_t)(*(const uint32_t*)(uintptr_t)(ctx + 0x18) + 0x48);
    int32_t a = env.a42a(unit, s10, 0);
    if (a >= 0x28) a = 0x28;
    if (a > 0) {
        const int32_t t = env.f14d0(a, 100, amount);
        *acc = (int32_t)((uint32_t)*acc + (uint32_t)t);
        amount -= t;
    }
    int32_t b = (int32_t)((uint32_t)env.a42a(unit, *(const int32_t*)(uintptr_t)(row + 0x14), 0) << 8);
    if (b > 0) {
        if (b >= amount) b = amount;
        *acc = (int32_t)((uint32_t)*acc + (uint32_t)b);
        amount -= b;
    }
    return amount;
}
