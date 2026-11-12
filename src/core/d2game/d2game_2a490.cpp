// 1.13c D2Game+0x2A490 -- ECX = block: hands it to the Storm free import (0xA2AA) with the constant 8 and returns its result.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f2a490(GE& e, uint32_t block)
{
    return e.h(0xA2AA, 0, 0, 0, 0, 0, block, 8);
}
}
