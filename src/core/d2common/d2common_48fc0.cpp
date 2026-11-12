// 1.13c D2Common+0x48FC0 -- ECX, EDX = an optional unit and seven stack arguments, ret 28 (first stack argument = item unit, second = property record, NULL fatal at line 0x37b): draws a value between the record bounds through 0x464D0, looks up the item record (0x719A0) by the class of the EDX unit when it is an item, else of the first stack argument (-1 without one), then applies the value through 0x47EF0 (EAX = 0, ECX = the item, EDI = the stat, stack = ECX value, EDX value, value, fourth, fifth, sixth stack argument) for up to three stats: 0x15 unless the item is of type 0x2D (0x24430) with byte +0xfe clear and byte +0x102 set, 0x17 unless it is of that type with byte +0x102 clear and byte +0xfe set, and 0x9f unless it is of that type and 0x24C30 answers 0. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2common {
uint32_t f48fc0(GE& e, uint32_t c, uint32_t d, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6)
{
    (void)s2; (void)s6;
    if (!s1) {
        const uint32_t f = e.h(0x921c);
        e.h(0x9216, 0, 0, 0, 0, 0, 0xB208a728u, f, 891);
        e.h(0x1b0d, 0, 0, 0, 0, 0, 0xffffffffu);
        return 0;
    }
    const uint32_t v = e.h(0x464d0, e.rd32(s1 + 0xc), e.rd32(s1 + 8), s0);
    const uint32_t w[2] = { s4, s5 };
    uint32_t id;
    if (d && e.rd32(d) == 4) id = e.rd32(d + 4);
    else if (!s0) id = 0xffffffffu;
    else id = e.rd32(s0 + 4);
    const uint32_t t = e.h(0x719a0, 0, 0, 0, 0, 0, id);
    if (!(e.h(0x24430, 0, 0, 0, 0, 0, s0, 0x2d) && !e.rd8(t + 254) && e.rd8(t + 258))) {
        e.ed(21);
        e.h(0x47ef0, 0, s0, 0, 0, 0, c, d, v, s3);
        e.x(0x47ef0, w, 2);
        e.edi(0x47ef0, 21);
    }
    if (!(e.h(0x24430, 0, 0, 0, 0, 0, s0, 0x2d) && !e.rd8(t + 258) && e.rd8(t + 254))) {
        e.ed(23);
        e.h(0x47ef0, 0, s0, 0, 0, 0, c, d, v, s3);
        e.x(0x47ef0, w, 2);
        e.edi(0x47ef0, 23);
    }
    if (!(e.h(0x24430, 0, 0, 0, 0, 0, s0, 0x2d) && !e.h(0x24c30, 0, 0, 0, 0, 0, s0))) {
        e.ed(159);
        e.h(0x47ef0, 0, s0, 0, 0, 0, c, d, v, s3);
        e.x(0x47ef0, w, 2);
        e.edi(0x47ef0, 159);
    }
    return 1;
}
}
