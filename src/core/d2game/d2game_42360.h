// 1.13c D2Game+0x42360: skill start callback (table entry): may the unit use a ranged skill on its target.
// ecx = game, edx = the unit, two stack words (skill, level; ret 8). After 0xE07A0 (eax = unit, ecx = game)
// the target is 0xA598 of the unit's path; the unit itself or none gives 0, as does a missing skill record.
// The reach is the calc at record +0x64 (0xAC9A: unit, calc, skill, level), squared. The target's y and x and
// then the unit's y and x are read, and 0xAE26 (unit x, unit y, target x, target y) above the squared reach
// gives 0. A target of type 0 or 1 must also pass 0xE0A60 (eax = target, ecx = unit; game), and neither the
// target's nor the unit's room (0xA478) may have 0xA808 set. 1 otherwise.
#pragma once
#include <cstdint>
#include "d2game_misshit.h"

namespace g42360 {
template <class E> uint32_t Fn(E& e, uint32_t dt, uint32_t game, uint32_t unit, uint32_t skill, uint32_t level)
{
    e.h(0xE07A0, unit, game);
    const uint32_t target = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (target == unit || !target) return 0;
    const uint32_t sr = D2GameSkillRec(e, dt, skill);
    if (!sr) return 0;
    const uint32_t r = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(sr + 0x64), skill, level);
    const int32_t lim = (int32_t)(r * r);
    const uint32_t ty = D2GameUnitY(e, target), tx = D2GameUnitX(e, target);
    const uint32_t uy = D2GameUnitY(e, unit), ux = D2GameUnitX(e, unit);
    if ((int32_t)e.h(0xAE26, 0, 0, 0, 0, 0, ux, uy, tx, ty) > lim) return 0;
    const uint32_t type = e.rd32(target);
    if (type == 0 || type == 1) {
        if (!e.h(0xE0A60, target, unit, 0, 0, 0, game)) return 0;
        if (e.h(0xA808, 0, 0, 0, 0, 0, e.h(0xA478, 0, 0, 0, 0, 0, target))) return 0;
        if (e.h(0xA808, 0, 0, 0, 0, 0, e.h(0xA478, 0, 0, 0, 0, 0, unit))) return 0;
    }
    return 1;
}
}
