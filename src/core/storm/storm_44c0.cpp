// 1.13c Storm+0x44C0 -- ECX = holder: hands the dword it holds to the worker 0x30580 and returns its result.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace stm {
uint32_t f44c0(GE& e, uint32_t p)
{
    return e.h(0x30580, 0, 0, 0, 0, 0, e.rd32(p));
}
}
