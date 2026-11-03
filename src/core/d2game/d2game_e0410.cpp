// 1.13c D2Game+0xE0410 -- clear the +0x148/+0x14c pair of a unit's record and notify 0x8E8C0.
// regs ECX = game (kept in EDI), EDX = unit; `ret 0xc`. Callees: 0xA4C0 (D2Common #10920, stdcall, one word = unit) and
// 0x8E8C0 (EAX = unit, ECX = game, EBX = 0xE, stack word = this function's own address 0x6FD00410).
#include <cstdint>
#include <cstring>
struct D2GameE0410Env {
    uint32_t (*ordinal10920)(uint32_t unit);
    void (*sub8E8C0)(uint32_t owner, uint32_t game, uint32_t type, uint32_t id);
};
uint32_t d2game_port_e0410(uint32_t game, uint32_t unit, const D2GameE0410Env* e)
{
    uint32_t first;
    if (!unit) return 0;
    std::memcpy(&first, (const void*)(uintptr_t)unit, 4);
    if (first) return 0;
    const uint32_t rec = e->ordinal10920(unit);
    if (!rec) return 0;
    const uint32_t zero = 0;
    std::memcpy((void*)(uintptr_t)(rec + 0x148), &zero, 4);
    std::memcpy((void*)(uintptr_t)(rec + 0x14c), &zero, 4);
    e->sub8E8C0(unit, game, 0xe, 0x6fd00410);
    return 1;
}
