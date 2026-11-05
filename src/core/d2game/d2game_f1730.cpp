// D2Game 0xF1730: run the 0xF14F0 search for a unit that has no state 0x36, then hand its result to 0x7A440.
// Convention (disassembly of the 1.13c bytes): EAX = unit, EBX = a pass-through word, stack s0..s3 (ret 0x10).
// 0xF14F0 gets EAX = 0, ECX = unit and ten stack words: pUnit+0x80, s0, s1, s2, EBX, &l2, &l1, &s2 (the s2 slot, zeroed
// first), &l0 (zeroed first), s3. It writes through them; a nonzero s2 slot picks (slot, l0), else (s2, EBX) goes on.
#include "d2game_s15.h"

// 1.13c D2Game+0xF1730
extern "C" uint32_t d2game_f1730(uint32_t unit, uint32_t ebx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, const D2S15* e)
{
    if (!unit) return 0;
    const uint32_t pv = d2rd32(unit + 0x80);
    if (!pv) return 0;
    if (e->sm->call(0xA68E, 0, 0, 0, 0, 0, unit, 0x36)) return 0;
    uint32_t slot = 0, l0 = 0, l1 = 0, l2 = 0;
    uint32_t w[10] = { pv, s0, s1, s2, ebx, (uint32_t)(uintptr_t)&l2, (uint32_t)(uintptr_t)&l1, (uint32_t)(uintptr_t)&slot,
                       (uint32_t)(uintptr_t)&l0, s3 };
    if (!e->f14f0(e->ctx, 0, unit, w)) return 0;
    const uint32_t a = slot ? slot : s2, b = slot ? l0 : ebx;
    const uint32_t v[5] = { s0, s1, a, b, s3 };
    e->sm->call(0x7A440, 0, 0, 0, 0, 0, v[0], v[1], v[2], v[3], v[4]);
    e->sm->blk(0x7A440, v, 5);
    return 1;
}
