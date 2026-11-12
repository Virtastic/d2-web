// 1.13c D2Launch+0x1A180 -- stdcall message handler (Battle.net gateway accept): a zero argument runs the worker 0x17DF0 and returns 1. Otherwise D2Win 10108 on the control at image offset 0x25504 answering 0 returns 0. Then the worker 0x11950 (ECX = the control at 0x254FC) and the worker 0xEAB0 with two zeros: 0 from it returns 1. Otherwise the gateway text at +0x89 of the launch record (0x25ABC) is written to the Storm registry value "Last BNet"; when the mode at 0x25AD4 is 1, or the worker 0x11F30 answers non-zero, D2Sound 10019 and the workers 0x15D30 and 0xD5D0 run, else the worker 0x16E80. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25504;
__attribute__((weak)) uint32_t g_d2launch_254fc;
__attribute__((weak)) uint32_t g_d2launch_25abc;
__attribute__((weak)) uint32_t g_d2launch_25ad4;
namespace d2launch {
uint32_t f1a180(GE& e, uint32_t a)
{
    if (!a) { e.h(0x17DF0); return 1; }
    if (!e.h(0x98d4, 0, g_d2launch_25504)) return 0;
    e.h(0x11950, 0, g_d2launch_254fc);
    if (!e.h(0xEAB0, 0, 0, 0, 0, 0, 0, 0)) return 1;
    e.h(0x9796, 0, 0, 0, 0, 0, 0xB201e204u, 0xB201e0b4u, 0, g_d2launch_25abc + 0x89);
    if (g_d2launch_25ad4 != 1 && !e.h(0x11F30)) { e.h(0x16E80); return 1; }
    e.h(0x9a5a);
    e.h(0x15D30);
    e.h(0xD5D0);
    return 1;
}
}
