// 1.13c D2Launch+0x18710 -- stdcall message handler: reads the text of the control at image offset 0x254FC through D2Win 10166, narrows it into the launch record (pointer at 0x25ABC) at +0x89 with the D2Lang conversion (limit 0x100), runs the worker 0x10820 and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_254fc;
__attribute__((weak)) uint32_t g_d2launch_25abc;
namespace d2launch {
uint32_t f18710(GE& e, uint32_t)
{
    const uint32_t t = e.h(0x98c2, 0, g_d2launch_254fc);
    e.h(0x1c03c, 0, g_d2launch_25abc + 0x89, t, 0, 0, 0x100);
    e.h(0x10820);
    return 1;
}
}
