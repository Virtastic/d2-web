// 1.13c D2Launch+0x13B30 -- stdcall message handler (TCP/IP join panel): loads the sky palette pair through the worker 0x12BF0, drops the saved control at 0x252AC through D2Win 10050, then creates seven controls through D2Win 10098 (descriptors at image offsets 0x241F0, 0x241C0, 0x24160, 0x24190, 0x24220, 0x24250 in call order) appending each to the array at 0x253C0 (count 0x25E34; 64 is fatal): the first gets D2Win 10196 with 0, the third its caption (string 0x2B1F) through D2Win 10020, the fourth is the address box saved at 0x25E44, filled from the buffer at 0x25AE0 (read from the Storm registry value LastTcpIp when empty) through D2Win 10185, focused through D2Win 10053 and passed to D2Win 10021; the fifth goes to D2Win 10031. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25e34;
__attribute__((weak)) uint32_t g_d2launch_253c0_a16[16];
__attribute__((weak)) uint32_t g_d2launch_25e44;
__attribute__((weak)) uint32_t g_d2launch_25ae0;
namespace d2launch {
uint32_t f13b30(GE& e, uint32_t)
{
    e.h(0x12BF0, 0xB201e99cu, 0, 0, 0, 0, 0xB201e988u);
    e.h(0x9922, 0, 0, 0, 0, 0, 0xB20252acu);
    auto add = [&](uint32_t def) { const uint32_t v = e.h(0x9952, 0, 0, 0, 0, 0, def); g_d2launch_253c0_a16[g_d2launch_25e34] = v; g_d2launch_25e34++; return v; };
    uint32_t v = add(0xB20241f0u);
    e.h(0x995e, 0, 0, 0, 0, 0, v, 0);
    add(0xB20241c0u);
    v = add(0xB2024160u);
    e.h(0x98c8, 0, v, e.h(0x9b32, 0, 0x2b1f));
    g_d2launch_25e44 = add(0xB2024190u);
    if (!(g_d2launch_25ae0 & 0xff)) e.h(0x9754, 0, 0, 0, 0, 0, 0xB201e204u, 0xB201dc74u, 0, 0xB2025ae0u);
    e.h(0x98bc, 0, g_d2launch_25e44, 0xB2025ae0u);
    e.h(0x988c, 0, 0, 0, 0, 0, g_d2launch_25e44);
    e.h(0x9916, 0, g_d2launch_25e44);
    v = add(0xB2024220u);
    e.h(0x989e, 0, 0, 0, 0, 0, v);
    add(0xB2024250u);
    return 1;
}
}
