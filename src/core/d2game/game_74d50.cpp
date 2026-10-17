// 1.13c D2Game+0x74D50: the standalone path setter, 1:1 with the vendored
// 1.10f body (src/core/d2moo/src/D2Game/MONSTER/MonsterMode.cpp:sub_6FC65680) and with the runtime
// disassembly at 6fc94d50. Same statement order, same cache compare, same switch (type - 2 <= 0xb plus the
// four chase cases 2/7/9/0xd), same PATHTYPE_WALL_FOLLOW (0xf) rewrite and `ret 8`.
#include "game_74d50.h"

namespace {
constexpr int32_t kPathTypeToward = 2;        // PATHTYPE_TOWARD
constexpr int32_t kPathTypeStraight = 7;      // PATHTYPE_STRAIGHT
constexpr int32_t kPathTypeLeap = 9;          // PATHTYPE_LEAP
constexpr int32_t kPathTypeTowardFinish = 13; // PATHTYPE_TOWARD_FINISH
constexpr int32_t kPathTypeWallFollow = 15;   // PATHTYPE_WALL_FOLLOW
constexpr uint32_t kUnitFlagDoUpdate = 1;     // UNITFLAG_DOUPDATE, the unit's dwFlags bit 0
}

int32_t GAME_74D50(D2G74D50Unit* pUnit, int32_t nPathType, D2G74D50AiParam* pAiParam, int32_t dwNewDist,
                   D2G74D50SetDistanceFn setDistance, D2G74D50GetTargetFn getTarget,
                   D2G74D50GetXFn getFirstX, D2G74D50GetYFn getFirstY, D2G74D50SetTypeFn setType,
                   D2G74D50PathStepFn pathStep, D2G74D50RefreshFn refresh, D2G74D50NumPointsFn numPoints)
{
    const uint32_t pPath = pUnit->pDynamicPath;
    setDistance(pPath, (uint32_t)dwNewDist);

    const uint32_t pTarget = getTarget(pPath);
    const int32_t nX = getFirstX(pPath);
    const int32_t nY = getFirstY(pPath);

    if (pTarget != pAiParam->pTarget || nX != pAiParam->unk0x08 || nY != pAiParam->unk0x0C)
    {
        pAiParam->pTarget = pTarget;
        pAiParam->unk0x08 = nX;
        pAiParam->unk0x0C = nY;
        pAiParam->unk0x00 = 0;
        pAiParam->unk0x01 = 0;
    }

    setType(pPath, nPathType);
    pathStep(pPath, (uint32_t)(uintptr_t)pUnit, 0);

    if (numPoints(pPath))
    {
        refresh((uint32_t)(uintptr_t)pUnit);
        pUnit->dwFlags |= kUnitFlagDoUpdate;
        pAiParam->unk0x00 = 0;
        pAiParam->unk0x01 = 0;
    }
    else
    {
        switch (nPathType)
        {
        case kPathTypeToward:
        case kPathTypeStraight:
        case kPathTypeLeap:
        case kPathTypeTowardFinish:
            refresh((uint32_t)(uintptr_t)pUnit);
            pUnit->dwFlags |= kUnitFlagDoUpdate;
            setType(pPath, kPathTypeWallFollow);
            pathStep(pPath, (uint32_t)(uintptr_t)pUnit, 0);
            return kPathTypeWallFollow;

        default:
            break;
        }
    }

    return nPathType;
}
