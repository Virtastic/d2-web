// 1.13c D2Multi+0x8F60 -- stdcall, one ignored argument: asks the D2Win import 0x7E72 for an event handle and waits on it with a zero timeout; a wait that answers 0, or a set flag at image offset 0x3A09C, calls D2Win 10199. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2multi_3a09c;
namespace d2multi {
uint32_t f8f60(GE& e, uint32_t)
{
    uint32_t h = e.h(0x7E72);
    uint32_t w = e.h(0x162E8, 0, 0, 0, 0, 0, h, 0);
    if (w == 0 || g_d2multi_3a09c) e.h(0x7CA4);
    return 1;
}
}
