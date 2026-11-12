// 1.13c D2Launch+0x165D0 -- stdcall message handler (gateway panel): runs the worker 0x1B310, then creates six controls through D2Win 10098 (descriptors at image offsets 0x21C70, 0x21CA0, 0x21CD0, 0x21D00, 0x21D30, 0x21D60) appending each to the control list (0x252B8, count 0x25E30; 64 is fatal): the second and third get their captions (strings 0x2B2F, 0x2B30) through D2Win 10020, the fourth is the gateway list saved at 0x25500. The gateway table comes from the interface slot +0x98: each of its entries (count at +8) is named through BNGatewayAccess::Name and added through D2Win 10167. The list then gets its selection: with the selection D2Win 10030 and the page D2Win 10103, the current gateway (+0xC, minus one) outside the page is first set through D2Win 10037 and then 0, and that goes to D2Win 10079. The worker 0x13D60 runs. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25e30;
__attribute__((weak)) uint32_t g_d2launch_252b8_a16[16];
__attribute__((weak)) uint32_t g_d2launch_25500;
__attribute__((weak)) uint32_t g_d2launch_256c4;
namespace d2launch {
uint32_t f165d0(GE& e, uint32_t)
{
    e.h(0x1B310);
    auto add = [&](uint32_t def) { const uint32_t v = e.h(0x9952, 0, 0, 0, 0, 0, def); g_d2launch_252b8_a16[g_d2launch_25e30] = v; g_d2launch_25e30++; return v; };
    add(0xB2021c70u);
    uint32_t v = add(0xB2021ca0u);
    e.h(0x98c8, 0, v, e.h(0x9b32, 0, 0x2b2f));
    v = add(0xB2021cd0u);
    e.h(0x98c8, 0, v, e.h(0x9b32, 0, 0x2b30));
    g_d2launch_25500 = add(0xB2021d00u);
    add(0xB2021d30u);
    add(0xB2021d60u);
    const uint32_t gw = e.h(0x9b38, g_d2launch_256c4);
    const uint32_t cur = e.rd32(gw + 0xc);
    for (int32_t i = 1; i <= (int32_t)e.rd32(gw + 8); i++) e.h(0x9976, 0, g_d2launch_25500, e.h(0x9b4a, 0, 0, 0, 0, 0, gw, (uint32_t)i));
    const int32_t top = (int32_t)e.h(0x9910, 0, g_d2launch_25500);
    const int32_t page = (int32_t)e.h(0x9958, 0, g_d2launch_25500);
    uint32_t d = cur - 1;
    if ((int32_t)d < top || (int32_t)(d - (uint32_t)top) > page) { e.h(0x99a6, 0, g_d2launch_25500, d); d = 0; }
    e.h(0x9970, 0, g_d2launch_25500, d);
    e.h(0x13D60);
    return 1;
}
}
