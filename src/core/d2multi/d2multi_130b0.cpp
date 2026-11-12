// 1.13c D2Multi+0x130B0 -- stdcall, one ignored argument: calls the worker at 0x11510 and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2multi {
uint32_t f130b0(GE& e, uint32_t)
{
    e.h(0x11510);
    return 1;
}
}
