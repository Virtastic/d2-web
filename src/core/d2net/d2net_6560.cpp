// 1.13c D2Net+0x6560 -- ECX unused, EDX = value, two ignored stack arguments: builds the two-byte message AF 01 in a local and calls the worker 0x63E0 with (0, value, the message, 2). Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2n {
uint32_t f6560(GE& e, uint32_t, uint32_t d, uint32_t, uint32_t)
{
    uint8_t msg[4] = { 0xaf, 1, 0, 0 };
    e.h(0x63E0, 0, 0, 0, 0, 0, 0, d, e.stk(msg), 2);
    return 1;
}
}
