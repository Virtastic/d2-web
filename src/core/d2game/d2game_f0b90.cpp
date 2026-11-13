// 1.13c D2Game+0xF0B90 -- party-pair update: the player unit id of the unit is compared with the pair word, then the pair is told about each other
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t ff0b90(NE& e, uint32_t a, uint32_t u, uint32_t p)
{
    uint32_t di = 0xffff;
    if (u && e.rd32(u + 0x80) && e.rd32(u) == 0) di = e.h(0x9BEB0, u) & 0xffff;
    const uint32_t p0 = e.rd32(p), p1 = e.rd32(p + 4);
    e.h(0xF02F0, p0, u, 0, 0, 0, a);
    e.h(0xF02F0, u, p0, 0, 0, 0, a);
    e.h(0xF02F0, p1, u, 0, 0, 0, a);
    e.h(0xF02F0, u, p1, 0, 0, 0, a);
    e.h(0xF09E0, 0, 0, 0, u, 0, p0);
    e.h(0xF09E0, 0, 0, 0, u, 0, p1);
    if (di == NE::rd16(p + 8) && di != 0xffff && u != p1) {
        const uint32_t edi = p1 ? e.rd32(p1 + 0xc) : ~0u;
        uint32_t v = 0;
        if (u && e.rd32(u) == 0) v = e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, u) + 0x9c);
        e.h(0xF0720, 0, 0, edi, 0, 0, v, 7);
    }
    e.h(0xF0130, p0); e.edi(0xF0130, u);
    e.h(0xF0130, p1); e.edi(0xF0130, u);
    return e.h(0x8CFB0, u, 0, 0, 0, 0, a);
}
}
