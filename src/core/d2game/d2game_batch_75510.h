#pragma once
#include <cstdint>

struct D2GameBatch75510Env {
    uint32_t (*sub74F20)(uint32_t eax, uint32_t unit);  // 0x74F20: EAX = record value, ECX = unit
    int32_t  (*o10800)(uint32_t id);                  // stdcall(id), thunk 0xAC70
    int32_t  (*sub18A0)(uint32_t unit);              // 0x18A0: EAX = unit
    void     (*o10334)(uint32_t id, uint32_t unit, uint32_t zero); // stdcall thunk 0xAAF0
    void     (*subDA750)(uint32_t game);              // 0xDA750: stack = game (ECX of the caller)
    void     (*o11090)(uint32_t unit, uint32_t value); // 0xA4A2
    int32_t  (*o10511)(uint32_t unit);                // 0xAABA
    void     (*subA0830)(uint32_t unit, uint32_t game); // 0xA0830: ESI = unit, stack = game
    int32_t  tableCount;                              // *(sgptDataTables + 0xA80)
};

extern "C" uint32_t d2_game_75510(uint32_t param1, const uint32_t* param2, const D2GameBatch75510Env* env);
