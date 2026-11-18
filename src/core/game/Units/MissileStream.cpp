#include "Units/MissileStream.h"

#include "D2Collision.h"
#include "Units/Missile.h"
#include "Path/Path.h"
#include "Path/PathMisc.h"
#include "Path/IDAStar.h"
#include "Units/Units.h"


// 1.13c D2Common+0x1D760 (#10330)
// The missile-data stream getter and setter are inlined here, as at 0x1D710 and 0x1D7C0.
void __stdcall MISSTREAM_AllocMissileStream(D2UnitStrc* pMissile)
{
	if (!pMissile || pMissile->dwUnitType != UNIT_MISSILE || !pMissile->pMissileData || !pMissile->pMissileData->pStream)
	{
		D2MissileStreamStrc* pStream = D2_ALLOC_STRC_POOL(pMissile ? pMissile->pMemoryPool : nullptr, D2MissileStreamStrc);
		pStream->unk0x00 = 0;
		pStream->unk0x04 = 0;

		if (pMissile && pMissile->dwUnitType == UNIT_MISSILE && pMissile->pMissileData)
		{
			pMissile->pMissileData->pStream = pStream;
		}
	}
}

// 1.13c D2Common+0x1D710 (#10998)
void __stdcall MISSTREAM_FreeMissileStream(D2UnitStrc* pMissile)
{
	if (pMissile && pMissile->dwUnitType == UNIT_MISSILE && pMissile->pMissileData && pMissile->pMissileData->pStream)
	{
		D2MissileStreamStrc* pStream = pMissile->pMissileData->pStream;
		if (pStream->unk0x00)
		{
			D2_FREE_POOL(pMissile->pMemoryPool, (void*)pStream->unk0x00);
		}

		D2_FREE_POOL(pMissile->pMemoryPool, pStream);
	}
}

// 1.13c D2Common+0x1DAD0 (#11112)
// SetType(MISSILE_STREAM) is inlined: the flag word keeps its upper bits, the type is 0x10 and the
// direction offset 0; the two previous-type checks follow the writes.
void __stdcall MISSTREAM_ExecuteHit(D2UnitStrc* pUnit, int nCollisionMask, int nCollisionPattern, void (__fastcall* pfnHit)(D2UnitStrc*, D2UnitStrc*))
{
	D2DynamicPathStrc* pPath = pUnit->pDynamicPath;

	pPath->dwFlags &= 0xFFF800FF;
	pPath->dwPathType = PATHTYPE_MISSILE_STREAM;
	pPath->nDirOffset = 0;
	D2_ASSERT(pPath->dwPrevPathType != PATHTYPE_KNOCKBACK_CLIENT);
	D2_ASSERT(pPath->dwPrevPathType != PATHTYPE_KNOCKBACK_SERVER);

	D2Common_10142(pPath, pUnit, 0);

	const int nPathPoints = pPath->dwPathPoints;
	D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pUnit);

	for (int i = 0; i < nPathPoints; ++i)
	{
		const int nX = pPath->PathPoints[i].X;
		const int nY = pPath->PathPoints[i].Y;
		if (COLLISION_CheckMaskWithSize(pRoom, nX, nY, nCollisionPattern, nCollisionMask) != 0)
		{
			D2UnitStrc* pTargetUnit = D2Common_10407(pRoom, nX, nY, MISSTREAM_Return1, 0, nCollisionPattern);
			if (pTargetUnit)
			{
				D2_ASSERT(!IsBadCodePtr((FARPROC)pfnHit));

				pfnHit(pUnit, pTargetUnit);
			}
		}
	}
}

// 1.13c D2Common+0x1D700
int __fastcall MISSTREAM_Return1(D2UnitStrc* pUnit1, void* pUnit2)
{
	return 1;
}

// 1.13c D2Common+0x1D7C0 (#11216)
// SetType(MISSILE_STREAM) and the stream getter are inlined. The create callback takes the first
// parameter as its unit, the old stream value, then X, Y, the running value, the mode (0 plain, 1 key,
// 2 filler) and the direction byte of the segment.
void __stdcall MISSTREAM_Update(D2UnitStrc* a1, D2UnitStrc* pMissile, int a3, int a4, int (__fastcall* pfCreate)(D2UnitStrc*, int, int, int, int, int, int), void (__fastcall* a6)(int))
{
	D2DynamicPathStrc* pPath = pMissile->pDynamicPath;

	pPath->dwFlags &= 0xFFF800FF;
	pPath->dwPathType = PATHTYPE_MISSILE_STREAM;
	pPath->nDirOffset = 0;
	D2_ASSERT(pPath->dwPrevPathType != PATHTYPE_KNOCKBACK_CLIENT);
	D2_ASSERT(pPath->dwPrevPathType != PATHTYPE_KNOCKBACK_SERVER);

	D2Common_10142(pPath, pMissile, 0);

	const int nPathPoints = pPath->dwPathPoints;
	if (nPathPoints == 0 || nPathPoints == 1)
	{
		return;
	}

	D2MissileStreamStrc* pStream = nullptr;
	if (pMissile->dwUnitType == UNIT_MISSILE && pMissile->pMissileData)
	{
		pStream = pMissile->pMissileData->pStream;
	}
	D2_ASSERT(pStream);

	const int nTemp = nPathPoints - 1;
	const int nRatio = a3 / a4;
	const BOOL bCreate = nTemp < nRatio;
	const int nPoints = bCreate ? nRatio : nTemp;

	if (pStream->unk0x04 == 0)
	{
		pStream->unk0x04 = nPoints;
		pStream->unk0x00 = (int*)D2_CALLOC_POOL(pMissile->pMemoryPool, sizeof(int) * nPoints);
	}
	else if (pStream->unk0x04 < nPoints)
	{
		pStream->unk0x00 = (int*)D2_REALLOC_POOL(pMissile->pMemoryPool, pStream->unk0x00, sizeof(int) * nPoints);
	}

	const int nMax = nTemp / nRatio;
	const int v29 = bCreate ? (4 * (nRatio - nTemp)) / nTemp : 0;

	D2_ASSERT(!IsBadCodePtr((FARPROC)pfCreate));

	int v20 = a3;
	int v27 = 4;
	int nCounter = 0;
	int nIndex = 0;
	while (nIndex < nTemp)
	{
		const int nX = pPath->PathPoints[nIndex].X;
		const int nY = pPath->PathPoints[nIndex].Y;
		const uint8_t nDir = (uint8_t)PATH_ComputeDirection(pPath->PathPoints[nIndex + 1].X, pPath->PathPoints[nIndex + 1].Y, nX, nY);

		if (nCounter >= nMax && a3)
		{
			nCounter = 1;
			pStream->unk0x00[nIndex] = pfCreate(a1, pStream->unk0x00[nIndex], nX, nY, v20, 1, nDir);
			v20 -= a4;

			if (bCreate)
			{
				int v24 = v29 / 4;
				if (v27 < v29 % 4)
				{
					++v24;
				}

				--v27;
				if (v27 <= 0)
				{
					v27 = 4;
				}

				for (int i = 0; i < v24; ++i)
				{
					pStream->unk0x00[nIndex] = pfCreate(a1, pStream->unk0x00[nIndex], nX, nY, v20, 2, nDir);
					v20 -= a4;
				}
			}
		}
		else
		{
			++nCounter;
			pStream->unk0x00[nIndex] = pfCreate(a1, pStream->unk0x00[nIndex], nX, nY, v20, 0, nDir);
		}

		++nIndex;
	}

	while (nIndex < pStream->unk0x04)
	{
		a6(pStream->unk0x00[nIndex]);
		pStream->unk0x00[nIndex] = 0;
		++nIndex;
	}
}
