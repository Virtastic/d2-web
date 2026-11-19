// 1.13c D2Game+0x74F90: finishes a monster mode change. The AI-param overrides
// (path type / velocity / distance, each overriding the argument and then cleared), the "100 = no path"
// branch (velocity forced to 0, no path set up), the 0 -> 5 distance and 101 -> 13 path-type adjustments,
// the path setup through 0x74D50, the -1/10 AI sentinel and the two game path-type counters.
// 1:1 with the runtime disassembly at 6fc94f90 (e.g. `or ebp,0xffffffff` = the -1 sentinel, `mov BYTE PTR
// [esp+0x1c],0x5` = the distance default, `mov edx,0xd` = the 101 remap) and with the same block inlined in
// the vendored 1.10f source (MonsterMode.cpp:1837), which is byte-for-byte the same rule.
#include "game_74f90.h"

// PATHTYPE_ASTAR / PATHTYPE_TOWARD_FINISH from the vendored enum (src/core/d2moo/include/Path/Path.h);
// 100 and 101 are the mode-change selectors the function itself resolves, they are not path types.
namespace {
constexpr int32_t kPathTypeAstar = 1;
constexpr int32_t kPathTypeTowardFinish = 13;
constexpr int32_t kNoPath = 100;
constexpr int32_t kTowardFinish = 101;
constexpr int32_t kDefaultDistance = 5;
constexpr uint32_t kSentinelNoTarget = 0xFFFFFFFFu;   // -1
constexpr uint32_t kSentinelHasTarget = 10;
}

uint32_t GAME_74F90(D2G74F90AiParam* pAiParam, D2G74F90Unit* pUnit, int32_t nPathType, D2G74F90Game* pGame,
                    int32_t nVelocity, uint8_t nNewDist, D2G74F90SetPathFn setPath, D2G74F90HasTargetFn hasTarget)
{
    if (pAiParam->nPathTypeOverride)
    {
        nPathType = pAiParam->nPathTypeOverride;
    }

    if (pAiParam->nVelocityOverride)
    {
        nVelocity = pAiParam->nVelocityOverride;
    }

    if (pAiParam->nDistOverride)
    {
        nNewDist = pAiParam->nDistOverride;
    }

    uint32_t nSentinel = kSentinelNoTarget;
    pAiParam->nPathTypeOverride = 0;
    pAiParam->nVelocityOverride = 0;
    pAiParam->nDistOverride = 0;

    uint32_t nNewPathType = 0;
    if (nPathType == kNoPath)
    {
        nVelocity = 0;
    }
    else
    {
        if (!nNewDist)
        {
            nNewDist = kDefaultDistance;
        }

        if (nPathType == kTowardFinish)
        {
            nPathType = kPathTypeTowardFinish;
        }

        nNewPathType = setPath((uint32_t)(uintptr_t)pUnit, nPathType, nNewDist);
        if (nNewPathType != (uint32_t)kPathTypeAstar && hasTarget(pUnit->pDynamicPath))
        {
            nSentinel = kSentinelHasTarget;
        }
    }

    pAiParam->nVelocity = (uint32_t)nVelocity;
    pAiParam->nSentinel = nSentinel;
    ++pGame->dwPathTypesCount[nNewPathType];
    if (nNewPathType)
    {
        ++pGame->nTotalPathTypesCount;
    }

    return 0;
}
