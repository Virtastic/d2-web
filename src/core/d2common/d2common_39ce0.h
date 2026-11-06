// 1.13c D2Common+0x39CE0 with injectable callees (0x39530 lookup, 0x38940 record, 0x38B70 unit stat, 0x382B0 list search).
#pragma once
#include <cstdint>
struct D2Common39CE0Callees {
    void* ctx;
    int32_t (*lookup)(void* ctx, uint32_t owner, uint32_t key);                                              // 0x39530: ECX = owner, EDX = key
    void (*record)(void* ctx, uint32_t owner, uint32_t key, int32_t value, uint32_t rec, uint32_t flags);   // 0x38940: EBX = owner, stack key, value, rec, flags (ret 0x10)
    int32_t (*readStat)(void* ctx, uint32_t unit, uint32_t stat);                                            // 0x38B70: stdcall (unit, stat, 0)
    int32_t (*find)(void* ctx, uint32_t list, uint32_t key);                                                 // 0x382B0: EAX = list header, EDI = key
};
// Retail convention: fastcall (ECX = owner, EDX = key), stack rec and flags, ret 8. iscBase / iscCount: sgptDataTables +0xBCC / +0xBD4.
extern "C" int32_t D2Common_39CE0_Injected(const D2Common39CE0Callees* c, uint32_t owner, uint32_t key, uint32_t rec, uint32_t flags,
                                           uint32_t iscBase, int32_t iscCount);
