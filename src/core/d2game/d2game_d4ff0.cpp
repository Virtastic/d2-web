// 1.13c D2Game+0xD4FF0, injectable callees. ECX = object (+0x60 = linked unit), EAX = unit (EDI),
// stack word 0 unused (ret 4). 0xD47C0 takes ESI/EDI/EBX in registers plus one stack word.
#include <cstdint>
struct GD4FF0Env {
    uint32_t (*a97c)(uint32_t unit);                        // low byte compared with 0xB/0xC
    void     (*a910)(uint32_t linked, uint32_t unit);
    int32_t  (*a988)(uint32_t unit);
    int32_t  (*a640)(uint32_t unit, uint32_t obj);
    void     (*d47c0)(uint32_t esi, uint32_t edi, uint32_t ebx, uint32_t flag);
};
// 1.13c D2Game+0xD4FF0
void g_d4ff0(uint32_t unit, uint32_t obj, const GD4FF0Env& env)
{
    uint32_t linked = *(const uint32_t*)(uintptr_t)(obj + 0x60);
    if (linked == 0) return;
    uint8_t t = (uint8_t)env.a97c(unit);
    if (t == 0xB || t == 0xC) return;
    env.a910(linked, unit);
    if (env.a988(unit) != 1) {
        if (env.a988(unit) != 0) return;
        if (env.a640(unit, obj) == 0) return;
    }
    env.d47c0(obj, unit, linked, 1);
}
