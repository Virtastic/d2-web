// 1.13c D2Net+0x62D0 -- ECX unused, EDX = value, two ignored stack arguments: builds the one-byte message 0x70 in a local and calls the Fog import 0x5E5E with (the module global at image offset 0xB424, the message, 1, value). Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2net_b424;
namespace d2n {
uint32_t f62d0(GE& e, uint32_t, uint32_t d, uint32_t, uint32_t)
{
    uint8_t msg[4] = { 0, 0, 0, 0x70 };
    e.h(0x5E5E, 0, 0, 0, 0, 0, g_d2net_b424, e.stk(&msg[3]), 1, d);
    return 1;
}
}
