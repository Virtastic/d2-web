// 1.13c D2Game+0xBA270 -- quest start test: an idle active quest moves to state 1; a quest in state 5 or an inactive one defers to its parent quest
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fba270(NE& e, uint32_t eax0, uint32_t q)
{
    const uint32_t c = e.rd8(q + 0xc);
    if (!c && e.rd8(q + 9) == 1) { e.wr8(q + 0xc, 1); e.say(e.rd32(q + 4), 1); return 1; }
    uint32_t eax = NE::lo(eax0, c);
    if (c != 5) { eax = NE::lo(eax, e.rd8(q + 9)); if (e.rd8(q + 9)) return NE::lo(eax, 1); }
    const uint32_t n = e.h(0x33F30, e.rd32(q + 4), e.rd32(q + 0x10));
    if (!n) return 0;
    e.h(0xF8D14, 0, 0, 0, 0, 0, e.rd32(n + 0xf0));          // the code-pointer check, then the quest's own state callback with ECX = the record (not reached by the vectors)
    return e.h(e.rd32(n + 0xf0) & 0xffffffu, 0, n);
}
}
