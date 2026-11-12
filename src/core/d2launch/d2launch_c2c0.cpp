// 1.13c D2Launch+0xC2C0 -- stdcall message handler (line down): when the page size (image offset 0x24DBC) plus the first visible line (0x25ED4) is below the line count (0x25ED0), the selection (0x24DB0) advances unless it is the last line, the first visible line advances, and the workers 0xB600 and 0xA8C0 run. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_24dbc;
__attribute__((weak)) uint32_t g_d2launch_25ed4;
__attribute__((weak)) uint32_t g_d2launch_25ed0;
__attribute__((weak)) uint32_t g_d2launch_24db0;
namespace d2launch {
uint32_t fc2c0(GE& e, uint32_t)
{
    if ((int32_t)(g_d2launch_24dbc + g_d2launch_25ed4) < (int32_t)g_d2launch_25ed0) {
        if ((int32_t)g_d2launch_24db0 < (int32_t)(g_d2launch_25ed0 - 1)) g_d2launch_24db0++;
        g_d2launch_25ed4++;
        e.h(0xB600);
        e.h(0xA8C0);
    }
    return 1;
}
}
