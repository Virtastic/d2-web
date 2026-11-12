// 1.13c D2Game+0xB8A50 -- ECX unused, EDX = unit, one stack argument = record: the D2Common import 0xAB86(unit, record +0x14) is asked; a signed answer below 5 sets the record's byte +0x18 to 1. Returns the answer.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fb8a50(GE& e, uint32_t, uint32_t unit, uint32_t rec)
{
    const uint32_t v = e.h(0xAB86, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x14));
    if ((int32_t)v < 5) e.wr8(rec + 0x18, 1);
    return v;
}
}
