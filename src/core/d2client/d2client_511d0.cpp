// 1.13c D2Client+0x511D0 -- ECX = unit (may be null). For a unit of type 4 (item) D2Common ordinal 11064 is asked with the unit's class (+4); returns 1 when it answers non-zero, 0 otherwise.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2cl {
uint32_t f511d0(GE& e, uint32_t u)
{
    if (!u || e.rd32(u) != 4) return 0;
    return e.h(0xCCEE, 0, 0, 0, 0, 0, e.rd32(u + 4)) != 0;
}
}
