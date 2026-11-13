// 1.13c D2Game+0x655F0 -- quest 6 countdown: the last tick with the mode word 2 runs the finish step
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f655f0(NE& e, uint32_t q)
{
    if (!e.rd8(q + 9)) return q;
    const uint32_t data = e.rd32(q + 0x18), left = e.rd32(data + 0x9c) - 1;
    e.wr32(data + 0x9c, left);
    if (left || e.rd32(data + 0x98) != 2) return q;
    return e.h(0x64C10, q);
}
}
