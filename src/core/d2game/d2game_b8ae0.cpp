// 1.13c D2Game+0xB8AE0 QuestGate16F196 (retail asm: ECX = quest record, EDX = skill, stack words w0 (unused), w1 = unit, w2 (unused), ret 0xC;
// returns AL = 0 or 1, the upper bytes of EAX are stale): with the record's +9 set and quest flag 0x19/0 of the unit clear, skill 0x16F
// passes when flag 0x19/1 == 1 or the record's +0xC == 1, skill 0x196 when flag 0x19/5 is clear. Flags come from D2Common #10174 (thunk 0xA568).
#include "d2game_kmem.h"

namespace d2g_b8ae0 {
struct Env {
    D2gMem m;
    uint32_t (*ord10174)(uint32_t unit, uint32_t a, uint32_t b);   // 0xA568, 3 stack words
};
inline uint32_t run(const Env& e, uint32_t rec, uint32_t skill, uint32_t unit) {
    if (e.m.r8(rec + 9) == 0) return 0;
    if (e.ord10174(unit, 0x19, 0) != 0) return 0;
    if (skill == 0x16F) {
        if (e.ord10174(unit, 0x19, 1) == 1) return 1;
        return e.m.r8(rec + 0xC) == 1 ? 1 : 0;
    }
    if (skill == 0x196) return e.ord10174(unit, 0x19, 5) == 0 ? 1 : 0;
    return 0;
}
}
