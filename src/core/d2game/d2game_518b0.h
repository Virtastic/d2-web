// 1.13c D2Game+0x518B0: ECX = pGame, EDX = pUnit, stack = skill id, skill level (ret 8). Replaces the unit's
// owner-linked monster with a freshly spawned one. The rule over an environment E (HookEnv in the FN probe src/core/fn/d2game_518b0.cpp); every
// callee is one E::h call in retail's order (disassembled; the batch port d2game_batch_452c0b.cpp:D2GAME_518B0 makes the same calls).
// - 0xE07A0 (EAX = unit, ECX = game); owner = (#thunk 0xA598)(unit +0x2C); owner == unit or null gives 0; 0xADC0 (owner, 0) zero gives 0.
// - 0x4F450 (EBX = game, ESI = unit); record = skill table row (id in [0, count)), null gives 0.
// - mon = 0x9EE50 (EAX = 0, ECX = 0, EDX = &out; unit, 0, id, level), negative gives 0; class = signed byte record +0xBE outside
//   [0, data tables +0xBF0) gives 0.
// - a = 0x1100 (ECX = owner), b = 0x1130 (ECX = owner); 0xA44E (0xA478 (owner, owner type, owner guid)); 0xE20A0 (ECX = game, EDX = owner).
// - c = 0xAC9A (unit, record +0xC0, id, level); request {1, unit, mon, 0, out, a, b, class, c}; pet = 0xA3610 (EAX = &request; game);
//   null gives 0.
// - 0x4E2A0 (EAX = unit, ECX = 0; game, pet, level); 0x4EB50 (EAX = pet, ECX = unit; id, level); 0x4F970 (game, unit, pet, id, level, 0);
//   0x4E180 (EAX = unit, EDI = pet); 0xEFE40 (ECX = game, EBX = unit +0xD0, EDI = pet; 0); 1.
#pragma once
#include <cstdint>

namespace g518b0 {
template <class E> uint32_t SummonReplace(E& e, uint32_t dt, uint32_t game, uint32_t unit, int32_t id, uint32_t level)
{
    e.h(0xE07A0, unit, game);
    const uint32_t owner = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (owner == unit || !owner) return 0;
    if (!e.h(0xADC0, 0, 0, 0, 0, 0, owner, 0)) return 0;
    e.h(0x4F450, 0, 0, 0, game, unit);
    if (id < 0 || id >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t rec = (uint32_t)id * 0x23Cu + e.rd32(dt + 0xB98);
    if (!rec) return 0;
    uint32_t out = 0;
    const int32_t mon = (int32_t)e.h(0x9EE50, 0, 0, e.stk(&out), 0, 0, unit, 0, (uint32_t)id, level);
    if (mon < 0) return 0;
    const int32_t cls = (int8_t)e.rd8(rec + 0xBE);
    if (cls < 0 || cls >= (int32_t)e.rd32(dt + 0xBF0)) return 0;
    const uint32_t a = e.h(0x1100, 0, owner), b = e.h(0x1130, 0, owner);
    e.h(0xA44E, 0, 0, 0, 0, 0, e.h(0xA478, 0, 0, 0, 0, 0, owner, e.rd32(owner), e.rd32(owner + 0xC)));
    e.h(0xE20A0, 0, game, owner);
    uint32_t t[9] = { 1, unit, (uint32_t)mon, 0, out, a, b, (uint32_t)cls, 0 };
    t[8] = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0xC0), (uint32_t)id, level);
    const uint32_t pet = e.h(0xA3610, e.stk(t), 0, 0, 0, 0, game);
    e.blk(0xA3610, t, 9);
    if (!pet) return 0;
    e.h(0x4E2A0, unit, 0, 0, 0, 0, game, pet, level);
    e.h(0x4EB50, pet, unit, 0, 0, 0, (uint32_t)id, level);
    const uint32_t w[6] = { game, unit, pet, (uint32_t)id, level, 0 };
    e.h(0x4F970, 0, 0, 0, 0, 0, w[0], w[1], w[2], w[3], w[4], w[5]);
    e.blk(0x4F970, w, 6);
    e.ed(pet);
    e.h(0x4E180, unit);
    e.edi(0x4E180, pet);
    e.ed(pet);
    e.h(0xEFE40, 0, game, 0, e.rd32(unit + 0xD0), 0, 0);
    e.edi(0xEFE40, pet);
    return 1;
}
}
