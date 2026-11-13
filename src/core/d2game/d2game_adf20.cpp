// 1.13c D2Game+0xADF20 -- ECX game, EDX unit, five stack words (ret 0x14, only word 0 read), no return value. 0xA53E(word 0 ? word0+4 : -1) must answer nonzero; 0xE07A0 (EAX unit, ECX game); the unit's owner via 0xA598(unit+0x2C): another non-null owner gets 0xF17E0 (ECX unit, stack 0x2F, 1, 0), otherwise 0xAAD8(unit+0x2C, 0) and 0xAAC6(unit+0x2C) feed 0xF1730 (EAX unit, EBX = the 0xAAD8 answer, stack 0x2F, 1, 0xAAC6 answer).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
void fadf20(GE& e, uint32_t game, uint32_t unit, uint32_t w0)
{
    if (!e.h(0xA53E, 0, 0, 0, 0, 0, w0 ? e.rd32(w0 + 4) : 0xffffffffu)) return;
    e.h(0xE07A0, unit, game);
    const uint32_t p = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2c));
    if (p != unit && p) { e.h(0xF17E0, 0, unit, 0, 0, 0, 0x2f, 1, 0); return; }
    const uint32_t b = e.h(0xAAD8, 0, 0, 0, 0, 0, e.rd32(unit + 0x2c), 0);
    const uint32_t c = e.h(0xAAC6, 0, 0, 0, 0, 0, e.rd32(unit + 0x2c));
    e.h(0xF1730, unit, 0, 0, b, 0, 0x2f, 1, c);
}
}
