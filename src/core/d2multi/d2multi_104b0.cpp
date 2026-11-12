// 1.13c D2Multi+0x104B0 -- stdcall, one ignored argument: runs the worker 0x121F0; a non-zero module global at image offset 0x3A0D0 is then handed to the D2Win import 0x7C26. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2multi_3a0d0;
namespace d2multi {
uint32_t f104b0(GE& e, uint32_t)
{
    e.h(0x121F0);
    if (g_d2multi_3a0d0) e.h(0x7C26, 0, 0, 0, 0, 0, g_d2multi_3a0d0);
    return 1;
}
}
