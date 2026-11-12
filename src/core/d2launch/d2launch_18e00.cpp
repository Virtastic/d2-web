// 1.13c D2Launch+0x18E00 -- stdcall, one ignored argument: calls the worker at 0x17780 and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2launch {
uint32_t f18e00(GE& e, uint32_t)
{
    e.h(0x17780);
    return 1;
}
}
