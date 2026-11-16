// Env of the 1.13c D2Common+0x2DDB0 port: retail's callees (0x44CE0 link-list fetch, 0x2D9B0 level allocation, the pool free
// thunk 0x9228) as functions, and the raw 32-bit reads and writes of the DRLG records.
#pragma once
#include <cstdint>
struct D2Common2DDB0Env {
    uint32_t (*rd32)(uint32_t);
    uint32_t (*rd8)(uint32_t);
    void     (*wr32)(uint32_t, uint32_t);
    uint32_t (*visIds)(uint32_t drlg, int32_t levelId);       // 0x44CE0: EAX = drlg, EDX = id; returns the eight-id array
    uint32_t (*allocLevel)(uint32_t drlg, int32_t levelId);   // 0x2D9B0: ECX = drlg, EDX = id
    void     (*freePool)(uint32_t pool, uint32_t p, uint32_t line);   // 0x9228: ECX = pool, EDX = pointer, then file/line/0 (line 0x58 DrlgRoom.cpp node, 0x16F Drlg.cpp extra)
};
extern "C" int32_t D2Common_2DDB0_LevelDiscard(uint32_t level, const D2Common2DDB0Env* e);
