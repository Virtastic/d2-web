// Env of the 1.13c D2Common+0x39530 port (a stat's value with its ItemStatCost op chain applied): sgptDataTables and nine callees as inputs.
#pragma once
#include <cstdint>
struct D2Common39530Env {
    uint32_t dataTables;                                                  // sgptDataTables (+0xBCC records, +0xBD4 count)
    int32_t (*base)(uint32_t statList, uint32_t key, uint32_t rec);       // 0x38CD0 (ECX = list, EAX = the incoming key, stack = record)
    uint32_t (*statRec)(uint32_t stat);                                   // 0x17A0 (EAX = stat)
    uint32_t (*charRec)(uint32_t classId);                                // 0x12D0 (EAX = class)
    int32_t (*find)(uint32_t hdr, uint32_t key);                          // 0x382B0 (EAX = array header, EDI = key)
    int32_t (*ownerStat)(uint32_t owner, uint32_t stat, uint32_t layer);  // 0x38C20 (stdcall 3)
    int32_t (*unitStat)(uint32_t unit, uint32_t stat, uint32_t layer);    // 0x38B70 (stdcall 3)
    uint32_t (*skillOf)(uint32_t g, uint32_t* out);                       // 0x37470 (stdcall 2)
    int32_t (*eval)(uint32_t r, uint32_t a, uint32_t t);                  // 0x46330 (stdcall 6, last three zero)
    int32_t (*pct)(int32_t a, int32_t d);                                 // 0x11E0 (EAX = a, ECX = 100, EDX = d)
};
extern "C" int32_t D2Common_39530_StatWithOps(uint32_t pStatList, uint32_t key, const D2Common39530Env* e);
