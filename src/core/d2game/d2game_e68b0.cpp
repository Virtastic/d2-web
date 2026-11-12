// 1.13c D2Game+0xE68B0 -- EAX = a stale register, EDX = quest record q (+4 game, +0xB state byte, +0x14 byte, +0x18 data). A state other than 4 sets it to 4, clears +0x14 and runs the registration worker 0xDFCC0 (EAX 0, EBX = the callback 0xE5BE0, EDI = the +4 game, stack 0); the data byte 0 is cleared either way. Returns EAX with its low byte 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fe68b0(GE& e, uint32_t eax, uint32_t q)
{
    const uint32_t d = e.rd32(q + 0x18);
    if (e.rd8(q + 0xb) != 4) {
        e.wr8(q + 0xb, 4);
        e.wr8(q + 0x14, 0);
        eax = e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xe5be0, 0, 0);
        e.j(0xDFCC0, e.rd32(q + 4));
    }
    e.wr8(d, 0);
    return (eax & ~0xffu) | 1;
}
}
