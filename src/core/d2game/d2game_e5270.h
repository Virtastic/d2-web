// 1.13c D2Game+0xE5270, transcribed from the 1.13c disassembly: the Act 1 quest (group 0x11) event handler that moves a quest node on when the player picks up its item.
// Convention: ECX = node (this), EDX = event record (+0 game, +8 pointer whose +4 is the event type or NULL for -1, +0xC player, +0x14 value); plain ret. qf = 0xA4C0(player) + 0x10
// + 4 * game byte +0x6D; a = group 0x11 bit 0, b = bit 0xD. a == 1 with b == 0 returns 0. The global item 0xE660 code 0x20626262 sets state 3 through 0x33EB0 (EAX = state, ECX = node,
// stack value, event type). a == 1: 0x337D0 (ESI = the player's unit id or -1, EDX = node) true sets state 4. Else node byte +9 set and node byte +0xC < 4 index the 4-word table at
// D2Game+0x100520 ({-1, 0, 1, 2}); a value below 6 other than -1 is the state. EAX at exit is the 0x33EB0 result, the table word, the node byte, or 0.
#pragma once
#include <cstdint>

namespace re5270 {
template <class E> uint32_t Do(E& e, uint32_t th, uint32_t ev)
{
    static const uint32_t kTab[4] = { 0xFFFFFFFFu, 0, 1, 2 };   // D2Game+0x100520 (.data)
    const uint32_t game = e.rd32(ev), pl = e.rd32(ev + 0xC);
    const uint32_t pq = e.h(0xA4C0, 0, 0, 0, 0, 0, pl);
    const uint32_t qf = e.rd32(pq + e.rd8(game + 0x6D) * 4 + 0x10);
    const uint32_t a = e.h(0xA568, 0, 0, 0, 0, 0, qf, 0x11, 0);
    const uint32_t b = e.h(0xA568, 0, 0, 0, 0, 0, qf, 0x11, 0xD);
    const uint32_t p8 = e.rd32(ev + 8);
    const uint32_t ty = p8 ? e.rd32(p8 + 4) : 0xFFFFFFFFu;
    auto set = [&](uint32_t s) { return e.h(0x33EB0, s, th, 0, 0, 0, e.rd32(ev + 0x14), ty); };
    if (a == 1 && b == 0) return b;
    if (e.h(0xE660, pl, 0, 0, game, 0, 0x20626262)) return set(3);
    if (a == 1) {
        const uint32_t u = pl ? e.rd32(pl + 0xC) : 0xFFFFFFFFu;
        if (!e.h(0x337D0, 0, 0, th, 0, u)) return 0;
        return set(4);
    }
    if (!e.rd8(th + 9)) return 0;
    const uint32_t cl = e.rd8(th + 0xC);
    if (cl >= 4) return cl;
    const uint32_t t = kTab[cl];
    if (t == 0xFFFFFFFFu || t >= 6) return t;
    return set(t);
}
}
