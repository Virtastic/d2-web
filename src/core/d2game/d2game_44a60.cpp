// 1.13c D2Game+0x44A60 -- ECX = unit, EDX = state, one stack argument (a second unit, may be null). The helper 0x9EA30 runs (EBX = unit, stack = state, 1). A null argument returns 0. Otherwise the D2Common import 0xA712(argument, 0x15E, 0) gives a value and the fastcall import 0xA3F4 (ECX = unit, EDX = value) an item; none returns 0. Else the helper 0x449F0 (EAX = unit, stack 10) gives a value for 0xAD9C(item, value), and the result of 0xA3CA(unit, state, 0) is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f44a60(GE& e, uint32_t u, uint32_t st, uint32_t a)
{
    e.h(0x9EA30, 0, 0, 0, u, 0, st, 1);
    if (!a) return 0;
    const uint32_t v = e.h(0xA712, 0, 0, 0, 0, 0, a, 0x15e, 0);
    const uint32_t it = e.h(0xA3F4, 0, u, v, 0, 0);
    if (!it) return 0;
    const uint32_t w = e.h(0x449F0, u, 0, 0, 0, 0, 0xa);
    e.h(0xAD9C, 0, 0, 0, 0, 0, it, w);
    return e.h(0xA3CA, 0, 0, 0, 0, 0, u, st, 0);
}
}
