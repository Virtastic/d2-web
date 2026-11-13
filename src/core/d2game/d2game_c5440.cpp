// 1.13c D2Game+0xC5440 -- quest-event predicate. ECX = quest record, EDX = event code, three stack words (ret 0xC).
// Only code 0xF4 is considered: if the D2Common import 0xA568(s1, 0xA, 1) answers 1 the result is 1, otherwise the
// local helper 0xC4030 decides (EDI = ECX, ESI = s0, one stack word = the address of s1, which it overwrites); its AL
// non-zero gives 1. Every other path returns AL = 0. The upper bytes of EAX are whatever the last call left; on entry they are zero here.
#include "../fn/hookenv.h"
namespace d2g {
uint32_t fc5440(HookEnv& e, uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t /*s2*/)
{
    if (edx != 0xF4) return 0;
    if (e.h(0xA568, 0, 0, 0, 0, 0, s1, 0xA, 1) == 1) return 1;
    uint32_t slot = s1;   // the callee takes this stack slot's address
    const uint32_t v = e.h(0xC4030, 0, 0, 0, 0, s0, e.stk(&slot));
    e.edi(0xC4030, ecx);
    return (v & 0xFF) ? ((v & ~0xFFu) | 1) : (v & ~0xFFu);
}
}
