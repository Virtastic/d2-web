// 1.13c D2Game+0x39CE0 -- monster-mode callback: takes min(100, 0xB240 value) percent of event record +8 (0x14D0 scaled, capped at the value), subtracts it from the
// record and hands it to 0x39260 (EAX = unit, ESI = record, EDI = amount)
// fastcall: ECX = game, EDX = unit, stack word 0 unused, stack word 1 = event record (ret 8)
// Returns EAX at the ret: 0 on a null unit or row, the id on the range exits, the 0xB240 value when not positive, else the 0x39260 result.
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f39ce0(NE& e, uint32_t dt, uint32_t game, uint32_t unit, uint32_t rec)
{
    if (!unit) return 0;
    const int32_t id = (int32_t)e.rd32(unit + 4);
    if (id < 0 || id >= (int32_t)e.rd32(dt + 0xb6c)) return (uint32_t)id;
    const uint32_t row = e.rd32(dt + 0xb64) + (uint32_t)id * 0x1a4u;
    if (!row) return 0;
    const uint32_t owner = (e.rd32(unit + 0xc8) >> 10 & 1) ? e.h(0xE03A0, e.rd32(unit + 0x94), game, e.rd32(unit + 0x98)) : 0;
    int32_t pct = (int32_t)e.imp(0xB240, unit, owner, e.rd32(row + 0x90), 0xffffffffu, 0xffffffffu);
    if (pct <= 0) return (uint32_t)pct;
    if (pct >= 100) pct = 100;
    const int32_t cur = (int32_t)e.rd32(rec + 8);
    int32_t x = (int32_t)e.h(0x14D0, (uint32_t)pct, 100, (uint32_t)cur);
    if (x >= cur) x = cur;
    e.wr32(rec + 8, (uint32_t)(cur - x));
    return e.h(0x39260, unit, 0, 0, 0, rec);
}
}
