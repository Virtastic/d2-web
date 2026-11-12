// 1.13c D2Launch+0xC310 -- stdcall, one ignored argument: the two module counters at image offsets 0x25ed4 and 0x24db0 each step down by one while positive (signed); then the workers 0xB600 and 0xA8C0 run. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25ed4;
__attribute__((weak)) uint32_t g_d2launch_24db0;
namespace d2launch {
uint32_t fc310(GE& e, uint32_t)
{
    if ((int32_t)g_d2launch_25ed4 > 0) --g_d2launch_25ed4;
    if ((int32_t)g_d2launch_24db0 > 0) --g_d2launch_24db0;
    e.h(0xB600);
    e.h(0xA8C0);
    return 1;
}
}
