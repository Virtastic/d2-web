// 1.13c D2Multi+0x8A40 -- stdcall, one ignored argument: stores 4 in the module global at image offset 0x3a0dc, then calls the D2Win import at 0x7CA4 and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2multi_3a0dc;
namespace d2multi {
uint32_t f8a40(GE& e, uint32_t)
{
    g_d2multi_3a0dc = 4;
    e.h(0x7CA4);
    return 1;
}
}
