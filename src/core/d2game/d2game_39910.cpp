// 1.13c D2Game+0x39910 -- ECX = a (kept only in a stack local), EDX = b, three stack arguments c, d and an ignored third: calls 0x98490 (EAX = b; stack c, 0x7C, d, 1) and returns its result.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f39910(GE& e, uint32_t, uint32_t b, uint32_t c, uint32_t d, uint32_t)
{
    return e.h(0x98490, b, 0, 0, 0, 0, c, 0x7c, d, 1);
}
}
