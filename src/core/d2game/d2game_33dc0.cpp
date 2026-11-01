// Frees the game's quest control (game+0x10F4): the quest chain at +0 (next at +0xF4, each with an optional block at
// +0x18), the timer list at +0x10 (next at +0x10), the +0xC object through D2Common #10901, then the control.
#include "d2game_2e790.h"
using namespace d2g32;

// 1.13c D2Game+0x33DC0
void QUESTS_FreeControl(uint32_t game, D2GPoolFree poolFree, uint32_t file, void (*free10901)(uint32_t pool, uint32_t p))
{
    const uint32_t control = rd32(game + 0x10F4);
    if (!control) return;
    for (uint32_t q = rd32(control); q;) {
        const uint32_t data = rd32(q + 0x18), next = rd32(q + 0xF4);
        if (data) poolFree(rd32(game + 0x1C), data, file, 0x1DF, 0);
        poolFree(rd32(game + 0x1C), q, file, 0x1E1, 0);
        q = next;
    }
    for (uint32_t t = rd32(control + 0x10); t;) {
        const uint32_t next = rd32(t + 0x10);
        poolFree(rd32(game + 0x1C), t, file, 0x1E8, 0);
        t = next;
    }
    free10901(rd32(game + 0x1C), rd32(control + 0xC));
    poolFree(rd32(game + 0x1C), control, file, 0x1EC, 0);
}
