// 1.13c D2Game+0xC6AF0 -- quest check-and-post. ECX = this (passed through), EDX = quest record {+4 game, +0x18 data {+1, +2, +3}}.
// Clears data+1. If data+2 is clear: ask 0xC6A30 (ECX, ESI = rec); a zero AL sets data+3 and returns; otherwise set data+2 and post callback
// D2Game+0xC3750 through 0xDFCC0 (EAX 0, ECX, EBX = callback, EDI = [rec+4], word 0). AL = 1, the upper bytes of EAX are the last call's (entry EAX is zero).
#include "../fn/hookenv.h"
namespace d2g {
uint32_t fc6af0(HookEnv& e, uint32_t ecx, uint32_t q)
{
    const uint32_t d = e.rd32(q + 0x18);
    uint32_t eax = 0;
    e.wr8(d + 1, 0);
    if (!e.rd8(d + 2)) {
        eax = e.h(0xC6A30, 0, ecx, 0, 0, q);
        if (!(eax & 0xFF)) { e.wr8(d + 3, 1); return (eax & ~0xFFu) | 1; }
        e.wr8(d + 2, 1);
        eax = e.h(0xDFCC0, 0, 0 /* ECX is an uninitialised local in retail; not compared */, 0, 0xB2000000u | 0xC3750u, 0, 0);
        e.edi(0xDFCC0, e.rd32(q + 4));
    }
    return (eax & ~0xFFu) | 1;
}
}
