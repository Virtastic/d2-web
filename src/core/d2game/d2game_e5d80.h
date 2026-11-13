// 1.13c D2Game+0xE5D80, transcribed from the 1.13c disassembly: the Act 1 quest (group 0x15) party check for a player unit.
// Convention: ECX = game, EDX = player unit, stack word unused (ret 4); always returns 0. rec = 0xA478(unit) (0 returns); the quest flags are qf = 0xA4C0(unit) + 0x10 + 4 * game byte
// +0x6D; group 0x15 bit 0 or bit 4 set returns, and so does a 0xA56E(0xA556(rec)) low byte other than 2. The node list at [[game+0x10F4]] (next +0xF4) is searched for type 0x13 (a
// miss dereferences NULL in retail), dat = [node+0x18], the bucket game+0x1320+4*([dat] & 0x7F) (next +0xE4) for [b+0xC] == [dat]; esi = 0xA478(bucket node) (0 returns). The party
// flag is 1 when rec == esi, else 0xA478(unit) (assert path at 0xE4) and 0xA55C(g, &arr, &count) with arr[i] == esi. The flag set: s = 0x1020 (EAX = unit, stack game: the
// quest flags again) gets 0xA57A(s, 0x15, 0 if group 0x12 bit 0 is set else 4) and 0xA57A(s, 0x15, 0xD).
#pragma once
#include <cstdint>

namespace re5d80 {
template <class E> uint32_t Do(E& e, uint32_t game, uint32_t unit, uint32_t a1)
{
    (void)a1;
    const uint32_t pq = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t qf = e.rd32(pq + e.rd8(game + 0x6D) * 4 + 0x10);
    const uint32_t rec = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    if (!rec) return 0;
    if (e.h(0xA568, 0, 0, 0, 0, 0, qf, 0x15, 0)) return 0;
    if (e.h(0xA568, 0, 0, 0, 0, 0, qf, 0x15, 4)) return 0;
    const uint32_t k = e.h(0xA556, 0, 0, 0, 0, 0, rec);
    if ((e.h(0xA56E, 0, 0, 0, 0, 0, k) & 0xFF) != 2) return 0;
    uint32_t node = 0;
    const uint32_t head = e.rd32(game + 0x10F4);
    if (head) for (node = e.rd32(head); node; node = e.rd32(node + 0xF4)) if (e.rd32(node) == 0x13) break;
    const uint32_t key = e.rd32(e.rd32(node + 0x18));
    uint32_t b = e.rd32(game + (key & 0x7F) * 4 + 0x1320);
    while (b && e.rd32(b + 0xC) != key) b = e.rd32(b + 0xE4);
    if (!b) return 0;
    const uint32_t esi = e.h(0xA478, 0, 0, 0, 0, 0, b);
    if (!esi) return 0;
    uint32_t flag = 0;
    if (rec == esi) flag = 1;
    else {
        uint32_t g = e.h(0xA478, 0, 0, 0, 0, 0, unit);
        if (!g) {   // assert 0xDF (string 0x6FD19FF9 in retail; not part of the capture), then 0x22A3C(-1)
            const uint32_t f = e.h(0xA2DA);
            e.h(0xA2C2, 0, 0, 0, 0, 0, 0xF9FF9, f, 0xDF);
            g = e.h(0x22A3C, 0, 0, 0, 0, 0, 0xFFFFFFFFu);
        }
        uint32_t arr = 0, cnt = 0;
        e.h(0xA55C, 0, 0, 0, 0, 0, g, e.stk(&arr), e.stk(&cnt));
        for (uint32_t i = 0; i < cnt; i++) if (e.rd32(arr + 4 * i) == esi) { flag = 1; break; }
    }
    const uint32_t s = e.h(0x1020, unit, 0, 0, 0, 0, game);
    if (!flag) return 0;
    const uint32_t v = e.h(0xA568, 0, 0, 0, 0, 0, s, 0x12, 0) ? 0 : 4;
    e.h(0xA57A, 0, 0, 0, 0, 0, s, 0x15, v);
    e.h(0xA57A, 0, 0, 0, 0, 0, s, 0x15, 0xD);
    return 0;
}
}
