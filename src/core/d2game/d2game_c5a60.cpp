// 1.13c D2Game+0xC5A60 -- quest-event callback, ECX = quest record (+4 game, +0xB state, +0x14, +0x18 data, +0xA8), EDX = event record (+8 trigger {+4 id}); plain ret, leftover EAX mirrored. The trigger id is matched on its low word (-1 without a trigger). Id 0xFB with data byte +9 = 1: +0x14 = 0, state 4, 0xDFCC0 (EAX 0, EBX callback 0xC4D30, EDI game, stack 0), data +9 = 0, +0xA8 = 0, returns 0. Id 0xC9: data byte +8 = 1 (state 1) or else byte +0xA = 1 (state 6) posts the same callback and clears the byte, then +0xA8 = 0 and 0xDFCC0 with callback 0xC2A90 answers (neither byte set returns the id with its low byte forced to 1). Other ids return the id. The callback pointer constants are never null, so the quest's fatal path is unreachable.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc5a60(GE& e, uint32_t q, uint32_t ev)
{
    const uint32_t p8 = e.rd32(ev + 8), id = p8 ? e.rd32(p8 + 4) : ~0u, d = e.rd32(q + 0x18);
    auto post = [&](uint32_t fn) {
        const uint32_t v = e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | fn, 0, 0);
        e.edi(0xDFCC0, e.rd32(q + 4));
        return v;
    };
    if ((id & 0xffff) == 0xfb) {
        if (e.rd8(d + 9) != 1) return id;
        e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 4);
        post(0xC4D30);
        e.wr8(d + 9, 0); e.wr32(q + 0xa8, 0);
        return 0;
    }
    if ((id & 0xffff) == 0xc9) {
        if (e.rd8(d + 8) == 1) {
            e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 1);
            post(0xC4D30);
            e.wr8(d + 8, 0);
        } else {
            if (e.rd8(d + 0xa) != 1) return (id & ~0xffu) | 1;
            e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 6);
            post(0xC4D30);
            e.wr8(d + 0xa, 0);
        }
        e.wr32(q + 0xa8, 0);
        return post(0xC2A90);
    }
    return id;
}
}
