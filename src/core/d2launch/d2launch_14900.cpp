// 1.13c D2Launch+0x14900 -- stdcall, one ignored argument: calls the worker at 0x12560 and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2launch {
uint32_t f14900(GE& e, uint32_t)
{
    e.h(0x12560);
    return 1;
}
}
