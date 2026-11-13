// 1.13c D2Game+0xE6190, transcribed from the 1.13c disassembly: the Act 1 quest (group 0x12) pickup handler that picks the next quest state from which of five items the player holds.
// Convention: ECX = node (this), EDX = event record (+0 game, +8 event data pointer, +0xC player); plain ret. qf = 0xA4C0(player) + 0x10 + 4 * game byte +0x6D. Group 0xF bit 0 clear
// returns 0 and group 0x12 bit 0 set returns that value. c = 0xA532(event data) is the item code picked up. f16 = group 0x12 bit 5 clear, f15 = bit 7 clear; the player's items (0xE660,
// EAX = player, EBX = game, stack code) give A = 0x20316671, B = 0x20796571, C = 0x20726271, D = 0x20726871 (the 0x20326671 result is not used). Code 0x20326671 sets node byte +0xB to 7
// when nonzero, returns when group 0xF bit 0 is set, then runs 0xE4A50 (EAX = node, DL = state, stack event) with 5 (f15) or 6; the other four codes pick states 1-7 from the items held.
// EDX at the 0xE4A50 call is the event data pointer with its low byte replaced by the state. EAX at exit is the 0xE4A50 result, the 0xA568 value, 0, or the item code.
#pragma once
#include <cstdint>

namespace re6190 {
template <class E> uint32_t Do(E& e, uint32_t th, uint32_t ev)
{
    const uint32_t game = e.rd32(ev), pl = e.rd32(ev + 0xC);
    const uint32_t pq = e.h(0xA4C0, 0, 0, 0, 0, 0, pl);
    const uint32_t qf = e.rd32(pq + e.rd8(game + 0x6D) * 4 + 0x10);
    auto q = [&](uint32_t b) { return e.h(0xA568, 0, 0, 0, 0, 0, qf, 0x12, b); };
    if (e.h(0xA568, 0, 0, 0, 0, 0, qf, 0xF, 0) == 0) return 0;
    uint32_t v = q(0);
    if (v) return v;
    const uint32_t ev8 = e.rd32(ev + 8);
    const uint32_t c = e.h(0xA532, 0, 0, 0, 0, 0, ev8);
    q(2); q(3); q(4);
    const bool f16 = q(5) == 0;
    q(6);
    const bool f15 = q(7) == 0;
    auto it = [&](uint32_t code) { return e.h(0xE660, pl, 0, 0, game, 0, code) != 0; };
    const bool A = it(0x20316671);
    it(0x20326671);
    const bool B = it(0x20796571);
    const bool C = it(0x20726271);
    const bool D = it(0x20726871);
    auto go = [&](uint32_t st) { return e.h(0xE4A50, th, 0, (ev8 & ~0xFFu) | st, 0, 0, ev); };
    if (c == 0x20326671) {
        if (e.rd8(th + 0xB)) e.wr8(th + 0xB, 7);
        v = e.h(0xA568, 0, 0, 0, 0, 0, qf, 0xF, 0);
        if (v) return v;
        return go(f15 ? 5 : 6);
    }
    if (c == 0x20316671) {
        if (!B) return go(1);
        if (!C) return go(2);
        if (!D) return go(4);
        return go(f16 ? 5 : 7);
    }
    if (c == 0x20796571) {
        if (!C) return go(2);
        if (!A) return go(3);
        return go(D ? 7 : 4);
    }
    if (c == 0x20726871) {
        if (!C) return go(2);
        if (!A) return go(3);
        return go(B ? 7 : 1);
    }
    if (c == 0x20726271) {
        if (!D) return go(4);
        if (!A) return go(3);
        return go(B ? 7 : 1);
    }
    return c;
}
}
