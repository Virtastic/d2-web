// 1.13c D2Launch+0x19870 -- stdcall, one ignored argument: calls the worker at 0x19290 and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2launch {
uint32_t f19870(GE& e, uint32_t)
{
    e.h(0x19290);
    return 1;
}
}
