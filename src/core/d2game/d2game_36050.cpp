// 1.13c D2Game+0x36050: ESI = unit, one stack word (ret 4).
// Ordinal 10331 of the unit (thunk 0xA478) feeds ordinal 10826 (thunk 0xA556); a result below 0x6C (signed)
// calls 0xE73C0 with EAX = the stack word and ECX = the unit. No return value.
#include <cstdint>
extern "C" void d2game_36050(uint32_t unit, uint32_t arg,
                             uint32_t (*ordinal10331)(uint32_t),
                             int32_t (*ordinal10826)(uint32_t),
                             void (*subE73C0)(uint32_t eax, uint32_t ecx))
{
    if (ordinal10826(ordinal10331(unit)) < 0x6C) subE73C0(arg, unit);
}
