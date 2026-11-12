// 1.13c D2Launch+0xA540 -- stdcall message handler: when Fog 10227 answers non-zero, hands the address of each of the first N slots of the image array at 0x253C0 to D2Win 10050 (N = the count at 0x25E34) and zeroes the count. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25e34;
namespace d2launch {
uint32_t fa540(GE& e, uint32_t)
{
    if (e.h(0x980e)) {
        for (int32_t i = 0; i < (int32_t)g_d2launch_25e34; i++) e.h(0x9922, 0, 0, 0, 0, 0, 0xB20253C0u + 4 * (uint32_t)i);
        g_d2launch_25e34 = 0;
    }
    return 1;
}
}
