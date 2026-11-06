// 1.13c D2Common+0x39B60 with injectable callees (0x39530 stat read, 0x38940 stat apply).
#pragma once
#include <cstdint>
struct D2Common39B60Callees {
    void* ctx;
    int32_t (*readStat)(void* ctx, uint32_t unitData, uint32_t idShifted);                                   // 0x39530: ECX = unit+0x5C, EDX = id << 16
    void (*apply)(void* ctx, uint32_t unitData, uint32_t idShifted, int32_t value, uint32_t opRow, uint32_t unit); // 0x38940: EBX = unit+0x5C, ESI = id << 16, stack id << 16, value, opRow, unit (ret 0x10)
};
// iscBase / iscCount: sgptDataTables +0xBCC / +0xBD4 (itemstatcost rows, stride 0x144).
extern "C" void D2Common_39B60_Injected(const D2Common39B60Callees* c, uint32_t unit, uint32_t param, uint32_t iscBase, int32_t iscCount);
