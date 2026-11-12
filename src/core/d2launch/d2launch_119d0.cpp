// 1.13c D2Launch+0x119D0 -- stdcall, one ignored argument: when the callback global at image offset 0x2552C is set it is
// called with no arguments; returns 1 either way.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_2552c;
namespace d2launch {
uint32_t f119d0(GE& e, uint32_t)
{
    if (g_d2launch_2552c) e.h(HookEnv::handlerRva(g_d2launch_2552c));
    return 1;
}
}
