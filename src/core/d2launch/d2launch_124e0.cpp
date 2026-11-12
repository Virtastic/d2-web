// 1.13c D2Launch+0x124E0 -- stdcall message handler: calls D2Win 10131 with ECX = the control at image offset 0x25ACC and EDX = 1 when the message's +0xC word is non-zero, else 0. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25acc;
namespace d2launch {
uint32_t f124e0(GE& e, uint32_t m)
{
    e.h(0x987a, 0, g_d2launch_25acc, e.rd32(m + 0xc) ? 1 : 0);
    return 1;
}
}
