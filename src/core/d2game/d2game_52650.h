// 1.13c D2Game+0x52650 TASK_GetQueueSlot (ordinal 10021), retail layout: the slot table and the queue words are 32-bit retail values.
#pragma once
#include <cstdint>

// 0x6FD30318: one task-slot address per queue slot (32 words).
uint32_t* D2GAME_TASK_SlotTable();
// 0x6FD30398: the queue words {head, tail, perf freq low, perf freq high, slot counter} (5 words; the counter is at +0x10).
uint32_t* D2GAME_TASK_QueueWords();

struct D2TaskSlotEnv {
    uint32_t selfAddr;                    // the address of the head word as retail stores it (head and tail point at it once set)
    void (*lockGlobal)(bool enter);       // 0x6FD31F58
    void (*lockSlot)(uint32_t idx, bool enter);   // 0x6FD31C58 + idx * 0x18
    uint32_t (*alloc)(uint32_t pool, uint32_t size, const char* file, uint32_t line, uint32_t flags);   // Fog 10045 (thunk 0xA2E6)
    void (*enqueue)(uint32_t slot);       // 0x524C0
};

uint32_t D2GAME_TASK_GetQueueSlot_52650(const D2TaskSlotEnv* e);
