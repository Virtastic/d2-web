// 1.13c D2Game+0xC5BE0 -- quest two-step advance. ECX = game, EDX = quest record {+4 game, +0xB state, +0x14 flag, +0x18 data {+8, +9}}.
// data+9 clear: set it and post callback D2Game+0xC4DE0 through 0xDFCC0 (EAX 0, ECX, EBX = callback, EDI = ECX, word 0); result 0.
// data+9 set: clear +0x14, state 4, post callback D2Game+0xC4E30 (EDI = [rec+4]), clear data+8; result 1. The upper bytes of EAX are DFCC0's.
#include "../fn/hookenv.h"
namespace d2g {
uint32_t fc5be0(HookEnv& e, uint32_t ecx, uint32_t q)
{
    const uint32_t d = e.rd32(q + 0x18);
    if (!e.rd8(d + 9)) {
        e.wr8(d + 9, 1);
        const uint32_t r = e.h(0xDFCC0, 0, ecx, 0, 0xB2000000u | 0xC4DE0u, 0, 0);
        e.edi(0xDFCC0, ecx);
        return r & ~0xFFu;
    }
    e.wr8(q + 0x14, 0); e.wr8(q + 0xB, 4);
    const uint32_t r = e.h(0xDFCC0, 0, ecx, 0, 0xB2000000u | 0xC4E30u, 0, 0);
    e.edi(0xDFCC0, e.rd32(q + 4));
    e.wr8(d + 8, 0);
    return (r & ~0xFFu) | 1;
}
}
