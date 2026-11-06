// 1.13c D2Game+0xB9F20 (retail asm: ECX = game, EDX = unit, one ignored stack word, ret 4, returns 0): when the unit's quest flag
// 0x1A/0 of its difficulty slot is not 1, its room (#10331 thunk 0xA478) matches the quest record's +0x3C (directly or through
// the room's neighbour list, #10383 thunk 0xA55C: out array, out count) and flags 0x1A/0 and 0x1A/1 are both clear, runs 0xB9A30.
#include "d2game_kmem.h"

namespace d2g_b9f20 {
struct Env {
    D2gMem m;
    uint32_t (*ord10920)(uint32_t unit);                              // 0xA4C0
    uint32_t (*ord10174)(uint32_t qh, uint32_t a, uint32_t b);        // 0xA568
    uint32_t (*ord10331)(uint32_t unit);                              // 0xA478
    void     (*ord10383)(uint32_t room, uint32_t* arr, uint32_t* n);  // 0xA55C
    void     (*b9a30)(uint32_t qh, uint32_t unit, uint32_t game);     // 0xB9A30 (EAX, ECX, EBX)
};
inline uint32_t run(const Env& e, uint32_t game, uint32_t unit) {
    const D2gMem& m = e.m;
    uint32_t u = m.r32(e.ord10920(unit) + 0x10 + m.r8(game + 0x6D) * 4);
    if (e.ord10174(u, 0x1A, 0) == 1) return 0;
    uint32_t w = e.ord10331(unit);
    if (!w) return 0;
    uint32_t q = d2gQuestRec(m, game);
    if (!q) return 0;
    uint32_t want = m.r32(q + 0x3C);
    bool hit = false;
    if (w == want) hit = true;
    else {
        uint32_t arr = 0, n = 0;
        e.ord10383(w, &arr, &n);
        for (uint32_t i = 0; i < n; i++) if (m.r32(arr + 4 * i) == want) { hit = true; break; }
    }
    if (e.ord10174(u, 0x1A, 0) == 0 && e.ord10174(u, 0x1A, 1) == 0 && hit) e.b9a30(u, unit, game);
    return 0;
}
}
