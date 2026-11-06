// 1.13c D2Game+0xB8CE0 quest-event callback (retail asm: ECX = state (ESI), EDX = event (EDI), plain ret, void):
// event: +0 game (+0x6D difficulty), +8 a record (+4 = L0, else -1), +0xC the other unit X (its +0xC id), +0x14 the value handed on.
// qh = [#10920(X) + difficulty * 4 + 0x10]; K = state+0xE0; L1 = #10174(qh, K, 0), L2 = #10174(qh, K, 0xD).
// #10174(qh, K, 1) set: 0x33EB0(2); else a state id list (u16 count +0x9C, ids from +0x1C) holding X's id (-1 without X): 0x33EB0(3);
// else, when state+0xC != 0, L1 != 1, (state+0xC < 4 or L2 != 0), #10174(qh, 0x1B, 1) clear and state+9 set: 0xE660(EAX = X, EBX = game, 'mss ')
// and the state's +0xB / +0x18 -> +0x1C bytes decide 0x33EB0(0 or 1). 0x33EB0(EAX = kind, ECX = state, stack: event+0x14, L0).
#include "d2game_kmem.h"

namespace d2g_b8ce0 {
struct Env {
    D2gMem m;
    uint32_t (*ord10920)(uint32_t unit);                                        // 0xA4C0, 1 stack word
    uint32_t (*ord10174)(uint32_t qh, uint32_t a, uint32_t b);                  // 0xA568, 3 stack words
    uint32_t (*sE660)(uint32_t unit, uint32_t game, uint32_t tag);              // EAX, EBX, 1 stack word
    void (*s33EB0)(uint32_t kind, uint32_t state, uint32_t s0, uint32_t s1);    // EAX, ECX, 2 stack words
};
inline void run(const Env& e, uint32_t st, uint32_t ev) {
    const D2gMem& m = e.m;
    uint32_t game = m.r32(ev), X = m.r32(ev + 0xC), Y = m.r32(ev + 8);
    uint32_t pd = e.ord10920(X);
    uint32_t qh = m.r32(pd + m.r8(game + 0x6D) * 4 + 0x10);
    uint32_t L0 = Y ? m.r32(Y + 4) : 0xFFFFFFFFu;
    uint32_t K = m.r32(st + 0xE0);
    uint32_t L1 = e.ord10174(qh, K, 0);
    uint32_t L2 = e.ord10174(qh, K, 0xD);
    if (e.ord10174(qh, K, 1)) { e.s33EB0(2, st, m.r32(ev + 0x14), L0); return; }
    X = m.r32(ev + 0xC);
    uint32_t id = 0xFFFFFFFFu;
    if (X) id = m.r32(X + 0xC);
    uint32_t cnt = m.r16(st + 0x9C);
    for (uint32_t i = 0; i < cnt; i++)
        if (m.r32(st + 0x1C + 4 * i) == id) { e.s33EB0(3, st, m.r32(ev + 0x14), L0); return; }
    uint32_t c = m.r8(st + 0xC);
    if (c == 0 || L1 == 1) return;
    if (c >= 4 && L2 == 0) return;
    if (e.ord10174(qh, 0x1B, 1) != 0) return;
    if (m.r8(st + 9) == 0) return;
    game = m.r32(ev); X = m.r32(ev + 0xC);
    uint32_t kind;
    if (c == 1) {
        if (e.sE660(X, game, 0x2073736d) != 0) kind = 0;
        else {
            if (m.r8(m.r32(st + 0x18) + 0x1C) != 0) return;
            if (m.r8(st + 0xB) >= 3) return;
            kind = 1;
        }
    } else {
        if (e.sE660(X, game, 0x2073736d) != 0) return;
        if (m.r8(st + 0xB) >= 3) return;
        if (m.r8(m.r32(st + 0x18) + 0x1C) != 0) return;
        kind = 1;
    }
    e.s33EB0(kind, st, m.r32(ev + 0x14), L0);
}
}
