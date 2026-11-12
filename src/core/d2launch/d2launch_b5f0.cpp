// 1.13c D2Launch+0xB5F0 -- stdcall, one ignored argument: calls the worker at 0xA410 and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2launch {
uint32_t fb5f0(GE& e, uint32_t)
{
    e.h(0xA410);
    return 1;
}
}
