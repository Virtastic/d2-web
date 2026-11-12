// 1.13c D2Launch+0x187D0 -- stdcall, one ignored argument: calls the worker at 0x16E80 and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2launch {
uint32_t f187d0(GE& e, uint32_t)
{
    e.h(0x16E80);
    return 1;
}
}
