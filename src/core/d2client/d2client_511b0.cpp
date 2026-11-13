// 1.13c D2Client+0x511B0 -- ECX = unit (may be null). Returns 1 for a unit of type 1 (monster), 0 otherwise.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2cl {
uint32_t f511b0(GE& e, uint32_t u)
{
    return u && e.rd32(u) == 1;
}
}
