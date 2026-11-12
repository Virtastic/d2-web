// 1.13c D2Common+0x51B60 -- ECX = value, EDX = three-dword block (may be null). A null block returns 0; otherwise the worker 0x51070 is called with (block[0], value, block[1], block[2]) and its result returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2c {
uint32_t f51b60(GE& e, uint32_t v, uint32_t p)
{
    if (!p) return 0;
    return e.h(0x51070, 0, 0, 0, 0, 0, e.rd32(p), v, e.rd32(p + 4), e.rd32(p + 8));
}
}
