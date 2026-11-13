// 1.13c D2Game+0xC69D0 -- quest first-time latch. ECX = quest record {+4 game, +9 active, +0xC done}. With +0xC clear and +9 == 1: set +0xC,
// format "changing state to %d" into a stack buffer (wsprintfA, no state), report it through 0xCBE70 (EDX = the buffer, words [rec+4] and 4)
// when the debug global D2Game+0x131C20 is non-zero, then call 0xC1FF0 (no arguments). AL = 1; the upper bytes of EAX are 0xC1FF0's, else the entry EAX (zero).
#include "../fn/hookenv.h"
namespace d2g {
uint32_t fc69d0(HookEnv& e, uint32_t q, uint32_t dbg)
{
    uint32_t eax = 0;
    if (!e.rd8(q + 0xC) && e.rd8(q + 9) == 1) {
        e.wr8(q + 0xC, 1);
        char buf[128];
        if (dbg) e.h(0xCBE70, 0, 0, e.stk(buf), 0, 0, e.rd32(q + 4), 4);
        eax = e.h(0xC1FF0);
    }
    return (eax & ~0xFFu) | 1;
}
}
