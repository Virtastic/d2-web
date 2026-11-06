// 1.13c D2Game+0xB9A30: quest-completion body of the act quest gate (retail asm: EAX = quest handle, ECX = unit, EBX = game, plain ret).
// Calls D2Common #10003 (set quest flag) x2, #10495, and with game+0x70 == 0 two more #10003 and the unit's player data (#10920)
// whose +0x9C record gets its +0xA bits 8..12 raised to difficulty * (4 or 5) + 4; then bumps the quest record's +0x1C.
#include "d2game_kmem.h"

namespace d2g_b9a30 {
struct Env {
    D2gMem m;
    void     (*ord10003)(uint32_t qh, uint32_t a, uint32_t b);   // thunk 0xA57A, 3 stack words
    void     (*ord10495)(uint32_t qh, uint32_t a);               // thunk 0xA574, 2 stack words
    uint32_t (*ord10920)(uint32_t unit);                         // thunk 0xA4C0, 1 stack word
};
inline void run(const Env& e, uint32_t qh, uint32_t unit, uint32_t game) {
    const D2gMem& m = e.m;
    e.ord10003(qh, 0x1A, 0xD);
    e.ord10003(qh, 0x1A, 0);
    e.ord10495(qh, 0x1A);
    if (m.r32(game + 0x70) == 0) {
        e.ord10003(qh, 0x1A, 6);
        e.ord10003(qh, 0x1A, 7);
        uint32_t it = 0;
        if (unit && m.r32(unit) == 0) it = m.r32(e.ord10920(unit) + 0x9C);
        uint32_t diff = m.r8(game + 0x6D);
        uint32_t w = m.r16(it + 0xA);
        uint32_t u = ((w & 0x20) | 0x80) >> 5;
        if ((int32_t)(u * diff + 4) >= (int32_t)((w >> 8) & 0x1F))
            m.w16(it + 0xA, (uint16_t)(((u * diff + 4) << 8) | (w & 0xE0FF)));
    }
    uint32_t q = d2gQuestRec(m, game);
    if (q) m.w32(q + 0x1C, m.r32(q + 0x1C) + 1);
}
}
