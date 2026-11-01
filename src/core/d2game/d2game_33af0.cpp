// Queues a quest timer: a 0x14 record {callback, quest, due = control+0x14 + frames, frames, next} pushed on the quest
// control (game+0x10F4) list at +0x10. A control whose +4 is set is fatal (0x417).
#include "d2game_2e790.h"
using namespace d2g32;

// 1.13c D2Game+0x33AF0
void QUESTS_QueueTimer(uint32_t quest, uint32_t callback, uint32_t frames, D2GPoolAlloc poolAlloc, uint32_t file,
                       void (*fatal)(uint32_t line))
{
    const uint32_t game = rd32(quest + 4);
    const uint32_t control = rd32(game + 0x10F4);
    if (!control) return;
    if (rd32(control + 4)) { fatal(0x417); return; }
    const uint32_t head = rd32(control + 0x10);
    const uint32_t rec = poolAlloc(rd32(game + 0x1C), 0x14, file, 0x41A, 0);
    wr32(rec + 0x10, head);
    wr32(rec + 4, quest);
    wr32(rec, callback);
    wr32(rec + 8, rd32(control + 0x14) + frames);
    wr32(rec + 0xC, frames);
    wr32(control + 0x10, rec);
}
