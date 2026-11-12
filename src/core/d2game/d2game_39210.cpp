// 1.13c D2Game+0x39210 -- EAX = whatever the caller left (returned for a null unit), ECX = unit. The D2Common import 0xAD06(unit) gives a value capped at 0xFF (signed); with the unit's +0x2C path the import 0xAAC6(path) gives the second argument of the fastcall import 0xA628 (ECX = unit + 0x20), then 0xAAFC(path, 10), 0xAB14(path, value) and 0xAAF0(path, unit, 0), whose result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f39210(GE& e, uint32_t a, uint32_t u)
{
    if (!u) return a;
    uint32_t v = e.h(0xad06, 0, 0, 0, 0, 0, u);
    if ((int32_t)v > 0xff) v = 0xff;
    const uint32_t p = e.rd32(u + 0x2c);
    const uint32_t w = e.h(0xaac6, 0, 0, 0, 0, 0, p);
    e.h(0xA628, 0, u + 0x20, w, 0, 0);
    e.h(0xAAFC, 0, 0, 0, 0, 0, p, 0xa);
    e.h(0xAB14, 0, 0, 0, 0, 0, p, v);
    return e.h(0xAAF0, 0, 0, 0, 0, 0, p, u, 0);
}
}
