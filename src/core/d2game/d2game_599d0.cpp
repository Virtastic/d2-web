// 1.13c D2Game+0x599D0 -- ECX = a, EDX = b, one ignored stack argument: calls 0x58A40 (EAX = b, one stack word = a) and returns 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f599d0(GE& e, uint32_t a, uint32_t b, uint32_t)
{
    e.h(0x58A40, b, 0, 0, 0, 0, a);
    return 0;
}
}
