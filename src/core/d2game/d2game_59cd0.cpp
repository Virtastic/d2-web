// 1.13c D2Game+0x59CD0 -- EAX = whatever the caller left, EDX = quest record. Unless the record's +0xB byte is already 4 it becomes 4, the +0x14 byte 0 and the registration worker 0xDFCC0 runs (EAX 0, EBX = the callback 0x590C0, EDI = the record's +4 game, stack 0). The +0x85 byte of the record's +0x18 data is cleared. AL = 1 over the upper bytes of the worker's result, or of EAX when it did not run.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f59cd0(GE& e, uint32_t a, uint32_t, uint32_t q)
{
    const uint32_t d = e.rd32(q + 0x18);
    uint32_t v = a;
    if (e.rd8(q + 0xb) != 4) {
        e.wr8(q + 0xb, 4);
        e.wr8(q + 0x14, 0);
        v = e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0x590c0, 0, 0);
        e.j(0xDFCC0, e.rd32(q + 4));
    }
    e.wr8(d + 0x85, 0);
    return (v & ~0xffu) | 1;
}
}
