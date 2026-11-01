// 1.13c D2Common 0x186D0 / 0x18830 path-walker helpers (see d2common_186d0.cpp, d2common_18830.cpp).
#pragma once
#include <cstdint>
// 0x4CE90 collision probe (stdcall room, x, y, pattern, mask): nonzero = blocked.
struct D2CommonWalkEnv { uint32_t (*chk)(uint32_t room, uint32_t x, uint32_t y, uint32_t pattern, uint32_t mask); };
extern "C" uint32_t D2Common_186D0_StepWalker(uint32_t pInfo, uint32_t* walker, const D2CommonWalkEnv* env);
extern "C" int32_t D2Common_18830_FindSubpathWithoutObstacles(uint32_t pInfo, uint32_t startPt, uint32_t* buf,
    int32_t* pIdx, int32_t* pEnd, int32_t maxLen, int32_t majorDir, const D2CommonWalkEnv* env);
