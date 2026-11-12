// src/core/d2game/d2game_611b0.cpp -- 1.13c D2Game+0x611b0 (LAB_6fc811b0): the quest 5 state callback. The 0x63350 completion step installs it on
// the unit (unit+0xC8), so it is reached through a function pointer, never a fixed-signature call. Its __fastcall
// arguments are the quest record (ECX) and the quest argument block (EDX), written here as plain parameters.
// Transcribed from build-wasm/ghidra-d2game/decomp/0x611B0.c. Retail struct offsets are used directly, as the
// other d2game batch files do: the quest record is 32-bit retail memory addressed through intptr_t.
//
// The helper at 0x33830 is QUESTS_FastRemovePlayerGUID (1.10f src/D2Game/QUESTS/Quests.cpp, no delta): it removes
// pid from the GUID list at info+0x8C (count at +0x10C). The rest removes pid from the list at info+0x34 (count
// u16 at +0x64) and from the list at info+0x00 (count u16 at +0x30), each by swapping in the last entry.
#include <cstdint>
#include "D2Game/QUESTS/Quests.h"

using i32 = int32_t;
using u16 = uint16_t;

static inline i32 rd(i32 p, i32 o) { return *(i32*)(intptr_t)(p + o); }
static inline u16 rd16(i32 p, i32 o) { return *(u16*)(intptr_t)(p + o); }
static inline void wr16(i32 p, i32 o, u16 v) { *(u16*)(intptr_t)(p + o) = v; }
static inline void wr(i32 p, i32 o, i32 v) { *(i32*)(intptr_t)(p + o) = v; }

// 1.13c D2Game+0x611B0 (LAB_6fc811b0). Registers: ECX = quest record, EDX = quest argument block. Retail returns nothing; EAX is left
// holding the second list's scan index (0 for an empty list, the count when pid is absent), which d2g_611b0 returns.
extern "C" uint32_t d2g_611b0(i32 quest, i32 args)
{
    // args->pPlayer (+0xC) ? pPlayer->dwUnitId (+0xC) : -1
    i32 pid = (rd(args, 0x0c) == 0) ? -1 : rd(rd(args, 0x0c), 0x0c);
    i32 info = rd(quest, 0x18);   // pQuestDataEx

    QUESTS_FastRemovePlayerGUID((D2QuestGUIDStrc*)(intptr_t)(info + 0x8c), pid);

    // list at +0x34, count u16 at +0x64: remove pid, the last entry takes its slot
    u16 n = rd16(info, 0x64);
    if (n != 0) {
        u16 i = 0;
        do {
            if (rd(info, 0x34 + i * 4) == pid) {
                if (i != n - 1) wr(info, 0x34 + i * 4, rd(info, 0x30 + n * 4));
                wr16(info, 0x64, (u16)(rd16(info, 0x64) - 1));
                break;
            }
            ++i;
        } while (i < rd16(info, 0x64));
    }

    // list at +0x00, count u16 at +0x30
    u16 m = rd16(info, 0x30);
    u16 i = 0;
    if (m != 0) {
        while (rd(info, i * 4) != pid) {
            ++i;
            if (rd16(info, 0x30) <= i) return i;
        }
        if (i != m - 1) wr(info, i * 4, rd(info, -4 + m * 4));
        wr16(info, 0x30, (u16)(rd16(info, 0x30) - 1));
    }
    return i;
}

extern "C" void LAB_6fc811b0(i32 quest, i32 args) { d2g_611b0(quest, args); }
