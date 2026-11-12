// 1.13c D2Game+0xF0820 -- ECX is not used (the worker reads only EDX and its stack), EDX = unit, one stack argument = other unit. When both are the same unit the argument is returned. Otherwise, with id = the other unit's +0xC word (-1 for none): for a player unit (non-null, type 0) the D2Common import 0xA4C0(unit) gives the player data and the worker 0xF0720 runs with (its +0x9C word, 9), EDX = id; for anything else it runs with (0, 9). Its result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t ff0820(GE& e, uint32_t c, uint32_t unit, uint32_t other)
{
    if (unit == other) return other;
    const uint32_t id = other ? e.rd32(other + 0xc) : ~0u;
    uint32_t v = 0;
    if (unit && !e.rd32(unit)) v = e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, unit) + 0x9c);
    (void)c;
    return e.h(0xF0720, 0, 0, id, 0, 0, v, 9);
}
}
