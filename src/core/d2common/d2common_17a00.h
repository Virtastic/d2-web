#pragma once
#include <cstdint>
// Retail-layout seams of the act 3 layout init: the data tables and level-defs record base, and the five callees.
struct D2Common17A00Env {
    uint32_t dataTables, levelDefs;
    uint32_t (*getLevel)(uint32_t drlg, uint32_t levelId);          // 0x2D9B0 (ECX drlg, EDX id)
    uint32_t (*initJungle)(uint32_t level);                         // 0x15A80 (EAX level)
    void (*layoutChain)(uint32_t arg);                              // 0x15980 (stdcall 1)
    void (*linkOutdoor)(uint32_t drlg, uint32_t from, uint32_t to); // 0x16CF0 (stdcall 3)
    void (*linkLevels)(uint32_t drlg, uint32_t from, uint32_t to);  // 0x17760 (stdcall 3)
};
extern "C" void D2Common_17A00_InitAct3(uint32_t drlg, const D2Common17A00Env* env);
