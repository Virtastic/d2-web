// 1.13c D2Launch+0x148D0 -- stdcall message handler: asks the interface table (image offset 0x256C4) slot +0x4C for the state; when it is 0x65, or the flag at 0x25294 is set, calls D2Win 10199. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_256c4;
__attribute__((weak)) uint32_t g_d2launch_25294;
namespace d2launch {
uint32_t f148d0(GE& e, uint32_t)
{
    const uint32_t v = e.h(0x9b32);
    if (v == 0x65 || g_d2launch_25294) e.h(0x994c);
    return 1;
}
}
