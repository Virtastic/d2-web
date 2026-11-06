// 1.13c D2Game+0xF10F0, injectable callees. ECX = a, EDX = unit (may be null), 3 stack words unused (ret 0xC).
// EAX handed to 0x9C160 is 0x9BEB0's result when the unit gate passes, else 0xFFFF.
#include <cstdint>
struct GF10F0Env {
    uint32_t (*f9beb0)(uint32_t eax);               // register eax = unit; returns eax
    void     (*f9c160)(uint32_t eax, uint32_t cb);  // eax = the value above, stack word = the module callback constant
    void     (*f9c840)(uint32_t eax, uint32_t a);   // eax = unit, stack word a
};
// 1.13c D2Game+0xF10F0
void g_f10f0(uint32_t ecx, uint32_t edx, uint32_t cb, const GF10F0Env& env)
{
    uint32_t eax;
    if (edx != 0 && *(const uint32_t*)(uintptr_t)(edx + 0x80) != 0 && *(const uint32_t*)(uintptr_t)edx == 0)
        eax = env.f9beb0(edx);
    else
        eax = 0xFFFF;
    env.f9c160(eax, cb);
    env.f9c840(edx, ecx);
}
