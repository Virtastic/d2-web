// 1.13c D2Game+0x74D50: the path setter the 1.13c compiler factored out of
// D2GAME_ModeChange_6FC65220 -- give the dynamic path a new distance and type, rebuild it, and if it yields
// no point at all, retry the four chase types as PATHTYPE_WALL_FOLLOW. Vendored 1.10f body:
// src/core/d2moo/src/D2Game/MONSTER/MonsterMode.cpp:sub_6FC65680. Retail 32-bit layout only.
// Its eight D2Common callees need a live dynamic path and a live room, so -- as in game_74f90.h -- the vector
// scripts them: the capture hooks each thunk, the probe hands in a seam that logs and returns this tuple's
// scripted value (the retail capture tool/fn/d2game-74d50.txt).
#pragma once
#include <cstddef>
#include <cstdint>

#pragma pack(push, 1)
struct D2G74D50AiParam {              // D2AiParamStrc + the probe's four scripted callee returns
    uint8_t  unk0x00;                 // +0x00  cleared when the cache changes or the path has a point
    uint8_t  unk0x01;                 // +0x01
    uint8_t  pad02[2];
    uint32_t pTarget;                 // +0x04  cached target: compared, then written
    int32_t  unk0x08;                 // +0x08  cached first-point X
    int32_t  unk0x0C;                 // +0x0C  cached first-point Y
    uint32_t scriptedTarget;          // +0x10  probe only: PATH_GetTargetUnit's return
    int32_t  scriptedX;               // +0x14  probe only: PathGetFirstPointX's return
    int32_t  scriptedY;               // +0x18  probe only: PathGetFirstPointY's return
    int32_t  scriptedNumPoints;       // +0x1C  probe only: PATH_GetNumberOfPathPoints' return
};
struct D2G74D50Unit {                 // D2UnitStrc
    uint8_t  pad00[0x2C];
    uint32_t pDynamicPath;            // +0x2C
    uint8_t  pad30[0xC4 - 0x30];
    uint32_t dwFlags;                 // +0xC4  bit 0 = UNITFLAG_DOUPDATE
};
#pragma pack(pop)
static_assert(offsetof(D2G74D50AiParam, pTarget) == 0x04 && offsetof(D2G74D50AiParam, unk0x08) == 0x08 &&
              offsetof(D2G74D50AiParam, unk0x0C) == 0x0C && offsetof(D2G74D50AiParam, scriptedNumPoints) == 0x1C &&
              sizeof(D2G74D50AiParam) == 0x20, "ai param fields");
static_assert(offsetof(D2G74D50Unit, pDynamicPath) == 0x2C && offsetof(D2G74D50Unit, dwFlags) == 0xC4 &&
              sizeof(D2G74D50Unit) == 0xC8, "unit fields");

// The eight D2Common seams. Their 1.13c D2Game import-thunk RVAs (the call order below is the retail order):
typedef void     (*D2G74D50SetDistanceFn)(uint32_t pPath, uint32_t dwDist);             // 0xAB14 PATH_SetNewDistance
typedef uint32_t (*D2G74D50GetTargetFn)(uint32_t pPath);                                // 0xA598 PATH_GetTargetUnit
typedef int32_t  (*D2G74D50GetXFn)(uint32_t pPath);                                     // 0xAAC6 first-point X
typedef int32_t  (*D2G74D50GetYFn)(uint32_t pPath);                                     // 0xAAD8 first-point Y
typedef void     (*D2G74D50SetTypeFn)(uint32_t pPath, int32_t nType);                   // 0xAAFC PATH_SetType
typedef void     (*D2G74D50PathStepFn)(uint32_t pPath, uint32_t pUnit, int32_t bForce); // 0xAAF0 D2Common_10142
typedef void     (*D2G74D50RefreshFn)(uint32_t pUnit);                                  // 0xA4DE UNITROOM_RefreshUnit
typedef int32_t  (*D2G74D50NumPointsFn)(uint32_t pPath);                                // 0xAC70 PATH_GetNumberOfPathPoints

// Retail: EAX = dwNewDist (only AL used), EDI = pAiParam, [esp+4] = pUnit, [esp+8] = nPathType; `ret 8`.
int32_t GAME_74D50(D2G74D50Unit* pUnit, int32_t nPathType, D2G74D50AiParam* pAiParam, int32_t dwNewDist,
                   D2G74D50SetDistanceFn setDistance, D2G74D50GetTargetFn getTarget,
                   D2G74D50GetXFn getFirstX, D2G74D50GetYFn getFirstY, D2G74D50SetTypeFn setType,
                   D2G74D50PathStepFn pathStep, D2G74D50RefreshFn refresh, D2G74D50NumPointsFn numPoints);
