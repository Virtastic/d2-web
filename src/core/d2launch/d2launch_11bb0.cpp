// 1.13c D2Launch+0x11BB0 -- stdcall, one ignored argument: clears the module global at image offset 0x25ad0, calls the D2Sound import 0x9A48 with ECX = 0xB4, then the D2Win import 0x994C, and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25ad0;
namespace d2launch {
uint32_t f11bb0(GE& e, uint32_t)
{
    g_d2launch_25ad0 = 0;
    e.h(0x9A48, 0, 0xb4);
    e.h(0x994C);
    return 1;
}
}
