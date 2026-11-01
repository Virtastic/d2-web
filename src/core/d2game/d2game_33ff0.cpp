// Frees a quest record chain (next at +4) to the game pool.
#include "d2game_2e790.h"
using namespace d2g32;

// 1.13c D2Game+0x33FF0
void QUESTS_FreeRecordChain(uint32_t game, uint32_t record, D2GPoolFree poolFree, uint32_t file)
{
    while (record) {
        const uint32_t next = rd32(record + 4);
        poolFree(rd32(game + 0x1C), record, file, 0x50, 0);
        record = next;
    }
}
