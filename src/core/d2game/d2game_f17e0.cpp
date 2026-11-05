// D2Game 0xF17E0: as 0xF1730 with a fixed request: slot preset 6, l0 preset -1, two zero words, result to 0x7A4C0.
// Convention: ECX = unit, EAX = a unit-like record (must be nonzero), stack s0..s2 (ret 0xC). 0xF14F0 gets EAX = EAX,
// ECX = unit and ten words: pUnit+0x80, s0, s1, 0, 0, &slot, &lA, &lC, &lB, s2. After it, slot == 6 (untouched) means
// the record's own (type, id) = ([EAX], [EAX+0xC]) go to 0x7A4C0, else (slot, lA); 0x7A4C0 gets (s0, s1, a, b, s2).
#include "d2game_s15.h"

// 1.13c D2Game+0xF17E0
extern "C" uint32_t d2game_f17e0(uint32_t unit, uint32_t rec, uint32_t s0, uint32_t s1, uint32_t s2, const D2S15* e)
{
    if (!unit || !rec) return 0;
    const uint32_t pv = d2rd32(unit + 0x80);
    if (!pv) return 0;
    if (e->sm->call(0xA68E, 0, 0, 0, 0, 0, unit, 0x36)) return 0;
    uint32_t slot = 6, la = 0xFFFFFFFFu, lb = 0, lc = 0;
    uint32_t w[10] = { pv, s0, s1, 0, 0, (uint32_t)(uintptr_t)&slot, (uint32_t)(uintptr_t)&la, (uint32_t)(uintptr_t)&lc,
                       (uint32_t)(uintptr_t)&lb, s2 };
    if (!e->f14f0(e->ctx, rec, unit, w)) return 0;
    uint32_t a, b;
    if (slot == 6) { a = d2rd32(rec); b = d2rd32(rec + 0xC); } else { a = slot; b = la; }
    const uint32_t v[5] = { s0, s1, a, b, s2 };
    e->sm->call(0x7A4C0, 0, 0, 0, 0, 0, v[0], v[1], v[2], v[3], v[4]);
    e->sm->blk(0x7A4C0, v, 5);
    return 1;
}
