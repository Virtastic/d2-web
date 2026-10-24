// 1.13c D2Game+0x5A000: ACT5Q3_UpdateResistances, retail form.
// Convention: ESI = pUnit, EDI = nValue, one stack argument pGame (`ret 4`). Retail 32-bit layout; the vendored
// 1.10f body is src/core/d2moo/src/D2Game/QUESTS/ACT5/A5Q3.cpp:ACT5Q3_UpdateResistances. The callees are D2Common
// imports (0xA6A0, 0xA60A, 0xA6E2) and 0xCC2F0, handed in as seams (the way game_74ce0.h does).
#pragma once
#include <cstdint>

typedef uint32_t (*D2G5A000AllocFn)(uint32_t pPool, uint32_t fFilter, uint32_t dwTimeout, uint32_t nUnitType, uint32_t nUnitGUID); // 0xA6A0
typedef uint32_t (*D2G5A000PostFn)(uint32_t pUnit, uint32_t pStatList, uint32_t nReset);                                             // 0xA60A
typedef uint32_t (*D2G5A000SetFn)(uint32_t pStatList, uint32_t nStat, uint32_t nValue, uint32_t nLayer);                             // 0xA6E2
typedef uint32_t (*D2G5A000UpdFn)(uint32_t pUnit, uint32_t nStat, uint32_t nValue, uint32_t pPlayer);                                // 0xCC2F0

void GAME_5A000(uint32_t pUnit, uint32_t nValue, const uint32_t* pGame, D2G5A000AllocFn alloc, D2G5A000PostFn post,
                D2G5A000SetFn set, D2G5A000UpdFn upd);
