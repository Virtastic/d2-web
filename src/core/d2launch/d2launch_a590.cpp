// 1.13c D2Launch+0xA590 -- stdcall message handler (character list panel): does nothing unless Fog 10227 answers non-zero. Drops the first N controls of the image array at 0x253C0 (count 0x25E34) through D2Win 10050, then creates four through D2Win 10098 (descriptors at image offsets 0x23800, 0x23830, 0x23860, 0x23890) appending each to that array: the first gets D2Win 10196 with 1, each D2Win 10001 with 1, and the last three text lines (strings 0x58CE to 0x58D0) through D2Win 10000 with three zeros. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25e34;
__attribute__((weak)) uint32_t g_d2launch_253c0_a16[16];
namespace d2launch {
uint32_t fa590(GE& e, uint32_t)
{
    if (!e.h(0x980e)) return 1;
    for (int32_t i = 0; i < (int32_t)g_d2launch_25e34; i++) e.h(0x9922, 0, 0, 0, 0, 0, 0xB20253C0u + 4 * (uint32_t)i);
    g_d2launch_25e34 = 0;
    auto add = [&](uint32_t def) { const uint32_t v = e.h(0x9952, 0, 0, 0, 0, 0, def); g_d2launch_253c0_a16[g_d2launch_25e34] = v; g_d2launch_25e34++; return v; };
    uint32_t v = add(0xB2023800u);
    e.h(0x995e, 0, 0, 0, 0, 0, v, 1);
    e.h(0x98b0, 0, 0, 0, 0, 0, v, 1);
    v = add(0xB2023830u);
    e.h(0x98b0, 0, 0, 0, 0, 0, v, 1);
    v = add(0xB2023860u);
    e.h(0x98b0, 0, 0, 0, 0, 0, v, 1);
    v = add(0xB2023890u);
    e.h(0x98b0, 0, 0, 0, 0, 0, v, 1);
    for (uint32_t k = 0x58ce; k <= 0x58d0; k++) e.h(0x99be, 0, v, e.h(0x9b32, 0, k), 0, 0, 0, 0, 0);
    return 1;
}
}
