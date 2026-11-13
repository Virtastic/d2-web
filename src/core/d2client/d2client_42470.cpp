// 1.13c D2Client+0x42470 -- Three stack arguments (unused). Calls D2Gfx ordinal 10026 and then ordinal 10084, whose result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2cl {
uint32_t f42470(GE& e, uint32_t, uint32_t, uint32_t)
{
    e.h(0xD18C);
    return e.h(0xD198);
}
}
