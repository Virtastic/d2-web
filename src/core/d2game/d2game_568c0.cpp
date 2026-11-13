// 1.13c D2Game+0x568C0 -- npc message picker of the act 5 quest 0x26: writes the message index through the third stack word and answers 1
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f568c0(NE& e, uint32_t q, uint32_t s0, uint32_t buf, uint32_t out)
{
    e.wr8(out, 0);
    uint32_t eax = e.fl(buf, 0x26, 0);
    if (eax) return NE::lo(eax, 1);
    bool done = (eax = e.fl(buf, 0x26, 1)) != 0;
    if (!done) done = (eax = e.fl(buf, 0x26, 0xd)) != 0;
    if (done) { eax = e.fl(buf, 0x26, 4); e.wr8(out, (eax ? 1u : 0u) + 4); return NE::lo(eax, 1); }
    if (!e.rd8(q + 9)) return NE::lo(eax, 1);
    eax = e.fl(buf, 0x26, 0xe);
    if (eax) { e.wr8(out, 0xc); return NE::lo(eax, 1); }
    if (e.rd8(q + 0xc) < 4) e.wr8(out, e.rd8(q + 0xb));
    return NE::lo(eax, 1);
}
}
