// 1.13c D2Game+0x6A930 -- ECX = unit, EDX = second register argument, one stack argument: the helper 0x9F7A0 runs (ECX, EDX, the stack argument), then the D2Common import 0xAEB6(unit); a player unit (type 0) also runs the helper 0x96FA0 (the unit's +0x80 word, the unit). The last call's result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f6a930(GE& e, uint32_t u, uint32_t y, uint32_t s)
{
    e.h(0x9F7A0, 0, u, y, 0, 0, s);
    uint32_t v = e.h(0xaeb6, 0, 0, 0, 0, 0, u);
    if (u && e.rd32(u) == 0) v = e.h(0x96FA0, 0, 0, 0, 0, 0, e.rd32(u + 0x80), u);
    return v;
}
}
