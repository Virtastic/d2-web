// 1.13c D2Game+0xC5310 -- quest-event callback, ECX = quest record, EDX = event record (+0 game, +8 trigger {+4 id}, +0xC unit {+0xC id}, +0x14 argument), plain ret (leftover EAX mirrored). Reads state 0xA flags 0xD (a) and 0 (b) of the unit's quest-flag buffer into two stack slots; only trigger id 0xF4 acts (without a trigger EAX = 0, another id leaves EAX = the id). Flag 1 set calls 0x33EB0 mode 4; else 0xC4030(&local) (ESI = unit; writes the byte local, 0xFF first) gives mode = local when it answers nonzero and local != 0xFF; else 0x337D0 (EDX = quest, ESI = unit id) finding the unit in the quest's id list calls mode 5; else b == 1 with a in {0, 1} returns a, local above 9 needs flag 3 and becomes 6, and the call is made with mode = local. The out byte sits 1 below the b slot in the retail frame, so the port keeps the same 9-byte frame (local, b, a) for the callee's word write.
#include <cstdint>
#include <cstring>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc5310(GE& e, uint32_t q, uint32_t ctx)
{
    const uint32_t game = e.rd32(ctx), unit = e.rd32(ctx + 0xc);
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t ent = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    uint8_t fr[12] = {};   // fr[0] = local byte (esp+0x13), fr[1..4] = b (esp+0x14), fr[5..8] = a (esp+0x18)
    uint32_t v = e.h(0xA568, 0, 0, 0, 0, 0, ent, 0xa, 0xd); memcpy(fr + 5, &v, 4);
    v = e.h(0xA568, 0, 0, 0, 0, 0, ent, 0xa, 0); memcpy(fr + 1, &v, 4);
    const uint32_t p8 = e.rd32(ctx + 8);
    if (!p8) return 0;
    const uint32_t id = e.rd32(p8 + 4);
    if (id != 0xf4) return id;
    const uint32_t arg = e.rd32(ctx + 0x14);
    if (e.h(0xA568, 0, 0, 0, 0, 0, ent, 0xa, 1)) return e.h(0x33EB0, 4, q, 0, 0, 0, arg, 0xf4);
    const uint32_t al = e.h(0xC4030, 0, 0, 0, 0, unit, (uint32_t)(uintptr_t)fr);
    if ((al & 0xff) && fr[0] != 0xff) return e.h(0x33EB0, fr[0], q, 0, 0, 0, arg, 0xf4);
    const uint32_t cid = unit ? e.rd32(unit + 0xc) : ~0u;
    if (e.h(0x337D0, 0, 0, q, 0, cid)) return e.h(0x33EB0, 5, q, 0, 0, 0, arg, 0xf4);
    uint32_t b, a; memcpy(&b, fr + 1, 4); memcpy(&a, fr + 5, 4);
    if (b == 1 && (a == 0 || a == 1)) return a;
    if (fr[0] > 9) {
        const uint32_t r = e.h(0xA568, 0, 0, 0, 0, 0, ent, 0xa, 3);
        if (!r) return 0;
        fr[0] = 6;
    }
    return e.h(0x33EB0, fr[0], q, 0, 0, 0, arg, 0xf4);
}
}
