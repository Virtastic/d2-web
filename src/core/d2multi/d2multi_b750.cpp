// 1.13c D2Multi+0xB750 -- stdcall, one ignored argument: calls the worker at 0xE760 and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2multi {
uint32_t fb750(GE& e, uint32_t)
{
    e.h(0xE760);
    return 1;
}
}
