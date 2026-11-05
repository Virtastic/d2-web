// 1.13c D2Game+0xD3950 -- clear the cursor slot and finish the player removal; injectable callees.
// EAX = esi (pass-through, copied to ESI), EDI = player unit, stack word 0 = arg (ret 4). Callees:
//   0xA4C0 (D2Common #10920, stack word 0 = unit, returns the record), 0xAA5A (D2Common #10758, 3 stack words (&slot, 0x20, 1), ret 12),
//   0x8D3D0 (EBX = record[0x9C] or 0, ESI = esi, 3 stack words (0xF, slot, 0), ret 12), 0xD2CF0 (EAX = arg, ESI = esi, stack word 0 = unit, ret 4).
#include <cstdint>
struct GD3950Env {
    uint32_t (*ord10920)(uint32_t unit);
    void (*ord10758)(uint32_t* slot, uint32_t a, uint32_t b);
    void (*sub8d3d0)(uint32_t ebx, uint32_t esi, uint32_t w0, uint32_t w1, uint32_t w2);
    void (*subd2cf0)(uint32_t eax, uint32_t esi, uint32_t unit);
};
// 1.13c D2Game+0xD3950
void g_d3950(uint32_t eax, uint32_t unit, uint32_t arg, const GD3950Env& env)
{
    const uint32_t esi = eax;
    uint32_t ebx = 0;
    if (unit != 0 && *(const uint32_t*)(uintptr_t)unit == 0) ebx = *(const uint32_t*)(uintptr_t)(env.ord10920(unit) + 0x9c);
    uint32_t slot = 0;
    env.ord10758(&slot, 0x20, 1);
    env.sub8d3d0(ebx, esi, 0xf, slot, 0);
    env.subd2cf0(arg, esi, unit);
}
