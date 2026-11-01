// 1.13c D2Common 0x16C90 (see d2common_16c90.cpp).
#pragma once
#include <cstdint>
struct D2Common16C90Env {
    uint32_t (*allocLevel)(uint32_t pDrlg, int32_t nLevelId);                         // 0x2D9B0: ECX, EDX
    void (*addOrth)(uint32_t pLevel, uint32_t pHead, int32_t nDir, int32_t bPreset);  // 0x44F20: EAX; stack head, dir, preset
};
extern "C" void D2Common_16C90_LinkLevelById(uint32_t pLevel, int32_t nLevelId, const D2Common16C90Env* env);
