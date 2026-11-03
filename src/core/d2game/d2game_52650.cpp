// 1.13c D2Game+0x52650 TASK_GetQueueSlot (ordinal 10021).
#include "d2game_52650.h"
#include <cstring>

static uint32_t g_taskSlotTable[32];   // 0x6FD30318
static uint32_t g_taskQueueWords[5];   // 0x6FD30398: head, tail, freq lo, freq hi, counter
uint32_t* D2GAME_TASK_SlotTable() { return g_taskSlotTable; }
uint32_t* D2GAME_TASK_QueueWords() { return g_taskQueueWords; }

// Under the global task lock: point head and tail at the head word the first time; take slot (counter & 0x1F) and bump the
// counter; under that slot's lock, give an empty slot a 0x18-byte record {idx, 0, self-linked list at +8} and enqueue it.
uint32_t D2GAME_TASK_GetQueueSlot_52650(const D2TaskSlotEnv* e)
{
    e->lockGlobal(true);
    uint32_t* q = g_taskQueueWords;
    if (q[0] == 0) { q[0] = e->selfAddr; q[1] = e->selfAddr; }
    const uint32_t idx = q[4] & 0x1F;
    q[4] = q[4] + 1;
    e->lockSlot(idx, true);
    if (g_taskSlotTable[idx] == 0) {
        const uint32_t a = e->alloc(0, 0x18, "..\\Source\\D2Game\\GAME\\Task.cpp", 0x3F7, 0);
        uint8_t* p = (uint8_t*)(uintptr_t)a;
        const uint32_t w[4] = { idx, 0, a + 8, a + 8 };
        std::memcpy(p, w, sizeof w);
        g_taskSlotTable[idx] = a;
        e->enqueue(a);
    }
    e->lockSlot(idx, false);
    e->lockGlobal(false);
    return idx;
}
