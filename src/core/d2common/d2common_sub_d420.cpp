#include <cstdint>

#include "D2Collision.h"
#include "D2Dungeon.h"
#include "Path/Path.h"
#include "Units/Units.h"

// 1.13c D2Common+0x0D420 (eax = path, stack: dst, src; each packed x | y << 16).
// Moves the path's collision footprint src -> dst. Flag 4: unconditional reset(src)/set(dst),
// path unit must be a player or monster. Else a missile path (flag 0x40000) uses the sized move,
// others the pattern teleport; the hit mask is OR-ed into nCollidedWithMask. Returns 1 when the
// move is clean (missile: ignoring all but bits 1 and 4 of the accumulated mask).
uint32_t D2Common_D420_PathMoveCollision(D2DynamicPathStrc* pPath, uint32_t dst, uint32_t src)
{
	const int32_t nX1 = src & 0xFFFF, nY1 = src >> 16, nX2 = dst & 0xFFFF, nY2 = dst >> 16;

	if (pPath->dwFlags & 4)
	{
		D2_CHECK(pPath->pUnit && (pPath->pUnit->dwUnitType == UNIT_PLAYER || pPath->pUnit->dwUnitType == UNIT_MONSTER));
		if (pPath->pRoom)
		{
			COLLISION_ResetMaskWithPattern(pPath->pRoom, nX1, nY1, pPath->dwCollisionPattern, pPath->nFootprintCollisionMask);
			COLLISION_SetMaskWithPattern(pPath->pRoom, nX2, nY2, pPath->dwCollisionPattern, pPath->nFootprintCollisionMask);
		}
		return 1;
	}

	const bool bMissile = (pPath->dwFlags & PATH_MISSILE_MASK) != 0;
	uint32_t nCond = pPath->nMoveTestCollisionMask;
	if (nCond == 0x3401)
		nCond = 0x3C01;
	const uint16_t nHit = bMissile
		? COLLISION_TryMoveUnitCollisionMask(pPath->pRoom, nX1, nY1, nX2, nY2, pPath->dwUnitSize, pPath->nFootprintCollisionMask, nCond)
		: COLLISION_TryTeleportUnitCollisionMask(pPath->pRoom, nX1, nY1, nX2, nY2, pPath->dwCollisionPattern, pPath->nFootprintCollisionMask, nCond);
	pPath->nCollidedWithMask |= nHit;
	if (!bMissile)
		return pPath->nCollidedWithMask == 0;
	return (pPath->nCollidedWithMask & 5) == 0;
}
