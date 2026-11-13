// 1.13c D2Client+0x40130 -- ECX = unit, EDX = value. A null unit does nothing (EAX is left alone). Otherwise D2Common ordinal 10534 gives the unit's mode; a mode of 0x4E or more (signed) is replaced by 0x4D through ordinals 10558 and 10970 (unit, 0x4D). Then, with the path at unit +0x2C: ordinal 10614 (ECX = unit + 0x20, EDX = value), 10647 (path, 10), 10168 (path, mode), 10334 (path, unit, 0), whose result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2cl {
uint32_t f40130(GE& e, uint32_t eax, uint32_t u, uint32_t v)
{
    if (!u) return eax;
    uint32_t m = e.h(0xC430, 0, 0, 0, 0, 0, u);
    if ((int32_t)m >= 0x4e) {
        m = 0x4d;
        e.h(0xC4A2, 0, 0, 0, 0, 0, u, m);
        e.h(0xC2C8, 0, 0, 0, 0, 0, u, m);
    }
    const uint32_t path = e.rd32(u + 0x2c);
    e.h(0xC3B2, 0, u + 0x20, v);
    e.h(0xC38E, 0, 0, 0, 0, 0, path, 0xa);
    e.h(0xC3B8, 0, 0, 0, 0, 0, path, m);
    return e.h(0xC376, 0, 0, 0, 0, 0, path, u, 0);
}
}
