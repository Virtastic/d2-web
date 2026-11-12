#include <Path/PathUtil.h>
#include <Units/Units.h>
#include <Fog.h>

static_assert(offsetof(D2DynamicPathStrc, tTargetCoord) == 0x10);
static_assert(offsetof(D2DynamicPathStrc, pTargetUnit) == 0x58 + 4 * (sizeof(void*) - 4));
static_assert(offsetof(D2DynamicPathStrc, dwSpeed) == 0x68 + 5 * (sizeof(void*) - 4));
static_assert(offsetof(D2DynamicPathStrc, nDirection) == 0x64 + 5 * (sizeof(void*) - 4));
static_assert(offsetof(D2UnitStrc, pDynamicPath) == 0x2C + 3 * (sizeof(void*) - 4));

//1.10f: D2Common.0x6FDAB890
// 1.13c D2Common+0x3E080
// The speed byte folds into a 64-step index (x8) and is also the scale: each LUT value is multiplied by the speed (a
// float32 of the byte, so the product is exact in double), truncated toward zero (_ftol), and its low word added to the
// 16-bit X and Y. Void in retail; the Eax form returns what it leaves in EAX, the low dword of the Y step.
uint32_t PATHUtil_AdvanceTowardsTargetEax(D2DynamicPathStrc* ptPath)
{
	D2_ASSERT(ptPath);
	D2UnitStrc* ptTarget = ptPath->pTargetUnit;
	D2_ASSERT(ptTarget);
	const uint32_t nSpeed = (uint8_t)ptPath->dwSpeed;
	int32_t nIndex = ((PATH_GetDirection(ptTarget->pDynamicPath) + (int32_t)nSpeed * -0x20) & 0x3F) << 3;
	const int64_t nStepX = (int64_t)((double)FOG_10083_Cos_LUT((int16_t)nIndex) * (double)nSpeed);
	ptPath->tTargetCoord.X = (uint16_t)(ptPath->tTargetCoord.X + (uint16_t)nStepX);
	const int64_t nStepY = (int64_t)((double)FOG_10084_Sin_LUT((int16_t)nIndex) * (double)nSpeed);
	ptPath->tTargetCoord.Y = (uint16_t)(ptPath->tTargetCoord.Y + (uint16_t)nStepY);
	return (uint32_t)nStepY;
}

void __fastcall PATHUtil_AdvanceTowardsTarget_6FDAB890(D2DynamicPathStrc* ptPath)
{
	PATHUtil_AdvanceTowardsTargetEax(ptPath);
}

//1.10f: D2Common.0x6FDAB940
//1.13c: D2Common.0x6FD8E310
void __fastcall sub_6FDAB940(D2PathPointStrc* pOutPathPoint, D2DynamicPathStrc* ptPath)
{
	D2_ASSERT(ptPath);
	D2_ASSERT(ptPath->pTargetUnit);
	D2UnitStrc* pTargetUnit = ptPath->pTargetUnit;
	D2C_UnitTypes dwTargetUnitType = (D2C_UnitTypes)pTargetUnit->dwUnitType;

	if (dwTargetUnitType == UNIT_OBJECT || dwTargetUnitType == UNIT_ITEM || dwTargetUnitType == UNIT_TILE)
	{
		pOutPathPoint->X = pTargetUnit->pStaticPath->tGameCoords.nX;
		pOutPathPoint->Y = pTargetUnit->pStaticPath->tGameCoords.nY;
	}
	else
	{
		if (D2DynamicPathStrc* pTargetPath = pTargetUnit->pDynamicPath)
		{
			pOutPathPoint->X = PATH_GetXPosition(pTargetPath);
			pOutPathPoint->Y = PATH_GetYPosition(pTargetPath);
		}
		else
		{
			*pOutPathPoint = { 0,0 };
		}
	}

	if (ptPath->dwSpeed)
	{
		PATHUtil_AdvanceTowardsTarget_6FDAB890(ptPath);
	}
}
