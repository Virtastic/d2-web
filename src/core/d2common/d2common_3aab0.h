// Env of the 1.13c D2Common+0x3AAB0 port (state-list SET toggle): memory and retail's three callees as functions.
#pragma once
#include <cstdint>
struct D2Common3AAB0Env {
    uint32_t (*rd32)(uint32_t);
    void     (*wr32)(uint32_t, uint32_t);
    uint32_t (*lookup)(uint32_t statList, uint32_t state);     // 0x37F80 (ECX list, EDX state)
    void     (*expire)(uint32_t statList);                     // 0x3A3B0
    void     (*post)(uint32_t unit, uint32_t statList, uint32_t one);   // 0x3A7D0
};
extern "C" int32_t D2Common_3AAB0_StateSet(uint32_t unit, uint32_t state, uint32_t set, const D2Common3AAB0Env* e);
