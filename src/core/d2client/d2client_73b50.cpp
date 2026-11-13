// 1.13c D2Client+0x73B50 -- ECX, EDX and one stack argument (unused). Returns D2Common ordinal 10945 (ECX, EDX, 0).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2cl {
uint32_t f73b50(GE& e, uint32_t c, uint32_t d, uint32_t)
{
    return e.h(0xC30A, 0, 0, 0, 0, 0, c, d, 0);
}
}
