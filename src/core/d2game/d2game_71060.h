// Seam for D2Game+0x71060 (stdcall game, maxLevel, filter; ret 0xC; AX = picked slot or 0).
#pragma once
#include <cstdint>
struct D2Game71060Env {
    uint32_t (*table)();                                   // 0xAFB8: returns a pointer to {count, base}; entries are 0x1A8 bytes
    int (*slotOk)(uint32_t slot, uint32_t filter);         // 0xB0C0, two stack words
    void (*fatal)(uint32_t line);                          // never returns
};
uint32_t d2game_port_71060(uint32_t game, int32_t maxLevel, uint32_t filter, const D2Game71060Env* env);
