// 1.13c D2Client+0x12DC0 -- Clears EAX and continues in 0x3A6F0 (a tail jump); that routine's result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2cl {
uint32_t f12dc0(GE& e, uint32_t)
{
    return e.h(0x3A6F0, 0);
}
}
