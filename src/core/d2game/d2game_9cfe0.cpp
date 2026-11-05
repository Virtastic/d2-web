// Monster skill guard: EAX = skill-state record, ECX = unit, stack game; ret 4. A record without a +8 owner answers 0. The two early
// allow checks (unit+game in EAX/stack, then game+record on the stack) answer 1. Otherwise both class gates (EAX = the unit's +4 id or
// -1 for none, EDX = 1 then 2) must pass; the unit's monster row (three ordinal lookups) must carry a nonzero +0x2F byte that is below
// the record's +0x14 count, and the exemption check (EAX = the owner, ECX = unit) must say no. A three-dword request {0, 0x7FFFFFFF,
// owner} goes to the target picker (EAX = 0xB, ECX = request, EDX = game; stack unit, 0): a nonzero word 0 back makes it the record's
// new owner and word 1 its new count (answer 0); a zero word 0 fires the follow-up (AL = 4; stack game, unit) and answers 1. A rule over
// E (E::h = the callee call, E::blk = the request the picker is handed, E::rd32/rd8/wr32 = memory).
#ifndef D2GAME_RULE_9CFE0
#define D2GAME_RULE_9CFE0
#include <cstdint>

namespace g9cfe0 {
template <class E> uint32_t Guard(E& e, uint32_t rec, uint32_t unit, uint32_t game)
{
    if (!e.rd32(rec + 8)) return 0;
    if (e.h(0x9CA60, unit, 0, 0, 0, 0, game)) return 1;
    if (e.h(0x9CDE0, 0, 0, 0, 0, 0, game, rec)) return 1;
    if (!e.h(0x15D0, unit ? e.rd32(unit + 4) : 0xFFFFFFFFu, 0, 1)) return 0;
    if (!e.h(0x1560, unit ? e.rd32(unit + 4) : 0xFFFFFFFFu, 0, 2)) return 0;
    const uint32_t m = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    if (!m) return 0;
    const uint32_t row = e.h(0xA74E, 0, 0, 0, 0, 0, e.h(0xA556, 0, 0, 0, 0, 0, m));
    const uint32_t cap = e.rd8(row + 0x2F);
    if (!cap || e.rd32(rec + 0x14) <= cap) return 0;
    const uint32_t owner = e.rd32(rec + 8);
    if (e.h(0xB0E90, owner, unit)) return 0;
    uint32_t req[3] = { 0, 0x7FFFFFFFu, owner };
    const uint32_t sent[3] = { req[0], req[1], req[2] };
    e.h(0xB17B0, 0xB, e.stk(req), game, 0, 0, unit, 0);
    e.blk(0xB17B0, sent, 3);   // logged as the picker saw it, before its answer lands
    if (req[0]) { e.wr32(rec + 8, req[0]); e.wr32(rec + 0x14, req[1]); return 0; }
    e.h(0xF1B10, 4, 0, 0, 0, 0, game, unit);
    return 1;
}
}
#endif
