// 1.13c D2Game+0xD4550 -- guard for 0xD40F0, injectable callee.
// EAX = unit (copied to EDI before the call), EBX passes through untouched. Plain ret.
#include <cstdint>
struct GD4550Env { void (*d40f0)(uint32_t edi, uint32_t ebx); };
// 1.13c D2Game+0xD4550
void g_d4550(uint32_t unit, uint32_t ebx, const GD4550Env& env)
{
    if (unit == 0) return;
    if ((*(const uint32_t*)(uintptr_t)(unit + 0xC4) & 1) == 0) return;
    env.d40f0(unit, ebx);
}
