// 1.13c D2Launch+0x188A0 -- stdcall, one ignored argument: stores 2 in the module global at image offset 0x25ad4, then calls the worker at 0x17B30 and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25ad4;
namespace d2launch {
uint32_t f188a0(GE& e, uint32_t)
{
    g_d2launch_25ad4 = 2;
    e.h(0x17B30);
    return 1;
}
}
