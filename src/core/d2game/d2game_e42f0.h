// 1.13c D2Game+0xE42F0, transcribed from the 1.13c disassembly: the Act 1 quest (group 0x13) event handler for a quest node.
// Convention: ECX = node (this), EDX = event record (+0 game, +8 pointer whose +4 is the event type or NULL for -1, +0xC player, +0x14 value); plain ret. Quest flags:
// 0xA4C0 + 0x10 + 4 * difficulty (game byte +0x6D), read with 0xA568 (qs, group, bit); 0xE660 (EAX = player, EBX = game, stack code) finds an item; 0x33EB0 (EAX = new state,
// ECX = node, stack value, event type) sets the quest state. Node +0x1C is a list of up to 32 unit ids (count word at +0x9C); node byte +0xC indexes the 4-word table at
// D2Game+0x101550 ({-1, 0, 1, 2}). EAX at exit is the 0x33EB0 result, the list length, the node byte, the table word, or 0.
#pragma once
#include <cstdint>

namespace re42f0 {
template <class E> uint32_t Do(E& e, uint32_t th, uint32_t ev)
{
    static const uint32_t kTab[4] = { 0xFFFFFFFFu, 0, 1, 2 };   // D2Game+0x101550 (.data)
    const uint32_t game = e.rd32(ev), pl = e.rd32(ev + 0xC);
    const uint32_t pq = e.h(0xA4C0, 0, 0, 0, 0, 0, pl);
    const uint32_t qf = e.rd32(pq + e.rd8(game + 0x6D) * 4 + 0x10);
    auto q = [&](uint32_t b) { return e.h(0xA568, 0, 0, 0, 0, 0, qf, 0x13, b); };
    const uint32_t a0 = q(0);
    const uint32_t p8 = e.rd32(ev + 8);
    const uint32_t ty = p8 ? e.rd32(p8 + 4) : 0xFFFFFFFFu;
    auto set = [&](uint32_t s) { return e.h(0x33EB0, s, th, 0, 0, 0, e.rd32(ev + 0x14), ty); };
    if (a0 == 1) {
        const uint32_t u = pl ? e.rd32(pl + 0xC) : 0xFFFFFFFFu;
        const uint32_t n = e.rd8(th + 0x9C) | (e.rd8(th + 0x9D) << 8);
        if ((int32_t)n <= 0) return 0;
        for (uint32_t i = 0; i < n; i++)
            if (e.rd32(th + 0x1C + 4 * i) == u) return set(4);
        return n;
    }
    const uint32_t r = e.h(0xE660, pl, 0, 0, game, 0, 0x20333367);
    if (r && q(5) && !q(8)) return set(3);
    if (q(6)) {
        const uint32_t edi = q(8) == 0 ? 1 : 0;
        const uint32_t e7 = q(7) == 0 ? 1 : 0;
        if (ty == 0xFC) {
            if (e7) return set(5);
        } else if (ty == 0xFF && edi) {
            return set(6);
        } else if (e7) {
            return set(5);
        }
        if (edi) return set(6);
        return 0;
    }
    if (q(5)) return set(4);
    const uint32_t b = e.rd8(th + 0xC);
    if (b >= 4) return b;
    if (r) return b;
    const uint32_t t = kTab[b];
    if (t == 0xFFFFFFFFu || t >= 8) return t;
    return e.h(0x33EB0, t, th, 0, 0, 0, e.rd32(ev + 0x14), ty);
}
}
