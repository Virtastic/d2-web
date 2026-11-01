// 1.13c D2Game+0x35A20: set a quest state for each listed player. Stdcall (game, guids, notifyValue) with EBX = quest id. guids = u32 ids at +0,
// u16 count at +0x80. Each id is looked up in the game's player hash (game+0x1120 + (id&0x7F)*4, chain +0xE4, id at +0xC); for the node's
// player data, quest flags = pd[0x10+4*game[0x6D]]. When flag 0 and flag 1 of quest EBX are both not 1, set flags 0xD and 1; then notify
// (value, node) when the value is non-zero. A null game with a non-empty list is the 0x2D3 fatal (not ported). Seam: r[0]=game, r[1]=guids,
// r[2]=value, r[3]=EBX, r[5]=env.
#include <cstdint>
#include <cstring>
#include "d2game_batch20env.h"

static inline uint32_t rd(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }

extern "C" void D2Game_35a20(uint32_t* r) {
    const D2GameBatch20Env& env = *(const D2GameBatch20Env*)(uintptr_t)r[5];
    const uint32_t game = r[0], guids = r[1], value = r[2], quest = r[3];
    uint16_t n; std::memcpy(&n, (const void*)(uintptr_t)(guids + 0x80), 2);
    for (uint32_t i = 0; i < n; i++) {
        const uint32_t id = rd(guids, 4 * i);
        for (uint32_t node = rd(game, 0x1120 + (id & 0x7F) * 4); node; node = rd(node, 0xE4)) {
            if (rd(node, 0xC) != id) continue;
            const uint32_t pd = (uint32_t)(uintptr_t)env.getPlayerData((void*)(uintptr_t)node);
            uint8_t act; std::memcpy(&act, (const void*)(uintptr_t)(game + 0x6D), 1);
            void* flags = (void*)(uintptr_t)rd(pd, 0x10 + 4u * act);
            if (env.getQuestState(flags, (int32_t)quest, 0) != 1 && env.getQuestState(flags, (int32_t)quest, 1) != 1) {
                env.setQuestState(flags, (int32_t)quest, 0xD);
                env.setQuestState(flags, (int32_t)quest, 1);
                if (value) env.questNotify(value, (void*)(uintptr_t)node);
            }
            break;
        }
    }
    r[4] = 0;
}
