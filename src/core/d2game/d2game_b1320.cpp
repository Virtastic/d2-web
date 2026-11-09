// 1.13c D2Game+0xB1320: ESI = unit, stack p1..p3 (ret 0xC). A player unit (type 1) whose
// player data (unit+0x14 -> +0x28) names a linked unit (type pd+0x28, id pd+0x2C, pd+0x30) looks it up with 0xE03A0
// (EAX pd+0x30, ECX pd+0x28, EDX pd+0x2C); a monster (type 1) result goes to 0x21490 (EAX = that unit, EBX = the callback
// at 0xB06F0, ESI = the caller's unit; stack &request, &out; request = {p3, p1, unit, p2}; out starts 0) and the word the
// callback walk leaves in out is returned. Any missing link returns 0. The call target is 0x21490 (e8 rel32 at +0x73),
// not 0x41490.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_b1320(uint32_t unit, uint32_t p1, uint32_t p2, uint32_t p3, const D2Seam* e)
{
    // 1.13c D2Game+0xB1320
    if (!unit || d2rd32(unit) != 1) return 0;
    const uint32_t u14 = d2rd32(unit + 0x14);
    if (!u14) return 0;
    const uint32_t pd = d2rd32(u14 + 0x28);
    if (!pd || !d2rd32(pd + 0x28)) return 0;
    const uint32_t t = e->call(0xE03A0, d2rd32(pd + 0x30), d2rd32(pd + 0x28), d2rd32(pd + 0x2C));
    if (!t || d2rd32(t) != 1) return 0;
    uint32_t out = 0;
    uint32_t req[4] = { p3, p1, unit, p2 };
    // EBX = the callback 0xB06F0 (a module address, not in the descriptor's REGS for this hook: passed as 0)
    e->call(0x21490, t, 0, 0, 0, unit, (uint32_t)(uintptr_t)req, (uint32_t)(uintptr_t)&out);
    e->blk(0x21490, req, 4);
    return out;
}
