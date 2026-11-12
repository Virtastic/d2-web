// 1.13c D2Game+0x76F20: a monster-mode frame callback. ECX = game, EDX = unit (plain ret). When the unit is at the end of its
// frame cycle (#B084) the mode's end step (0x76BF0, ECX = game, EDX = unit) runs and the result is 2. Otherwise the used skill
// (#AABA) and its flags (#A79C) are read; with flag bit 0 the skill's server step (0xE07A0, EAX = unit, ECX = game) runs and, when
// the skill's state (0xE1930, EAX = unit) is 2, flag bit 1 is set (#A78A) and the attack event (0xA2060, game) fires, clearing the
// "check" word. Then the attack event fires when the action frame byte (+0x4E) is 4, or when the check word is still set, unit
// flag bit 6 (+0xC4) is clear and the frame is 1 or 2. Last, #B08A(unit); the result is 1.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_76f20(uint32_t game, uint32_t unit, const D2Seam* e)
{
    if (e->call(0xB084, 0, 0, 0, 0, 0, unit)) {
        e->call(0x76BF0, 0, game, unit);
        return 2;
    }
    const uint32_t skill = e->call(0xAABA, 0, 0, 0, 0, 0, unit);
    uint32_t flags = e->call(0xA79C, 0, 0, 0, 0, 0, skill);
    uint32_t check = 1;
    if (flags & 1) {
        e->call(0xE07A0, unit, game);
        const uint32_t st = e->call(0xE1930, unit);
        if (st == 2) {
            flags |= st;
            e->call(0xA78A, 0, 0, 0, 0, 0, skill, flags);
            e->call(0xA2060, 0, 0, 0, 0, 0, game);
            check = 0;
        }
    }
    const uint32_t frame = d2rd8(unit + 0x4E);
    bool fire = frame == 4;
    if (!fire && check && !((d2rd32(unit + 0xC4) >> 6) & 1) && (frame == 1 || frame == 2)) fire = true;
    if (fire) e->call(0xA2060, 0, 0, 0, 0, 0, game);
    e->call(0xB08A, 0, 0, 0, 0, 0, unit);
    return 1;
}
