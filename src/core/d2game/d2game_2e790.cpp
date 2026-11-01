// Frees an object's timer-argument block: every node of the chain at +0 (next at +4), then the block.
#include "d2game_2e790.h"
using namespace d2g32;

// 1.13c D2Game+0x2E790
void OBJECTS_FreeTimerArgs(uint32_t game, uint32_t timerArg, D2GPoolFree poolFree, uint32_t file)
{
    for (uint32_t node = rd32(timerArg); node;) {
        const uint32_t next = rd32(node + 4);
        poolFree(rd32(game + 0x1C), node, file, 0x1CE, 0);
        node = next;
    }
    poolFree(rd32(game + 0x1C), timerArg, file, 0x1D1, 0);
}
