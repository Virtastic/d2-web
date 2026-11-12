// 1.13c D2Multi+0x8FC0 -- stdcall, one ignored argument: calls the D2Win import 0x7C20 with ECX = the module global at image offset 0x39F88 and EDX = 1, and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2multi_39f88;
namespace d2multi {
uint32_t f8fc0(GE& e, uint32_t)
{
    e.h(0x7C20, 0, g_d2multi_39f88, 1);
    return 1;
}
}
