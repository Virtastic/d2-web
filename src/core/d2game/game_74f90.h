// 1.13c D2Game+0x74F90: the mode-change path resolver the 1.13c compiler
// factorised out of D2GAME_ModeChange_6FC65220 (vendored: src/core/d2moo/src/D2Game/MONSTER/MonsterMode.cpp,
// the inlined block at line 1837). Retail 32-bit layout only -- the three slices below pin exactly the fields
// the function reads or writes, the way src/core/d2game/game_8b340.cpp pins its client block.
#pragma once
#include <cstddef>
#include <cstdint>

#pragma pack(push, 1)
struct D2G74F90AiParam {              // D2AiParamStrc
    uint32_t pad00[4];                // +0x00..+0x0C (the probe parks the two scripted callee returns at +0/+4)
    uint32_t nVelocity;               // +0x10 written: the resolved velocity
    uint32_t nSentinel;               // +0x14 written: -1 or 10
    int32_t nPathTypeOverride;        // +0x18 read, then cleared
    int32_t nVelocityOverride;        // +0x1C read, then cleared
    uint8_t nDistOverride;            // +0x20 read (byte), then cleared (byte)
    uint8_t pad21[3];
};
struct D2G74F90Unit { uint8_t pad00[0x2C]; uint32_t pDynamicPath; };   // D2UnitStrc, +0x2C only
struct D2G74F90Game {                 // D2GameStrc
    uint8_t pad00[0x1D70];
    uint32_t dwPathTypesCount[17];    // +0x1D70
    uint32_t nTotalPathTypesCount;    // +0x1DB4
};
#pragma pack(pop)
static_assert(offsetof(D2G74F90AiParam, nVelocity) == 0x10 && offsetof(D2G74F90AiParam, nSentinel) == 0x14 &&
              offsetof(D2G74F90AiParam, nPathTypeOverride) == 0x18 && offsetof(D2G74F90AiParam, nVelocityOverride) == 0x1C &&
              offsetof(D2G74F90AiParam, nDistOverride) == 0x20 && sizeof(D2G74F90AiParam) == 0x24, "ai param fields");
static_assert(offsetof(D2G74F90Unit, pDynamicPath) == 0x2C, "unit field");
static_assert(offsetof(D2G74F90Game, dwPathTypesCount) == 0x1D70 && offsetof(D2G74F90Game, nTotalPathTypesCount) == 0x1DB4, "game counters");

// The two callees cannot run on blobs -- 0x74D50 needs a live dynamic path and a live room, PATH_GetTargetUnit
// (0xA598, VERIFIED) a live path -- so the caller hands them in as seams. Both take the 32-bit pointer the
// retail build used; the replay probes the low half of the wasm address (as everywhere else in src/core).
typedef uint32_t (*D2G74F90SetPathFn)(uint32_t pUnit, int32_t nPathType, uint8_t nNewDist);   // 0x74D50, ret 8, result = the type in use
typedef uint32_t (*D2G74F90HasTargetFn)(uint32_t pDynamicPath);                               // 0xA598, ret 4, non-zero = a target remains

// Retail's own order: EAX = pAiParam, ECX = pUnit, EDX = nPathType, then pGame, nVelocity, dwNewDist on the stack.
uint32_t GAME_74F90(D2G74F90AiParam* pAiParam, D2G74F90Unit* pUnit, int32_t nPathType, D2G74F90Game* pGame,
                    int32_t nVelocity, uint8_t nNewDist, D2G74F90SetPathFn setPath, D2G74F90HasTargetFn hasTarget);
