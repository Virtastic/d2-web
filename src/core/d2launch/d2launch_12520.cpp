// 1.13c D2Launch+0x12520 -- stdcall message handler: calls D2Win 10131 with ECX = the control at image offset 0x25AC8 and EDX = 1 when the message's +0xC word is non-zero, else 0. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25ac8;
namespace d2launch {
uint32_t f12520(GE& e, uint32_t m)
{
    e.h(0x987a, 0, g_d2launch_25ac8, e.rd32(m + 0xc) ? 1 : 0);
    return 1;
}
}
