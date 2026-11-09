// 1.13c D2Game+0x443F0 SKILLS_StartInferno: EAX = pUnit, EBX = pGame, stack = skill id, skill level,
// a5 (ret 0xC). The rule over an environment E (HookEnv in the FN probe src/core/fn/d2game_443f0.cpp): every callee is one E::h(rva, eax, ecx,
// edx, ebx, esi, s0..s4) call in retail's order; the vendored SkillSor.cpp SKILLS_StartInferno makes the same calls.
// - skill id outside [0, count) or a null record: 0.
// - a monster caster (type 1): param1 of its used skill (0xAABA, 0xAC94) := frame + max(1, calc 0xAC9A over record +0x13C).
// - inferno statlist present (0xA5D4 unit, state 0xC): 0xE2480 (ESI = unit; game, 1), expire it at frame + 6 (0xA7C6), remove-state
//   event 0xC at frame + 6 (0x8EA80, ECX = game, EDX = unit), the inferno step 0x441C0 (EAX = unit; game, id, level, a5); 1.
// - absent: allocate (0xA6A0 game+0x1C pool, 2, frame + 20, unit type or 6, unit guid or -1); null gives 0; else event 0xC at frame + 20,
//   attach (0xA60A unit, list, 1), remove callback D2Game+0x42290 (0xA75A), state 0xC (0xA5B6), toggle on (0xA3CA unit, 0xC, 1), param1 := 0; 1.
#pragma once
#include <cstdint>

namespace g443f0 {
template <class E> uint32_t StartInferno(E& e, uint32_t dt, uint32_t unit, uint32_t game, int32_t id, uint32_t level, uint32_t a5, uint32_t cbRemove)
{
    if (id < 0 || id >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t rec = (uint32_t)id * 0x23Cu + e.rd32(dt + 0xB98);
    if (!rec) return 0;
    if (unit && e.rd32(unit) == 1) {
        int32_t t = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x13C), (uint32_t)id, level);
        if (t < 1) t = 1;
        const uint32_t until = e.rd32(game + 0xA8) + (uint32_t)t;
        const uint32_t sk = e.h(0xAABA, 0, 0, 0, 0, 0, unit);
        e.h(0xAC94, 0, 0, 0, 0, 0, sk, until);
    }
    const uint32_t cur = e.h(0xA5D4, 0, 0, 0, 0, 0, unit, 0xC);
    if (!cur) {
        const uint32_t until = e.rd32(game + 0xA8) + 0x14;
        const uint32_t type = unit ? e.rd32(unit) : 6u, guid = unit ? e.rd32(unit + 0xC) : 0xFFFFFFFFu;
        const uint32_t sl = e.h(0xA6A0, 0, 0, 0, 0, 0, e.rd32(game + 0x1C), 2, until, type, guid);
        if (!sl) return 0;
        e.h(0x8EA80, 0, game, unit, 0, 0, 0xC, until, 0, 0, 0);
        e.h(0xA60A, 0, 0, 0, 0, 0, unit, sl, 1);
        e.h(0xA75A, 0, 0, 0, 0, 0, sl, cbRemove);
        e.h(0xA5B6, 0, 0, 0, 0, 0, sl, 0xC);
        e.h(0xA3CA, 0, 0, 0, 0, 0, unit, 0xC, 1);
        const uint32_t sk = e.h(0xAABA, 0, 0, 0, 0, 0, unit);
        e.h(0xAC94, 0, 0, 0, 0, 0, sk, 0);
        return 1;
    }
    e.h(0xE2480, 0, 0, 0, 0, unit, game, 1);
    const uint32_t until = e.rd32(game + 0xA8) + 6;
    e.h(0xA7C6, 0, 0, 0, 0, 0, cur, until);
    e.h(0x8EA80, 0, game, unit, 0, 0, 0xC, until, 0, 0, 0);
    e.h(0x441C0, unit, 0, 0, 0, 0, game, (uint32_t)id, level, a5);
    return 1;
}
}
