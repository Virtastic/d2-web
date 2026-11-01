// Frees an object's server data: its hover text (+0xA4) through D2Common #10422, the timer-argument block D2Common
// #10181 returns (0x2E790), then its timers (0x8E880).
#include "d2game_2e790.h"
using namespace d2g32;

struct D2G2EE60Env {
    void (*freeHover)(uint32_t pool, uint32_t hover);   // D2Common #10422 (thunk 0xAAF6), stdcall
    uint32_t (*timerArgs)(uint32_t object);             // D2Common #10181 (thunk 0xB168), ECX = object
    void (*freeTimers)(uint32_t game, uint32_t object); // 0x8E880: EAX = game, stack object
    D2GPoolFree poolFree;
    uint32_t file;
};

// 1.13c D2Game+0x2EE60
void OBJECTS_FreeObjectData(uint32_t game, uint32_t object, const D2G2EE60Env& env)
{
    if (object && rd32(object + 0xA4)) env.freeHover(rd32(game + 0x1C), rd32(object + 0xA4));
    OBJECTS_FreeTimerArgs(game, env.timerArgs(object), env.poolFree, env.file);
    env.freeTimers(game, object);
}
