// 1.13c D2Game+0xCB580 -- ECX = client, EDX = value: calls 0x8B470 (EAX = value, one stack word = client) and returns its result.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fcb580(GE& e, uint32_t client, uint32_t value)
{
    return e.h(0x8B470, value, 0, 0, 0, 0, client);
}
}
