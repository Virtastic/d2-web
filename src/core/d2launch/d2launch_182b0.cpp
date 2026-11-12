// 1.13c D2Launch+0x182B0 -- stdcall message handler: calls the interface table (image offset 0x256C4) slot +0x70, sets the flag at 0x25294 to 1, runs the worker 0x17DF0 and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_256c4;
__attribute__((weak)) uint32_t g_d2launch_25294;
namespace d2launch {
uint32_t f182b0(GE& e, uint32_t)
{
    e.h(0x9b32);
    g_d2launch_25294 = 1;
    e.h(0x17DF0);
    return 1;
}
}
