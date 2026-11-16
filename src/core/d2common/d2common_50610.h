// Env of the 1.13c D2Common+0x50610 port (the skill's item-type check for one hand): sgptDataTables and seven callees as inputs.
#pragma once
#include <cstdint>
struct D2Common50610Env {
    uint32_t dataTables;                                         // sgptDataTables (+0xBCC, +0xBD4, +0xBF8, +0xBFC read)
    uint32_t (*isType)(uint32_t item, uint32_t type);            // 0x24430 (stdcall 2)
    int32_t (*recIndex)(uint32_t classId);                       // 0x230A0 (stdcall 1)
    uint32_t (*hasStat)(uint32_t statList, uint32_t key, uint32_t rec);   // 0x38A80 (EBX = list, EDI = key, stack = rec)
    uint32_t (*itemType)(uint32_t item);                         // 0x24EB0 (stdcall 1)
    uint32_t (*getStat)(uint32_t item, uint32_t stat, uint32_t layer);    // 0x39430 (stdcall 3)
    uint32_t (*check)(uint32_t item);                            // 0x260F0 (stdcall 1)
    int32_t (*baseStat)(uint32_t item, uint32_t stat, uint32_t layer);    // 0x38B70 (stdcall 3)
};
extern "C" uint32_t D2Common_50610_HandTypeOk(uint32_t hand, uint32_t pItemA, uint32_t pItemB, uint32_t pTxt, const D2Common50610Env* e);
