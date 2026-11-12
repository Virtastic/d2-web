// 1.13c D2Launch+0x18C10 -- stdcall message handler (video options panel): runs the worker 0x15D30, loads the sky palette pair through the worker 0x12BF0, then creates six controls through D2Win 10098 appending each to the control list (0x252B8, count 0x25E30; 64 is fatal): descriptors at image offsets 0x216D0 and 0x21700, then by Fog 10227 either 0x21730, 0x21760, 0x21790 (zero) or 0x217F0, 0x21820, 0x21850, saved at 0x25E48, 0x25E4C and 0x25E50, then 0x217C0 saved at 0x25E54 and passed to D2Win 10069 and D2Win 10201. The worker 0x16B10 runs and the three saved sliders are set to 20 through D2Win 10037 (0x25E50, 0x25E48, 0x25E4C). Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25e30;
__attribute__((weak)) uint32_t g_d2launch_252b8_a16[16];
__attribute__((weak)) uint32_t g_d2launch_25e48_a4[4];
namespace d2launch {
uint32_t f18c10(GE& e, uint32_t)
{
    e.h(0x15D30);
    e.h(0x12BF0, 0xB201e99cu, 0, 0, 0, 0, 0xB201e988u);
    auto add = [&](uint32_t def) { const uint32_t v = e.h(0x9952, 0, 0, 0, 0, 0, def); g_d2launch_252b8_a16[g_d2launch_25e30] = v; g_d2launch_25e30++; return v; };
    add(0xB20216d0u);
    add(0xB2021700u);
    const bool x = e.h(0x980e) != 0;
    uint32_t* s = g_d2launch_25e48_a4;
    s[0] = add(x ? 0xB20217f0u : 0xB2021730u);
    s[1] = add(x ? 0xB2021820u : 0xB2021760u);
    s[2] = add(x ? 0xB2021850u : 0xB2021790u);
    s[3] = add(0xB20217c0u);
    e.h(0x99ac, 0, s[3]);
    e.h(0x9904, 0, s[3]);
    e.h(0x16B10);
    e.h(0x99a6, 0, s[2], 0x14);
    e.h(0x99a6, 0, s[0], 0x14);
    e.h(0x99a6, 0, s[1], 0x14);
    return 1;
}
}
