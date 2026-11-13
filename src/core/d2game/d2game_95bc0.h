// 1.13c D2Game+0x95BC0: skill start callback (table entry) of a skill that toggles a timed state on the
// unit. ecx = game, edx = the unit, two stack words (skill, level; ret 8). 0 without a skill record, when
// 0x91340 (eax = skill; unit) is negative, or without the unit's skill entry (0xA3F4: ecx = unit, edx =
// skill). The state is 0xA5D4 (unit, 0xC). Without it: a positive record short +0x184 needs stat 8 of the unit
// (0xA42A: unit, 8, 0) to be at least that many times 256, else 0; the end frame is the game's frame (+0xA8)
// plus 0x15; a state entry is made by 0xA6A0 (the game's pool +0x1C, 2, end frame, the unit's type and id,
// or 6 and -1 for no unit), none ends with 0; 0x8EA80 (ecx = game, edx = unit; 0xC, end frame, 0, 0, 0) arms
// the timer, the entry is attached (0xA60A: unit, entry, 1), given its end callback 0x90F10 (0xA75A) and its
// state id (0xA5B6: entry, 0xC), the state is switched on (0xA3CA: unit, 0xC, 1) and the skill entry's counter
// cleared (0xAC94: entry, 0); 1. With the state: its end frame becomes the game's frame plus 7 (0xA7C6) and
// the timer is armed the same way; when the skill entry's counter (0xAD12) is above the game's frame the unit
// is 0xE2480 (esi = unit; game, 1) and the result is 1, else the result is that of 0x95560 (ecx = game, edx =
// unit; skill, level), after which a nonzero result runs 0x9F850 (eax = skill, ecx = unit; game, level).
#pragma once
#include <cstdint>
#include "d2game_misshit.h"

namespace g95bc0 {
template <class E> uint32_t Fn(E& e, uint32_t dt, uint32_t game, uint32_t unit, uint32_t skill, uint32_t level)
{
    const uint32_t sr = D2GameSkillRec(e, dt, skill);
    if (!sr) return 0;
    if ((int32_t)e.h(0x91340, skill, 0, 0, 0, 0, unit) < 0) return 0;
    const uint32_t sk = e.h(0xA3F4, 0, unit, skill);
    if (!sk) return 0;
    const uint32_t st = e.h(0xA5D4, 0, 0, 0, 0, 0, unit, 0xC);
    const uint32_t zero = 0;
    if (!st) {
        const int32_t need = e.rd16s(sr + 0x184);
        if (need > 0 && (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, unit, 8, 0) < (need << 8)) return 0;
        const uint32_t f = e.rd32(game + 0xA8) + 0x15;
        const uint32_t type = unit ? e.rd32(unit) : 6, id = unit ? e.rd32(unit + 0xC) : 0xFFFFFFFFu;
        const uint32_t n = e.h(0xA6A0, 0, 0, 0, 0, 0, e.rd32(game + 0x1C), 2, f, type, id);
        e.x(0xA6A0, &id, 1);
        if (!n) return 0;
        e.h(0x8EA80, 0, game, unit, 0, 0, 0xC, f, 0, 0, 0);
        e.x(0x8EA80, &zero, 1);
        e.h(0xA60A, 0, 0, 0, 0, 0, unit, n, 1);
        e.h(0xA75A, 0, 0, 0, 0, 0, n, 0xB2000000u | 0x90F10);
        e.h(0xA5B6, 0, 0, 0, 0, 0, n, 0xC);
        e.h(0xA3CA, 0, 0, 0, 0, 0, unit, 0xC, 1);
        e.h(0xAC94, 0, 0, 0, 0, 0, sk, 0);
        return 1;
    }
    const uint32_t f = e.rd32(game + 0xA8) + 7;
    e.h(0xA7C6, 0, 0, 0, 0, 0, st, f);
    e.h(0x8EA80, 0, game, unit, 0, 0, 0xC, f, 0, 0, 0);
    e.x(0x8EA80, &zero, 1);
    if ((int32_t)e.h(0xAD12, 0, 0, 0, 0, 0, sk) > (int32_t)e.rd32(game + 0xA8)) {
        e.h(0xE2480, 0, 0, 0, 0, unit, game, 1);
        return 1;
    }
    const uint32_t r = e.h(0x95560, 0, game, unit, 0, 0, skill, level);
    if (r) e.h(0x9F850, skill, unit, 0, 0, 0, game, level);
    return r;
}
}
