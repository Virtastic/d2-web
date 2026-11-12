// 1.13c D2Win+0x17700 -- stdcall message handler: stores 1 in the module global at image offset 0x214C8 and ends in the Storm import 0x7624 with its own argument, whose result it returns.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2win_214c8;
namespace d2w {
uint32_t f17700(GE& e, uint32_t m)
{
    g_d2win_214c8 = 1;
    return e.h(0x7624, 0, 0, 0, 0, 0, m);
}
}
