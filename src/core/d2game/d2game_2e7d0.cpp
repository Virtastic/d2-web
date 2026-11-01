// Allocates an object's timer-argument block {head, id = -1} and its first node {guid = -1, next = 0}.
#include "d2game_2e790.h"
using namespace d2g32;

// 1.13c D2Game+0x2E7D0
uint32_t OBJECTS_AllocTimerArgs(uint32_t game, D2GPoolAlloc poolAlloc, uint32_t file)
{
    const uint32_t block = poolAlloc(rd32(game + 0x1C), 8, file, 0x1B8, 0);
    wr32(block, 0);
    wr32(block + 4, 0);
    wr32(block + 4, 0xFFFFFFFFu);
    const uint32_t node = poolAlloc(rd32(game + 0x1C), 8, file, 0x1BC, 0);
    wr32(node, 0xFFFFFFFFu);
    wr32(node + 4, 0);
    wr32(block, node);
    return block;
}
