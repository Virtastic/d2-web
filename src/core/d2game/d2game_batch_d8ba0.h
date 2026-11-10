// Seam table of src/core/d2game/d2game_batch_d8ba0.cpp (retail D2Game 0xD8BA0 batch); a probe fills it with recorders.
#pragma once
#include <cstdint>
struct D2GameBatchD8baEnv {
    int32_t (*stat)(void* unit, int32_t id, int32_t layer);        // D2Common #10973 stat get
    int32_t (*statBase)(void* unit, int32_t id, int32_t layer);    // D2Common #10910
    int32_t (*hasState)(void* unit, int32_t state);                // D2Common #10494
    int32_t (*hasSkill)(void* unit, int32_t id);                   // D2Common #10744
    int32_t (*isPlayer)(void* unit);                               // D2Common #10239
    int32_t (*isPlayerLike)(void* unit);                           // D2Common #10255
    int32_t (*isMonsterLike)(void* unit);                          // D2Common #11104
    int32_t (*unitTest10064)(int32_t a, void* unit);               // D2Common #10064
    int32_t (*unitTest10278)(void* unit);                          // D2Common #10278
    int32_t (*stat10702)(void* unit, int32_t id, void* buf, int32_t n); // D2Common #10702 stat list
    int32_t (*statMax)(void* unit);                                // D2Common #10186
    void    (*statSet)(void* unit, int32_t, int32_t);              // D2Common #10887
    // 0xD8BA0 / 0xD8DE0 seams
    void    (*sub29F0)(void* unit);                                // 0xD29F0
    int32_t (*ord11017)(int32_t v);
    int32_t (*ord10478)(void* node);
    int32_t (*sub70D60)(void* unit, void* game);                   // 0x70D60
    void*   (*ord10920)(void* unit);
    int32_t (*subAD0F0)(void* game, void* node, uint32_t a2, uint32_t a3, void* esiNode, void* ediUnit);                                         // 0xAD0F0
    uint32_t (*sub21100)(void* ecxNode);                               // 0x21100
    int32_t (*sub17B0)();                                          // 0xD17B0
    void*   (*ord10708)(void* ecxUnit, uint32_t edxIdx, int32_t m1);
    int32_t (*ord10353)(void* h);
    void*   (*ord10507)(void* unit);
    void    (*sub2100)(void* ecxUnit, uint32_t edx, uint32_t z, int32_t m1);           // 0xA2100
    void    (*ord10252)(void* h, int32_t v);
    void    (*sub8D570)(uint32_t al, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t stk);          // 0x8D570
    void    (*sub3950)(void* eaxNode, void* ediUnit, void* game);                                          // 0xD3950
    void    (*sub2540)(uint32_t eaxV, void* unit);                                // 0xD2540
    const uint8_t* dataTables;                                     // sgptDataTables deref
    // misc
    void*   (*sub372C0)(void* x);                                  // 0x372C0
    int32_t (*sub214D0)(int32_t a, int32_t b);                     // 0x214D0
    uint32_t (*sub21620)(int32_t skill, uint32_t id);                              // 0x21620
    int32_t (*ord10064_1)(void* a);
    int32_t (*sub42A80)(void* unit, int32_t v);                                // 0xF44B0 / 0x42A80 flag test
    int64_t (*alldiv)(int64_t n, int64_t d);
};
extern const D2GameBatchD8baEnv* d2game_d8ba_env;
