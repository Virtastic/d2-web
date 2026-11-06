// 1.13c D2Game+0xBA180 -- fastcall ECX = pGame (byte +0x6D selects the stat list), EDX = pUnit, one stack word ignored (ret 4); returns 0.
// For a player unit (type 0) with a unit-data pointer at +0x80 whose stat list has stat 0x19 (index 0xD) set, and whose 0x9BEB0 id is not 0xFFFF,
// 0x9C160 is called with callback 0xB94F0 (EAX = the id, EBX = 0, ESI = pGame). Callees injectable.
#include <cstdint>
#include <cstring>
struct GBA180Env {
    uint32_t (*pdata)(uint32_t unit);                                         // 0xA4C0 (stdcall 1)
    uint32_t (*stat)(uint32_t list, uint32_t id, uint32_t idx);               // 0xA568 (stdcall 3)
    uint32_t (*unitId)(uint32_t unit);                                        // 0x9BEB0 (EAX = unit)
    void (*notify)(uint32_t eax, uint32_t ebx, uint32_t esi, uint32_t cb);   // 0x9C160 (EAX, EBX, ESI, one stack word)
};
static uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
// 1.13c D2Game+0xBA180
uint32_t g_ba180(uint32_t game, uint32_t unit, const GBA180Env& env)
{
    const uint32_t pd = env.pdata(unit);
    const uint32_t list = rd32(pd + 0x10 + *(const uint8_t*)(uintptr_t)(game + 0x6D) * 4);
    if (env.stat(list, 0x19, 0xD) != 0 && unit != 0 && rd32(unit + 0x80) != 0 && rd32(unit) == 0) {
        const uint32_t id = env.unitId(unit);
        if ((id & 0xFFFF) != 0xFFFF) env.notify(id, 0, game, 0x6FC20000u + 0xB94F0);
    }
    return 0;
}
