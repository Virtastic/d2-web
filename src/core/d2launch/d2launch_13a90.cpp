// 1.13c D2Launch+0x13A90 -- stdcall message handler: hands each of the first N slots of the image array at 0x253C0 to D2Win 10050 (N = the count at 0x25E34) and zeroes the count; creates the control described at image offset 0x24130 through D2Win 10098, appends it to the control list (0x252B8, count 0x25E30; 64 or more entries is fatal), saves it at 0x252AC, then calls D2Win 10069 and D2Win 10201 on it. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25e34;
__attribute__((weak)) uint32_t g_d2launch_25e30;
__attribute__((weak)) uint32_t g_d2launch_252b8_a16[16];
__attribute__((weak)) uint32_t g_d2launch_252ac;
namespace d2launch {
uint32_t f13a90(GE& e, uint32_t)
{
    for (int32_t i = 0; i < (int32_t)g_d2launch_25e34; i++) e.h(0x9922, 0, 0, 0, 0, 0, 0xB20253C0u + 4 * (uint32_t)i);
    g_d2launch_25e34 = 0;
    auto add = [&](uint32_t def) { const uint32_t v = e.h(0x9952, 0, 0, 0, 0, 0, def); g_d2launch_252b8_a16[g_d2launch_25e30] = v; g_d2launch_25e30++; return v; };
    const uint32_t v = add(0xB2024130u);
    g_d2launch_252ac = v;
    e.h(0x99ac, 0, v);
    e.h(0x9904, 0, g_d2launch_252ac);
    return 1;
}
}
