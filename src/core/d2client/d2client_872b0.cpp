// 1.13c D2Client+0x872B0 -- stdcall, two arguments. Returns 0x14720 with ECX = argument 2 and one stack word, argument 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2cl {
uint32_t f872b0(GE& e, uint32_t a, uint32_t b)
{
    return e.h(0x14720, 0, b, 0, 0, 0, a);
}
}
