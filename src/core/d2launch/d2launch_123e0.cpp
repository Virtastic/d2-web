// 1.13c D2Launch+0x123E0 -- stdcall message handler: walks the control list (count at image offset 0x25E30, pointers at 0x252B8) asking D2Win 10044 for each one's kind until one answers 4; the control it stops on (the last one when none answers 4) has its selection read through D2Win 10030 and set to that plus one through D2Win 10037. Returns 1. An empty list or a null control is fatal in retail.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25e30;
__attribute__((weak)) uint32_t g_d2launch_252b8_a8[8];
namespace d2launch {
uint32_t f123e0(GE& e, uint32_t)
{
    uint32_t c = 0;
    for (int32_t i = 0; i <= (int32_t)g_d2launch_25e30; i++) {
        c = g_d2launch_252b8_a8[i];
        if (e.h(0x997c, 0, 0, 0, 0, 0, c) == 4) break;
    }
    const uint32_t v = e.h(0x9910, 0, c);
    e.h(0x99a6, 0, c, v + 1);
    return 1;
}
}
