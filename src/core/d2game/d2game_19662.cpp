// 1.13c D2Game+0x19662: the tail of an AI think (0x19640) after its unit check: ESI = unit, EDI = game, `ai` = the think's one stack
// argument (a pointer to the AI parameter dword). Outside town (#A808 over #A478(unit) is 0) the target search 0xB1C90 (EAX = game;
// unit, *ai, &local, &ai: the last two are out-pointers, the second reused as the "in range" flag) names a target. With a target, a
// 0..99 roll (0x1180, ECX = the unit seed at +0x20, EAX = 100) below 80 when the flag is set walks to it (0xF2220: EAX = 4, EDI = unit;
// game, target), below 30 when it is clear starts the attack sequence (0xF2540: ECX = game, EDX = unit; target, 2, 0, 0, 1, 7).
// In town or without a target the unit seed steps inline (lo * 0x6AC690C5 + hi) and below 20 of 100 the unit idles (0xF1B10: AL = 5;
// game, unit). Every other way ends in 0xF22C0 (EAX = game; 10). The function is void: EAX is the last callee's return.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_19662(uint32_t unit, uint32_t game, uint32_t ai, const D2Seam* e)
{
    const uint32_t seed = unit + 0x20;
    if (!e->call(0xA808, 0, 0, 0, 0, 0, e->call(0xA478, 0, 0, 0, 0, 0, unit))) {
        uint32_t slot = 0, flag = ai;
        const uint32_t tgt = e->call(0xB1C90, game, 0, 0, 0, 0, unit, d2rd32(ai), (uint32_t)(uintptr_t)&slot, (uint32_t)(uintptr_t)&flag);
        if (tgt) {
            const int32_t roll = (int32_t)e->call(0x1180, 100, seed);
            if (flag) {
                if (roll < 0x50) {
                    const uint32_t v = e->call(0xF2220, 4, 0, 0, 0, 0, game, tgt);
                    e->edi(0xF2220, unit);
                    return v;
                }
            } else if (roll < 0x1E) {
                const uint32_t w[6] = { tgt, 2, 0, 0, 1, 7 };
                const uint32_t v = e->call(0xF2540, 0, game, unit, 0, 0, w[0], w[1], w[2], w[3]);
                e->blk(0xF2540, w, 6);
                return v;
            }
            return e->call(0xF22C0, game, 0, 0, 0, 0, 10);
        }
    }
    const uint64_t n = (uint64_t)d2rd32(seed) * 0x6AC690C5u + d2rd32(seed + 4);
    d2wr32(seed, (uint32_t)n); d2wr32(seed + 4, (uint32_t)(n >> 32));
    if ((uint32_t)n % 100 < 20) return e->call(0xF1B10, 5, 0, 0, 0, 0, game, unit);
    return e->call(0xF22C0, game, 0, 0, 0, 0, 10);
}
