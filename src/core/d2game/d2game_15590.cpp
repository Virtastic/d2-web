// 1.13c D2Game+0x15590 -- stdcall, two arguments: unit and record {skip, last, count}. A unit other than the record's skip word is rated by 0xB0A20 (EBX = the skip word, one stack word = unit); a signed rating of at most 1 counts it (+8) and stores it as last (+4). Returns the rating, or EAX untouched for the skipped unit.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f15590(GE& e, uint32_t eax, uint32_t unit, uint32_t rec)
{
    const uint32_t skip = e.rd32(rec);
    if (skip == unit) return eax;
    const uint32_t v = e.h(0xB0A20, 0, 0, 0, skip, 0, unit);
    if ((int32_t)v <= 1) { e.wr32(rec + 8, e.rd32(rec + 8) + 1); e.wr32(rec + 4, unit); }
    return v;
}
}
