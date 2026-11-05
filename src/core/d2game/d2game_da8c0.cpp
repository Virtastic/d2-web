// 1.13c D2Game+0xDA8C0, injectable callees. EAX = target, EDI = unit, EBX = accumulator record
// (+0xC is added to). Plain ret. 0x372C0 takes the unit as stack word 0 (retail pushes EDI), ret 4.
#include <cstdint>
struct GDA8C0Env {
    uint32_t (*a508)(uint32_t eax);
    uint32_t (*f372c0)(uint32_t unit);
    uint32_t (*a430)(uint32_t p, uint32_t id);
    int32_t  (*a802)(uint32_t unit, uint32_t id, uint32_t idx);
};
// 1.13c D2Game+0xDA8C0
void g_da8c0(uint32_t eax, uint32_t edi, uint32_t ebx, const GDA8C0Env& env)
{
    if (env.a508(eax) == 0) return;
    int32_t acc = 0;
    uint32_t p = env.f372c0(edi);
    if (p != 0 && env.a430(p, 0x39) != 0) acc = 0x32;
    acc += env.a802(edi, 0x7A, 0);
    if (acc > 0) *(int32_t*)(uintptr_t)(ebx + 0xC) += acc;
}
