// 1.13c D2Game+0x5C850 -- quest 0x20 event: ECX = event record (+0 game, +0x14 / +0x18 = the pair to log). Finds the game's quest node 0x20 (a missing node faults in retail), needs node byte +9.
// Raises node state +0xC to 2 (trace "changing state to 2"); with node byte +0xB clear and the quest data word +0xA8 below 5: node +0x14 = 0, +0xB = 1, callback 0x793F0 over the players.
// The data's three slots (bytes +0x86..+0x88, pairs at +0x8C / +0x90, step 8) take the pair unless a used slot holds it: 0x5C2A0(node, event, slot) is added to word +0xA4, slot byte = 1, byte +0x10E + slot = 0.
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
void f5c850(NE& e, uint32_t a)
{
    const uint32_t game = e.rd32(a);
    const uint32_t node = e.quest(game, 0x20);
    if (!e.rd8(node + 9)) return;
    const uint32_t data = e.rd32(node + 0x18);
    if (e.rd8(node + 0xc) < 2) { e.wr8(node + 0xc, 2); e.say(e.rd32(node + 4), 2); }
    if (!e.rd8(node + 0xb) && (int32_t)e.rd32(data + 0xa8) < 5) {
        e.wr8(node + 0x14, 0); e.wr8(node + 0xb, 1);
        e.cb(e.rd32(node + 4), 0x593F0);
    }
    uint32_t i = 0;
    while (e.rd8(data + 0x86 + i)) if (++i > 2) return;
    for (uint32_t j = 0; j < i; j++)
        if (e.rd32(data + 0x8c + 8 * j) == e.rd32(a + 0x14) && e.rd32(data + 0x90 + 8 * j) == e.rd32(a + 0x18)) return;
    e.wr32(data + 0x8c + 8 * i, e.rd32(a + 0x14));
    e.wr32(data + 0x90 + 8 * i, e.rd32(a + 0x18));
    const uint32_t v = e.imp(0x5C2A0, node, a, i);
    e.wr32(data + 0xa4, e.rd32(data + 0xa4) + v);
    e.wr8(data + 0x86 + i, 1);
    e.wr8(data + 0x10e + i, 0);
}
}
