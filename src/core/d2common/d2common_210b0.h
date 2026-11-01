#pragma once
#include <cstdint>
// Retail-layout seams of the backpack stack search: the data tables base and the six callees.
struct D2Common210B0Env {
    uint32_t dataTables;
    uint32_t (*ownerPlayer)(uint32_t owner, uint32_t a, uint32_t b);   // 0x2FB60 (stdcall 3)
    void (*pageKey)(uint32_t p, uint32_t a, uint32_t out);             // 0x81FB0 (stdcall 3)
    uint32_t (*page)(uint32_t inv, uint32_t key);                      // 0x1E1B0 (EAX 2, EBX inv, stdcall 1)
    uint32_t (*canStack)(uint32_t it, uint32_t item);                  // 0x26930 (stdcall 2)
    int32_t (*statValue)(uint32_t statList, uint32_t desc);            // 0x38A80 (EBX list, EDI stat 0x46 << 16, stdcall 1)
    int32_t (*maxStack)(uint32_t it);                                  // 0x260F0 (stdcall 1)
};
extern "C" uint32_t D2Common_210B0_FindStack(uint32_t inv, uint32_t item, uint32_t start, const D2Common210B0Env* env);
