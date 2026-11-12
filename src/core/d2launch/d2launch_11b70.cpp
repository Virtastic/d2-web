// 1.13c D2Launch+0x11B70 -- stdcall, one ignored argument: calls the D2Win import at 0x994C and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2launch {
uint32_t f11b70(GE& e, uint32_t)
{
    e.h(0x994C);
    return 1;
}
}
