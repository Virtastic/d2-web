// 1.13c D2Common+0x5E350 -- ECX = string id. Calls the D2Lang import 0x92DC with (id, address of a local word) and returns its result; a result whose low 16 bits are zero becomes 0x1506.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2c {
uint32_t f5e350(GE& e, uint32_t id)
{
    uint32_t loc = id;
    const uint32_t v = e.h(0x92DC, 0, 0, 0, 0, 0, id, e.stk(&loc));
    return (v & 0xFFFF) ? v : 0x1506;
}
}
