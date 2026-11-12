// 1.13c D2Game+0x38E60 -- ECX = unit, EDX = state, one ignored stack argument: clears the state through the D2Common import 0xA3CA(unit, state, 0) and returns its result.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f38e60(GE& e, uint32_t unit, uint32_t state, uint32_t)
{
    return e.h(0xA3CA, 0, 0, 0, 0, 0, unit, state, 0);
}
}
