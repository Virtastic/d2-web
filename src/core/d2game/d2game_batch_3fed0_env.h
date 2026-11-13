#pragma once
#include <cstdint>
struct D2GameBatch3fEnv {
    void*    dataTables;                                   // *sgptDataTables: +0xB64 monstats, +0xB6C count, +0xB98/+0xBA0 skills, +0xC4 missiles cnt, +0xBD4, +0xBC0
    uint32_t (*tail)(void* game);                          // 0x3F320, the common mode tail
    void*    (*getTarget)(void* game, uint32_t guid);      // 0xE03A0
    uint32_t (*targetBusy)(void);                          // 0xDFA90
    void*    (*refreshTarget)(void* game, void* unit);     // 0xE07A0 (EAX = unit, ECX = game)
    uint32_t (*gameKey)(void* game);                       // 0xE0A20
    uint32_t (*unitX)(void* unit);                         // 0x1100 via 0x21100
    uint32_t (*unitY)(void* unit);                         // 0x1130 via 0x21130
    uint32_t (*roll)(uint32_t a, uint32_t b);              // 0x214D0 scaled-percent
    uint32_t (*sendRequest)(void* game, uint32_t* req);    // 0x6F930/0x8F930 request sink (23 dwords)
    void     (*sub_A0000)(void* game);                     // 0xA0000
    void     (*sub_CC0000)(void* game);                    // FUN_6fcc0000
    void     (*sub_3A1D0)(void* game);
    void     (*sub_3EDE0)(void* game, void* p, uint32_t f);
    void     (*sub_3D1E0)(uint32_t eax, void* game, uint32_t a, uint32_t b, uint32_t c);   // 0x3D1E0: EAX = value, stack = (game, a, b, c)
    uint32_t (*o10240get)(void* unit);                    // Ordinal_10240(unit)
    void     (*o10240set)(void* unit, uint32_t v);
    uint32_t (*o10985)(void* unit);
    uint32_t (*o11029)(void* unit);
    uint32_t (*o11029set)(void* unit, int32_t skill, uint32_t x, uint32_t y);
    uint32_t (*o10973)(void* unit, uint32_t stat, uint32_t p);
    uint32_t (*o10913)(void* unit);
    void     (*o10786)(void* unit, uint32_t a, uint32_t b);
    void     (*o10887)(void* unit, uint32_t stat, uint32_t v, uint32_t p);
    void     (*o10689)(void* unit, int32_t a, uint32_t b);
    void     (*o10970)(void* unit, uint32_t mode);
    void*    (*o10203)(void* room);                        // room validity
    uint32_t (*o10737)(void* unit);
    uint32_t (*o10522)(void* a, void* b);
    void*    (*o10392)(void* room);
    void     (*o10286)(void* room, uint32_t x, uint32_t y);
    void     (*o10334)(void* room, void* unit, uint32_t z);
    uint32_t (*sub_BD00)(void* unit);                      // 0xE0BD0 (ESI = unit)
    uint32_t (*sub_E0570)(void* unit);                     // 0xE0570 (ECX = unit)
    void*    (*sub_C00B0)(const uint32_t* blk);            // 0xA00B0 (EAX = 8-dword block)
    void*    (*o10786r)(void* unit, uint32_t a, uint32_t b, uint32_t c);
    void     (*o10188)(void* p, int32_t a, void* b, uint32_t c);
    void     (*o11039)(void* game, int32_t skill, uint32_t one);
};
