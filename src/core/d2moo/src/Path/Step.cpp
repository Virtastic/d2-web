#include "Path/Step.h"

#include "D2Collision.h"
#include "D2Dungeon.h"
#include "Path/Path.h"
#include "Path/PathMisc.h"
#include "Units/Units.h"
#include "Units/UnitRoom.h"
#include <utility>

D2PathPointStrc sgctZeroGameCoord = { 0,0 };

bool COORD_TEST_EQUAL(D2PathPointStrc lhs, D2PathPointStrc rhs) { return lhs == rhs; }


struct TanToDirectionLutEntryStrc {
	D2CoordStrc dirVector;
	uint32_t nAngle;
};

//1.00:  D2Common.0x1008AF38
//1.10f: D2Common.0x6FDD2598 
TanToDirectionLutEntryStrc lutTanToDirection[] = {
	{{ 0x0000, 0x1000 } , 0 },
	{{ 0x0020, 0x0FFF } , 0 },
	{{ 0x0040, 0x0FFF } , 0 },
	{{ 0x0060, 0x0FFE } , 0 },
	{{ 0x0080, 0x0FFD } , 0 },
	{{ 0x00A1, 0x0FFC } , 0 },
	{{ 0x00C1, 0x0FFB } , 0 },
	{{ 0x00E1, 0x0FF9 } , 0 },
	{{ 0x0101, 0x0FF7 } , 0 },
	{{ 0x0121, 0x0FF5 } , 0 },
	{{ 0x0141, 0x0FF3 } , 0 },
	{{ 0x0161, 0x0FF0 } , 0 },
	{{ 0x0181, 0x0FED } , 0 },
	{{ 0x01A1, 0x0FEA } , 1 },
	{{ 0x01C0, 0x0FE7 } , 1 },
	{{ 0x01E0, 0x0FE3 } , 1 },
	{{ 0x01FF, 0x0FDF } , 1 },
	{{ 0x021F, 0x0FDB } , 1 },
	{{ 0x023E, 0x0FD7 } , 1 },
	{{ 0x025E, 0x0FD2 } , 1 },
	{{ 0x027D, 0x0FCE } , 1 },
	{{ 0x029C, 0x0FC9 } , 1 },
	{{ 0x02BB, 0x0FC3 } , 1 },
	{{ 0x02D9, 0x0FBE } , 1 },
	{{ 0x02F8, 0x0FB8 } , 1 },
	{{ 0x0317, 0x0FB2 } , 1 },
	{{ 0x0335, 0x0FAC } , 2 },
	{{ 0x0353, 0x0FA6 } , 2 },
	{{ 0x0371, 0x0F9F } , 2 },
	{{ 0x038F, 0x0F99 } , 2 },
	{{ 0x03AD, 0x0F92 } , 2 },
	{{ 0x03CB, 0x0F8B } , 2 },
	{{ 0x03E8, 0x0F83 } , 2 },
	{{ 0x0406, 0x0F7C } , 2 },
	{{ 0x0423, 0x0F74 } , 2 },
	{{ 0x0440, 0x0F6C } , 2 },
	{{ 0x045D, 0x0F64 } , 2 },
	{{ 0x0479, 0x0F5C } , 2 },
	{{ 0x0496, 0x0F54 } , 2 },
	{{ 0x04B2, 0x0F4B } , 3 },
	{{ 0x04CE, 0x0F42 } , 3 },
	{{ 0x04EA, 0x0F39 } , 3 },
	{{ 0x0506, 0x0F30 } , 3 },
	{{ 0x0521, 0x0F27 } , 3 },
	{{ 0x053C, 0x0F1E } , 3 },
	{{ 0x0558, 0x0F14 } , 3 },
	{{ 0x0572, 0x0F0B } , 3 },
	{{ 0x058D, 0x0F01 } , 3 },
	{{ 0x05A8, 0x0EF7 } , 3 },
	{{ 0x05C2, 0x0EED } , 3 },
	{{ 0x05DC, 0x0EE3 } , 3 },
	{{ 0x05F6, 0x0ED8 } , 3 },
	{{ 0x0610, 0x0ECE } , 3 },
	{{ 0x0629, 0x0EC4 } , 4 },
	{{ 0x0642, 0x0EB9 } , 4 },
	{{ 0x065B, 0x0EAE } , 4 },
	{{ 0x0674, 0x0EA3 } , 4 },
	{{ 0x068D, 0x0E98 } , 4 },
	{{ 0x06A5, 0x0E8D } , 4 },
	{{ 0x06BD, 0x0E82 } , 4 },
	{{ 0x06D5, 0x0E77 } , 4 },
	{{ 0x06ED, 0x0E6C } , 4 },
	{{ 0x0704, 0x0E60 } , 4 },
	{{ 0x071C, 0x0E55 } , 4 },
	{{ 0x0733, 0x0E49 } , 4 },
	{{ 0x074A, 0x0E3E } , 4 },
	{{ 0x0760, 0x0E32 } , 4 },
	{{ 0x0777, 0x0E26 } , 4 },
	{{ 0x078D, 0x0E1A } , 5 },
	{{ 0x07A3, 0x0E0F } , 5 },
	{{ 0x07B9, 0x0E03 } , 5 },
	{{ 0x07CE, 0x0DF7 } , 5 },
	{{ 0x07E4, 0x0DEB } , 5 },
	{{ 0x07F9, 0x0DDF } , 5 },
	{{ 0x080E, 0x0DD3 } , 5 },
	{{ 0x0822, 0x0DC6 } , 5 },
	{{ 0x0837, 0x0DBA } , 5 },
	{{ 0x084B, 0x0DAE } , 5 },
	{{ 0x085F, 0x0DA2 } , 5 },
	{{ 0x0873, 0x0D96 } , 5 },
	{{ 0x0887, 0x0D89 } , 5 },
	{{ 0x089A, 0x0D7D } , 5 },
	{{ 0x08AD, 0x0D71 } , 5 },
	{{ 0x08C0, 0x0D64 } , 5 },
	{{ 0x08D3, 0x0D58 } , 5 },
	{{ 0x08E6, 0x0D4B } , 6 },
	{{ 0x08F8, 0x0D3F } , 6 },
	{{ 0x090A, 0x0D33 } , 6 },
	{{ 0x091C, 0x0D26 } , 6 },
	{{ 0x092E, 0x0D1A } , 6 },
	{{ 0x0940, 0x0D0D } , 6 },
	{{ 0x0951, 0x0D01 } , 6 },
	{{ 0x0962, 0x0CF5 } , 6 },
	{{ 0x0973, 0x0CE8 } , 6 },
	{{ 0x0984, 0x0CDC } , 6 },
	{{ 0x0995, 0x0CCF } , 6 },
	{{ 0x09A5, 0x0CC3 } , 6 },
	{{ 0x09B6, 0x0CB7 } , 6 },
	{{ 0x09C6, 0x0CAA } , 6 },
	{{ 0x09D6, 0x0C9E } , 6 },
	{{ 0x09E5, 0x0C92 } , 6 },
	{{ 0x09F5, 0x0C85 } , 6 },
	{{ 0x0A04, 0x0C79 } , 6 },
	{{ 0x0A14, 0x0C6D } , 6 },
	{{ 0x0A23, 0x0C61 } , 6 },
	{{ 0x0A31, 0x0C54 } , 7 },
	{{ 0x0A40, 0x0C48 } , 7 },
	{{ 0x0A4F, 0x0C3C } , 7 },
	{{ 0x0A5D, 0x0C30 } , 7 },
	{{ 0x0A6B, 0x0C24 } , 7 },
	{{ 0x0A79, 0x0C18 } , 7 },
	{{ 0x0A87, 0x0C0C } , 7 },
	{{ 0x0A95, 0x0C00 } , 7 },
	{{ 0x0AA2, 0x0BF4 } , 7 },
	{{ 0x0AB0, 0x0BE8 } , 7 },
	{{ 0x0ABD, 0x0BDC } , 7 },
	{{ 0x0ACA, 0x0BD0 } , 7 },
	{{ 0x0AD7, 0x0BC4 } , 7 },
	{{ 0x0AE4, 0x0BB8 } , 7 },
	{{ 0x0AF0, 0x0BAC } , 7 },
	{{ 0x0AFD, 0x0BA1 } , 7 },
	{{ 0x0B09, 0x0B95 } , 7 },
	{{ 0x0B15, 0x0B89 } , 7 },
	{{ 0x0B21, 0x0B7E } , 7 },
	{{ 0x0B2D, 0x0B72 } , 7 },
	{{ 0x0B39, 0x0B67 } , 7 },
	{{ 0x0B44, 0x0B5B } , 7 },
	{{ 0x0B50, 0x0B50 } , 7 },
};
static_assert(ARRAY_SIZE(lutTanToDirection) == 128, "There should be 128 entries in this lookup table");

//1.00:  D2Common.0x1005E6D0
//1.10f: D2Common.0x6FDAC5E0
// 1.13c D2Common+0xCF40
void __fastcall PATH_GetDirectionVector(D2CoordStrc* pDirectionVector, int* pOutDirection, DWORD dwStartPrecisionX, DWORD dwStartPrecisionY, DWORD dwTargetPrecisionX, DWORD dwTargetPrecisionY)
{
	bool bXStartLessThanTarget;
	DWORD dwMinPrecisionX;
	DWORD dwMaxPrecisionX;
	if (dwStartPrecisionX > dwTargetPrecisionX)
	{
		dwMinPrecisionX = dwTargetPrecisionX;
		dwMaxPrecisionX = dwStartPrecisionX;
		bXStartLessThanTarget = false;
	}
	else
	{
		dwMaxPrecisionX = dwTargetPrecisionX;
		dwMinPrecisionX = dwStartPrecisionX;
		bXStartLessThanTarget = true;
	}

	bool bYStartLessThanTarget;
	DWORD dwMinPrecisionY;
	DWORD dwMaxPrecisionY;
	if (dwStartPrecisionY > dwTargetPrecisionY)
	{
		dwMaxPrecisionY = dwStartPrecisionY;
		dwMinPrecisionY = dwTargetPrecisionY;
		bYStartLessThanTarget = false;
	}
	else
	{
		dwMinPrecisionY = dwStartPrecisionY;
		dwMaxPrecisionY = dwTargetPrecisionY;
		bYStartLessThanTarget = true;
	}

	bool bDeltaXGreaterThanY;
	if ((int)(dwMaxPrecisionX - dwMinPrecisionX) > (int)(dwMaxPrecisionY - dwMinPrecisionY))
	{
		std::swap(dwMinPrecisionX, dwMinPrecisionY);
		std::swap(dwMaxPrecisionX, dwMaxPrecisionY);
		bDeltaXGreaterThanY = TRUE;
	}
	else
	{
		bDeltaXGreaterThanY = FALSE;
	}
	int tangent = 0;
	if (dwMaxPrecisionY != dwMinPrecisionY)
		tangent = (int)(127 * (dwMaxPrecisionX - dwMinPrecisionX)) / (int)(dwMaxPrecisionY - dwMinPrecisionY);

	const auto& lutEntry = lutTanToDirection[tangent];

	uint8_t nAngle = (uint8_t)lutEntry.nAngle;
	if (!bDeltaXGreaterThanY)
	{
		pDirectionVector->nX = lutEntry.dirVector.nX;
		pDirectionVector->nY = lutEntry.dirVector.nY;
	}
	else
	{
		pDirectionVector->nY = lutEntry.dirVector.nX;
		pDirectionVector->nX = lutEntry.dirVector.nY;
		nAngle = (-1 - nAngle) & 0xF;
	}
	if (!bYStartLessThanTarget)
	{
		pDirectionVector->nY = -pDirectionVector->nY;
		nAngle = (-1 - nAngle) & 0x1F;
	}

	if (bXStartLessThanTarget)
	{
		nAngle = (-1 - nAngle) & 0x3F;
	}
	else
	{
		pDirectionVector->nX = -pDirectionVector->nX;
	}
	*pOutDirection = (nAngle + 8) & 0x3F;
}


//D2Common.0x6FDAB810
// 1.13c D2Common+0x3E110
// Prior art used: D2MOO Step.cpp; 1.13c retains signed, paired shifts at +/-65536.
void __fastcall sub_6FDAB810(int* a1, int* a2)
{
	if (*a1 < 0)
	{
		while (*a1 < -65536)
		{
			*a1 >>= 1;
			*a2 >>= 1;
		}
	}
	else
	{
		while (*a1 > 65536)
		{
			*a1 >>= 1;
			*a2 >>= 1;
		}
	}

	if (*a2 < 0)
	{
		while (*a2 < -65536)
		{
			*a1 >>= 1;
			*a2 >>= 1;
		}
	}
	else
	{
		while (*a2 > 65536)
		{
			*a1 >>= 1;
			*a2 >>= 1;
		}
	}
}

uint32_t D2Common_D420_PathMoveCollision(D2DynamicPathStrc* pPath, uint32_t dst, uint32_t src);

// 1.13c D2Common+0xD0A0
//1.13c: D2Common.0x6FD5D0A0
// The step this tick: the velocity, or (non-missile) the remaining offset to the current path point when that is
// no further than the velocity. Returns 1 when the offset to the path point was used.
static BOOL PATH_StepDelta_6FD5D0A0(D2DynamicPathStrc* pDynamicPath, int32_t* pDelta)
{
	pDelta[0] = pDynamicPath->tVelocityVector.nX;
	pDelta[1] = pDynamicPath->tVelocityVector.nY;
	if (pDynamicPath->dwPathType == PATHTYPE_MISSILE)
	{
		return 0;
	}

	const D2PathPointStrc tPoint = pDynamicPath->PathPoints[pDynamicPath->dwCurrentPointIdx];
	const int32_t nTargetX = PATH_ToFP16Center(tPoint.X);
	const int32_t nTargetY = PATH_ToFP16Center(tPoint.Y);
	int32_t nOffX = nTargetX - pDynamicPath->tGameCoords.dwPrecisionX;
	const int32_t nOffY = nTargetY - pDynamicPath->tGameCoords.dwPrecisionY;
	const int32_t nAbsOffX = nOffX < 0 ? -nOffX : nOffX;
	const int32_t nAbsOffY = nOffY < 0 ? -nOffY : nOffY;
	int32_t nAbsVelY = pDelta[1] < 0 ? -pDelta[1] : pDelta[1];
	int32_t nLimit = pDelta[0] < 0 ? -pDelta[0] : pDelta[0];
	if (nLimit <= nAbsVelY)
	{
		nLimit = nAbsVelY;
	}
	if (nAbsOffX <= nLimit && nAbsOffY <= nLimit)
	{
		pDelta[0] = nOffX;
		pDelta[1] = nOffY;
		return 1;
	}
	return 0;
}

// 1.13c D2Common+0xD580
//1.13c: D2Common.0x6FD5D580
// Walks the step tile by tile, moving the path's collision footprint each time. Returns 1 when a tile is blocked.
static BOOL PATH_StepCrossTiles_6FD5D580(D2DynamicPathStrc* pDynamicPath, int32_t nDx, int32_t nDy, D2FP32_16* a2, uint32_t* pCount)
{
	pDynamicPath->nSavedStepsCount = 0;
	int32_t nStepX = nDx;
	int32_t nStepY = nDy;
	sub_6FDAB810(&nStepX, &nStepY);

	uint32_t nX = pDynamicPath->tGameCoords.dwPrecisionX;
	uint32_t nY = pDynamicPath->tGameCoords.dwPrecisionY;
	const uint32_t nFlags = pDynamicPath->dwFlags;
	const uint32_t nEndX = nX + nDx;
	const uint32_t nEndY = nY + nDy;
	uint16_t nTileX = (uint16_t)(nX >> 16);
	uint16_t nTileY = (uint16_t)(nY >> 16);

	while (!(nTileX == (uint16_t)(nEndX >> 16) && nTileY == (uint16_t)(nEndY >> 16)))
	{
		const uint32_t nNextY = nY + nStepY;
		const uint16_t nNextTileX = (uint16_t)((nX + nStepX) >> 16);
		const uint16_t nNextTileY = (uint16_t)(nNextY >> 16);
		if (nTileX != nNextTileX || nTileY != nNextTileY)
		{
			if (!D2Common_D420_PathMoveCollision(pDynamicPath, nNextTileX | ((uint32_t)nNextTileY << 16), nTileX | ((uint32_t)nTileY << 16)))
			{
				a2->dwPrecisionX = PATH_FP16FitToCenter(nX);
				a2->dwPrecisionY = PATH_FP16FitToCenter(nY);
				*pCount = pDynamicPath->nSavedStepsCount;
				return 1;
			}
			if (nFlags & PATH_SAVE_STEPS_MASK)
			{
				pDynamicPath->SavedSteps[pDynamicPath->nSavedStepsCount] = D2PathPointStrc{ nNextTileX, nNextTileY };
			}
			const int32_t nCount = pDynamicPath->nSavedStepsCount++;
			if (nCount + 1 > 9)
			{
				break;
			}
		}
		nTileX = nNextTileX;
		nTileY = nNextTileY;
		nY = nNextY;
		nX += nStepX;
	}

	if (pDynamicPath->nSavedStepsCount)
	{
		pDynamicPath->dwFlags |= 8u;
	}
	a2->dwPrecisionX = nEndX;
	a2->dwPrecisionY = nEndY;
	*pCount = pDynamicPath->nSavedStepsCount;
	return 0;
}

//1.00:  D2Common.0x1005F420
//1.10f: D2Common.0x6FDACEC0
// 1.13c D2Common+0xD8E0
//1.13c: D2Common.0x6FD5D8E0
BOOL __fastcall sub_6FDACEC0(D2DynamicPathStrc* pDynamicPath, D2FP32_16* a2, D2UnitStrc** pUnit)
{
	D2UnitStrc* pPathUnit = pDynamicPath->pUnit;
	pDynamicPath->nCollidedWithMask = 0;
	*pUnit = 0;

	const bool bUnitPath = pPathUnit && pPathUnit->dwUnitType == UNIT_MONSTER && (pDynamicPath->dwFlags & PATH_UNKNOWN_FLAG_0x00010) != 0;

	if (!pDynamicPath->tVelocityVector.nX && !pDynamicPath->tVelocityVector.nY)
	{
		pDynamicPath->dwPathPoints = 0;
		pDynamicPath->dwCurrentPointIdx = 0;
		a2->dwPrecisionX = PATH_FP16FitToCenter(pDynamicPath->tGameCoords.dwPrecisionX);
		a2->dwPrecisionY = PATH_FP16FitToCenter(pDynamicPath->tGameCoords.dwPrecisionY);
		return 0;
	}

	int32_t aDelta[2];
	const BOOL bUsedPoint = PATH_StepDelta_6FD5D0A0(pDynamicPath, aDelta);
	const uint32_t nNewX = (uint32_t)aDelta[0] + pDynamicPath->tGameCoords.dwPrecisionX;
	const uint32_t nNewY = (uint32_t)aDelta[1] + pDynamicPath->tGameCoords.dwPrecisionY;

	if ((int16_t)(pDynamicPath->tGameCoords.dwPrecisionX >> 16) != (int16_t)(nNewX >> 16)
		|| (int16_t)(pDynamicPath->tGameCoords.dwPrecisionY >> 16) != (int16_t)(nNewY >> 16))
	{
		if (pDynamicPath->nDist && pDynamicPath->dwPathType != PATHTYPE_KNOCKBACK_SERVER && pDynamicPath->dwPathType != PATHTYPE_KNOCKBACK_CLIENT)
		{
			--pDynamicPath->nDist;
		}
		uint32_t nCount = 0;
		const BOOL bBlocked = PATH_StepCrossTiles_6FD5D580(pDynamicPath, aDelta[0], aDelta[1], a2, &nCount);
		*pUnit = (D2UnitStrc*)(uintptr_t)nCount;
		if (bBlocked)
		{
			if (!bUnitPath)
			{
				pDynamicPath->dwCurrentPointIdx = pDynamicPath->dwPathPoints;
				return 0;
			}
			return D2Common_10236(pPathUnit, pDynamicPath->pTargetUnit != 0) == 0;
		}
	}

	a2->dwPrecisionX = nNewX;
	a2->dwPrecisionY = nNewY;
	if (!bUsedPoint)
	{
		return 0;
	}
	++pDynamicPath->dwCurrentPointIdx;
	return 1;
}

void PATH_StopMovement(D2DynamicPathStrc* pDynamicPath);

// 1.13c D2Common+0xDF30
//1.00:  D2Common.0x1005FAB0
//1.10f: D2Common.0x6FDAD5E0
BOOL __fastcall sub_6FDAD5E0(D2DynamicPathStrc* pDynamicPath, D2ActiveRoomStrc* pDestRoom, D2PathPointStrc tDest)
{
	D2UnitStrc* pUnit = pDynamicPath->pUnit;
	if (pUnit && pUnit->dwUnitType == UNIT_MISSILE)
	{
		if (tDest == D2PathPointStrc{ 0, 0 })
		{
			COLLISION_ResetMaskWithSize(
				pDynamicPath->pRoom, pDynamicPath->tGameCoords.wPosX, pDynamicPath->tGameCoords.wPosY,
				pDynamicPath->dwUnitSize,
				pDynamicPath->nFootprintCollisionMask
			);
			pDynamicPath->nCollidedWithMask = 0;
		}
		else
		{
			D2PathPointStrc tPathPos = { pDynamicPath->tGameCoords.wPosX , pDynamicPath->tGameCoords.wPosY };
			if (pDynamicPath->tGameCoords.wPosX == tDest.X && pDynamicPath->tGameCoords.wPosY == tDest.Y)
			{
				pDynamicPath->dwFlags &= ~PATH_UNKNOWN_FLAG_0x00008;
			}
			else
			{
				pDynamicPath->dwFlags |= PATH_UNKNOWN_FLAG_0x00008;
			}
			pDynamicPath->nCollidedWithMask = COLLISION_ForceTeleportUnitCollisionMaskAndGetCollision(
				pDynamicPath->pRoom, pDynamicPath->tGameCoords.wPosX, pDynamicPath->tGameCoords.wPosY,
				pDestRoom, tDest.X, tDest.Y,
				pDynamicPath->dwUnitSize,
				pDynamicPath->nFootprintCollisionMask,
				pDynamicPath->nMoveTestCollisionMask
			);
			pDynamicPath->nSavedStepsCount = 1;
			pDynamicPath->SavedSteps[0] = tDest;
		}
	}
	else
	{
		if (tDest.X || tDest.Y)
		{
			COLLISION_SetUnitCollisionMask(
				pDynamicPath->pRoom, pDynamicPath->tGameCoords.wPosX, pDynamicPath->tGameCoords.wPosY,
				pDestRoom, tDest.X, tDest.Y,
				pDynamicPath->dwCollisionPattern,
				pDynamicPath->nFootprintCollisionMask
			);
		}
		else
		{
			COLLISION_ResetMaskWithPattern(
				pDynamicPath->pRoom, pDynamicPath->tGameCoords.wPosX, pDynamicPath->tGameCoords.wPosY,
				pDynamicPath->dwCollisionPattern,
				pDynamicPath->nFootprintCollisionMask
			);
		}
	}
	if (pDynamicPath->pRoom != pDestRoom)
		pDynamicPath->dwFlags |= PATH_UNKNOWN_FLAG_0x00001;

	// 1.13c: recache at the destination's subtile centre (0xDA40), then end the movement (0xDC80)
	PATH_RecacheRoomAtCoordIfNeeded(pDynamicPath, pDestRoom, PATH_ToFP16Center(tDest.X), PATH_ToFP16Center(tDest.Y));

	D2_ASSERT(COORD_TEST_EQUAL(tDest, sgctZeroGameCoord) || COLLISION_GetRoomBySubTileCoordinates(pDestRoom, tDest.X, tDest.Y));

	PATH_StopMovement(pDynamicPath);
	return TRUE;
}

//1.00:  D2Common.0x100604E0 Note: Changed since then. Used to look for the room using coordinates
//1.10f: D2Common.0x6FDAE250
// not-1.13c: retail inlines the room swap into PATH_RecacheRoom (D2Common+0xD760)
void __fastcall PATHMISC_SetRoom(D2DynamicPathStrc* pPath, D2ActiveRoomStrc* pNewRoom)
{
	D2ActiveRoomStrc* pRoomDest = pNewRoom;
#ifdef VERSION_100
	if (!pPath->pRoom || !DungeonTestRoomGame(pPath->pRoom, pPath->tGameCoords.wPosX, pPath->tGameCoords.wPosY))
	{
		pRoomDest = COLLISION_GetRoomBySubTileCoordinates(pPath->pRoom, pPath->tGameCoords.wPosX, pPath->tGameCoords.wPosY);
		if(!pRoomDest && pNewRoom)
			pRoomDest = COLLISION_GetRoomBySubTileCoordinates(pNewRoom, pPath->tGameCoords.wPosX, pPath->tGameCoords.wPosY);
	}
	if(!pRoomDest && (pPath->dwFlags & PATH_MISSILE_MASK) != 0)
	{
		return;
	}
#endif
	pPath->pPreviousRoom = pPath->pRoom;
	if (pPath->pRoom)
	{
		UNITROOM_RemoveUnitFromRoom(pPath->pUnit);
	}

#ifdef VERSION_100
	D2_ASSERT(pRoomDest || (pPath->dwFlags & PATH_MISSILE_MASK) || (!PATH_FromFP16(pPath->tGameCoords.dwPrecisionX) && !PATH_FromFP16(pPath->tGameCoords.dwPrecisionY)));
#endif
	pPath->pRoom = pRoomDest;
	pPath->dwFlags |= PATH_CURRENT_ROOM_INVALID;

	if (pRoomDest)
	{
		UNITROOM_AddUnitToRoom(pPath->pUnit, pRoomDest);
		UNITROOM_RefreshUnit(pPath->pUnit);
	}
}



// 1.13c D2Common+0xE150
//1.00:  D2Common.0x10060640 (#10227)
//1.10f: D2Common.0x6FDADF00 (#10230)
BOOL __stdcall D2Common_10230(D2DynamicPathStrc* pDynamicPath, int a2, D2ActiveRoomStrc* pRoom, unsigned int a4, __int16 a5)
{
	D2PathPointStrc tPos;
	tPos.X = a4;
	tPos.Y = a5;
	BOOL result = sub_6FDAD5E0(pDynamicPath, pRoom, tPos);
	if (result)
	{
		pDynamicPath->unk0x38 = 15;
		result = 1;
	}
	return result;
}

// TODO: Check name
//1.00:  Inlined
//1.10f: Inlined
//1.13c: D2Common.0x6FD5DC80
void PATH_StopMovement(D2DynamicPathStrc* pDynamicPath)
{
	PATH_RecacheRoomIfNeeded(pDynamicPath);
	pDynamicPath->dwFlags = pDynamicPath->dwFlags & (~PATH_UNKNOWN_FLAG_0x00020);
	pDynamicPath->dwPathPoints = 0;
	pDynamicPath->dwCurrentPointIdx = 0;
	pDynamicPath->tVelocityVector.nX = 0;
	pDynamicPath->tVelocityVector.nY = 0;
}

//1.00:  Inlined
//1.10f: Inlined
// Retail x86 offsets, adjusted only for the native pointer fields before each member.
static_assert(offsetof(D2DynamicPathStrc, tGameCoords) == 0x00 + 0 * (sizeof(void*) - 4));
static_assert(offsetof(D2DynamicPathStrc, pRoom) == 0x1C + 0 * (sizeof(void*) - 4));
static_assert(offsetof(D2DynamicPathStrc, dwCurrentPointIdx) == 0x24 + 2 * (sizeof(void*) - 4));
static_assert(offsetof(D2DynamicPathStrc, dwPathPoints) == 0x28 + 2 * (sizeof(void*) - 4));
static_assert(offsetof(D2DynamicPathStrc, pUnit) == 0x30 + 3 * (sizeof(void*) - 4));
static_assert(offsetof(D2DynamicPathStrc, dwFlags) == 0x34 + 4 * (sizeof(void*) - 4));
static_assert(offsetof(D2DynamicPathStrc, dwUnitSize) == 0x44 + 4 * (sizeof(void*) - 4));
static_assert(offsetof(D2DynamicPathStrc, dwCollisionPattern) == 0x48 + 4 * (sizeof(void*) - 4));
static_assert(offsetof(D2DynamicPathStrc, nFootprintCollisionMask) == 0x4C + 4 * (sizeof(void*) - 4));
static_assert(offsetof(D2DynamicPathStrc, tVelocityVector) == 0x72 + 5 * (sizeof(void*) - 4));
static_assert(offsetof(D2FP32_16, wPosX) == 2 && offsetof(D2FP32_16, wPosY) == 6);
static_assert(offsetof(D2UnitStrc, dwUnitType) == 0);
static_assert(offsetof(D2CoordStrc, nX) == 0 && offsetof(D2CoordStrc, nY) == 4);
// 1.13c D2Common+0x0DDC0
BOOL __stdcall D2Common_10231_Impl(D2DynamicPathStrc* pDynamicPath, D2ActiveRoomStrc* pRoomDest, D2PathPointStrc tDest)
{
	D2PathPointStrc tCurrentPos;
	tCurrentPos.X = pDynamicPath->tGameCoords.wPosX;
	tCurrentPos.Y = pDynamicPath->tGameCoords.wPosY;
	if (pDynamicPath->pUnit && pDynamicPath->pUnit->dwUnitType == UNIT_MISSILE)
	{
		if (tDest.X || tDest.Y)
		{
			COLLISION_TeleportUnitCollisionMask(pDynamicPath->pRoom, tCurrentPos.X, tCurrentPos.Y, pRoomDest, tDest.X, tDest.Y, pDynamicPath->dwUnitSize, pDynamicPath->nFootprintCollisionMask);
		}
		else
		{
			COLLISION_ResetMaskWithSize(pDynamicPath->pRoom, tCurrentPos.X, tCurrentPos.Y, pDynamicPath->dwUnitSize, pDynamicPath->nFootprintCollisionMask);
		}
	}
	else
	{
		if (tDest.X || tDest.Y)
		{
			COLLISION_SetUnitCollisionMask(pDynamicPath->pRoom, tCurrentPos.X, tCurrentPos.Y, pRoomDest, tDest.X, tDest.Y, pDynamicPath->dwCollisionPattern, pDynamicPath->nFootprintCollisionMask);
		}
		else
		{
			COLLISION_ResetMaskWithPattern(pDynamicPath->pRoom, tCurrentPos.X, tCurrentPos.Y, pDynamicPath->dwCollisionPattern, pDynamicPath->nFootprintCollisionMask);
		}
	}

	if (pDynamicPath->pRoom != pRoomDest)
	{
		pDynamicPath->dwFlags |= PATH_UNKNOWN_FLAG_0x00001;
	}
	PATH_RecacheRoomAtCoordIfNeeded(pDynamicPath, pRoomDest, PATH_ToFP16Center(tDest.X), PATH_ToFP16Center(tDest.Y));

	D2_ASSERT(COORD_TEST_EQUAL(tDest, sgctZeroGameCoord) || COLLISION_GetRoomBySubTileCoordinates(pRoomDest, tDest.X, tDest.Y));
	PATH_StopMovement(pDynamicPath);
	return TRUE;
}

//1.00:  D2Common.0x10060120 (#10228)
//1.10f: D2Common.0x6FDADC20 (#10231)
//1.13c: D2Common.0x6FD5E1A0 (#10075)
int __stdcall D2Common_10231(D2DynamicPathStrc* pDynamicPath, D2UnitStrc* pUnit_unused, D2ActiveRoomStrc* pRooms, uint16_t nX, uint16_t nY)
{
	D2_MAYBE_UNUSED(pUnit_unused);
	D2Common_10231_Impl(pDynamicPath, pRooms, D2PathPointStrc{ pDynamicPath->tGameCoords.wPosX, pDynamicPath->tGameCoords.wPosY });
	pDynamicPath->dwPathPoints = 0;
	return TRUE;
}



//1.00:  D2Common.0x1005F220
//1.10f: D2Common.0x6FDACC40
//1.13c: D2Common.0x6FD5DA40
// 1.13c D2Common+0x0DA40
void PATH_RecacheRoomAtCoordIfNeeded(D2DynamicPathStrc* pPath, D2ActiveRoomStrc* pHintRoom, uint32_t nPrecisionX, uint32_t nPrecisionY)
{
	if ((pPath->dwFlags & PATH_MISSILE_MASK) == 0
		|| COLLISION_GetRoomBySubTileCoordinates(pPath->pRoom, PATH_FromFP16(nPrecisionX), PATH_FromFP16(nPrecisionY)))
	{
		pPath->tGameCoords.dwPrecisionX = nPrecisionX;
		pPath->tGameCoords.dwPrecisionY = nPrecisionY;
		PATH_UpdateClientCoords(pPath);
		if (pPath->pUnit && (pPath->dwFlags & PATH_UNKNOWN_FLAG_0x00001) != 0)
		{
			PATH_RecacheRoom(pPath, pHintRoom);
		}
	}
	else
	{
		pPath->dwPathPoints = 0;
	}
}

//1.00:  Inlined
//1.10f: D2Common.0x6FDAD330
//1.13c: D2Common.0x6FD5DC80
// 1.13c D2Common+0x0DC80
void PATH_ResetMovement(D2DynamicPathStrc* pDynamicPath)
{
	PATH_RecacheRoomAtCoordIfNeeded(
		pDynamicPath,
		nullptr,
		PATH_FP16FitToCenter(pDynamicPath->tGameCoords.dwPrecisionX),
		PATH_FP16FitToCenter(pDynamicPath->tGameCoords.dwPrecisionY)
	);
	pDynamicPath->dwFlags &= ~PATH_UNKNOWN_FLAG_0x00020;
	pDynamicPath->dwPathPoints = 0;
	pDynamicPath->dwCurrentPointIdx = 0;
	pDynamicPath->tVelocityVector.nX = 0;
	pDynamicPath->tVelocityVector.nY = 0;
}

//1.00:  D2Common.0x100606B0 (#10229)
//1.10f: D2Common.0x6FDADF50 (#10232)
//1.13c: D2Common.0x6FD5DCE0 (#10223)
BOOL __stdcall D2Common_10232(D2DynamicPathStrc* pPath, D2UnitStrc* pUnit, D2ActiveRoomStrc* pDestRoom, int nTargetX, int nTargetY)
{
	D2_ASSERT(pUnit && (pUnit->dwUnitType == UNIT_PLAYER || pUnit->dwUnitType == UNIT_MONSTER)); 
	
	if (COLLISION_TrySetUnitCollisionMask(
		pPath->pRoom, pPath->tGameCoords.wPosX, pPath->tGameCoords.wPosY,
		pDestRoom, nTargetX, nTargetY, 
		pPath->dwCollisionPattern, pPath->nFootprintCollisionMask, pPath->nMoveTestCollisionMask))
	{
		return FALSE;
	}
	if (pPath->pRoom != pDestRoom)
	{
		pPath->dwFlags |= PATH_UNKNOWN_FLAG_0x00001;
	}
#ifdef D2_VERSION_113C
	PATH_RecacheRoomIfNeeded(pPath, PATH_ToFP16Center(nTargetX), PATH_ToFP16Center(nTargetY), pDestRoom);
#endif
	D2_ASSERT(COLLISION_GetRoomBySubTileCoordinates(pDestRoom, nTargetX, nTargetY));
	PATH_ResetMovement(pPath);
	return TRUE;
}

//1.00:  D2Common.0x10060A60 (#10230)
//1.10f: D2Common.0x6FDAE290 (#10233)
//1.13c: D2Common.0x6FD5DB40 (#10770)
void __stdcall PATH_RecacheRoomIfNeeded(D2DynamicPathStrc* pDynamicPath)
{
	PATH_RecacheRoomAtCoordIfNeeded(pDynamicPath, nullptr, PATH_FP16FitToCenter(pDynamicPath->tGameCoords.dwPrecisionX), PATH_FP16FitToCenter(pDynamicPath->tGameCoords.dwPrecisionY));
}


//1.00:  D2Common.0x10060C40 (#10231)
//1.10f: D2Common.0x6FDAE500 (#10234)
//TODO: Find a name
// 1.13c D2Common+0xCE90
BOOL __stdcall D2Common_10234(D2DynamicPathStrc* pDynamicPath)
{
	if (pDynamicPath)
	{
		return pDynamicPath->dwFlags & PATH_UNKNOWN_FLAG_0x00008;
	}

	return FALSE;
}

// 1.13c D2Common+0xDAB0
//1.00:  D2Common.0x10060C60 (#10231)
//1.10f: D2Common.0x6FDAE520 (#10235)
void __stdcall D2Common_10235_PATH_UpdateRiderPath(D2UnitStrc* pRiderUnit, D2UnitStrc* pMountUnit)
{
	D2_ASSERT(pRiderUnit); // Named hRiderUnit in original game
	D2_ASSERT(pMountUnit); // Named hHorseUnit in original game

	D2DynamicPathStrc* pRiderPath = pRiderUnit->pDynamicPath;
	D2DynamicPathStrc* pMountPath = pMountUnit->pDynamicPath;

	if (pRiderPath->tGameCoords.wPosX != pMountPath->tGameCoords.wPosX || pRiderPath->tGameCoords.wPosY != pMountPath->tGameCoords.wPosY)
	{
		pRiderPath->dwFlags |= PATH_UNKNOWN_FLAG_0x00008;
	}
	else
	{
		pRiderPath->dwFlags &= ~PATH_UNKNOWN_FLAG_0x00008;
	}

	if (pRiderPath->pRoom != pMountPath->pRoom)
	{
		// Rider is in different room than the mount 
		pRiderPath->dwFlags |= PATH_UNKNOWN_FLAG_0x00001;
	}

	// 1.13c: the mount's precise position and room go through the room-recache helper (0xDA40),
	// which also ends a missile's path when no room holds the position.
	PATH_RecacheRoomAtCoordIfNeeded(pRiderPath, pMountPath->pRoom,
		pMountPath->tGameCoords.dwPrecisionX, pMountPath->tGameCoords.dwPrecisionY);
}
