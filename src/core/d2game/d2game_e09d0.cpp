// 1.13c D2Game+0xE09D0, injectable callee. EAX = unit (null is a fatal exit, not reachable here), stack: p; ret 4.
#include <cstdint>
struct GE09D0Env { uint32_t (*e03a0)(uint32_t ecx, uint32_t edx, uint32_t eax); };
// 1.13c D2Game+0xE09D0
uint32_t g_e09d0(uint32_t unit, uint32_t p, const GE09D0Env& env)
{
    auto rd = [](uint32_t a) { return *(const uint32_t*)(uintptr_t)a; };
    if (rd(unit + 0xC8) & 0x20000)
        return env.e03a0(p, rd(unit + 0xA0), rd(unit + 0x9C));
    return 0;
}
