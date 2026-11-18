#include <algorithm>
#include <cstddef>
#include "Path/Path.h"

#include "D2Collision.h"
#include "D2DataTbls.h"
#include "D2Dungeon.h"
#include "D2Inventory.h"
#include "Drlg/D2DrlgDrlgRoom.h"
#include <D2BitManip.h>
#include "D2Monsters.h"
#include "Path/IDAStar.h"
#include "Units/UnitRoom.h"
#include "Units/Units.h"
#include <DataTbls/MonsterIds.h>
#include <D2Math.h>
#include "Path/IDAStar.h"
#include "Path/PathMisc.h"
#include "Path/PathUtil.h"
#include "Path/PathWF.h"
#include "Path/AStar.h"
#include "Path/Step.h"
#include <cmath>
#include <utility>

static const D2C_CollisionPattern gaCollisionPatternsFromSize_6FDD1DE4[COLLISION_UNIT_SIZE_COUNT] =
{
	COLLISION_PATTERN_NONE,
	COLLISION_PATTERN_SMALL_UNIT_PRESENCE,
	COLLISION_PATTERN_SMALL_UNIT_PRESENCE,
	COLLISION_PATTERN_BIG_UNIT_PRESENCE
};
static const int gaOffsetsForSnappingToCardinalDirection[9][9] =
{
	{-1,  0,  0,  0, 0, 0, 0, 0, 1},
	{-1, -1,  0,  0, 0, 0, 0, 1, 1},
	{-1, -1, -1,  0, 0, 0, 1, 1, 1},
	{-1, -1, -1, -1, 0, 1, 1, 1, 1},
	{-1, -1, -1, -1, 0, 1, 1, 1, 1},
	{-1, -1, -1, -1, 0, 1, 1, 1, 1},
	{-1, -1, -1,  0, 0, 0, 1, 1, 1},
	{-1, -1,  0,  0, 0, 0, 0, 1, 1},
	{-1,  0,  0,  0, 0, 0, 0, 0, 1}
};

static const uint32_t dword_6FDD1F88[PATH_NB_DIRECTIONS] =
{
	0x00000000, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 
	0x00000001, 0x00000004, 0x00000004, 0x00000004, 0x00000004, 0x00000004, 0x00000004, 0x00000004, 
	0x00000004, 0x00000008, 0x00000008, 0x00000008, 0x00000008, 0x00000008, 0x00000008, 0x00000008, 
	0x00000008, 0x00000008, 0x00000008, 0x00000008, 0x00000008, 0x00000008, 0x00000008, 0x00000008, 
	0xFFFFFFF8, 0xFFFFFFF8, 0xFFFFFFF8, 0xFFFFFFF8, 0xFFFFFFF8, 0xFFFFFFF8, 0xFFFFFFF8, 0xFFFFFFF8, 
	0xFFFFFFF8, 0xFFFFFFF8, 0xFFFFFFF8, 0xFFFFFFF8, 0xFFFFFFF8, 0xFFFFFFF8, 0xFFFFFFF8, 0xFFFFFFF8, 
	0xFFFFFFFC, 0xFFFFFFFC, 0xFFFFFFFC, 0xFFFFFFFC, 0xFFFFFFFC ,0xFFFFFFFC, 0xFFFFFFFC, 0xFFFFFFFC, 
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
};

static const int gaPathTypeFlags_6FDD2088[] =
{
/*[ 0]*/0x21900,
/*[ 1]*/0x1900,
/*[ 2]*/0,
/*[ 3]*/0,
/*[ 4]*/0x60000,
/*[ 5]*/0,
/*[ 6]*/0,
/*[ 7]*/0x21900,
/*[ 8]*/0x1E600,
/*[ 9]*/0x1E800,
/*[10]*/0x60000,
/*[11]*/0x1E604,
/*[12]*/0,
/*[13]*/0x100,
/*[14]*/0x60000,
/*[15]*/0x800,
/*[16]*/0,
/*[17]*/0,
};
static_assert(ARRAY_SIZE(gaPathTypeFlags_6FDD2088) == PATHTYPE_COUNT, "This array must have PATHTYPE_COUNT entries");
// 1.10f: D2Common.0x6FDD20D0
static const int32_t gaOffsetForPathType[] =
{
/*[ 0]*/0,
/*[ 1]*/0,
/*[ 2]*/0,
/*[ 3]*/0,
/*[ 4]*/0,
/*[ 5]*/2,
/*[ 6]*/-2,
/*[ 7]*/0,
/*[ 8]*/0,
/*[ 9]*/0,
/*[10]*/0,
/*[11]*/0,
/*[12]*/-4,
/*[13]*/0,
/*[14]*/0,
/*[15]*/0,
/*[16]*/0,
/*[17]*/0,
};
static_assert(ARRAY_SIZE(gaOffsetForPathType) == PATHTYPE_COUNT, "This array must have PATHTYPE_COUNT entries");

typedef int(__fastcall* PathFunctionType)(D2PathInfoStrc*);

//D2Common.0x6FDD1F40
static const PathFunctionType scpfnPathFunction[] = {
/*[ 0]*/PATH_IdaStar_6FDA7970
/*[ 1]*/,PATH_AStar_ComputePath
/*[ 2]*/,PATH_Toward_6FDAA9F0
/*[ 3]*/,sub_6FDAB0B0
/*[ 4]*/,nullptr
/*[ 5]*/,PATH_Toward_6FDAA9F0
/*[ 6]*/,PATH_Toward_6FDAA9F0
/*[ 7]*/,PATH_Straight_Compute
/*[ 8]*/,PATH_Knockback_Server
/*[ 9]*/,PATH_Leap_6FDAB1E0
/*[10]*/,nullptr
/*[11]*/,PATH_Knockback_Client
/*[12]*/,PATH_BackupTurn_Compute
/*[13]*/,PATH_Toward_6FDAA9F0
/*[14]*/,nullptr
/*[15]*/,PATH_ComputePathOrSlideAlongObstacles
/*[16]*/,PATH_IdaStar_6FDA7970
/*[17]*/,nullptr
};
static_assert(ARRAY_SIZE(scpfnPathFunction) == PATHTYPE_COUNT, "This array must have PATHTYPE_COUNT entries");

// not-1.13c: retail writes the isometric client coords inline at each call site (for example D2Common+0xDA40)
void PATH_UpdateClientCoords(D2DynamicPathStrc* pDynamicPath)
{
	// To game "pixels"
	int nX = pDynamicPath->tGameCoords.dwPrecisionX >> 11;
	int nY = pDynamicPath->tGameCoords.dwPrecisionY >> 11;

	DUNGEON_GameToClientCoords(&nX, &nY);

	pDynamicPath->dwClientCoordX = nX;
	pDynamicPath->dwClientCoordY = nY;
}

//D2Common.0x6FDA8220
// 1.13c D2Common+0x34E70: with path points and no room the flag is set (the retail loop runs only while the room exists, then falls to the set)
void __fastcall sub_6FDA8220(D2DynamicPathStrc* pDynamicPath)
{
	pDynamicPath->dwFlags &= (~PATH_UNKNOWN_FLAG_0x00001);

	if (!pDynamicPath->pRoom)
	{
		if (pDynamicPath->dwPathPoints > 0)
		{
			pDynamicPath->dwFlags |= PATH_UNKNOWN_FLAG_0x00001;
		}
	}
	else
	{
		for (int i = 0; i < pDynamicPath->dwPathPoints; ++i)
		{
			const D2DrlgCoordsStrc& tRoomCoords = pDynamicPath->pRoom->tCoords;
			if (pDynamicPath->PathPoints[i].X < tRoomCoords.nSubtileX)
			{
				pDynamicPath->dwFlags |= PATH_UNKNOWN_FLAG_0x00001;
				break;
			}

			if (pDynamicPath->PathPoints[i].X >= tRoomCoords.nSubtileX + tRoomCoords.nSubtileWidth)
			{
				pDynamicPath->dwFlags |= PATH_UNKNOWN_FLAG_0x00001;
				break;
			}

			if (pDynamicPath->PathPoints[i].Y < tRoomCoords.nSubtileY)
			{
				pDynamicPath->dwFlags |= PATH_UNKNOWN_FLAG_0x00001;
				break;
			}

			if (pDynamicPath->PathPoints[i].Y >= tRoomCoords.nSubtileY + tRoomCoords.nSubtileHeight)
			{
				pDynamicPath->dwFlags |= PATH_UNKNOWN_FLAG_0x00001;
				break;
			}
		}
	}
}

//D2Common.0x6FDA82A0 (#10141)
// 1.13c D2Common+0x35B00: the game-to-client transform is inlined (x' = (x - y) >> 1, y' = (y + x) >> 2).
void __stdcall PATH_GetClientCoordsVelocity(D2UnitStrc* pUnit, int* pX, int* pY)
{
	if (pUnit && UNITS_IsInMovingMode(pUnit) && pUnit->pDynamicPath->dwPathPoints)
	{
		*pX = pUnit->pDynamicPath->tVelocityVector.nX;
		*pY = pUnit->pDynamicPath->tVelocityVector.nY;
		*pX >>= 11;
		const int nY = *pY >> 11;
		*pY = nY;
		const int nX = *pX;
		*pX = (nX - nY) >> 1;
		*pY = (nY + nX) >> 2;
	}
	else
	{
		*pX = 0;
		*pY = 0;
	}
}

//D2Common.0x6FDA8320 (#10222)  // 1.13c D2Common+0x359D0
void __stdcall PATH_AddCollisionFootprintForUnit(D2UnitStrc* pUnit)
{
	D2CoordStrc pCoords = {};

	UNITS_GetCoords(pUnit, &pCoords);

	switch (pUnit->dwUnitType)
	{
	// d2-web: players take the pattern too (1.13c: a freshly allocated player's subtile reads 0x1080,
	// COLLIDE_PLAYER | COLLIDE_NO_PATH; the size branch leaves 0x0080), matching the removal below.
	case UNIT_PLAYER:
	case UNIT_MONSTER:
		COLLISION_SetMaskWithPattern(UNITS_GetRoom(pUnit), pCoords.nX, pCoords.nY, pUnit->pDynamicPath->dwCollisionPattern, UNITS_GetCollisionMask(pUnit));
		break;
	case UNIT_OBJECT:
		COLLISION_SetMaskWithSizeXY(UNITS_GetRoom(pUnit), pCoords.nX, pCoords.nY, UNITS_GetUnitSizeX(pUnit), UNITS_GetUnitSizeY(pUnit), UNITS_GetCollisionMask(pUnit));
		break;
	default:
		COLLISION_SetMaskWithSize(UNITS_GetRoom(pUnit), pCoords.nX, pCoords.nY, UNITS_GetUnitSizeX(pUnit), UNITS_GetCollisionMask(pUnit));
		break;
	}
}

//D2Common.0x6FDA8450 (#10223)
// 1.13c D2Common+0x35800
// d2-web: the fields this routine reads. The offsets are retail's 32-bit ones; each pointer ahead of a
// field widens the field by (sizeof(void*) - 4) in the wasm64 build.
static_assert(offsetof(D2UnitStrc, dwUnitType) == 0x00);
static_assert(offsetof(D2UnitStrc, dwAnimMode) == 0x10 + 1 * (sizeof(void*) - 4));                  // after pMemoryPool
static_assert(offsetof(D2UnitStrc, pObjectData) == 0x14 + 1 * (sizeof(void*) - 4));                 // after pMemoryPool
static_assert(offsetof(D2UnitStrc, pDynamicPath) == 0x2C + 3 * (sizeof(void*) - 4));                // after pMemoryPool, pPlayerData, pDrlgAct
static_assert(offsetof(D2DynamicPathStrc, tGameCoords) == 0x00);
static_assert(offsetof(D2FP32_16, wPosX) == 0x02 && offsetof(D2FP32_16, wPosY) == 0x06);            // the dynamic path's x and y words
static_assert(offsetof(D2DynamicPathStrc, dwCollisionPattern) == 0x48 + 4 * (sizeof(void*) - 4));   // after pRoom, pPreviousRoom, unk0x2C, pUnit
static_assert(offsetof(D2StaticPathStrc, tGameCoords) == 0x0C + 1 * (sizeof(void*) - 4));           // after pRoom
static_assert(offsetof(D2CoordStrc, nX) == 0 && offsetof(D2CoordStrc, nY) == 0x04);
static_assert(offsetof(D2ObjectDataStrc, pObjectTxt) == 0x00);
static_assert(offsetof(D2ObjectsTxt, nHasCollision) == 0x120);

// 1.13c D2Common+0x35800 (#10775): player/monster spare the dead and death modes unless forced; an object
// without collision is spared unless forced; any other unit is always cleared.
BOOL __stdcall PATH_RemoveCollisionFootprintForUnit(D2UnitStrc* pUnit, BOOL bForce)
{
	D2CoordStrc pCoords = {};
	UNITS_GetCoords(pUnit, &pCoords);

	switch (pUnit->dwUnitType)
	{
	case UNIT_PLAYER:
		if (bForce || (pUnit->dwAnimMode != PLRMODE_DEAD && pUnit->dwAnimMode != PLRMODE_DEATH))
		{
			COLLISION_ResetMaskWithPattern(UNITS_GetRoom(pUnit), pCoords.nX, pCoords.nY, pUnit->pDynamicPath->dwCollisionPattern, UNITS_GetCollisionMask(pUnit));
			return TRUE;
		}
		break;
	case UNIT_MONSTER:
		if (bForce || (pUnit->dwAnimMode != MONMODE_DEAD && pUnit->dwAnimMode != MONMODE_DEATH))
		{
			COLLISION_ResetMaskWithPattern(UNITS_GetRoom(pUnit), pCoords.nX, pCoords.nY, pUnit->pDynamicPath->dwCollisionPattern, UNITS_GetCollisionMask(pUnit));
			return TRUE;
		}
		break;
	case UNIT_OBJECT:
		if (bForce || UNITS_HasCollision(pUnit))
		{
			COLLISION_ResetMaskWithSizeXY(UNITS_GetRoom(pUnit), pCoords.nX, pCoords.nY, UNITS_GetUnitSizeX(pUnit), UNITS_GetUnitSizeY(pUnit), UNITS_GetCollisionMask(pUnit));
			return TRUE;
		}
		break;
	default:
		COLLISION_ResetMaskWithSize(UNITS_GetRoom(pUnit), pCoords.nX, pCoords.nY, UNITS_GetUnitSizeX(pUnit), UNITS_GetCollisionMask(pUnit));
		return TRUE;
	}
	return FALSE;
}


// d2-web diagnostic: each path the target-room test refuses, kept in a ring of 64 records --
// [unit type, class, id, from x, from y, to x, to y, start room's level, an active room of the act holds
// the target (and its level), a level rectangle holds the target (its level), frame]. Read by
// d2_path_refusals(); it never changes the answer (FALSE: the path is refused, as retail's is).
int32_t g_d2webPathRefusals[64][12]; uint32_t g_d2webPathRefusalCount = 0;
// not-1.13c: d2-web diagnostic recorder with no retail counterpart (0x36230 tests the target room and fails with no call)
static BOOL D2WEB_PATH_LogRefusal(D2UnitStrc* pUnit, D2PathInfoStrc* pInfo)
{
	int32_t* r = g_d2webPathRefusals[g_d2webPathRefusalCount++ & 63];
	memset(r, 0, sizeof(int32_t) * 12);
	r[0] = pUnit ? (int32_t)pUnit->dwUnitType : -1; r[1] = pUnit ? (int32_t)pUnit->dwClassId : -1; r[2] = pUnit ? (int32_t)pUnit->dwUnitId : -1;
	r[3] = pInfo->tStartCoord.X; r[4] = pInfo->tStartCoord.Y; r[5] = pInfo->tTargetCoord.X; r[6] = pInfo->tTargetCoord.Y;
	D2ActiveRoomStrc* pStart = pInfo->pStartRoom;
	r[7] = (pStart && pStart->pDrlgRoom && pStart->pDrlgRoom->pLevel) ? pStart->pDrlgRoom->pLevel->nLevelId : -1;
	r[8] = r[9] = r[10] = -1;
	if (pStart && pStart->pDrlgRoom && pStart->pDrlgRoom->pLevel)
		for (D2DrlgLevelStrc* pLevel = pStart->pDrlgRoom->pLevel->pDrlg->pLevel; pLevel; pLevel = pLevel->pNextLevel) {
			const int nX = pInfo->tTargetCoord.X, nY = pInfo->tTargetCoord.Y;
			if (r[9] < 0 && nX >= pLevel->nPosX * 5 && nX < (pLevel->nPosX + pLevel->nWidth) * 5 && nY >= pLevel->nPosY * 5 && nY < (pLevel->nPosY + pLevel->nHeight) * 5) r[9] = pLevel->nLevelId;
			for (D2DrlgRoomStrc* pDrlgRoom = pLevel->pFirstRoomEx; pDrlgRoom; pDrlgRoom = pDrlgRoom->pDrlgRoomNext) {
				const int x0 = pDrlgRoom->nTileXPos * 5, y0 = pDrlgRoom->nTileYPos * 5, x1 = x0 + pDrlgRoom->nTileWidth * 5, y1 = y0 + pDrlgRoom->nTileHeight * 5;
				if (nX >= x0 && nX < x1 && nY >= y0 && nY < y1) { r[8] = pDrlgRoom->pRoom ? 1 : 0; r[10] = pLevel->nLevelId; }
			}
		}
	r[11] = (int32_t)g_d2webPathRefusalCount;
	return FALSE;
}
//1.10f: Inlined
// 1.13c D2Common+0x356E0
BOOL __fastcall PATH_IsTargetDestinationAllowed(D2PathInfoStrc* pPathInfo, D2UnitStrc* pUnit)
{
	if (pUnit && pUnit->dwUnitType == UNIT_MONSTER) // Only monsters may have restricted access to town
	{
		if (MONSTERS_CanBeInTown(pUnit)) // If it can be in town, everything's fine
		{
			return TRUE;
		}

		// Otherwise check if the target is in town
		D2ActiveRoomStrc* pTargetRoom = pPathInfo->pTargetRoom;
		if (!pTargetRoom)
		{
			// Try to find target room
			D2ActiveRoomStrc* pRoomHalfway = COLLISION_GetRoomBySubTileCoordinates(
				pPathInfo->pStartRoom,
				pPathInfo->tStartCoord.X + (pPathInfo->tTargetCoord.X - pPathInfo->tStartCoord.X) / 2,
				pPathInfo->tStartCoord.Y + (pPathInfo->tTargetCoord.Y - pPathInfo->tStartCoord.Y) / 2);
			pTargetRoom = COLLISION_GetRoomBySubTileCoordinates(pRoomHalfway, pPathInfo->tTargetCoord.X, pPathInfo->tTargetCoord.Y);
		}
		if (pTargetRoom && DUNGEON_IsRoomInTown(pTargetRoom))
		{
			return FALSE;
		}
	}

	return TRUE;
}

// Author: Araksson
//1.10f: Inlined
// 1.13c D2Common+0x35780: any type other than missile / charged bolt / blessed hammer is a fatal assert.
static int PATH_MissileToTarget(D2DynamicPathStrc* pPath, D2UnitStrc* pUnit)
{
	switch (pPath->dwPathType)
	{
	case PATHTYPE_MISSILE:
		if (!PATH_ComputePathClassicMissile(pPath))
		{
			pPath->dwFlags &= ~PATH_UNKNOWN_FLAG_0x00020;
			return 0;
		}
		break;
	case PATHTYPE_CHARGEDBOLT:
		PATH_ComputePathChargedBolt_6FDAB4A0(pPath, &pUnit->pSeed);
		break;
	default:
		D2_ASSERT(pPath->dwPathType == PATHTYPE_BLESSEDHAMMER);
		PATH_ComputePathBlessedHammer_6FDAB3C0(pPath);
		break;
	}

	pPath->dwFlags |= PATH_UNKNOWN_FLAG_0x00020;
	return pPath->dwPathPoints;
}

//1.10f: Inlined
// 1.13c D2Common+0x35470
uint8_t PATH_UpdateTargetUnit(D2PathInfoStrc* pPathInfo)
{
	D2DynamicPathStrc* pPath = pPathInfo->pDynamicPath;
	if (D2UnitStrc* pPathTargetUnit = pPath->pTargetUnit)
	{
		pPath->tTargetCoord.X = UNITS_GetXPosition(pPathTargetUnit);
		pPath->tTargetCoord.Y = UNITS_GetYPosition(pPathTargetUnit);

		if (pPath->tTargetCoord == D2PathPointStrc{ 0,0 })
		{
			return 0;
		}
		switch (pPathTargetUnit->dwUnitType)
		{
		case UNIT_PLAYER:
		case UNIT_MONSTER:
			if (pPath->dwSpeed)
			{
				PATHUtil_AdvanceTowardsTarget_6FDAB890(pPath);
			}
			return 1;
		case UNIT_OBJECT:
			if (!UNITS_IsDoor(pPathTargetUnit))
			{
				return 2;
			}
			// 1.13c inlines the door step here: shift the target 2 tiles past the door along its axis.
			D2_ASSERT(UNITS_IsDoor(pPathTargetUnit));
			if (pPathTargetUnit->pObjectData->pObjectTxt->nOrientation != 1)
			{
				if (pPath->tGameCoords.wPosX >= UNITS_GetXPosition(pPathTargetUnit)) pPath->tTargetCoord.X += 2;
				else pPath->tTargetCoord.X -= 2;
			}
			else
			{
				if (pPath->tGameCoords.wPosY >= UNITS_GetYPosition(pPathTargetUnit)) pPath->tTargetCoord.Y += 2;
				else pPath->tTargetCoord.Y -= 2;
			}
			return 2;
		case UNIT_ITEM:
			return 2;
		default:
			return 1;
		}
	}
	return 1;
}

//1.00:  D2Common.0x10059110 (#10141)
//1.10f: D2Common.0x6FDA8600 (#10142)
// 1.13c D2Common+0x36230 (#10334)
int __stdcall D2Common_10142(D2DynamicPathStrc* pPath, D2UnitStrc* pUnit, int bAllowInTown)
{
	if (!pPath)
	{
		return 0;
	}
	D2_ASSERT(pPath->pUnit == pUnit);

	if (pPath->dwFlags & PATH_MISSILE_MASK)
	{
		return PATH_MissileToTarget(pPath, pUnit);
	}
	pPath->nCollidedWithMask = 0;
	D2UnitStrc* const pTargetUnit = pPath->pTargetUnit;
	if (pTargetUnit && (UNITS_GetXPosition(pTargetUnit) == 0 || UNITS_GetYPosition(pTargetUnit) == 0))
	{
		return 0;
	}

	D2PathInfoStrc tPathInfo{};
	D2PathPointStrc tStart = { pPath->tGameCoords.wPosX, pPath->tGameCoords.wPosY };
	tPathInfo.tStartCoord = tStart;
	if (tPathInfo.tStartCoord != D2PathPointStrc{ 0,0 })
	{
		tPathInfo.nUnitSize = pPath->dwUnitSize;
		tPathInfo.nCollisionPattern = pPath->dwCollisionPattern;
		tPathInfo.pDynamicPath = pPath;
		tPathInfo.nCollisionMask = pPath->nMoveTestCollisionMask;
		tPathInfo.nDistMax = pPath->nDistMax;
		tPathInfo.nMinimumFScoreToEvaluate = pPath->nIDAStarInitFScore;
		tPathInfo.field_14 = PATH_UpdateTargetUnit(&tPathInfo);

		if (pPath->tTargetCoord != D2PathPointStrc{ 0,0 })
		{
			tPathInfo.tTargetCoord = pPath->tTargetCoord;
			if (tPathInfo.tStartCoord != tPathInfo.tTargetCoord
				&& std::abs(tPathInfo.tStartCoord.X - tPathInfo.tTargetCoord.X) <= 100
				&& std::abs(tPathInfo.tStartCoord.Y - tPathInfo.tTargetCoord.Y) <= 100
				)
			{
				tPathInfo.pStartRoom = pPath->pRoom;
				tPathInfo.pTargetRoom = COLLISION_GetRoomBySubTileCoordinates(tPathInfo.pStartRoom, tPathInfo.tTargetCoord.X, tPathInfo.tTargetCoord.Y);
				// F671: capture the resolver result from ordinal 10334 itself, after it has used the
				// path's actual starting room. The wrapper's earlier room check is a distinct stage.
				if (g_d2webWalkProbeActive && g_d2webWalkProbe.sequence && pUnit == pPath->pUnit) {
					g_d2webWalkProbe.resolverCalled = 1;
					g_d2webWalkProbe.resolverRoomFound = tPathInfo.pTargetRoom != nullptr;
					g_d2webWalkProbe.resolverLevel = tPathInfo.pTargetRoom && tPathInfo.pTargetRoom->pDrlgRoom
						? DRLGROOM_GetLevelId(tPathInfo.pTargetRoom->pDrlgRoom) : -1;
					g_d2webWalkProbe.resolverPathType = pPath->dwPathType;
					g_d2webWalkProbe.resolverStartX = tPathInfo.tStartCoord.X;
					g_d2webWalkProbe.resolverStartY = tPathInfo.tStartCoord.Y;
					g_d2webWalkProbe.resolverTargetX = tPathInfo.tTargetCoord.X;
					g_d2webWalkProbe.resolverTargetY = tPathInfo.tTargetCoord.Y;
				}

				// d2-web: retail 1.13c D2Common 0x6FD86396..0x6FD863AD tests the start room AND the target room
				// (`test ecx,ecx; je fail; test eax,eax; je fail`): a path to a subtile no room holds fails.
				// Upstream compiled that only under D2_VERSION_113C, never defined here (s232, act5barb1 #28 at
				// frame 323 walked toward (4892,5244), south of the Bloody Foothills, where retail refused it).
				if (tPathInfo.pStartRoom &&
					(tPathInfo.pTargetRoom || D2WEB_PATH_LogRefusal(pUnit, &tPathInfo)) &&
					(PATH_IsTargetDestinationAllowed(&tPathInfo, pUnit) || bAllowInTown))
				{
					tPathInfo.nPathType = pPath->dwPathType;

					BOOL bRemovedTargetUnitFootprint = FALSE;
					PATH_RemoveCollisionFootprintForUnit(pUnit, FALSE);
					if (pPath->pTargetUnit
						&& UNITS_GetUnitSizeX(pPath->pTargetUnit)
						&& (pPath->dwFlags & PATH_UNKNOWN_FLAG_0x00800) != 0)
					{
						bRemovedTargetUnitFootprint = PATH_RemoveCollisionFootprintForUnit(pPath->pTargetUnit, FALSE);
					}

					BOOL bUpdatePath = 1;
					if ((pPath->dwFlags & PATH_UNKNOWN_FLAG_0x01000) != 0
						&& COLLISION_CheckAnyCollisionWithPattern(
							tPathInfo.pStartRoom,
							tPathInfo.tTargetCoord.X,
							tPathInfo.tTargetCoord.Y,
							pPath->dwCollisionPattern,
							pPath->nMoveTestCollisionMask))
					{
						bUpdatePath = PATH_PreparePathTargetForPathUpdate(&tPathInfo);
					}

					pPath->dwCurrentPointIdx = 0;
					pPath->dwPathPoints = 0;
					pPath->tFinalTargetCoord = tPathInfo.tTargetCoord;
					if (bUpdatePath)
					{
						D2_ASSERT(scpfnPathFunction[tPathInfo.nPathType]);
						pPath->dwPathPoints = scpfnPathFunction[tPathInfo.nPathType](&tPathInfo);
					}
					if (bRemovedTargetUnitFootprint)
					{
						PATH_AddCollisionFootprintForUnit(pPath->pTargetUnit);
					}
					PATH_AddCollisionFootprintForUnit(pUnit);
					if (!pPath->dwPathPoints)
					{
						pPath->dwFlags &= ~PATH_UNKNOWN_FLAG_0x00020;
						return 0;
					}

					PATH_ComputeVelocityAndDirectionVectorsToNextPoint(pPath, 1, 1);
					if (pPath->dwCurrentPointIdx < pPath->dwPathPoints)
					{
						sub_6FDA8220(pPath);
						pPath->tPrevTargetCoord = pPath->tTargetCoord;
						pPath->unk0x38 = 0;
						if ((pPath->dwFlags & PATH_UNKNOWN_FLAG_0x00010) == 0 && !pPath->pTargetUnit)
						{
							pPath->tTargetCoord = pPath->PathPoints[pPath->dwPathPoints - 1];
						}

						if (pPath->dwPathPoints)
						{
							pPath->dwFlags |= PATH_UNKNOWN_FLAG_0x00020;
						}
						else
						{
							pPath->dwFlags &= ~PATH_UNKNOWN_FLAG_0x00020;
						}
						return pPath->dwPathPoints;
					}
				}

			}
		}
	}
	pPath->dwCurrentPointIdx = 0;
	pPath->dwPathPoints = 0;
	pPath->dwFlags &= ~PATH_UNKNOWN_FLAG_0x00020;
	return 0;
}

//D2Common.0x6FDA8E30
// 1.13c D2Common+0x35530: a target 100+ tiles away on either axis is refused (returns 0), not asserted.
int __fastcall PATH_ComputePathClassicMissile(D2DynamicPathStrc* pDynamicPath)
{
	D2CoordStrc pCoords = {};

	pDynamicPath->dwCurrentPointIdx = 0;

	if (pDynamicPath->pTargetUnit)
	{
		UNITS_GetCoords(pDynamicPath->pTargetUnit, &pCoords);

		pDynamicPath->tTargetCoord.X = pCoords.nX;
		pDynamicPath->tTargetCoord.Y = pCoords.nY;
	}

	int nXDistance = (int)pDynamicPath->tGameCoords.wPosX - (int)pDynamicPath->tTargetCoord.X;
	if (nXDistance < 0)
	{
		nXDistance = -nXDistance;
	}
	if (nXDistance > 99)
	{
		return 0;
	}

	int nYDistance = (int)pDynamicPath->tGameCoords.wPosY - (int)pDynamicPath->tTargetCoord.Y;
	if (nYDistance < 0)
	{
		nYDistance = -nYDistance;
	}
	if (nYDistance > 99)
	{
		return 0;
	}

	if (pDynamicPath->tTargetCoord.X && pDynamicPath->tTargetCoord.Y)
	{
		pDynamicPath->PathPoints[0].X = pDynamicPath->tTargetCoord.X;
		pDynamicPath->PathPoints[0].Y = pDynamicPath->tTargetCoord.Y;

		pDynamicPath->dwPathPoints = 1;
		pDynamicPath->unk0x38 = 0;
		PATH_ComputeVelocityAndDirectionVectorsToNextPoint(pDynamicPath, 1, 1);

		if (!(pDynamicPath->pRoom && DungeonTestRoomGame(pDynamicPath->pRoom, pDynamicPath->tTargetCoord.X, pDynamicPath->tTargetCoord.Y)))
		{
			pDynamicPath->dwFlags |= PATH_UNKNOWN_FLAG_0x00001;
		}

		return pDynamicPath->dwPathPoints;
	}

	return 0;
}

//D2Common.0x6FDA8FE0
// 1.13c D2Common+0x35D40
void __fastcall PATH_FindValidTargetCoordsByMovingOrthogonally(D2PathInfoStrc* pPathInfo)
{
	D2PathPointStrc tCoords = pPathInfo->tTargetCoord;

	const int nDeltaX = tCoords.X - pPathInfo->tStartCoord.X;
	const int nDeltaY = tCoords.Y - pPathInfo->tStartCoord.Y;
	const int nAbsDeltaX = std::abs(nDeltaX);
	const int nAbsDeltaY = std::abs(nDeltaY);
	if (nAbsDeltaX < 5 && nAbsDeltaY < 5 && (nDeltaX != 0 || nDeltaY != 0))
	{
		const int nRemappedX = nDeltaX + 4;
		const int nRemappedY = nDeltaY + 4;
		// A convoluted way to chose one the 4 cardinal directions.
		const int nOffsetX = gaOffsetsForSnappingToCardinalDirection[nRemappedY][nRemappedX];
		const int nOffsetY = gaOffsetsForSnappingToCardinalDirection[nRemappedX][nRemappedY];

		int nChebyshevDistance = std::max(nAbsDeltaX, nAbsDeltaY);
		while (nChebyshevDistance < 5)
		{
			tCoords.X += nOffsetX;
			tCoords.Y += nOffsetY;

			if (COLLISION_CheckAnyCollisionWithPattern(pPathInfo->pStartRoom, tCoords.X, tCoords.Y, pPathInfo->nCollisionPattern, pPathInfo->nCollisionMask))
			{
				break;
			}

			++nChebyshevDistance;
			pPathInfo->tTargetCoord.X = tCoords.X;
			pPathInfo->tTargetCoord.Y = tCoords.Y;
		}
	}
}

//D2Common.0x6FDA9190 (#10156)
// 1.13c D2Common+0x34DC0
void __stdcall PATH_FreeDynamicPath(void* pMemPool, D2DynamicPathStrc* pDynamicPath)
{
	D2_FREE_POOL(pMemPool, pDynamicPath);
}

//D2Common.0x6FDA91B0 (#11282)
// 1.13c D2Common+0x353D0
//Unused
int __stdcall PATH_GetCollisionPatternFromMonStats2Txt(int nMonsterId)
{
	D2MonStats2Txt* pMonStats2TxtRecord = NULL;

	pMonStats2TxtRecord = UNITS_GetMonStats2TxtRecordFromMonsterId(nMonsterId);
	if (pMonStats2TxtRecord && pMonStats2TxtRecord->nSizeX >= 0 && pMonStats2TxtRecord->nSizeX < COLLISION_UNIT_SIZE_COUNT)
	{
		D2C_CollisionPattern nCollisionPattern = gaCollisionPatternsFromSize_6FDD1DE4[pMonStats2TxtRecord->nSizeX];

		if (pMonStats2TxtRecord->dwFlags & gdwBitMasks[MONSTATS2FLAGINDEX_REVIVE] || pMonStats2TxtRecord->dwFlags & gdwBitMasks[MONSTATS2FLAGINDEX_SMALL])
		{
			if (!(pMonStats2TxtRecord->dwFlags & gdwBitMasks[MONSTATS2FLAGINDEX_ISATT]))
			{
				if (nCollisionPattern == COLLISION_PATTERN_SMALL_UNIT_PRESENCE)
				{
					nCollisionPattern = COLLISION_PATTERN_SMALL_PET_PRESENCE;
				}
				else if (nCollisionPattern == COLLISION_PATTERN_BIG_UNIT_PRESENCE)
				{
					nCollisionPattern = COLLISION_PATTERN_BIG_PET_PRESENCE;
				}
			}
		}

		return nCollisionPattern;
	}

	return COLLISION_PATTERN_SMALL_UNIT_PRESENCE;
}

//D2Common.0x6FDA9250 (#11281)
// 1.13c D2Common+0x35360
int __stdcall D2Common_11281_CollisionPatternFromSize(D2UnitStrc* pUnit, int nSize)
{
	if (nSize < 0 || nSize >= COLLISION_UNIT_SIZE_COUNT)
	{
		return COLLISION_PATTERN_SMALL_UNIT_PRESENCE;
	}
	
	const D2C_CollisionPattern nCollisionPattern = gaCollisionPatternsFromSize_6FDD1DE4[nSize];
	if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && MONSTERS_CanBeInTown(pUnit))
	{
		const D2MonStatsTxt* pMonStatsTxtRecord = DATATBLS_GetMonStatsTxtRecord(pUnit->dwClassId);
		if (!pMonStatsTxtRecord || !(pMonStatsTxtRecord->dwMonStatsFlags & gdwBitMasks[MONSTATSFLAGINDEX_INTERACT]))
		{
			if (nCollisionPattern == COLLISION_PATTERN_SMALL_UNIT_PRESENCE)
			{
				return COLLISION_PATTERN_SMALL_PET_PRESENCE;
			}
			else if (nCollisionPattern == COLLISION_PATTERN_BIG_UNIT_PRESENCE)
			{
				return COLLISION_PATTERN_BIG_PET_PRESENCE;
			}
		}
	}

	return nCollisionPattern;
}

//D2Common.0x6FDA92F0 (#10214)
//TODO: Find a name
// 1.13c D2Common+0x35CF0
void __stdcall D2Common_10214(D2UnitStrc* pUnit)
{
	if (pUnit->pDynamicPath)
	{
		if (UNITS_GetRoom(pUnit))
		{
			PATH_RemoveCollisionFootprintForUnit(pUnit, 1);
			pUnit->pDynamicPath->dwCollisionPattern = D2Common_11281_CollisionPatternFromSize(pUnit, pUnit->pDynamicPath->dwUnitSize);
			PATH_AddCollisionFootprintForUnit(pUnit);
		}
		else
		{
			pUnit->pDynamicPath->dwCollisionPattern = D2Common_11281_CollisionPatternFromSize(pUnit, pUnit->pDynamicPath->dwUnitSize);
		}
	}
}

//D2Common.0x6FDA9480 (#10152)
// 1.13c D2Common+0x36030
void __stdcall PATH_AllocDynamicPath(void* pMemPool, D2ActiveRoomStrc* pRoom, int nX, int nY, D2UnitStrc* pUnit, BOOL bSetFlag)
{
	D2DynamicPathStrc* pDynamicPath = D2_CALLOC_STRC_POOL(pMemPool, D2DynamicPathStrc);

	pUnit->pDynamicPath = pDynamicPath;

	pDynamicPath->pUnit = pUnit;
	pDynamicPath->dwUnitSize = UNITS_GetUnitSizeX(pUnit);

	pDynamicPath->dwCollisionPattern = D2Common_11281_CollisionPatternFromSize(pUnit, pDynamicPath->dwUnitSize);

	pDynamicPath->dwVelocity = 2048;
	pDynamicPath->pRoom = pRoom;
	pDynamicPath->nStepNum = 0;
	pDynamicPath->tGameCoords.dwPrecisionX = PATH_ToFP16Center(nX);
	pDynamicPath->tGameCoords.dwPrecisionY = PATH_ToFP16Center(nY);

	pDynamicPath->nSavedStepsCount = 1;
	pDynamicPath->SavedSteps[0].X = nX;
	pDynamicPath->SavedSteps[0].Y = nY;

	if (pUnit->dwUnitType == UNIT_PLAYER)
	{
		pDynamicPath->nFootprintCollisionMask = COLLIDE_PLAYER;
		pDynamicPath->nMoveTestCollisionMask = COLLIDE_MASK_PLAYER_PATH;
		PATH_SetType(pDynamicPath, PATHTYPE_STRAIGHT);
		pDynamicPath->nDistMax = 73;
		pDynamicPath->nIDAStarInitFScore = 70;
	}
	else if (pUnit->dwUnitType == UNIT_MONSTER)
	{
		pDynamicPath->nFootprintCollisionMask = COLLIDE_MONSTER;
		PATH_SetType(pDynamicPath, PATHTYPE_TOWARD);

		int nClassId = pUnit->dwClassId;

		D2MonStatsTxt* pMonStatsTxtRecord = DATATBLS_GetMonStatsTxtRecord(nClassId);
		if (pMonStatsTxtRecord)
		{
			nClassId = pMonStatsTxtRecord->nBaseId;
		}

		if (nClassId == MONSTER_WRAITH1)
		{
			pDynamicPath->dwCollisionPattern = COLLISION_PATTERN_SMALL_NO_PRESENCE;
			pDynamicPath->nMoveTestCollisionMask = COLLIDE_MISSILE_BARRIER | COLLIDE_DOOR;
			pDynamicPath->nDistMax = 14;
		}
		else
		{
			if (pMonStatsTxtRecord && pMonStatsTxtRecord->dwMonStatsFlags & gdwBitMasks[MONSTATSFLAGINDEX_FLYING])
			{
				pDynamicPath->nDistMax = 14;
				pDynamicPath->nMoveTestCollisionMask = COLLIDE_MASK_FLYING_UNIT;
			}
			else
			{
				pDynamicPath->nDistMax = 14;
				pDynamicPath->nMoveTestCollisionMask = 
					(pMonStatsTxtRecord && (pMonStatsTxtRecord->dwMonStatsFlags & gdwBitMasks[MONSTATSFLAGINDEX_OPENDOORS]) != 0) 
					? COLLIDE_MASK_MONSTER_THAT_CAN_OPEN_DOORS
					: COLLIDE_MASK_MONSTER_PATH;
			}
		}
	}
	else if (pUnit->dwUnitType == UNIT_MISSILE)
	{
		pDynamicPath->nFootprintCollisionMask = COLLIDE_NONE;
		pDynamicPath->nMoveTestCollisionMask = COLLIDE_NONE;
		PATH_SetType(pDynamicPath, PATHTYPE_MISSILE);
	}

	if (pRoom)
	{
		PATH_AddCollisionFootprintForUnit(pUnit);
		UNITROOM_AddUnitToRoom(pUnit, pDynamicPath->pRoom);
	}
	
	PATH_UpdateClientCoords(pDynamicPath);
	
	if (bSetFlag)
	{
		pDynamicPath->dwFlags |= PATH_UNKNOWN_FLAG_0x00010;
	}
}

//D2Common.0x6FDA9720
//TODO: Find a name
// 1.13c D2Common+0x350B0 (verified line by line: type 2/4 set, missile flag 0x40, default smoothing)
void __fastcall sub_6FDA9720(D2DynamicPathStrc* pDynamicPath, uint8_t nDirection)
{
	D2C_UnitTypes nUnitType;

	if (pDynamicPath->pUnit)
	{
		nUnitType = D2C_UnitTypes(pDynamicPath->pUnit->dwUnitType);
	}
	else
	{
		nUnitType = UNIT_TYPES_COUNT;
	}

	const uint8_t nNormalizedDirection = PATH_NormalizeDirection(nDirection);
	switch (nUnitType)
	{
	case UNIT_OBJECT:
	case UNIT_ITEM:
		pDynamicPath->nDirection = nNormalizedDirection;
		break;
	case UNIT_MISSILE:
		if (!(pDynamicPath->dwFlags & PATH_UNKNOWN_FLAG_0x00040))
		{
			pDynamicPath->nDirection = nNormalizedDirection;
		}
		break;
	default:
		if (nNormalizedDirection != pDynamicPath->nNewDirection)
		{
			pDynamicPath->nNewDirection = nNormalizedDirection;
			pDynamicPath->nDiffDirection = dword_6FDD1F88[PATH_NormalizeDirection(nNormalizedDirection - pDynamicPath->nDirection)];
		}
		break;
	}
}

//D2Common.0x6FDA9770 (#10193)
//TODO: Check name
// 1.13c D2Common+0x34D70
void __stdcall D2COMMON_10193_PATH_AdjustDirection(D2DynamicPathStrc* pDynamicPath)
{
	if ((pDynamicPath->nDirection != pDynamicPath->nNewDirection) && !(pDynamicPath->dwFlags & PATH_UNKNOWN_FLAG_0x00040))
	{
		pDynamicPath->nDirection = PATH_NormalizeDirection(pDynamicPath->nDirection + pDynamicPath->nDiffDirection);

		const uint8_t dirDiff = PATH_NormalizeDirection(pDynamicPath->nDirection - pDynamicPath->nNewDirection);
		if (dirDiff > 31)
		{
			if (pDynamicPath->nDiffDirection > 63)
			{
				pDynamicPath->nDirection = pDynamicPath->nNewDirection;
			}
		}
		else if (dirDiff < 31)
		{
			if (pDynamicPath->nDiffDirection < 63)
			{
				pDynamicPath->nDirection = pDynamicPath->nNewDirection;
			}
		}
	}
}

//D2Common.0x6FDA97C0 (#10216)
//TODO: Find a name
// 1.13c D2Common+0x352F0 (#10739): calls the direction-vector routine directly toward the tile centre.
void __stdcall D2Common_10216(D2DynamicPathStrc* pDynamicPath, int nX, int nY, int a4)
{
	if (pDynamicPath)
	{
		D2CoordStrc tVector = {};
		int nDirection = 0;
		PATH_GetDirectionVector(&tVector, &nDirection, pDynamicPath->tGameCoords.dwPrecisionX, pDynamicPath->tGameCoords.dwPrecisionY, nX * 0x10000 + 0x8000, nY * 0x10000 + 0x8000);
		if (a4)
		{
			pDynamicPath->nDirection = (uint8_t)(nDirection & 0x3F);
			pDynamicPath->nNewDirection = (uint8_t)(nDirection & 0x3F);
		}
		else
		{
			sub_6FDA9720(pDynamicPath, (uint8_t)nDirection);
		}
	}
}

//D2Common.0x6FDA9850 (#10228)
//TODO: Find a name
// 1.13c D2Common+0x35CD0
void __stdcall D2Common_10228(D2UnitStrc* pUnit)
{
	if (pUnit->pDynamicPath)
	{
		PATH_RemoveCollisionFootprintForUnit(pUnit, 1);
	}
}

//D2Common.0x6FDA9870 (#10143)
// 1.13c D2Common+0x365A0
void __stdcall PATH_SetUnitDeadCollision(D2UnitStrc* pUnit, BOOL bForGameLogic)
{
	D2_ASSERT(pUnit->pDynamicPath != nullptr);
	if (bForGameLogic || pUnit->dwUnitType != UNIT_MONSTER)
	{
		PATH_RemoveCollisionFootprintForUnit(pUnit, TRUE);
		COLLISION_SetMaskWithSizeXY(pUnit->pDynamicPath->pRoom, pUnit->pDynamicPath->tGameCoords.wPosX, pUnit->pDynamicPath->tGameCoords.wPosY, 3, 3, COLLIDE_CORPSE);
		PATH_RecacheRoomIfNeeded(pUnit->pDynamicPath);
	}
	else // Called from Client, related to corpse being displayed
	{
		PATH_RemoveCollisionFootprintForUnit(pUnit, TRUE);
		pUnit->pDynamicPath->dwCollisionPattern = COLLISION_PATTERN_SMALL_NO_PRESENCE;
		PATH_SetFootprintCollisionMask(pUnit->pDynamicPath, COLLIDE_CORPSE);
		PATH_AddCollisionFootprintForUnit(pUnit);
		PATH_RecacheRoomIfNeeded(pUnit->pDynamicPath);
	}
}

//D2Common.0x6FDA98F0 (#10144)
// 1.13c D2Common+0x35BE0
void __stdcall PATH_SetUnitAliveCollision(D2UnitStrc* pUnit, BOOL bForGameLogic)
{
	if (bForGameLogic)
	{
		return;
	}
	//else: Called from Client to remove corpse collision

	D2_ASSERT(pUnit && pUnit->dwUnitType == UNIT_MONSTER);
	if (pUnit->pDynamicPath && pUnit->pDynamicPath->nFootprintCollisionMask == COLLIDE_CORPSE)
	{
		int nCollisionPattern = D2Common_11281_CollisionPatternFromSize(pUnit, UNITS_GetUnitSizeX(pUnit));

		if (pUnit->dwUnitType == UNIT_MONSTER)
		{
			const int nClassId = MONSTERS_GetBaseIdFromMonsterId(pUnit->dwClassId);

			if (nClassId == MONSTER_WRAITH1 || nClassId == MONSTER_BIRD1 || nClassId == MONSTER_BIRD2 || nClassId == MONSTER_PARROT)
			{
				nCollisionPattern = COLLISION_PATTERN_SMALL_NO_PRESENCE;
			}
		}

		if (UNITS_GetRoom(pUnit))
		{
			PATH_RemoveCollisionFootprintForUnit(pUnit, 1);
			pUnit->pDynamicPath->dwCollisionPattern = nCollisionPattern;
			PATH_SetFootprintCollisionMask(pUnit->pDynamicPath, COLLIDE_MONSTER);
			PATH_AddCollisionFootprintForUnit(pUnit);
		}
		else
		{
			pUnit->pDynamicPath->dwCollisionPattern = nCollisionPattern;
			PATH_SetFootprintCollisionMask(pUnit->pDynamicPath, COLLIDE_MONSTER);
		}
	}
}

//1.10f: D2Common.0x6FDA9A70 (#10146)
//1.13c: D2Common.0x6FD84D40 (#10488)
// 1.13c D2Common+0x34D40
void __stdcall PATH_SetVelocity(D2DynamicPathStrc* pDynamicPath, int nVelocity, const char* szFile, int nLine)
{
	if (pDynamicPath)
	{
		if (nVelocity != pDynamicPath->dwVelocity)
		{
			pDynamicPath->unk0x38 = 15;
		}

		pDynamicPath->dwVelocity = nVelocity;
		pDynamicPath->dwMaxVelocity = nVelocity;
	}
	else
	{
		FOG_Trace("PathSetVelocity passed NULL hPath from FILE:%s  LINE:%d", szFile, nLine);
	}
}

//D2Common.0x6FDA9AB0 (#10147)
// 1.13c D2Common+0x34D30
int __stdcall PATH_GetVelocity(D2DynamicPathStrc* pDynamicPath)
{
	return pDynamicPath->dwVelocity;
}

//D2Common.0x6FDA9AC0 (#10148)
// 1.13c D2Common+0x34D10
void __stdcall PATH_SetMaxVelocity(D2DynamicPathStrc* pDynamicPath, int nMaxVelocity)
{
	pDynamicPath->dwMaxVelocity = nMaxVelocity;
}

//D2Common.0x6FDA9AE0 (#10149)
// 1.13c D2Common+0x34D00
int __stdcall PATH_GetMaxVelocity(D2DynamicPathStrc* pDynamicPath)
{
	return pDynamicPath->dwMaxVelocity;
}

//D2Common.0x6FDA9AF0 (#10150)
// 1.13c D2Common+0x34CE0
void __stdcall PATH_SetAcceleration(D2DynamicPathStrc* pDynamicPath, int nAcceleration)
{
	pDynamicPath->dwAcceleration = nAcceleration;
}

//D2Common.0x6FDA9B10 (#10151)
// 1.13c D2Common+0x34CD0
int __stdcall PATH_GetAcceleration(D2DynamicPathStrc* pDynamicPath)
{
	return pDynamicPath->dwAcceleration;
}

//D2Common.0x6FDA9B20 (#10153)
//TODO: Find a name
// 1.13c D2Common+0x34CB0
void __stdcall D2Common_10153(D2DynamicPathStrc* pDynamicPath)
{
	pDynamicPath->dwFlags &= (~PATH_UNKNOWN_FLAG_0x00020);
	pDynamicPath->dwPathPoints = 0;
}

//D2Common.0x6FDA9B40 (#10208)
// 1.13c D2Common+0x34C80
//TODO: Check name
void __stdcall D2COMMON_10208_PathSetPathingFlag(D2DynamicPathStrc* pDynamicPath, BOOL bSet)
{
	if (bSet)
	{
		pDynamicPath->dwFlags |= PATH_UNKNOWN_FLAG_0x00020;
	}
	else
	{
		pDynamicPath->dwFlags &= (~PATH_UNKNOWN_FLAG_0x00020);
	}
}

//D2Common.0x6FDA9B70 (#10209)
//TODO: Check name
// 1.13c D2Common+0x34C70
BOOL __stdcall D2COMMON_10209_PathCheckPathingFlag(D2DynamicPathStrc* pDynamicPath)
{
	return (pDynamicPath->dwFlags & PATH_UNKNOWN_FLAG_0x00020);
}

//D2Common.0x6FDA9B80 (#10154)
// 1.13c D2Common+0x34C60
int __stdcall PATH_GetNumberOfPathPoints(D2DynamicPathStrc* pDynamicPath)
{
	return pDynamicPath->dwPathPoints;
}

// 1.13c D2Common+0x35080
//D2Common.0x6FDA9B90 (#11291)
void __stdcall PATH_SetNumberOfPathPoints(D2DynamicPathStrc* pDynamicPath, int a2)
{
	if (a2 < D2DynamicPathStrc::MAXPATHLEN)
	{
		pDynamicPath->dwPathPoints = a2;
	}
	else
	{
		pDynamicPath->dwPathPoints = D2DynamicPathStrc::MAXPATHLEN - 1;
	}
}

//D2Common.0x6FDA9BC0 (#10155)
//TODO: Find a name
// 1.13c D2Common+0x34C50
int __stdcall D2Common_10155(D2DynamicPathStrc* pDynamicPath)
{
	return pDynamicPath->dwCurrentPointIdx;
}

//D2Common.0x6FDA9BD0 (#10157)
// 1.13c D2Common+0x34C30
int __stdcall PATH_GetPathPoints(D2DynamicPathStrc* pDynamicPath, D2PathPointStrc** ppPathPoints)
{
	*ppPathPoints = pDynamicPath->PathPoints;
	return pDynamicPath->dwPathPoints;
}

//D2Common.0x6FDA9BF0 (#10158)
// 1.13c D2Common+0x34C10
uint8_t __stdcall PATH_GetDirection(D2DynamicPathStrc* pDynamicPath)
{
	if (pDynamicPath)
	{
		return pDynamicPath->nDirection;
	}

	return 0;
}

//D2Common.0x6FDA9C10 (#10159)
// 1.13c D2Common+0x34C00
uint8_t __stdcall PATH_GetNewDirection(D2DynamicPathStrc* pDynamicPath)
{
	return pDynamicPath->nNewDirection;
}

//D2Common.0x6FDA9C20 (#10160)
// 1.13c D2Common+0x35270
//TODO: Check name
void __stdcall D2COMMON_10160_PathUpdateDirection(D2DynamicPathStrc* pDynamicPath, uint8_t nDirection)
{
	D2_ASSERT(nDirection < PATH_NB_DIRECTIONS);

	sub_6FDA9720(pDynamicPath, nDirection);
}

//D2Common.0x6FDA9C90 (#10161)
// 1.13c D2Common+0x34BE0
void __stdcall PATH_SetDirection(D2DynamicPathStrc* pDynamicPath, uint8_t nDirection)
{
	if (pDynamicPath)
	{
		pDynamicPath->nDirection = PATH_NormalizeDirection(nDirection);
		pDynamicPath->nNewDirection = PATH_NormalizeDirection(nDirection);
	}
}

//D2Common.0x6FDA9CB0 (#10162)
// 1.13c D2Common+0x34BB0
int __stdcall PATH_GetXPosition(D2DynamicPathStrc* pDynamicPath)
{
	D2_ASSERT(pDynamicPath);

	return pDynamicPath->tGameCoords.wPosX;
}

//D2Common.0x6FDA9CF0 (#10163)
// 1.13c D2Common+0x34B80
int __stdcall PATH_GetYPosition(D2DynamicPathStrc* pDynamicPath)
{
	D2_ASSERT(pDynamicPath);

	return pDynamicPath->tGameCoords.wPosY;
}

//D2Common.0x6FDA9D30 (#10194)
// 1.13c D2Common+0x34B50
int __stdcall PATH_GetPrecisionX(D2DynamicPathStrc* pDynamicPath)
{
	D2_ASSERT(pDynamicPath);

	return pDynamicPath->tGameCoords.dwPrecisionX;
}

//D2Common.0x6FDA9D60 (#10195)
// 1.13c D2Common+0x34B20
int __stdcall PATH_GetPrecisionY(D2DynamicPathStrc* pDynamicPath)
{
	D2_ASSERT(pDynamicPath);

	return pDynamicPath->tGameCoords.dwPrecisionY;
}

//D2Common.0x6FDA9D90 (#10196)
// 1.13c D2Common+0x34B10
void __stdcall PATH_SetPrecisionX(D2DynamicPathStrc* pDynamicPath, int nPrecisionX)
{
	pDynamicPath->tGameCoords.dwPrecisionX = nPrecisionX;
}

//D2Common.0x6FDA9DA0 (#10197)
// 1.13c D2Common+0x34B00
void __stdcall PATH_SetPrecisionY(D2DynamicPathStrc* pDynamicPath, int nPrecisionY)
{
	pDynamicPath->tGameCoords.dwPrecisionY = nPrecisionY;
}

//D2Common.0x6FDA9DB0 (#10164)
// 1.13c D2Common+0x34AF0
int __stdcall PATH_GetClientCoordX(D2DynamicPathStrc* pDynamicPath)
{
	return pDynamicPath->dwClientCoordX;
}

//D2Common.0x6FDC3CE0 (#10165)
// 1.13c D2Common+0x34AE0
int __stdcall PATH_GetClientCoordY(D2DynamicPathStrc* pDynamicPath)
{
	return pDynamicPath->dwClientCoordY;
}

//D2Common.0x6FDA9DE0 (#10175)
//TODO: Check name
// 1.13c D2Common+0x34AB0
int __stdcall D2COMMON_10175_PathGetFirstPointX(D2DynamicPathStrc* pDynamicPath)
{
	return pDynamicPath->tTargetCoord.X;
}

//D2Common.0x6FDA9DF0 (#10176)
//TODO: Check name
// 1.13c D2Common+0x34AA0
int __stdcall D2COMMON_10176_PathGetFirstPointY(D2DynamicPathStrc* pDynamicPath)
{
	return pDynamicPath->tTargetCoord.Y;
}

//D2Common.0x6FDA9E00 (#10224)
//TODO: Find a name
// 1.13c D2Common+0x34A90
int __stdcall D2Common_10224(D2DynamicPathStrc* pDynamicPath)
{
	return pDynamicPath->tFinalTargetCoord.X;
}

//D2Common.0x6FDA9E10 (#10225)
//TODO: Find a name
// 1.13c D2Common+0x34A80
int __stdcall D2Common_10225(D2DynamicPathStrc* pDynamicPath)
{
	return pDynamicPath->tFinalTargetCoord.Y;
}

//D2Common.0x6FDA9E20 (#10177)
// 1.13c D2Common+0x34A60
int __stdcall D2COMMON_10177_PATH_GetLastPointX(D2DynamicPathStrc* pDynamicPath)
{
	if (pDynamicPath->dwPathPoints > 0)
	{
		return pDynamicPath->PathPoints[pDynamicPath->dwPathPoints-1].X;
	}

	return 0;
}

//D2Common.0x6FDA9E40 (#10178)
// 1.13c D2Common+0x34A40
int __stdcall D2COMMON_10178_PATH_GetLastPointY(D2DynamicPathStrc* pDynamicPath)
{
	if (pDynamicPath->dwPathPoints > 0)
	{
		return pDynamicPath->PathPoints[pDynamicPath->dwPathPoints-1].Y;
	}

	return 0;
}

//D2Common.0x6FDB9C10 (#10166)
// 1.13c D2Common+0x34A30
D2ActiveRoomStrc* __stdcall PATH_GetRoom(D2DynamicPathStrc* pDynamicPath)
{
	return pDynamicPath->pRoom;
}

//D2Common.0x6FDA9E60 (#10167)
// 1.13c D2Common+0x34A20
void __stdcall PATH_SetRoom(D2DynamicPathStrc* pDynamicPath, D2ActiveRoomStrc* pRoom)
{
	pDynamicPath->pRoom = pRoom;
}

//D2Common.0x6FDA9E70 (#10168)
// 1.13c D2Common+0x34A10
D2ActiveRoomStrc* __stdcall PATH_GetNextRoom(D2DynamicPathStrc* pDynamicPath)
{
	return pDynamicPath->pPreviousRoom;
}

//D2Common.0x6FDA9E80 (#10169)
// 1.13c D2Common+0x34A00
void __stdcall PATH_ClearNextRoom(D2DynamicPathStrc* pDynamicPath)
{
	pDynamicPath->pPreviousRoom = NULL;
}

//D2Common.0x6FDA9E90 (#10170)
//TODO: Check name
// 1.13c D2Common+0x349D0
void __stdcall D2COMMON_10170_PathSetTargetPos(D2DynamicPathStrc* pDynamicPath, int nX, int nY)
{
	if (pDynamicPath)
	{
		pDynamicPath->tTargetCoord.X = nX;
		pDynamicPath->tTargetCoord.Y = nY;
		pDynamicPath->pTargetUnit = NULL;
	}
}

//D2Common.0x6FDA9EC0 (#10172)
//TODO: Find a name
// 1.13c D2Common+0x349C0
BOOL __stdcall PATH_IsCurrentRoomInvalid(D2DynamicPathStrc* pDynamicPath)
{
	return pDynamicPath->dwFlags & PATH_CURRENT_ROOM_INVALID;
}

//D2Common.0x6FDA9ED0 (#10173)
//TODO: Find a name
// 1.13c D2Common+0x34990
void __stdcall PATH_SetCurrentRoomInvalid(D2DynamicPathStrc* pDynamicPath, BOOL bSet)
{
	if (bSet)
	{
		pDynamicPath->dwFlags |= PATH_CURRENT_ROOM_INVALID;
	}
	else
	{
		pDynamicPath->dwFlags &= (~PATH_CURRENT_ROOM_INVALID);
	}
}

// 1.13c D2Common+0x34960
//D2Common.0x6FDA9F00 (#10145)
void __stdcall PATH_SetUnusedFlag_0x00004(D2DynamicPathStrc* pDynamicPath, BOOL bSet)
{
	if (bSet)
	{
		pDynamicPath->dwFlags |= PATH_UNUSED_FLAG_0x00004;
	}
	else
	{
		pDynamicPath->dwFlags &= (~PATH_UNUSED_FLAG_0x00004);
	}
}

//D2Common.0x6FDA9F30 (#10174)
// 1.13c D2Common+0x34950
BOOL __stdcall PATH_GetUnusedFlag_0x00004(D2DynamicPathStrc* pDynamicPath)
{
	return pDynamicPath->dwFlags & PATH_UNUSED_FLAG_0x00004;
}

// 1.13c D2Common+0x35040
//D2Common.0x6FDA9F40 (#10179)
void __stdcall PATH_SetTargetUnit(D2DynamicPathStrc* pDynamicPath, D2UnitStrc* pUnit)
{
	pDynamicPath->pTargetUnit = pUnit;

	if (pUnit)
	{
		pDynamicPath->dwTargetType = pUnit->dwUnitType;
		pDynamicPath->dwTargetId = pUnit->dwUnitId;
	}
}

//D2Common.0x6FDA9F60 (#10171)
// 1.13c D2Common+0x34910
void __stdcall PATH_GetTargetTypeAndGUID(D2DynamicPathStrc* pDynamicPath, int* pTargetType, D2UnitGUID* pTargetGUID)
{
	D2_ASSERT(pDynamicPath->pTargetUnit);

	*pTargetType = pDynamicPath->dwTargetType;
	*pTargetGUID = pDynamicPath->dwTargetId;
}

//D2Common.0x6FDA9FA0 (#10180)
// 1.13c D2Common+0x348F0
D2UnitStrc* __stdcall PATH_GetTargetUnit(D2DynamicPathStrc* pDynamicPath)
{
	if (pDynamicPath)
	{
		return pDynamicPath->pTargetUnit;
	}

	return NULL;
}

//D2Common.0x6FDA9FC0 (#10181)
// 1.13c D2Common+0x348D0
int __stdcall PATH_GetFootprintCollisionMask(D2DynamicPathStrc* pDynamicPath)
{
	if (pDynamicPath)
	{
		return pDynamicPath->nFootprintCollisionMask;
	}
	
	// Note: this returns 0xFFFF not 0xFFFFFFFF because D2C_CollisionFlags is 16bits.
	return COLLIDE_ALL_MASK;
}

//D2Common.0x6FDA9FE0 (#10182)
// 1.13c D2Common+0x35640
void __stdcall PATH_SetFootprintCollisionMask(D2DynamicPathStrc* pDynamicPath, int nCollisionMask)
{
	if (pDynamicPath->pRoom)
	{
		if (pDynamicPath->pUnit && pDynamicPath->pUnit->dwUnitType == UNIT_MISSILE)
		{
			COLLISION_ResetMaskWithSize(pDynamicPath->pRoom, pDynamicPath->tGameCoords.wPosX, pDynamicPath->tGameCoords.wPosY, pDynamicPath->dwUnitSize, pDynamicPath->nFootprintCollisionMask);
		}
		else
		{
			COLLISION_ResetMaskWithPattern(pDynamicPath->pRoom, pDynamicPath->tGameCoords.wPosX, pDynamicPath->tGameCoords.wPosY, pDynamicPath->dwCollisionPattern, pDynamicPath->nFootprintCollisionMask);
		}
	}

	pDynamicPath->nFootprintCollisionMask = nCollisionMask;

	if (pDynamicPath->pRoom)
	{
		if (pDynamicPath->pUnit && pDynamicPath->pUnit->dwUnitType == UNIT_MISSILE)
		{
			COLLISION_SetMaskWithSize(pDynamicPath->pRoom, pDynamicPath->tGameCoords.wPosX, pDynamicPath->tGameCoords.wPosY, pDynamicPath->dwUnitSize, nCollisionMask);
		}
		else
		{
			COLLISION_SetMaskWithPattern(pDynamicPath->pRoom, pDynamicPath->tGameCoords.wPosX, pDynamicPath->tGameCoords.wPosY, pDynamicPath->dwCollisionPattern, nCollisionMask);
		}
	}
}

//D2Common.0x6FDAA0C0 (#10183)
// 1.13c D2Common+0x348C0
int __stdcall PATH_GetMoveTestCollisionMask(D2DynamicPathStrc* pDynamicPath)
{
	return pDynamicPath->nMoveTestCollisionMask;
}

//D2Common.0x6FDAA0D0 (#10184)
// 1.13c D2Common+0x348B0
void __stdcall PATH_SetMoveTestCollisionMask(D2DynamicPathStrc* pDynamicPath, int nCollisionMask)
{
	pDynamicPath->nMoveTestCollisionMask = nCollisionMask;
}

//D2Common.0x6FDAA0E0 (#10185)
// 1.13c D2Common+0x34F80
void __stdcall PATH_SetType(D2DynamicPathStrc* pDynamicPath, int nPathType)
{

	D2_ASSERT(!(pDynamicPath->pUnit && pDynamicPath->pUnit->dwUnitType == UNIT_PLAYER && nPathType == PATHTYPE_TOWARD));

	int nFlag = gaPathTypeFlags_6FDD2088[nPathType];
	if ((nFlag & PATH_UNKNOWN_FLAG_0x02000) && !(pDynamicPath->dwFlags & PATH_UNKNOWN_FLAG_0x04000))
	{
		pDynamicPath->dwPrevPathType = pDynamicPath->dwPathType;
	}

	if ((nFlag & PATH_WITH_PREVIOUS_VELOCITY) && !(pDynamicPath->dwFlags & PATH_UNKNOWN_FLAG_0x10000))
	{
		pDynamicPath->nPreviousVelocity = pDynamicPath->dwVelocity;
	}

	pDynamicPath->dwPathType = nPathType;
	pDynamicPath->dwFlags = nFlag | pDynamicPath->dwFlags & 0xFFF800FF;
	pDynamicPath->nDirOffset = gaOffsetForPathType[nPathType];

	D2_ASSERT(pDynamicPath->dwPrevPathType != PATHTYPE_KNOCKBACK_CLIENT);
	D2_ASSERT(pDynamicPath->dwPrevPathType != PATHTYPE_KNOCKBACK_SERVER);
	D2_ASSERT((nPathType != PATHTYPE_MISSILE) || (pDynamicPath->nDistMax < D2DynamicPathStrc::MAXPATHLEN));
}

//D2Common.0x6FDAA1E0 (#10186)
// 1.13c D2Common+0x35210
void __stdcall PATH_ResetToPreviousType(D2DynamicPathStrc* pDynamicPath)
{
	D2_ASSERT(pDynamicPath->pUnit);

	if (pDynamicPath->dwFlags & PATH_WITH_PREVIOUS_VELOCITY)
	{
		pDynamicPath->dwVelocity = pDynamicPath->nPreviousVelocity;
	}

	if (pDynamicPath->pUnit->dwUnitType != UNIT_PLAYER)
	{
		if (pDynamicPath->dwFlags & PATH_UNKNOWN_FLAG_0x02000)
		{
			PATH_SetType(pDynamicPath, pDynamicPath->dwPrevPathType);
		}
	}
	else
	{
		PATH_SetType(pDynamicPath, PATHTYPE_STRAIGHT);
	}
}

//D2Common.0x6FDAA240 (#10187)
// 1.13c D2Common+0x348A0
int __stdcall PATH_GetType(D2DynamicPathStrc* pDynamicPath)
{
	return pDynamicPath->dwPathType;
}

//D2Common.0x6FDAA250 (#10190)
// 1.13c D2Common+0x34880
// TODO: rename
void __stdcall D2COMMON_10190_PATH_SetDistance(D2DynamicPathStrc* pDynamicPath, uint8_t nDistance)
{
	pDynamicPath->nDist = nDistance;
}

//D2Common.0x6FDAA270 (#10191)
// TODO: rename
// 1.13c D2Common+0x34870
uint8_t __stdcall D2COMMON_10191_PATH_GetDistance(D2DynamicPathStrc* pDynamicPath)
{
	return pDynamicPath->nDist;
}

// 1.13c D2Common+0x34F50
//D2Common.0x6FDAA280 (#10188)
void __stdcall PATH_SetNewDistance(D2DynamicPathStrc* pDynamicPath, uint8_t nNewDistance)
{
	if (pDynamicPath)
	{
		if (nNewDistance >= (D2DynamicPathStrc::MAXPATHLEN - 1))
		{
			nNewDistance = (D2DynamicPathStrc::MAXPATHLEN - 1);
		}

		pDynamicPath->nDistMax = nNewDistance;
		pDynamicPath->nDist = nNewDistance;
	}
}

//D2Common.0x6FDAA2B0 (#10189)
// 1.13c D2Common+0x34860
uint8_t __stdcall PATH_GetMaxDistance(D2DynamicPathStrc* pDynamicPath)
{
	return pDynamicPath->nDistMax;
}

//D2Common.0x6FDAA2C0 (#10201)
// 1.13c D2Common+0x35BA0
//TODO: Find a name
uint16_t __stdcall D2Common_10201(D2DynamicPathStrc* pDynamicPath)
{
	if (!pDynamicPath->dwVelocity)
	{
		pDynamicPath->nCollidedWithMask = COLLISION_CheckMaskWithSize(pDynamicPath->pRoom, pDynamicPath->tGameCoords.wPosX, pDynamicPath->tGameCoords.wPosY, pDynamicPath->dwUnitSize, (uint16_t)~COLLIDE_CORPSE);
	}

	return pDynamicPath->nCollidedWithMask;
}

//D2Common.0x6FDAA300 (#10202)
// 1.13c D2Common+0x34850
//TODO: Find a name
uint16_t __stdcall D2Common_10202(D2DynamicPathStrc* pDynamicPath)
{
	return pDynamicPath->nCollidedWithMask;
}

//D2Common.0x6FDAA310 (#10192)
// 1.13c D2Common+0x34810 (#10359): retail asserts dwPathType == PATHTYPE_IDASTAR (1.10f source had it inverted)
void __stdcall PATH_SetIDAStarInitFScore(D2DynamicPathStrc* pDynamicPath, int nIDAStarInitFScore)
{
	D2_ASSERT(pDynamicPath->dwPathType == PATHTYPE_IDASTAR);

	pDynamicPath->nIDAStarInitFScore = nIDAStarInitFScore;
}

//D2Common.0x6FDAA350 (#10198)
//TODO: Check name
// 1.13c D2Common+0x347C0
int __stdcall D2COMMON_10198_PathGetSaveStep(D2DynamicPathStrc* pDynamicPath, D2PathPointStrc** ppPathPoints)
{
	D2_ASSERT(pDynamicPath->dwFlags & PATH_SAVE_STEPS_MASK);

	*ppPathPoints = pDynamicPath->SavedSteps;
	return pDynamicPath->nSavedStepsCount;
}

//D2Common.0x6FDAA390 (#10199)
// 1.13c D2Common+0x34770
//TODO: Check name
int __stdcall D2COMMON_10199_PathGetSaveX(D2DynamicPathStrc* pDynamicPath)
{
	D2_ASSERT(pDynamicPath->dwFlags & PATH_SAVE_STEPS_MASK);

	if (pDynamicPath->nSavedStepsCount)
	{
		return pDynamicPath->SavedSteps[pDynamicPath->nSavedStepsCount -1].X;
	}

	return 0;
}

//D2Common.0x6FDAA3E0 (#10200)
// 1.13c D2Common+0x34720
//TODO: Check name
int __stdcall D2COMMON_10200_PathGetSaveY(D2DynamicPathStrc* pDynamicPath)
{
	D2_ASSERT(pDynamicPath->dwFlags & PATH_SAVE_STEPS_MASK);

	if (pDynamicPath->nSavedStepsCount)
	{
		return pDynamicPath->SavedSteps[pDynamicPath->nSavedStepsCount - 1].Y;
	}

	return 0;
}

//D2Common.0x6FDAA430 (#10203)
// 1.13c D2Common+0x346F0
//TODO: Check name
void __stdcall D2COMMON_10203_PATH_SetRotateFlag(D2DynamicPathStrc* pDynamicPath, BOOL bReset)
{
	if (pDynamicPath)
	{
		if (bReset)
		{
			pDynamicPath->dwFlags &= (~PATH_UNKNOWN_FLAG_0x00040);
		}
		else
		{
			pDynamicPath->dwFlags |= PATH_UNKNOWN_FLAG_0x00040;
		}
	}
}

//D2Common.0x6FDAA460 (#10204)
//TODO: Check name
// 1.13c D2Common+0x346D0
void __stdcall D2COMMON_10204_PATH_ClearPoint2(D2DynamicPathStrc* pDynamicPath)
{
	pDynamicPath->tPrevTargetCoord.X = 0;
	pDynamicPath->tPrevTargetCoord.Y = 0;
}

//D2Common.0x6FDAA480 (#10205)
// 1.13c D2Common+0x346A0
void __stdcall PATH_SetStepNum(D2DynamicPathStrc* pDynamicPath, uint8_t nSteps)
{
	if (pDynamicPath)
	{
		if (nSteps > 0 && nSteps < PATH_MAX_STEPNUM)
		{
			pDynamicPath->nStepNum = nSteps - 1;
		}
		else
		{
			pDynamicPath->nStepNum = 0;
		}
	}
}

//D2Common.0x6FDAA4B0 (#10206)
// 1.13c D2Common+0x34690
int __stdcall PATH_GetStepNum(D2DynamicPathStrc* pDynamicPath)
{
	return pDynamicPath->nStepNum + 1;
}

//D2Common.0x6FDAA4C0 (#10207)
//TODO: Find a name
// 1.13c D2Common+0x34670
void __stdcall D2Common_10207(D2DynamicPathStrc* pDynamicPath, char a2, char a3)
{
	pDynamicPath->unk0x67 = a2;
	pDynamicPath->dwSpeed = a3;
}

//D2Common.0x6FDAA4E0 (#10217)
// 1.13c D2Common+0x34630
void __stdcall PATH_SetDistance(D2DynamicPathStrc* pDynamicPath, int nDist)
{
	D2_ASSERT(nDist >= 0 && nDist <= 255);

	pDynamicPath->nDistance = nDist;
}

//D2Common.0x6FDAA520 (#10218)
// 1.13c D2Common+0x34620
int __stdcall PATH_GetDistance(D2DynamicPathStrc* pDynamicPath)
{
	return pDynamicPath->nDistance;
}

// 1.13c D2Common+0x345E0
//D2Common.0x6FDAA530 (#10219)
void __stdcall PATH_AddToDistance(D2DynamicPathStrc* pDynamicPath, int nAddition)
{
	int nDistance = nAddition + pDynamicPath->nDistance;
	pDynamicPath->nDistance = D2Clamp(nDistance, 0, 0xFF);
}

// 1.13c D2Common+0x345D0
//D2Common.0x6FDAA570 (#10210)
int __stdcall PATH_GetUnitCollisionPattern(D2UnitStrc* pUnit)
{
	return pUnit->pDynamicPath->dwCollisionPattern;
}

//D2Common.0x6FDAA580 (#10211)
// 1.13c D2Common+0x345B0
void __stdcall PATH_SetUnitCollisionPattern(D2UnitStrc* pUnit, int nCollisionPattern)
{
	pUnit->pDynamicPath->dwCollisionPattern = nCollisionPattern;
}

// 1.13c D2Common+0x34EE0
//D2Common.0x6FDAA5A0 (#10212)
//TODO: Check name
void __stdcall D2COMMON_10212_PATH_SetMoveFlags(D2UnitStrc* pUnit, BOOL bSet)
{
	D2DynamicPathStrc* pDynamicPath = pUnit->pDynamicPath;

	D2_ASSERT(pDynamicPath);

	if (bSet)
	{
		pUnit->dwFlagEx |= UNITFLAGEX_UNK_PATH_RELATED;
		pDynamicPath->unk0x38 = 0;
	}
	else
	{
		pUnit->dwFlagEx &= (~UNITFLAGEX_UNK_PATH_RELATED);
		pDynamicPath->unk0x38 = 0;
	}
}

//D2Common.0x6FDAA600 (#10213)
//TODO: Find a name
// 1.13c D2Common+0x36650
// 1.13c: the counter at path+0x38 is compared signed; the return is the word retail leaves in EAX
// (the async bit, the ex-flag bit, the path pointer, or the last callee's result). A null unit
// leaves the caller's EAX: returned as nEntryEax.
int32_t __stdcall D2Common_10213(D2UnitStrc* pUnit, int32_t nEntryEax)
{
	if (!pUnit)
	{
		return nEntryEax;
	}
	if (pUnit->dwFlags & UNITFLAG_ISASYNC)
	{
		return 1;
	}
	if (!(pUnit->dwFlagEx & UNITFLAGEX_UNK_PATH_RELATED))
	{
		return 0;
	}
	D2DynamicPathStrc* pDynamicPath = pUnit->pDynamicPath;
	D2_ASSERT(pDynamicPath);

	++pDynamicPath->unk0x38;

	if (static_cast<int32_t>(pDynamicPath->unk0x38) <= 1)
	{
		return static_cast<int32_t>(reinterpret_cast<uintptr_t>(pDynamicPath));
	}
	pDynamicPath->unk0x38 = 0;
	int32_t nResult = D2Common_10227(pUnit);
	if (nResult)
	{
		return nResult;
	}
	nResult = D2Common_10236(pUnit, pDynamicPath->pTargetUnit != nullptr);
	if (!nResult)
	{
		return 0;
	}
	return UNITS_ChangeAnimMode(pUnit, 2);
}

//D2Common.0x6FDAA6A0 (#10220)
// 1.13c D2Common+0x34580
int __stdcall PATH_ComputeSquaredDistance(int nX1, int nY1, int nX2, int nY2)
{
	const int deltaX = (nX2 - nX1);
	const int deltaY = (nY2 - nY1);
	return deltaX * deltaX + deltaY * deltaY;
}

//D2Common.0x6FDAA6D0 (#10221)
// 1.13c D2Common+0x35b80
void __stdcall PATH_AddCollisionFootprintForOptionalUnit(D2UnitStrc* pUnit)
{
	if (pUnit)
	{
		PATH_AddCollisionFootprintForUnit(pUnit);
	}
}

//D2Common.0x6FDAA6F0 (#10237)
//TODO: Find a name
// 1.13c D2Common+0x34550
BOOL __stdcall D2Common_10237(D2UnitStrc* pUnit)
{
	if (pUnit && pUnit->pDynamicPath)
	{
		return (pUnit->pDynamicPath->dwCurrentPointIdx < pUnit->pDynamicPath->dwPathPoints);
	}

	return FALSE;
}
