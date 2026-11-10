// 1.13c D2Game batch 0xE2C40..0xE4890: callee environment, shared with the function probes.
#pragma once
#include <cstdint>

struct D2GameBatchE2C40Env {
    uint32_t (*d2common)(uint32_t ordinal, uint32_t a, uint32_t b, uint32_t c); // D2Common #ordinal, stdcall/fastcall args as listed
    void*    (*sub33F30)(uint32_t eax, uint32_t ecx); // FUN_6fc53f30: EAX = p[4], ECX = p[0x10]; returns the record with the callback at +0xF0
    int      (*isBadCodePtr)(const void* fn);       // kernel32 IsBadCodePtr
    uint32_t (*callCb)(void* rec);                  // (*(rec+0xF0))() with ECX = rec
    void     (*fatal)(uint32_t line);               // Ordinal_10265(line); Ordinal_10024(msg,...); _exit(-1)
    // ITEMS_FindQuestItem (0xE660): 4CC code; the unit is a live register not shown in the decompile.
    int      (*findQuestItem)(uint32_t code);
};

void d2game_E34D0(D2GameBatchE2C40Env& e, uint32_t eax, const uint32_t* unit, const void* game);
uint32_t d2game_E2D40(D2GameBatchE2C40Env& e, void* p);
uint32_t d2game_E2E80(D2GameBatchE2C40Env& e, void* p);
uint32_t d2game_E2F00(D2GameBatchE2C40Env& e, void* p);
void d2game_E3290(D2GameBatchE2C40Env& e, uint32_t* p);
