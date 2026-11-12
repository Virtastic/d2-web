// 1.13c D2Launch+0xC250 -- stdcall message handler (scroll bar moved): reads the bar position through D2Win 10089 (ECX = the control at image offset 0x25AA0). Unchanged from the saved one (0x25ED8): returns 0. Otherwise the first visible line (0x25ED4) moves by twice the difference, clamped to 0 and to the line count (0x25ED0) minus one, the position is saved, the workers 0xB600 and 0xA8C0 run and the handler returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25aa0;
__attribute__((weak)) uint32_t g_d2launch_25ed8;
__attribute__((weak)) uint32_t g_d2launch_25ed4;
__attribute__((weak)) uint32_t g_d2launch_25ed0;
namespace d2launch {
uint32_t fc250(GE& e, uint32_t)
{
    const uint32_t v = e.h(0x9a3c, 0, g_d2launch_25aa0);
    if (g_d2launch_25ed8 == v) return 0;
    int32_t top = (int32_t)(g_d2launch_25ed4 + (v - g_d2launch_25ed8) * 2);
    g_d2launch_25ed4 = (uint32_t)top;
    if (top < 0) { top = 0; g_d2launch_25ed4 = 0; }
    if (top >= (int32_t)g_d2launch_25ed0) g_d2launch_25ed4 = g_d2launch_25ed0 - 1;
    g_d2launch_25ed8 = v;
    e.h(0xB600);
    e.h(0xA8C0);
    return 1;
}
}
