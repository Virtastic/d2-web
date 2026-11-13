// 1.13c D2Game+0xA5AC0 -- ECX game, EDX unit, one unused stack word (ret 4). Calls 0xE07A0 (EAX unit, ECX game), looks up the unit's owner pointer (+0x2C) through 0xA598; an owner other than the unit itself and non-null of type 0, 1 or 3 is handed to 0x48750 (ESI = unit, EBX = 1, 2 or 4, stack = owner+0xC).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
void fa5ac0(GE& e, uint32_t game, uint32_t unit)
{
    e.h(0xE07A0, unit, game);
    const uint32_t p = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2c));
    if (p == unit || !p) return;
    const uint32_t t = e.rd32(p);
    const uint32_t n = t == 0 ? 1 : t == 1 ? 2 : t == 3 ? 4 : 0;
    if (n) e.h(0x48750, 0, 0, 0, n, unit, e.rd32(p + 0xc));
}
}
