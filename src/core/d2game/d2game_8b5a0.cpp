// 1.13c D2Game+0x8B5A0 -- send the skill list packet 0x94 (stack = client, unit; ret 8).
// Written from the 1.13c disassembly as a rule over E. unit+0xA8 = the skill list (null is fatal); its nodes are counted with the list walkers (0xAE0E first, 0xAE02 next).
// A zero client, or 256 skills or more, is fatal. Packet = {0x94, count, unit+0xC as a dword, then per skill: id word (0xA49C), level byte (0xA400(unit, skill, 0))}
// in a buffer from the allocator 0xA2E6 (ECX = [client+0x1A8]+0x1C pool, EDX = 6 + 3*count). 0x8A3E0(EAX = client, stack = buffer, size) sends it; 0xA2EC frees it.
// The id read for the header is the unit argument's (+0xC), not the client player's.
#ifndef D2GAME_RULE_8B5A0
#define D2GAME_RULE_8B5A0
#include <cstdint>

namespace g8b5a0 {
template <class E> void Send(E& e, uint32_t client, uint32_t unit)
{
    const uint32_t sl = e.rd32(unit + 0xA8);
    uint32_t s = e.h(0xAE0E, 0, 0, 0, 0, 0, sl);
    uint32_t n = 0;
    while (s) { s = e.h(0xAE02, 0, 0, 0, 0, 0, s); ++n; }
    const uint32_t pu = e.rd32(client + 0x1A8);
    const uint32_t pool = e.rd32(pu + 0x1C);
    const uint32_t size = n * 3 + 6;
    const uint32_t file = 0xB2000000u | 0xFBAE8;
    const uint32_t buf = e.h(0xA2E6, 0, pool, size, 0, 0, file, 0x568, 0);
    E::wr8(buf, 0x94);
    E::wr8(buf + 1, n);
    E::wr32(buf + 2, e.rd32(unit + 0xC));
    uint32_t p = buf + 6;
    s = e.h(0xAE0E, 0, 0, 0, 0, 0, sl);
    while (s) {
        const uint32_t lv = e.h(0xA400, 0, 0, 0, 0, 0, unit, s, 0);
        E::wr8(p + 2, lv);
        const uint32_t id = e.h(0xA49C, 0, 0, 0, 0, 0, s, file, 0x576);
        E::wr8(p, id); E::wr8(p + 1, id >> 8);
        s = e.h(0xAE02, 0, 0, 0, 0, 0, s);
        p += 3;
    }
    e.h(0x8A3E0, client, 0, 0, 0, 0, buf, size);
    uint32_t w[64] = {};
    const uint32_t nw = (size + 3) / 4;
    for (uint32_t i = 0; i < size; ++i) w[i / 4] |= (uint32_t)E::rd8(buf + i) << (8 * (i % 4));
    e.blk(0x8A3E0, w, nw);
    e.h(0xA2EC, 0, pool, buf, 0, 0, file, 0x57E, 0);
}
}
#endif
