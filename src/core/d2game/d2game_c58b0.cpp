// 1.13c D2Game+0xC58B0 -- quest state-3 setter. ECX = game (passed through), EDX = quest record {+4 game, +0xB state, +0x14 flag, +0x18 data}.
// A state other than 3, 4 or 5 clears +0x14, sets +0xB to 3 and posts the callback D2Game+0xC4D30 through 0xDFCC0 (EAX 0, EBX = callback, ECX,
// EDI = [rec+4], one stack word 0). The data block's first byte is then cleared; AL = 1, the upper bytes of EAX are what the last call left.
#include "../fn/hookenv.h"
namespace d2g {
uint32_t fc58b0(HookEnv& e, uint32_t ecx, uint32_t q)
{
    const uint32_t st = e.rd8(q + 0xB), d = e.rd32(q + 0x18);
    uint32_t eax = 0;   // entry EAX (zero in the capture), AL replaced by the state byte
    if (st != 5 && st != 4 && st != 3) {
        e.wr8(q + 0x14, 0); e.wr8(q + 0xB, 3);
        eax = e.h(0xDFCC0, 0, ecx, 0, 0xB2000000u | 0xC4D30u, 0, 0);
        e.edi(0xDFCC0, e.rd32(q + 4));
    }
    e.wr8(d, 0);
    return (eax & ~0xFFu) | 1;
}
}
