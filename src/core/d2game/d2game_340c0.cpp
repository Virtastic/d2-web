// When the quest flag (D2Common #10174 on the player's quest record, picked by D2Common #10920 and the difficulty
// byte at [unit+4]+0x6D) is set with its done bit clear and the unit's +0x9C count is non-zero, 0x33830 drops the
// GUID (EAX = [[ctx+0xC]+0xC], or -1 without one) from the list at unit+0x1C.
#include "d2game_2e790.h"
using namespace d2g32;

struct D2G340C0Env {
    uint32_t (*records)(uint32_t p);                          // D2Common #10920 (thunk 0xA4C0)
    uint32_t (*flag)(uint32_t rec, uint32_t id, uint32_t bit); // D2Common #10174 (thunk 0xA568)
    void (*removeGuid)(uint32_t guid, uint32_t list);         // 0x33830: EAX guid, ECX list
};

// 1.13c D2Game+0x340C0
void QUESTS_RemovePlayerGuid(uint32_t ctx, uint32_t unit, const D2G340C0Env& env)
{
    const uint32_t data = rd32(unit + 4);
    const uint32_t recs = env.records(rd32(ctx + 0xC));
    const uint32_t rec = rd32(recs + 0x10 + rd8(data + 0x6D) * 4u);
    if (env.flag(rec, rd32(unit + 0xE0), 1) != 1) return;
    if (env.flag(rec, rd32(unit + 0xE0), 0) == 0) return;
    if (rd16(unit + 0x9C) == 0) return;
    const uint32_t g = rd32(ctx + 0xC);
    env.removeGuid(g ? rd32(g + 0xC) : 0xFFFFFFFFu, unit + 0x1C);
}
