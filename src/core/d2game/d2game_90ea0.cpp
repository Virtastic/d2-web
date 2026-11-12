// 1.13c D2Game+0x90EA0 -- EAX = whatever the caller left (returned for a null unit), ECX = unit, EDX = second argument. The D2Common import 0xAD06(unit) gives a value capped at 0x4D; 0xAD42(unit, value) and 0xAC7C(unit, value) follow, then the fastcall import 0xA628 (ECX = unit + 0x20, EDX = the argument), then with the unit's +0x2C path 0xAAFC(path, 10), 0xAB14(path, value) and 0xAAF0(path, unit, 0), whose result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f90ea0(GE& e, uint32_t a, uint32_t u, uint32_t x)
{
    if (!u) return a;
    uint32_t v = e.h(0xad06, 0, 0, 0, 0, 0, u);
    if ((int32_t)v >= 0x4e) v = 0x4d;
    e.h(0xAD42, 0, 0, 0, 0, 0, u, v);
    e.h(0xAC7C, 0, 0, 0, 0, 0, u, v);
    const uint32_t p = e.rd32(u + 0x2c);
    e.h(0xA628, 0, u + 0x20, x, 0, 0);
    e.h(0xAAFC, 0, 0, 0, 0, 0, p, 0xa);
    e.h(0xAB14, 0, 0, 0, 0, 0, p, v);
    return e.h(0xAAF0, 0, 0, 0, 0, 0, p, u, 0);
}
}
