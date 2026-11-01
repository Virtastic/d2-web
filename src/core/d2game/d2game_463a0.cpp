// 1.13c D2Game+0x463A0 -- timed state then its follow-up (EAX = target, ESI = level, EDI = skill id; stack: game, unit, duration; ret 0xC).
// Written from the 1.13c disassembly as a rule over the seam environment (d2game_gp.h).
// level <= 0: 0. 0x45EE0(game, unit, target, duration, skill, level); when it reports success, 0x45410(EAX = skill; game, unit, duration, level). Then 1.
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f463a0(GE& e, uint32_t tgt, uint32_t lv, uint32_t sk, uint32_t game, uint32_t unit, uint32_t dur)
{
    if ((int32_t)lv <= 0) return 0;
    if (e.h(0x45EE0, 0, 0, 0, 0, 0, game, unit, tgt, dur))
        e.h(0x45410, sk, 0, 0, 0, 0, game, unit, dur, lv);
    return 1;
}
}
