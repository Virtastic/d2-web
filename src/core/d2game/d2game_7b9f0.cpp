// 1.13c D2Game+0x7B9F0 -- EDX = unit, two stack arguments (a candidate unit and a {unit, distance} pair). A candidate that is another, non-null monster (type 1) for which the D2Common imports 0xA514 and 0xA688 both answer 0 is measured with the helper 0x1900 (EAX = unit, EBX = candidate); a distance below the pair's +4 word replaces the pair. Returns 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f7b9f0(GE& e, uint32_t, uint32_t u, uint32_t t, uint32_t out)
{
    if (u == t || !t || e.rd32(t) != 1) return 0;
    if (e.h(0xa514, 0, 0, 0, 0, 0, t)) return 0;
    if (e.h(0xa688, 0, 0, 0, 0, 0, t)) return 0;
    const uint32_t d = e.h(0x1900, u, 0, 0, t, 0);
    if ((int32_t)d < (int32_t)e.rd32(out + 4)) {
        e.wr32(out, t);
        e.wr32(out + 4, d);
    }
    return 0;
}
}
