// 1.13c D2Launch+0x187C0 -- stdcall, one ignored argument: calls the worker at 0x17DF0 and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2launch {
uint32_t f187c0(GE& e, uint32_t)
{
    e.h(0x17DF0);
    return 1;
}
}
