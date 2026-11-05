// 1.13c D2Game+0xD40F0 -- unit-state guard, then dispatch; injectable callees.
// EDI = unit, EBX = argument forwarded to the callees. Plain ret. Callees (all ret 4, stack word 0 = 0):
//   0x4AD10: ESI = EBX; 0x8E030 / 0x8DFD0: ECX = EBX, ESI = unit.
#include <cstdint>
struct GD40F0Env {
    void (*sub4ad10)(uint32_t ebx, uint32_t esi);
    void (*sub8e030)(uint32_t ecx, uint32_t ebx, uint32_t esi);
    void (*sub8dfd0)(uint32_t ecx, uint32_t ebx, uint32_t esi);
};
// 1.13c D2Game+0xD40F0
void g_d40f0(uint32_t unit, uint32_t ebx, const GD40F0Env& env)
{
    if (unit == 0) return;
    if ((*(const uint32_t*)(uintptr_t)(unit + 0xC4) >> 4) & 1) return;
    if (*(const uint32_t*)(uintptr_t)(unit + 0x10) != 3) return;
    env.sub4ad10(ebx, ebx);
    if ((*(const uint32_t*)(uintptr_t)(unit + 0xC4) >> 0xC) & 1) { env.sub8e030(ebx, ebx, unit); return; }
    env.sub8dfd0(ebx, ebx, unit);
}
