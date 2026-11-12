// 1.13c D2Multi+0x8F90 -- stdcall, three ignored arguments: calls the D2Win import 0x7CB6 with ECX = the module global at image offset 0x3A038 and EDX = -1, and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2multi_3a038;
namespace d2multi {
uint32_t f8f90(GE& e, uint32_t, uint32_t, uint32_t)
{
    e.h(0x7CB6, 0, g_d2multi_3a038, ~0u);
    return 1;
}
}
