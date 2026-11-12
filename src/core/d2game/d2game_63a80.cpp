// 1.13c D2Game+0x63A80 -- EDX = quest record: its +0x14 byte becomes 0 and its +0xB byte 3, the registration worker 0xDFCC0 runs (EAX 0, EBX = the callback 0x62C00, EDI = the record's +4 game, stack 0); AL = 1 over the upper bytes of the worker's result.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f63a80(GE& e, uint32_t, uint32_t q)
{
    e.wr8(q + 0x14, 0);
    e.wr8(q + 0xb, 3);
    const uint32_t v = e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0x62c00, 0, 0);
    e.j(0xDFCC0, e.rd32(q + 4));
    return (v & ~0xffu) | 1;
}
}
