// 1.13c D2Launch+0x18760 -- stdcall, one ignored argument: calls the worker at 0x17DF0 and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2launch {
uint32_t f18760(GE& e, uint32_t)
{
    e.h(0x17DF0);
    return 1;
}
}
