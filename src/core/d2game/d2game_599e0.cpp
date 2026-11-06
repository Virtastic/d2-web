// 1.13c D2Game+0x599E0 (ACT5Q6_StatusOnClass84): ECX = game (+0x6D level byte), EDX = unit, one unused stack word (ret 4). Returns 0.
// h = word [#10920(unit) + 0x10 + level * 4]. When #10174(h, 0x28, 0) == 1, stop. Room = #10331(unit); none stops. When #10826(room) == 0x84 and both
// #10174(h, 0x28, 0) and #10174(h, 0x28, 1) are 0, 0x58BE0 runs (EAX = h, ECX = unit, EBX = game). Callees are the seam; the handle is read again after the
// room lookup, as retail re-reads it.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_599e0(uint32_t game, uint32_t unit, const D2Seam* e)
{
    const uint32_t lvl = d2rd8(game + 0x6D);
    uint32_t h = d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, unit) + 0x10 + lvl * 4);
    if (e->call(0xA568, 0, 0, 0, 0, 0, h, 0x28, 0) == 1) return 0;
    const uint32_t room = e->call(0xA478, 0, 0, 0, 0, 0, unit);
    if (!room) return 0;
    h = d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, unit) + 0x10 + lvl * 4);
    if (e->call(0xA556, 0, 0, 0, 0, 0, room) != 0x84) return 0;
    if (e->call(0xA568, 0, 0, 0, 0, 0, h, 0x28, 0) != 0) return 0;
    if (e->call(0xA568, 0, 0, 0, 0, 0, h, 0x28, 1) != 0) return 0;
    e->call(0x58BE0, h, unit, 0, game);
    return 0;
}
