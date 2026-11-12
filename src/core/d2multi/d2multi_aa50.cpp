// 1.13c D2Multi+0xAA50 -- stdcall, one ignored argument: calls the worker at 0x8790 and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2multi {
uint32_t faa50(GE& e, uint32_t)
{
    e.h(0x8790);
    return 1;
}
}
