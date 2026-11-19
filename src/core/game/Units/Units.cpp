#include "Units/Units.h"

#include "D2Collision.h"
#include "D2Composit.h"
#include "D2DataTbls.h"
#include "Drlg/D2DrlgMaze.h"
#include "D2Dungeon.h"
#include "D2Inventory.h"
#include "D2Items.h"
#include "D2Monsters.h"
#include "Path/Path.h"
#include "Path/PathMisc.h"
#include "Path/IDAStar.h"
#include "D2QuestRecord.h"
#include "D2Seed.h"
#include "D2Skills.h"
#include "D2States.h"
#include "D2StatList.h"
#include "Units/UnitRoom.h"
#include "D2Waypoints.h"
#include <D2BitManip.h>
#include <DataTbls/LevelsIds.h>
#include <DataTbls/MonsterIds.h>
#include <DataTbls/ObjectsIds.h>
#include <D2Math.h>
#include <limits>
#include <cstddef>
#include "../../d2common/d2common_33110.h"   // d2-web: the 1.13c body of UNITS_UpdateAnimRateAndVelocity

extern D2InventoryGridInfoStrc gBodyLocInventoryGridInfo;   // D2Inventory.cpp; 1.13c DAT_6FDED378 = {13,1}

//D2Common.0x6FDBD520 (#10457)
// 1.13c D2Common+0x304E0
uint8_t __stdcall UNITS_GetDirection(D2UnitStrc* pUnit)
{
	D2_ASSERT(pUnit);

	if (pUnit->dwUnitType == UNIT_OBJECT || pUnit->dwUnitType == UNIT_ITEM)
	{
		return pUnit->pStaticPath->nDirection;
	}
	else
	{
		return PATH_GetDirection(pUnit->pDynamicPath);
	}
}

// 1.13c D2Common+0x304A0 (#10320)
D2SkillStrc* __stdcall UNITS_GetStartSkill(D2UnitStrc* pUnit)
{
	D2_ASSERT(pUnit);
	return SKILLS_GetFirstSkillFromSkillList(pUnit->pSkills);
}

// 1.13c D2Common+0x30460 (#10321)
D2SkillStrc* __stdcall UNITS_GetLeftSkill(D2UnitStrc* pUnit)
{
	D2_ASSERT(pUnit);
	D2_ASSERT(pUnit->pSkills);
	return pUnit->pSkills->pLeftSkill;
}

// 1.13c D2Common+0x30420 (#10322)
D2SkillStrc* __stdcall UNITS_GetRightSkill(D2UnitStrc* pUnit)
{
	D2_ASSERT(pUnit);
	D2_ASSERT(pUnit->pSkills);
	return pUnit->pSkills->pRightSkill;
}

// 1.13c D2Common+0x303E0 (#10324)
void __stdcall UNITS_SetUsedSkill(D2UnitStrc* pUnit, D2SkillStrc* pUsedSkill)
{
	D2_ASSERT(pUnit);
	if (pUnit->pSkills)
	{
		pUnit->pSkills->pUsedSkill = pUsedSkill;
	}
}

//1.10f: D2Common.0x6FDBD670 (#10323)
// 1.13c D2Common+0x303A0 (#10511)
D2SkillStrc* __stdcall UNITS_GetUsedSkill(D2UnitStrc* pUnit)
{
	D2_ASSERT(pUnit);
	return pUnit->pSkills ? pUnit->pSkills->pUsedSkill : NULL;
}

//D2Common.0x6FDBD6B0 (#11259)
// 1.13c D2Common+0x2F370
D2UnitStrc* __stdcall UNITS_AllocUnit(void* pMemPool, int nUnitType)
{
	D2UnitStrc* pUnit = D2_CALLOC_STRC_POOL(pMemPool, D2UnitStrc);

	pUnit->pMemoryPool = pMemPool;
	pUnit->dwUnitType = nUnitType;

	if (nUnitType == UNIT_MONSTER)
	{
		pUnit->pMonsterData = D2_CALLOC_STRC_POOL(pMemPool, D2MonsterDataStrc);
	}

	return pUnit;
}

// 1.13c D2Common+0x33990 (inlined STATLIST_FreeStatListEx and the ordinal renumbering are the only deltas)
//D2Common.0x6FDBD720 (#11260)
void __stdcall UNITS_FreeUnit(D2UnitStrc* pUnit)
{
	if (pUnit)
	{
		STATLIST_FreeStatListEx(pUnit);
		SKILLS_FreeSkillList(pUnit);

		if (pUnit->dwUnitType == UNIT_MONSTER)
		{
			D2_FREE_POOL(pUnit->pMemoryPool, pUnit->pMonsterData);
		}

		pUnit->dwUnitType = 6;
		pUnit->dwUnitId = -1;

		D2_FREE_POOL(pUnit->pMemoryPool, pUnit);
	}
}

//D2Common.0x6FDBD780 (#10327)
// 1.13c D2Common+0x30340
int __stdcall UNITS_GetPrecisionX(D2UnitStrc* pUnit)
{
	D2_ASSERT(pUnit);

	switch (pUnit->dwUnitType)
	{
	case UNIT_OBJECT:
	case UNIT_ITEM:
	case UNIT_TILE:
		return PATH_ToFP16Corner(pUnit->pStaticPath->tGameCoords.nX);

	default:
		// retail reads the dynamic path for every other type
		return PATH_GetPrecisionX(pUnit->pDynamicPath);
	}
}

//D2Common.0x6FDBD7D0 (#10330)
// 1.13c D2Common+0x302E0
int __stdcall UNITS_GetPrecisionY(D2UnitStrc* pUnit)
{
	D2_ASSERT(pUnit);

	switch (pUnit->dwUnitType)
	{
	case UNIT_OBJECT:
	case UNIT_ITEM:
	case UNIT_TILE:
		return PATH_ToFP16Corner(pUnit->pStaticPath->tGameCoords.nY);

	default:
		// retail reads the dynamic path for every other type
		return PATH_GetPrecisionY(pUnit->pDynamicPath);
	}
}

//1.10f: D2Common.0x6FDBD820 (#10328)
// 1.13c D2Common+0x2F2C0
void __stdcall UNITS_SetXForStaticUnit(D2UnitStrc* pUnit, int nX)
{
	D2_ASSERT(pUnit);

	switch (pUnit->dwUnitType)
	{
	case UNIT_OBJECT:
	case UNIT_ITEM:
	case UNIT_TILE:
		pUnit->pStaticPath->tGameCoords.nX = nX;
		break;

	default:
		D2_UNREACHABLE;
	}
}

//1.10f: D2Common.0x6FDBD890 (#10331)
// 1.13c D2Common+0x2F270
void __stdcall UNITS_SetYForStaticUnit(D2UnitStrc* pUnit, int nY)
{
	D2_ASSERT(pUnit);

	switch (pUnit->dwUnitType)
	{
	case UNIT_OBJECT:
	case UNIT_ITEM:
	case UNIT_TILE:
		pUnit->pStaticPath->tGameCoords.nY = nY;
		break;

	default:
		D2_UNREACHABLE;
	}
}

//D2Common.0x6FDBD900 (#10336)
// 1.13c D2Common+0x30F80
int __stdcall UNITS_GetUnitSizeX(D2UnitStrc* pUnit)
{
	D2MonStats2Txt* pMonStats2TxtRecord = NULL;
	D2MissilesTxt* pMissilesTxtRecord = NULL;

	if (pUnit)
	{
		switch (pUnit->dwUnitType)
		{
		case UNIT_PLAYER:
			return COLLISION_UNIT_SIZE_SMALL;

		case UNIT_MONSTER:
			pMonStats2TxtRecord = UNITS_GetMonStats2TxtRecordFromMonsterId(pUnit->dwClassId);
			if (pMonStats2TxtRecord)
			{
				return (int8_t)pMonStats2TxtRecord->nSizeX;
			}
			return COLLISION_UNIT_SIZE_NONE;

		case UNIT_OBJECT:
			return pUnit->pObjectData->pObjectTxt->dwSizeX;

		case UNIT_ITEM:
			return COLLISION_UNIT_SIZE_POINT;

		case UNIT_MISSILE:
			pMissilesTxtRecord = DATATBLS_GetMissilesTxtRecord(pUnit->dwClassId);
			if (pMissilesTxtRecord)
			{
				return pMissilesTxtRecord->nSize;
			}
			return COLLISION_UNIT_SIZE_NONE;

		default:
			return COLLISION_UNIT_SIZE_NONE;
		}
	}
	
	return COLLISION_UNIT_SIZE_NONE;
}

//D2Common.0x6FDBDA00 (#10337)
// 1.13c D2Common+0x30EE0
int __stdcall UNITS_GetUnitSizeY(D2UnitStrc* pUnit)
{
	D2MonStats2Txt* pMonStats2TxtRecord = NULL;
	D2MissilesTxt* pMissilesTxtRecord = NULL;

	if (pUnit)
	{
		switch (pUnit->dwUnitType)
		{
		case UNIT_PLAYER:
			return 2;

		case UNIT_MONSTER:
			pMonStats2TxtRecord = UNITS_GetMonStats2TxtRecordFromMonsterId(pUnit->dwClassId);
			if (pMonStats2TxtRecord)
			{
				return (int8_t)pMonStats2TxtRecord->nSizeY;
			}
			return 0;

		case UNIT_OBJECT:
			return pUnit->pObjectData->pObjectTxt->dwSizeY;

		case UNIT_ITEM:
			return 1;

		case UNIT_MISSILE:
			pMissilesTxtRecord = DATATBLS_GetMissilesTxtRecord(pUnit->dwClassId);
			if (pMissilesTxtRecord)
			{
				return pMissilesTxtRecord->nSize;
			}
			return 0;

		default:
			return 0;
		}
	}

	return 0;
}

//D2Common.0x6FDBDB10 (#10333)
// 1.13c D2Common+0x30290
int __stdcall UNITS_GetClientCoordX(D2UnitStrc* pUnit)
{
	D2_ASSERT(pUnit);
	if (pUnit->dwUnitType == UNIT_OBJECT || pUnit->dwUnitType == UNIT_ITEM || pUnit->dwUnitType == UNIT_TILE)
	{
		return pUnit->pStaticPath->dwClientCoordX;
	}
	// Retail uses the dynamic-path layout for every other type, including invalid IDs.
	return PATH_GetClientCoordX(pUnit->pDynamicPath);
}

//D2Common.0x6FDBDB60 (#10334)
// 1.13c D2Common+0x30240
int __stdcall UNITS_GetClientCoordY(D2UnitStrc* pUnit)
{
	D2_ASSERT(pUnit);
	if (pUnit->dwUnitType == UNIT_OBJECT || pUnit->dwUnitType == UNIT_ITEM || pUnit->dwUnitType == UNIT_TILE)
	{
		return pUnit->pStaticPath->dwClientCoordY;
	}
	// Retail uses the dynamic-path layout for every other type, including invalid IDs.
	return PATH_GetClientCoordY(pUnit->pDynamicPath);
}

//D2Common.0x6FDBDBB0 (#10411)
// 1.13c D2Common+0x301D0
int __stdcall UNITS_GetAbsoluteXDistance(D2UnitStrc* pUnit1, D2UnitStrc* pUnit2)
{
	int nDistance = 0;
	int nX1 = 0;
	int nX2 = 0;

	switch (pUnit1->dwUnitType)
	{
	case UNIT_OBJECT:
	case UNIT_ITEM:
	case UNIT_TILE:
		nX1 = pUnit1->pStaticPath->tGameCoords.nX;
		break;

	default:
		if (pUnit1->pDynamicPath)
		{
			nX1 = PATH_GetXPosition(pUnit1->pDynamicPath);
		}
		else
		{
			nX1 = 0;
		}
		break;
	}

	switch (pUnit2->dwUnitType)
	{
	case UNIT_OBJECT:
	case UNIT_ITEM:
	case UNIT_TILE:
		nX2 = pUnit2->pStaticPath->tGameCoords.nX;
		break;

	default:
		if (pUnit2->pDynamicPath)
		{
			nX2 = PATH_GetXPosition(pUnit2->pDynamicPath);
		}
		else
		{
			nX2 = 0;
		}
		break;
	}

	nDistance = nX1 - nX2;
	if (nDistance < 0)
	{
		nDistance = -nDistance;
	}

	return nDistance;
}

//D2Common.0x6FDBDC20 (#10412)
// 1.13c D2Common+0x30160
int __stdcall UNITS_GetAbsoluteYDistance(D2UnitStrc* pUnit1, D2UnitStrc* pUnit2)
{
	int nDistance = 0;
	int nY1 = 0;
	int nY2 = 0;

	switch (pUnit1->dwUnitType)
	{
	case UNIT_OBJECT:
	case UNIT_ITEM:
	case UNIT_TILE:
		nY1 = pUnit1->pStaticPath->tGameCoords.nY;
		break;

	default:
		if (pUnit1->pDynamicPath)
		{
			nY1 = PATH_GetYPosition(pUnit1->pDynamicPath);
		}
		else
		{
			nY1 = 0;
		}
		break;
	}

	switch (pUnit2->dwUnitType)
	{
	case UNIT_OBJECT:
	case UNIT_ITEM:
	case UNIT_TILE:
		nY2 = pUnit2->pStaticPath->tGameCoords.nY;
		break;

	default:
		if (pUnit2->pDynamicPath)
		{
			nY2 = PATH_GetYPosition(pUnit2->pDynamicPath);
		}
		else
		{
			nY2 = 0;
		}
		break;
	}

	nDistance = nY1 - nY2;
	if (nDistance < 0)
	{
		nDistance = -nDistance;
	}

	return nDistance;
}

//D2Common.0x6FDBDC90 (#10340)
// 1.13c D2Common+0x30120
void __stdcall UNITS_SetTargetX(D2UnitStrc* pUnit, int nTargetX)
{
	D2_ASSERT(pUnit);

	pUnit->pDynamicPath->dwClientCoordX = nTargetX;
}

// 1.13c D2Common+0x300E0
//D2Common.0x6FDBDCD0 (#10341)
void __stdcall UNITS_SetTargetY(D2UnitStrc* pUnit, int nTargetY)
{
	D2_ASSERT(pUnit);

	pUnit->pDynamicPath->dwClientCoordY = nTargetY;
}

//D2Common.0x6FDBDD10 (#10332)
// 1.13c D2Common+0x30050
void __stdcall UNITS_GetCoords(D2UnitStrc* pUnit, D2CoordStrc* pCoord)
{
	D2_ASSERT(pUnit);

	switch (pUnit->dwUnitType)
	{
	case UNIT_OBJECT:
	case UNIT_ITEM:
	case UNIT_TILE:
		pCoord->nX = pUnit->pStaticPath->tGameCoords.nX;
		pCoord->nY = pUnit->pStaticPath->tGameCoords.nY;
		break;

	default:
		if (pUnit->pDynamicPath)
		{
			pCoord->nX = PATH_GetXPosition(pUnit->pDynamicPath);
			pCoord->nY = PATH_GetYPosition(pUnit->pDynamicPath);
		}
		else
		{
			pCoord->nX = 0;
			pCoord->nY = 0;
		}
		break;
	}
}

//D2Common.0x6FDBDDA0 (#10335)
// 1.13c D2Common+0x2FFE0
void __stdcall UNITS_GetClientCoords(D2UnitStrc* pUnit, D2CoordStrc* pClientCoords)
{
	D2_ASSERT(pUnit);

	switch (pUnit->dwUnitType)
	{
	case UNIT_OBJECT:
	case UNIT_ITEM:
	case UNIT_TILE:
		pClientCoords->nX = pUnit->pStaticPath->dwClientCoordX;
		pClientCoords->nY = pUnit->pStaticPath->dwClientCoordY;
		break;

	default:
		pClientCoords->nX = PATH_GetClientCoordX(pUnit->pDynamicPath);
		pClientCoords->nY = PATH_GetClientCoordY(pUnit->pDynamicPath);
		break;
	}
}

//D2Common.0x6FDBDE10 (#10338)
// 1.13c D2Common+0x2FF30
int __fastcall UNITS_GetCollisionMask(D2UnitStrc* pUnit)
{
	D2_ASSERT(pUnit);

	switch (pUnit->dwUnitType)
	{
	case UNIT_OBJECT:
		if (pUnit->pObjectData->pObjectTxt->nIsDoor)
		{
			if (pUnit->pObjectData->pObjectTxt->nBlocksVis)
			{
				return COLLIDE_MASK_DOOR_BLOCK_VIS;
			}
			else if(pUnit->pObjectData->pObjectTxt->nBlockMissile)
			{
				return COLLIDE_DOOR | COLLIDE_MISSILE_BARRIER;
			}
			else
			{
				return COLLIDE_OBJECT;
			}
		}
		else
		{
			if (pUnit->pObjectData->pObjectTxt->nSubClass & OBJSUBCLASS_TOWNPORTAL)
			{
				return COLLIDE_CORPSE;
			}
			else if (pUnit->pObjectData->pObjectTxt->nBlockMissile)
			{
				return COLLIDE_OBJECT | COLLIDE_MISSILE_BARRIER;
			}
			else
			{
				return COLLIDE_OBJECT;
			}
		}

	case UNIT_ITEM:
		return COLLIDE_ITEM;

	case UNIT_TILE:
		return COLLIDE_WALL;

	default:
		return PATH_GetFootprintCollisionMask(pUnit->pDynamicPath);
	}
}

//D2Common.0x6FDBDEC0 (#10352)
// 1.13c D2Common+0x318C0
void __stdcall UNITS_FreeCollisionPath(D2UnitStrc* pUnit)
{
	D2CoordStrc pCoords = {};

	UNITS_GetCoords(pUnit, &pCoords);

	COLLISION_ResetMaskWithSizeXY(UNITS_GetRoom(pUnit), pCoords.nX, pCoords.nY, UNITS_GetUnitSizeX(pUnit), UNITS_GetUnitSizeY(pUnit), UNITS_GetCollisionMask(pUnit));
}

//D2Common.0x6FDBE060 (#10351)
// 1.13c D2Common+0x31850
void __stdcall UNITS_BlockCollisionPath(D2UnitStrc* pUnit, D2ActiveRoomStrc* pRoom, int nX, int nY)
{
	int nSizeX = 0;
	int nSizeY = 0;

	D2_ASSERT(pUnit);

	nSizeX = UNITS_GetUnitSizeX(pUnit);
	nSizeY = UNITS_GetUnitSizeY(pUnit);

	if (nSizeX && nSizeY)
	{
		COLLISION_SetMaskWithSizeXY(pRoom, nX, nY, nSizeX, nSizeY, UNITS_GetCollisionMask(pUnit));
	}
}

//D2Common.0x6FDBE1A0 (#10350)
// 1.13c D2Common+0x2FED0
void __stdcall UNITS_InitializeStaticPath(D2UnitStrc* pUnit, D2ActiveRoomStrc* pRoom, int nX, int nY)
{
	D2_ASSERT(pUnit);

	pUnit->pStaticPath->nDirection = 0;
	pUnit->pStaticPath->tGameCoords.nX = nX;
	pUnit->pStaticPath->tGameCoords.nY = nY;
	pUnit->pStaticPath->pRoom = pRoom;

	DUNGEON_GameSubtileToClientCoords(&nX, &nY);

	pUnit->pStaticPath->dwClientCoordX = nX;
	pUnit->pStaticPath->dwClientCoordY = nY;
}

//1.10f: D2Common.0x6FDBE210 (#10343)
// 1.13c D2Common+0x2FE70
void __stdcall UNITS_ResetRoom(D2UnitStrc* pUnit)
{
	D2_ASSERT(pUnit);

	switch (pUnit->dwUnitType)
	{
	case UNIT_OBJECT:
	case UNIT_ITEM:
	case UNIT_TILE:
		pUnit->pStaticPath->pRoom = nullptr;
		break;

	default:
		if (pUnit->pDynamicPath)
		{
			PATH_SetRoom(pUnit->pDynamicPath, nullptr);
		}
		break;
	}
}

//1.10f: D2Common.0x6FDBE270 (#10342)
// 1.13c D2Common+0x2FE10 (#10331)
D2ActiveRoomStrc* __stdcall UNITS_GetRoom(D2UnitStrc* pUnit)
{
	D2_ASSERT(pUnit);

	switch (pUnit->dwUnitType)
	{
	case UNIT_OBJECT:
	case UNIT_ITEM:
	case UNIT_TILE:
		return pUnit->pStaticPath->pRoom;
	default:
		if (pUnit->pDynamicPath)
		{
			return PATH_GetRoom(pUnit->pDynamicPath);
		}
		return nullptr;
	}
}

//D2Common.0x6FDBE2D0 (#10344)
// 1.13c D2Common+0x30E80
void __stdcall UNITS_SetTargetUnitForDynamicUnit(D2UnitStrc* pUnit, D2UnitStrc* pTargetUnit)
{
	D2_ASSERT(pUnit);
	D2_ASSERT((pUnit->dwUnitType == UNIT_PLAYER) || (pUnit->dwUnitType == UNIT_MONSTER) || (pUnit->dwUnitType == UNIT_MISSILE));
	PATH_SetTargetUnit(pUnit->pDynamicPath, pTargetUnit);
}

//D2Common.0x6FDBE330 (#10345)
// 1.13c D2Common+0x2FDB0
int __stdcall UNITS_GetTargetTypeFromDynamicUnit(D2UnitStrc* pUnit)
{
	int nUnitType = 0;
	D2UnitGUID nUnitId = 0;
	D2_ASSERT(pUnit);
	D2_ASSERT((pUnit->dwUnitType == UNIT_PLAYER) || (pUnit->dwUnitType == UNIT_MONSTER) || (pUnit->dwUnitType == UNIT_MISSILE));

	PATH_GetTargetTypeAndGUID(pUnit->pDynamicPath, &nUnitType, &nUnitId);
	return nUnitType;
}

//D2Common.0x6FDBE3A0 (#10346)
// 1.13c D2Common+0x2FD50
D2UnitGUID __stdcall UNITS_GetTargetGUIDFromDynamicUnit(D2UnitStrc* pUnit)
{
	int nUnitType = 0;
	D2UnitGUID nUnitId = 0;
	D2_ASSERT(pUnit);
	D2_ASSERT((pUnit->dwUnitType == UNIT_PLAYER) || (pUnit->dwUnitType == UNIT_MONSTER) || (pUnit->dwUnitType == UNIT_MISSILE));

	PATH_GetTargetTypeAndGUID(pUnit->pDynamicPath, &nUnitType, &nUnitId);
	return nUnitId;
}

//D2Common.0x6FDBE410 (#10347)
// 1.13c D2Common+0x30E20
void __stdcall UNITS_SetTargetUnitForPlayerOrMonster(D2UnitStrc* pUnit, D2UnitStrc* pTargetUnit)
{
	D2_ASSERT(pUnit);
	D2_ASSERT((pUnit->dwUnitType == UNIT_PLAYER) || (pUnit->dwUnitType == UNIT_MONSTER));

	PATH_SetTargetUnit(pUnit->pDynamicPath, pTargetUnit);
}

//D2Common.0x6FDBE470 (#10354)
// 1.13c D2Common+0x2FD10
void __stdcall UNITS_GetRunAndWalkSpeedForPlayer(int nUnused, int nCharId, int* pWalkSpeed, int* pRunSpeed)
{
	D2_MAYBE_UNUSED(nUnused);
	if (nCharId >= 0 && nCharId < sgptDataTables->nCharStatsTxtRecordCount)
	{
		*pWalkSpeed = sgptDataTables->pCharStatsTxt[nCharId].nWalkSpeed;
		*pRunSpeed = sgptDataTables->pCharStatsTxt[nCharId].nRunSpeed;
	}
}

//1.10f: D2Common.0x6FDBE4C0 (#10325)
//1.13c: D2Common.0x6FD82670 (#11122)
void __stdcall UNITS_SetAnimData(D2UnitStrc* pUnit, int nUnitType, int nClassId, int nMode)
{
	D2_ASSERT(pUnit);
	pUnit->pAnimData = DATATBLS_GetAnimDataRecord(pUnit, nClassId, nMode, nUnitType, pUnit->pInventory);
}

//Helper function
// 1.13c D2Common+0x32E10 (EAX = nClassId, [esp+4] = pUnit, ret 4). 1.13c delta: a null unit allocates from no pool with
// owner type 6, owner id -1 (1.10f dereferenced it); the post that follows still needs a unit, as in retail.
void __stdcall UNITS_CharacterStartRunningOrKnockback(D2UnitStrc* pUnit, int nClassId)
{
	D2StatListStrc* pStatList = pUnit ? STATLIST_AllocStatList(pUnit->pMemoryPool, STATLIST_TEMPONLY, 0, pUnit->dwUnitType, pUnit->dwUnitId)
	                                  : STATLIST_AllocStatList(nullptr, STATLIST_TEMPONLY, 0, 6, (D2UnitGUID)-1);
	if (D2CharStatsTxt* pCharStatsTxtRecord = UNITS_GetCharStatsTxtRecord(nClassId))
	{
		if (pCharStatsTxtRecord->nWalkSpeed != 0)
		{
			STATLIST_SetStat(pStatList, STAT_VELOCITYPERCENT, 100 * pCharStatsTxtRecord->nRunSpeed / pCharStatsTxtRecord->nWalkSpeed - 100, 0);
			D2COMMON_10475_PostStatToStatList(pUnit, pStatList, TRUE);
		}
	}
}

//D2Common.0x6FDBE510 (#10349)
extern int64_t d2common_allmul(uint32_t lo1, int32_t hi1, uint32_t lo2, int32_t hi2);	// 1.13c D2Common+0x3750

// 1.13c D2Common+0x335F0
void __stdcall UNITS_SetAnimStartFrame(D2UnitStrc* pUnit)
{
	D2_ASSERT(pUnit);

	pUnit->nActionFrame = 0;
	pUnit->nSeqCurrentFramePrecise = UNITS_GetFrameBonus(pUnit) << 8;

	int nUnitType = pUnit->dwUnitType;
	int nClassId = pUnit->dwClassId;
	int nNewMode = pUnit->dwAnimMode;

	D2COMMON_11013_ConvertMode(pUnit, &nUnitType, &nClassId, &nNewMode, __FILE__, __LINE__);

	switch (nUnitType)
	{
	case UNIT_PLAYER:
	{
		if (nNewMode == PLRMODE_RUN || nNewMode == PLRMODE_KNOCKBACK)
		{
			UNITS_CharacterStartRunningOrKnockback(pUnit, nClassId);
		}
		if (nNewMode == PLRMODE_SEQUENCE)
		{
			UNITS_InitializeSequence(pUnit);
			UNITS_UpdateAnimRateAndVelocity(pUnit, __FILE__, __LINE__);
		}
		else
		{
			pUnit->pAnimSeq = nullptr;
			UNITS_UpdateAnimRateAndVelocity(pUnit, __FILE__, __LINE__);
			pUnit->dwFrameCountPrecise = pUnit->pAnimData->dwFrames << 8;
		}
		break;
	}
	case UNIT_MONSTER:
	{
		if (pUnit->dwUnitType == UNIT_PLAYER && STATES_IsUnitShapeShifted(pUnit) && (nNewMode == MONMODE_KNOCKBACK || nNewMode == MONMODE_RUN))
		{
			UNITS_CharacterStartRunningOrKnockback(pUnit, pUnit->dwClassId);
		}
		if (nNewMode == MONMODE_SEQUENCE)
		{
			UNITS_InitializeSequence(pUnit);
			UNITS_UpdateAnimRateAndVelocity(pUnit, __FILE__, __LINE__);
		}
		else
		{
			UNITS_UpdateAnimRateAndVelocity(pUnit, __FILE__, __LINE__);
			pUnit->pAnimSeq = nullptr;
			pUnit->dwFrameCountPrecise = pUnit->pAnimData->dwFrames << 8;
		}
		break;
	}
	case UNIT_OBJECT:
	{
#define NUM_OBJECTMODES 8
		D2_ASSERT(nNewMode < NUM_OBJECTMODES);

		D2ObjectsTxt* pObjectsTxtRecord = DATATBLS_GetObjectsTxtRecord(nClassId);
		pUnit->dwFrameCountPrecise = pObjectsTxtRecord->dwFrameCnt[nNewMode];
		pUnit->nSeqCurrentFramePrecise = uint16_t(pObjectsTxtRecord->nStart[nNewMode]) << 8;
		int16_t wFrameDelta = pObjectsTxtRecord->wFrameDelta[nNewMode];

		if (pObjectsTxtRecord->nSync)
		{
			pUnit->wAnimSpeed = wFrameDelta;
		}
		else
		{
			// 1.13c rolls over delta / 16 (1.10f: delta / 8); SEED_RollLimitedRandomNumber returns 0 for a limit <= 0 without drawing
			const int nRollLimit = wFrameDelta >> 4;
			const int uRandom = SEED_RollLimitedRandomNumber(&pUnit->pSeed, nRollLimit);
			const int newAnimSpeed = uRandom + wFrameDelta - nRollLimit;
			pUnit->wAnimSpeed = static_cast<int16_t>(D2Clamp(newAnimSpeed, 0, 0x7FFF));
		}
		break;
	}
	case UNIT_MISSILE:
	{
		if (D2MissilesTxt* pMissilesTxtRecord = DATATBLS_GetMissilesTxtRecord(nClassId))
		{
			// 1.13c's three-way scale of the signed anim rate over 1024; a short never reaches the 64-bit branch, which
			// retail still carries and which calls the CRT multiply (D2Common+0x3750).
			const int32_t nRate = (int16_t)pMissilesTxtRecord->wAnimRate;
			if (nRate < 0x10001)
			{
				pUnit->wAnimSpeed = (int16_t)((nRate * 0x100 + ((nRate * 0x100 >> 31) & 0x3FF)) >> 10);
			}
			else if ((nRate & ~0xF) < 0x4000)
			{
				pUnit->wAnimSpeed = (int16_t)(d2common_allmul((uint32_t)nRate, nRate >> 31, 0x100, 0) / 0x400);
			}
			else
			{
				pUnit->wAnimSpeed = (int16_t)(((nRate + ((nRate >> 31) & 0x3FF)) >> 10) << 8);
			}
			pUnit->dwFrameCountPrecise = pMissilesTxtRecord->nAnimLen << 8;
		}
		break;
	}
	case UNIT_ITEM:
	{
		if (nNewMode == IMODE_ONGROUND)
		{
			pUnit->wAnimSpeed = 0;
			pUnit->dwFrameCountPrecise = 4352;
			pUnit->nSeqCurrentFramePrecise = 4096;
		}
		else if (nNewMode == IMODE_DROPPING)
		{
			pUnit->wAnimSpeed = 256;
			pUnit->dwFrameCountPrecise = 4352;
		}
		break;
	}
	default:
		break;
	}
}

//D2Common.0x6FDBEA60 (#10348)
// 1.13c D2Common+0x33920
BOOL __stdcall UNITS_ChangeAnimMode(D2UnitStrc* pUnit, int nMode)
{
	if (pUnit && pUnit->dwUnitType != UNIT_TILE)
	{
		if (pUnit->dwAnimMode != nMode)
		{
			UNITROOM_RefreshUnit(pUnit);
			pUnit->dwAnimMode = nMode;
			pUnit->dwFlags |= UNITFLAG_DOUPDATE;
			STATLIST_DeactivateTemporaryStates(pUnit);
			UNITS_SetAnimStartFrame(pUnit);
			return TRUE;
		}
		// Force refresh unless the unit is a resting (neutral) monster
		if (!(pUnit->dwUnitType == UNIT_MONSTER && pUnit->dwAnimMode == MONMODE_NEUTRAL))
		{
			UNITROOM_RefreshUnit(pUnit);
			pUnit->dwFlags |= UNITFLAG_DOUPDATE;
		}
	}

	return FALSE;
}

//D2Common.0x6FDBEAD0 (#10355)
//TODO: Find a name
// 1.13c D2Common+0x2FCC0
int __stdcall UNITS_IsCurrentRoomInvalid(D2UnitStrc* pUnit)
{
	D2_ASSERT(pUnit);

	if (pUnit->dwUnitType == UNIT_OBJECT || pUnit->dwUnitType == UNIT_ITEM)
	{
		return pUnit->pStaticPath->bRoomNeedsUpdate;
	}
	else
	{
		return PATH_IsCurrentRoomInvalid(pUnit->pDynamicPath);
	}
}

//D2Common.0x6FDBEB20 (#10356)
//TODO: Find a name
// 1.13c D2Common+0x2FC50
void __stdcall UNITS_SetCurrentRoomInvalid(D2UnitStrc* pUnit, int a2)
{
	D2_ASSERT(pUnit);

	switch (pUnit->dwUnitType)
	{
	case UNIT_OBJECT:
	case UNIT_ITEM:
	case UNIT_TILE:
		pUnit->pStaticPath->bRoomNeedsUpdate = a2 != 0;
		return;

	case UNIT_PLAYER:
	case UNIT_MONSTER:
	case UNIT_MISSILE:
		PATH_SetCurrentRoomInvalid(pUnit->pDynamicPath, a2);
		return;

	default:
		return;
	}
}

//D2Common.0x6FDBEB80 (#10357)
// 1.13c D2Common+0x30DC0
void __stdcall UNITS_RefreshInventory(D2UnitStrc* pUnit, BOOL bSetFlag)
{
	if (bSetFlag)
	{
		UNITROOM_RefreshUnit(pUnit);
	}

	if (pUnit)
	{
		if (!bSetFlag)
		{
			pUnit->dwFlagEx &= ~UNITFLAGEX_HASINV;
			return;
		}
		else
		{
			pUnit->dwFlagEx |= UNITFLAGEX_HASINV;
		}
	}

	if (bSetFlag && pUnit && pUnit->dwUnitType == UNIT_PLAYER)
	{
		pUnit->dwFlagEx |= UNITFLAGEX_UPDATEINV;
	}
}

//D2Common.0x6FDBEBE0 (#10409)
// 1.13c D2Common+0x2FB60
int __stdcall UNITS_GetInventoryRecordId(D2UnitStrc* pUnit, int nInvPage, BOOL bLoD)
{
	static const int nInventoryRecordMappings[NUMBER_OF_PLAYERCLASSES] =
	{
		INVENTORYRECORD_AMAZON,
		INVENTORYRECORD_SORCERESS,
		INVENTORYRECORD_NECROMANCER,
		INVENTORYRECORD_PALADIN,
		INVENTORYRECORD_BARBARIAN,
		INVENTORYRECORD_DRUID,
		INVENTORYRECORD_ASSASSIN
	};
	D2_ASSERT(pUnit);

	if (pUnit->dwUnitType == UNIT_PLAYER)
	{
		switch ((uint8_t)nInvPage)
		{
		case INVPAGE_EQUIP:
			return INVENTORYRECORD_TRADE_PAGE_1;

		case INVPAGE_TRADE:
			return INVENTORYRECORD_TRADE_PAGE_2;

		case INVPAGE_STASH:
			return bLoD != 0 ? INVENTORYRECORD_BIG_BANK_PAGE_1 : INVENTORYRECORD_BANK_PAGE_1;

		case INVPAGE_CUBE:
			return INVENTORYRECORD_TRANSMOGRIFY_BOX_PAGE_1;

		default:
			if (pUnit->dwClassId >= 0 && pUnit->dwClassId < ARRAY_SIZE(nInventoryRecordMappings))
			{
				return nInventoryRecordMappings[pUnit->dwClassId];
			}
			return -1;
		}
	}
	else if (pUnit->dwUnitType == UNIT_MONSTER)
	{
		return INVENTORYRECORD_MONSTER;
	}
	else if (pUnit->dwUnitType == UNIT_OBJECT)
	{
		if (pUnit->dwClassId == OBJECT_GUILD_VAULT)
		{
			return INVENTORYRECORD_GUILD_VAULT_PAGE_1;
		}
		else if (pUnit->dwClassId == OBJECT_TROPHY_CASE)
		{
			return INVENTORYRECORD_TROPHY_CASE_PAGE_1;
		}
	}

	return -1;
}

// 1.13c D2Common+0x2F1E0
//D2Common.0x6FDBECD0 (#10383)
D2GfxLightStrc* __stdcall UNITS_ResetLightMap(D2UnitStrc* pUnit)
{
	D2_ASSERT(pUnit);

	D2GfxLightStrc* pLight = pUnit->pLight;
	pUnit->pLight = nullptr;

	return pLight;
}

// 1.13c D2Common+0x2F1B0
//D2Common.0x6FDBED10 (#10369)
int __stdcall UNITS_GetAnimOrSeqMode(D2UnitStrc* pUnit)
{
	if (pUnit)
	{
		if (UNITS_GetAnimSeq(pUnit))
		{
			return pUnit->dwSeqMode;
		}
		else
		{
			return pUnit->dwAnimMode;
		}
	}

	return 0;
}

// 1.13c D2Common+0x2F160 (#10199 in 1.13c; #10370 in 1.10f)
void __stdcall UNITS_SetAnimOrSeqMode(D2UnitStrc* pUnit, int nAnimMode)
{
	D2_ASSERT(pUnit);

	if (UNITS_GetAnimSeq(pUnit))
	{
		pUnit->dwSeqMode = nAnimMode;
	}
	else
	{
		pUnit->dwAnimMode = nAnimMode;
	}
}

//D2Common.0x6FDBED90 (#10371)
// 1.13c D2Common+0x32820 (the 0x2E840 record reads and the 0x2F100 sequence update with old frame 0 are inlined here)
void __stdcall UNITS_InitializeSequence(D2UnitStrc* pUnit)
{
	if (pUnit)
	{
		pUnit->pAnimSeq = DATATBLS_GetMonSeqTxtRecordFromUnit(pUnit);
		if (pUnit->pAnimSeq)
		{
			pUnit->dwSeqFrameCount = DATATBLS_GetSeqFramePointsCount(pUnit);
			pUnit->dwSeqFrame = 0;
			pUnit->dwSeqSpeed = 256;
			pUnit->dwFrameCountPrecise = DATATBLS_GetSeqFrameCount(pUnit) << 8;

			unsigned nMode = 0, nFrame = 0;
			int nDirection = 0, nEvent = 0;
			DATATBLS_ComputeSequenceAnimation(pUnit->pAnimSeq, pUnit->dwSeqFrame, 0, &nMode, &nFrame, &nDirection, &nEvent);

			pUnit->dwSeqMode = nMode;
			pUnit->nActionFrame = nEvent;
			pUnit->nSeqCurrentFramePrecise = nFrame << 8;
			pUnit->dwFlags |= UNITFLAG_SQGFXCHANGE;
		}
	}
}

//D2Common.0x6FDBEE20 (#10372)
// 1.13c D2Common+0x2F0C0
void __stdcall UNITS_SetAnimationFrame(D2UnitStrc* pUnit, int nFrame)
{
	D2_ASSERT(pUnit);

	pUnit->dwSeqFrame = (nFrame << 8) + (pUnit->dwSeqFrame & 0xFF);
}

//D2Common.0x6FDBEE60 (#10373)
// 1.13c D2Common+0x32460
void __stdcall UNITS_StopSequence(D2UnitStrc* pUnit)
{
	pUnit->nActionFrame = 0;

	if (pUnit->pAnimSeq)
	{
		const unsigned nOldFrame = pUnit->dwSeqFrame;

		pUnit->dwFlags &= ~UNITFLAG_SQGFXCHANGE;
		pUnit->dwSeqFrame += pUnit->dwSeqSpeed;

		if (pUnit->dwSeqFrame >= pUnit->dwSeqFrameCount)
		{
			pUnit->dwSeqFrame -= pUnit->dwSeqFrameCount;
		}

		pUnit->dwFrameCountPrecise -= pUnit->dwSeqSpeed;

		unsigned nOldMode = pUnit->dwSeqMode;
		unsigned nNewMode, nFrame;
		int nDirection, nEvent;
		DATATBLS_ComputeSequenceAnimation(pUnit->pAnimSeq, pUnit->dwSeqFrame, nOldFrame, &nNewMode, &nFrame, &nDirection, &nEvent);

		pUnit->dwSeqMode = nNewMode;
		pUnit->nSeqCurrentFramePrecise = nFrame << 8;
		pUnit->nActionFrame = nEvent;

		if (nNewMode != nOldMode)
		{
			pUnit->dwFlags |= UNITFLAG_SQGFXCHANGE;
		}
	}
	else
	{
		int32_t nFrame = pUnit->nSeqCurrentFramePrecise >> 8;
		if (pUnit->wAnimSpeed >= 256)
		{
			nFrame++;
		}

		pUnit->nSeqCurrentFramePrecise += pUnit->wAnimSpeed;
		
		while (pUnit->nSeqCurrentFramePrecise >= pUnit->dwFrameCountPrecise)
		{
			const int32_t nFrameCount = (pUnit->dwFrameCountPrecise >> 8);
			for(int i = nFrame; i < nFrameCount; i++)
			{
				UNITS_SetAnimActionFrame(pUnit, i);
			}
			// Animation looped
			nFrame = 0;
			pUnit->nSeqCurrentFramePrecise -= pUnit->dwFrameCountPrecise;
			pUnit->nSeqCurrentFramePrecise += (UNITS_GetFrameBonus(pUnit) << 8);
		}

		const int32_t nLastFrameReached = (pUnit->nSeqCurrentFramePrecise >> 8);
		for (; nFrame <= nLastFrameReached; nFrame++)
		{
			UNITS_SetAnimActionFrame(pUnit, nFrame);
		}
	}
}

// 1.13c D2Common+0x2F090
//D2Common.0x6FDBEFF0 (#10374)
void __stdcall UNITS_UpdateFrame(D2UnitStrc* pUnit)
{
	pUnit->nSeqCurrentFramePrecise += pUnit->wAnimSpeed;

	if (pUnit->nSeqCurrentFramePrecise >= pUnit->dwFrameCountPrecise)
	{
		pUnit->nSeqCurrentFramePrecise -= pUnit->dwFrameCountPrecise;
	}
}

// 1.13c D2Common+0x2F060 (#10023)
void __stdcall D2COMMON_10375_UNITS_SetFrameNonRate(D2UnitStrc* pUnit, int nRate, int nFailRate)
{
	pUnit->nSeqCurrentFramePrecise += pUnit->wAnimSpeed;

	if (pUnit->nSeqCurrentFramePrecise >= (nFailRate << 8))
	{
		pUnit->nSeqCurrentFramePrecise -= ((nFailRate << 8) - (nRate << 8));
	}
}

struct D2AnimUnkStrc
{
	BOOL bCanUseVelocityModifierByPassiveSkill;
	BOOL bCanUseVelocityModifier;
	BOOL bCanUseSkillAttackRate;
	BOOL bCanUseAttackRate;
	BOOL bAllowAnimSpeed;
};

//1.10f: 0x6FDD2C28
//1.13c: 0x6FDDC5F8
static D2AnimUnkStrc gaPlayerModesAnimModulators[NUMBER_OF_PLRMODES] = {
	{ 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 1, 0, 0, 0 },
	{ 0, 1, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 1, 0, 0, 0 },
	{ 0, 0, 0, 1, 0 },
	{ 0, 0, 0, 1, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 1, 0 },
	{ 0, 0, 0, 1, 0 },
	{ 1, 0, 1, 0, 0 },
	{ 1, 0, 1, 0, 0 },
	{ 1, 0, 1, 0, 0 },
	{ 1, 0, 1, 0, 0 },
	{ 0, 0, 0, 0, 1 },
	{ 1, 0, 1, 0, 0 },
	{ 0, 0, 0, 0, 0 }
};

//1.10f: 0x6FDD2DB8
//1.13c: 0x6FDDC788
D2AnimUnkStrc gaMonModesAnimModulators[16] = {
	{ 0, 0, 0, 0, 1},
	{ 0, 0, 0, 0, 0},
	{ 0, 1, 0, 0, 0},
	{ 0, 0, 0, 0, 0},
	{ 0, 0, 0, 1, 0},
	{ 0, 0, 0, 1, 0},
	{ 0, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 0},
	{ 1, 1, 1, 0, 0},
	{ 1, 1, 1, 0, 0},
	{ 1, 1, 1, 0, 0},
	{ 1, 1, 1, 0, 0},
	{ 0, 0, 0, 0, 1},
	{ 0, 0, 0, 0, 0},
	{ 1, 0, 1, 0, 0},
	{ 0, 1, 0, 0, 0},
};

//1.10f: 0x6FDD2EF8
//1.13c: 0x6FDDC8C8
static D2AnimUnkStrc gaMonModesAnimModulatorsExpansion[NUMBER_OF_MONMODES] = {
	{ 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 1, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 1, 0 },
	{ 0, 0, 0, 1, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 1, 0, 1, 0, 0 },
	{ 1, 0, 1, 0, 0 },
	{ 1, 0, 1, 0, 0 },
	{ 1, 0, 1, 0, 0 },
	{ 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 0, 0 },
	{ 1, 0, 1, 0, 0 },
	{ 0, 1, 0, 0, 0 }
};

// The three animation-modulator tables as the retail layout holds them (player, monster, expansion monster), for the
// replay probes that read them through their 0x14-byte rows.
extern "C" const void* UNITS_GetAnimModulatorTable(int nWhich)
{
	switch (nWhich)
	{
	case 0: return gaPlayerModesAnimModulators;
	case 1: return gaMonModesAnimModulators;
	case 2: return gaMonModesAnimModulatorsExpansion;
	}
	return nullptr;
}

struct D2UnitAnimModeStatBasePercentageStrc
{
	BOOL bHasBasePercentage;
	int32_t nBasePercentage;
	int32_t nStatId;
};

// D2C_UnitAnimModeStats is declared in Units.h (the 0x323E0 probe calls UNITS_GetAnimStatBasePercentage).
//1.10f: Inlined
//1.13c: 0x6FDE4608
D2UnitAnimModeStatBasePercentageStrc gaAnimModeStatBasePercentage[5] = {
	{TRUE, 120, STAT_ITEM_FASTERATTACKRATE},
	{TRUE, 120, STAT_ITEM_FASTERGETHITRATE},
	{TRUE, 120, STAT_ITEM_FASTERCASTRATE},
	{TRUE, 120, STAT_ITEM_FASTERBLOCKRATE},
	{TRUE, 150, STAT_ITEM_FASTERMOVEVELOCITY},
};

//1.10f: Inlined
// 1.13c D2Common+0x323E0 (stat row in EAX, unit on the stack)
int UNITS_GetAnimStatBasePercentage(D2C_UnitAnimModeStats nStat, D2UnitStrc* pUnit)
{
	const int nStatId = gaAnimModeStatBasePercentage[nStat].nStatId;
	const int nValue = pUnit->pStatListEx ? STATLIST_UnitGetItemStatOrSkillStatValue(pUnit, nStatId, 0) : 0;

	if (gaAnimModeStatBasePercentage[nStat].bHasBasePercentage && nValue)
	{
		return (nValue * gaAnimModeStatBasePercentage[nStat].nBasePercentage)
			/ (nValue + gaAnimModeStatBasePercentage[nStat].nBasePercentage);
	}
	return nValue;
}

//1.10f: Inlined
// 1.13c D2Common+0x2FB10
int UNITS_GetBaseAnimSpeed(int nUnitType, int nClassId, int nAnimMode)
{
	switch (nUnitType)
	{
	case UNIT_PLAYER:
		if (nAnimMode == PLRMODE_RUN)
		{
			return 101;
		}
		else
		{
			return 213;
		}
	case UNIT_MONSTER:
	{
		const D2MonStatsTxt* pMonStatsTxtRecord = DATATBLS_GetMonStatsTxtRecord(nClassId);
		if (nAnimMode == MONMODE_RUN)
		{
			return pMonStatsTxtRecord->nRunAnimSpeed;
		}
		else
		{
			return pMonStatsTxtRecord->nWalkAnimSpeed;
		}
	}
	default:
		return 0;
	}
}

//1.10f: Inlined
// 1.13c D2Common+0x30D50
int UNITS_GetBaseVelocity(int nUnitType, int nClassId)
{
	if (nUnitType == UNIT_PLAYER)
	{
		return UNITS_GetCharStatsTxtRecord(nClassId)->nWalkSpeed << 8;
	}
	else if (nUnitType == UNIT_MONSTER)
	{
		return DATATBLS_GetMonStatsTxtRecord(nClassId)->nVelocity << 8;
	}
	return 0;
}

//1.10f: Inlined
// 1.13c D2Common+0x30BD0 (returns the raw masked flag byte, not a BOOL: see the skill branch)
BOOL UNITS_CanAnimModeUseAttackRate(int nUnitType, int nAnimMode, D2UnitStrc* pUnit, int nClassId)
{
	D2_ASSERT((nUnitType == UNIT_PLAYER) || (nUnitType == UNIT_MONSTER));
	D2AnimUnkStrc* pAnimModeModulators = nullptr;
	switch (nUnitType)
	{
	case UNIT_PLAYER:
		pAnimModeModulators = &gaPlayerModesAnimModulators[nAnimMode];
		break;
	case UNIT_MONSTER:
		if (nClassId >= MONSTER_FIRST_EXPANSION)
		{
			pAnimModeModulators = &gaMonModesAnimModulatorsExpansion[nAnimMode];
		}
		else
		{
			pAnimModeModulators = &gaMonModesAnimModulators[nAnimMode];
		}
		break;

	default:
		return FALSE;

	}
	if (pAnimModeModulators->bCanUseAttackRate)
	{
		return TRUE;
	}

	if (pAnimModeModulators->bCanUseSkillAttackRate)
	{
		// Sanitizer-flagged: upstream dereferenced
		// UNITS_GetUsedSkill(pUnit)->pSkillsTxt unconditionally -- a unit changing into such a mode
		// with no used skill (a town NPC via AITHINK_Fn032_Npc; s000c walking into the Great Marsh
		// trapped this) read through a null D2SkillStrc. Retail 1.13c D2Common 0x6FD80BD0 (Ghidra):
		// `call 0x6FD803A0 [UNITS_GetUsedSkill]; test eax,eax; jne <continue>` else `xor eax,eax;
		// ret 8` -- retail guards the skill pointer and returns FALSE for "no used skill", it does
		// not fault. D2SkillStrc::pSkillsTxt is only ever set from a looked-up D2SkillsTxt record
		// (D2Skills.cpp), so a null pSkillsTxt on an existing skill is the same "not a real skill"
		// case and gets the same guard.
		D2SkillStrc* pUsedSkill = UNITS_GetUsedSkill(pUnit);
		if (!pUsedSkill || !pUsedSkill->pSkillsTxt)
		{
			return FALSE;
		}

		D2SkillsTxt* pSkillsRecord = DATATBLS_GetSkillsTxtRecord(pUsedSkill->pSkillsTxt->nSkillId);
		// 1.13c returns the masked byte itself (record +6 & bit 3), not a BOOL
		return reinterpret_cast<const uint8_t*>(pSkillsRecord)[6] & gdwBitMasks[3];
	}

	return FALSE;
}

//1.10f: Inlined
// 1.13c D2Common+0x2EF90
BOOL UNITS_IsAnimModeBlocking(int nUnitType, int nAnimMode)
{
	switch (nUnitType)
	{
	case UNIT_PLAYER:
		return nAnimMode == PLRMODE_BLOCK;
	case UNIT_MONSTER:
		return nAnimMode == MONMODE_BLOCK;
	default:
		return FALSE;

	}
}

//1.10f: Inlined
// 1.13c D2Common+0x30B80
BOOL UNITS_IsSeqAnimSpeedModulatedByFCR(D2UnitStrc* pUnit, int nUnitType, int nAnimMode)
{
	switch (nUnitType)
	{
	case UNIT_PLAYER:

		if (nAnimMode == PLRMODE_CAST)
		{
			return TRUE;
		}
		if (nAnimMode == PLRMODE_SEQUENCE)
		{
			if (pUnit)
			{
				if (D2SkillStrc* pUsedSkill = UNITS_GetUsedSkill(pUnit))
				{
					if (D2SkillsTxt* pSkillsTxtRecord = DATATBLS_GetSkillsTxtRecord(pUsedSkill->pSkillsTxt->nSkillId))
					{
						return pSkillsTxtRecord->nSeqTrans == PLRMODE_CAST;
					}
				}
			}
		}
		return FALSE;
	case UNIT_MONSTER:
		return nAnimMode == MONMODE_CAST;
	default:
		return FALSE;

	}
}

//1.10f: Inlined
// 1.13c D2Common+0x2EFC0
BOOL UNITS_IsAnimModeGetHit(int nUnitType, int nAnimMode)
{
	// d2-web: the 1.10f source tested the BLOCK modes here (a copy of UNITS_IsAnimModeBlocking), so a get-hit ran
	// at the full base rate instead of base * (FHR + 50) / 100; s212 captured retail's 128 against our 256.
	switch (nUnitType)
	{
	case UNIT_PLAYER:
		return nAnimMode == PLRMODE_GETHIT;
	case UNIT_MONSTER:
		return nAnimMode == MONMODE_GETHIT;
	default:
		return FALSE;

	}
}

//1.10f: Inlined
// 1.13c D2Common+0x2F010
BOOL UNITS_IsAnimModeKnockBack(int nUnitType, int nAnimMode)
{
	switch (nUnitType)
	{
	case UNIT_PLAYER:
		return nAnimMode == PLRMODE_KNOCKBACK;
	case UNIT_MONSTER:
		return nAnimMode == MONMODE_KNOCKBACK;
	default:
		return FALSE;

	}
}

//1.10f: Inlined
// 1.13c D2Common+0x2FAC0
BOOL UNITS_AnimModeAllowsAnimSpeed(int nUnitType, int nAnimMode, int nClassId)
{
	switch (nUnitType)
	{
	case UNIT_PLAYER:
		return gaPlayerModesAnimModulators[nAnimMode].bAllowAnimSpeed;
	case UNIT_MONSTER:
		if (nClassId >= MONSTER_FIRST_EXPANSION)
		{
			return gaMonModesAnimModulatorsExpansion[nAnimMode].bAllowAnimSpeed;
		}
		else
		{
			return gaMonModesAnimModulators[nAnimMode].bAllowAnimSpeed;
		}
	default:
		return FALSE;
	}
}


//1.10f: Inlined
// 1.13c D2Common+0x30C70
BOOL UNITS_CanAnimModeUseVelocityModifier(int nUnitType, int nAnimMode, D2UnitStrc* pUnit, int nClassId)
{
	D2_ASSERT((nUnitType == UNIT_PLAYER) || (nUnitType == UNIT_MONSTER));
	D2AnimUnkStrc* pAnimModeModulators = nullptr;
	switch (nUnitType)
	{
	case UNIT_PLAYER:
		pAnimModeModulators = &gaPlayerModesAnimModulators[nAnimMode];
		break;
	case UNIT_MONSTER:

		if (D2MonStatsTxt* pMonStatsTxtRecord = DATATBLS_GetMonStatsTxtRecord(nClassId))
		{
			if (pMonStatsTxtRecord->dwMonStatsFlags & MONSTATSFLAG_NPC)
			{
				return nAnimMode == MONMODE_WALK || nAnimMode == MONMODE_RUN;
			}
		}

		if (nClassId >= MONSTER_FIRST_EXPANSION)
		{
			pAnimModeModulators = &gaMonModesAnimModulatorsExpansion[nAnimMode];
		}
		else
		{
			pAnimModeModulators = &gaMonModesAnimModulators[nAnimMode];
		}
		break;

	default:
		return FALSE;

	}  
	
	if (pAnimModeModulators->bCanUseVelocityModifier)
	{
		return TRUE;
	}

	if (pAnimModeModulators->bCanUseVelocityModifierByPassiveSkill)
	{
		if (D2SkillStrc* pUsedSkill = UNITS_GetUsedSkill(pUnit))
		{
			return (pUsedSkill->dwFlags & SKFLAG_PASSIVE) != 0 && (pUsedSkill->dwFlags & SKFLAG_STUN) == 0;
		}
	}
	return FALSE;
}


//1.10f: D2Common.0x6FDBF050 (#10376)
//1.13c: D2Common.0x6FD83110 (#10819)
// 1.13c D2Common+0x33110: the body is the 1.13c port in d2common/d2common_33110.cpp; this binds its callees to the 1.10f source
// functions. 1.13c deltas: no trace on a null unit, bad type or null path; cast, block, get-hit, attack and the stat-rate
// "other" path divide the rate product unsigned (a negative product saturates to 0x7FFF); the holy-shield test guards the
// raw unit type and the states.txt count inline; pAnimData is read as SetAnimData left it.
// The callee table of UNITS_UpdateAnimRateAndVelocity; the client rate update copies it with its own seams.
const D2Common33110Env& UNITS_AnimRateEnv()
{
	static const D2Common33110Env kEnv = {
		[](D2UnitStrc* u, int* t, int* c, int* m) { D2COMMON_11013_ConvertMode(u, t, c, m, __FILE__, __LINE__); },
		[](D2UnitStrc* u, int t, int c, int m) { UNITS_SetAnimData(u, t, c, m); },
		[](D2UnitStrc* u, int t, int m) -> int { return UNITS_IsSeqAnimSpeedModulatedByFCR(u, t, m); },
		[](int n, D2UnitStrc* u) -> int { return UNITS_GetAnimStatBasePercentage((D2C_UnitAnimModeStats)n, u); },
		[](int t, int m) -> int { return UNITS_IsAnimModeBlocking(t, m); },
		[](D2UnitStrc* u, int n) -> int { return STATES_CheckState(u, n); },
		[](int t, int m) -> int { return UNITS_IsAnimModeGetHit(t, m); },
		[](int t, int m) -> int { return UNITS_IsAnimModeKnockBack(t, m); },
		[](int t, int c, int m) -> int { return UNITS_GetBaseAnimSpeed(t, c, m); },
		[](D2DynamicPathStrc* p, int v) { PATH_SetVelocity(p, v, __FILE__, __LINE__); },
		[](int t, int m, D2UnitStrc* u, int c) -> int { return UNITS_CanAnimModeUseVelocityModifier(t, m, u, c); },
		[](D2UnitStrc* u, int n, int l) -> int { return STATLIST_UnitGetStatValue(u, n, (uint16_t)l); },
		[](int t, int c) -> int { return UNITS_GetBaseVelocity(t, c); },
		[](int t, int m, D2UnitStrc* u, int c) -> int { return UNITS_CanAnimModeUseAttackRate(t, m, u, c); },
		[](D2UnitStrc* u, int b) { UNITS_MergeDualWieldWeaponStatLists(u, b); },
		[](D2UnitStrc* u) -> int { return UNITS_CanDualWield(u); },
		[](D2InventoryStrc* i, int l) { return INVENTORY_GetItemFromBodyLoc(i, l); },
		[](D2UnitStrc* i) -> int { return ITEMS_CanBeEquipped(i); },
		[](D2UnitStrc* i, int n) -> int { return ITEMS_CheckItemTypeId(i, n); },
		[](D2UnitStrc* u, int n) -> int { return STATES_CheckStateMaskOnUnit(u, n); },
		[](D2UnitStrc* u) -> int { return D2Common_11043(u); },
		[](int t, int m, int c) -> int { return UNITS_AnimModeAllowsAnimSpeed(t, m, c); },
	};
	return kEnv;
}

// 1.13c D2Common+0x33110 (body: d2common/d2common_33110.cpp)
void __stdcall UNITS_UpdateAnimRateAndVelocity(D2UnitStrc* pUnit, const char* szFile, int nLine)
{
	(void)szFile; (void)nLine;
	D2Common_33110_UpdateAnimRate(pUnit, UNITS_AnimRateEnv());
}

// 1.13c D2Common+0x30B40
//D2Common.0x6FDBF8D0 (#10377)
void __stdcall UNITS_SetAnimationSpeed(D2UnitStrc* pUnit, int nSpeed)
{
	pUnit->wAnimSpeed = static_cast<int16_t>(D2Clamp<int>(nSpeed, 0, std::numeric_limits<int16_t>::max()));
}

//D2Common.0x6FDBF910 (#10378)
// 1.13c D2Common+0x2EF30
int __stdcall UNITS_IsAtEndOfFrameCycle(D2UnitStrc* pUnit)
{
	D2_ASSERT(pUnit);
	
	if (pUnit->pAnimSeq)
	{
		return (int)pUnit->dwFrameCountPrecise <= 0;
	}
	else
	{
		return (int)(pUnit->nSeqCurrentFramePrecise + pUnit->wAnimSpeed) >= (int)pUnit->dwFrameCountPrecise;
	}
}

//D2Common.0x6FDBF970 (#10379)
// 1.13c D2Common+0x2EEC0
void __stdcall UNITS_GetShiftedFrameMetrics(D2UnitStrc* pUnit, int* pFrameNo, int* pFrameCount)
{
	D2_ASSERT(pUnit);

	if (pUnit->dwUnitType == UNIT_OBJECT)
	{
		*pFrameNo = pUnit->nSeqCurrentFramePrecise >> 8;
		*pFrameCount = pUnit->pObjectData->pObjectTxt->dwFrameCnt[pUnit->dwAnimMode];
	}
	else
	{
		*pFrameNo = pUnit->nSeqCurrentFramePrecise >> 8;
		*pFrameCount = pUnit->dwFrameCountPrecise >> 8;
	}
}

//D2Common.0x6FDBF9E0 (#10380)
// 1.13c D2Common+0x2EE60
void __stdcall UNITS_GetFrameMetrics(D2UnitStrc* pUnit, int* pFrame, int* pFrameCount)
{
	if (pUnit)
	{
		if (pUnit->dwUnitType == UNIT_OBJECT)
		{
			*pFrame = pUnit->nSeqCurrentFramePrecise;
			*pFrameCount = pUnit->pObjectData->pObjectTxt->dwFrameCnt[pUnit->dwAnimMode];
		}
		else
		{
			*pFrame = pUnit->nSeqCurrentFramePrecise;
			*pFrameCount = pUnit->dwFrameCountPrecise;
		}
	}
	else
	{
		*pFrame = 0;
		*pFrameCount = 0;
	}
}

//D2Common.0x6FDBFA40 (#10381)
// 1.13c D2Common+0x2EE10
void __stdcall UNITS_SetAnimActionFrame(D2UnitStrc* pUnit, int nFrame)
{
	if (pUnit && pUnit->pAnimData)
	{
		if (nFrame < 144)
		{
			if (pUnit->pAnimData->pFrameFlags[nFrame] == 1)
			{
				pUnit->nActionFrame = 1;
			}
			else if (pUnit->pAnimData->pFrameFlags[nFrame] == 2)
			{
				pUnit->nActionFrame = 2;
			}
			else if (pUnit->pAnimData->pFrameFlags[nFrame] == 3)
			{
				pUnit->nActionFrame = 3;
			}
			else if (pUnit->pAnimData->pFrameFlags[nFrame] == 4)
			{
				pUnit->nActionFrame = 4;
			}
		}
	}
}

//D2Common.0x6FDBFA90 (#10382)
// 1.13c D2Common+0x32770 (validated, no delta: unit type, mode 0x12 / 0x0E, frame bound 0x8F, sequence record and compute)
int __stdcall UNITS_GetEventFrameInfo(D2UnitStrc* pUnit, int nFrame)
{
	D2_ASSERT(pUnit);

	if (nFrame >= 144)
	{
		return 0;
	}

	BOOL bSequence = FALSE;
	switch (pUnit->dwUnitType)
	{
	case UNIT_PLAYER:
		bSequence = (pUnit->dwAnimMode == PLRMODE_SEQUENCE);
		break;
	case UNIT_MONSTER:
		break;
	default:
		return pUnit->pAnimData->pFrameFlags[nFrame];
	}

	if (pUnit->dwAnimMode != MONMODE_SEQUENCE && !bSequence)
	{
		return pUnit->pAnimData->pFrameFlags[nFrame];
	}

	unsigned int nMode = 0;
	unsigned int nTemp = 0;
	int nDirection = 0;
	int nEvent = 0;
	DATATBLS_ComputeSequenceAnimation(DATATBLS_GetMonSeqTxtRecordFromUnit(pUnit), nFrame << 8, nFrame << 8, &nMode, &nTemp, &nDirection, &nEvent);
	return nEvent;
}

// 1.13c D2Common+0x2EDE0
//D2Common.0x6FDBFB40 (#10410)
BOOL __stdcall UNITS_HasCollision(D2UnitStrc* pUnit)
{
	if (pUnit->dwUnitType == UNIT_OBJECT)
	{
		return pUnit->pObjectData->pObjectTxt->nHasCollision[pUnit->dwAnimMode];
	}

	return TRUE;
}

//D2Common.0x6FDBFB70 (#10358)
// 1.13c D2Common+0x30AC0
D2SkillStrc* __stdcall UNITS_GetSkillFromSkillId(D2UnitStrc* pUnit, int nSkillId)
{
	D2SkillsTxt* pSkillsTxtRecord = DATATBLS_GetSkillsTxtRecord(nSkillId);

	D2_ASSERT(pSkillsTxtRecord);
	D2_ASSERT(pUnit);

	D2SkillStrc* pSkill = SKILLS_GetFirstSkillFromSkillList(pUnit->pSkills);
	while (pSkill && pSkill->pSkillsTxt != pSkillsTxtRecord)
	{
		pSkill = pSkill->pNextSkill;
	}
	return pSkill;
}

//1.10f: D2Common.0x6FDBFC10 (#10392)
//1.13c: D2Common.0x6FD7EDA0 (#11020)
BOOL __stdcall UNITS_IsDoor(D2UnitStrc* pUnit)
{
	D2_ASSERT(pUnit);

	if (pUnit->dwUnitType == UNIT_OBJECT)
	{
		return pUnit->pObjectData->pObjectTxt->nIsDoor;
	}

	return FALSE;
}

//D2Common.0x6FDBFC90 (#10393)
// 1.13c D2Common+0x2ed30
BOOL __stdcall UNITS_IsShrine(D2UnitStrc* pUnit)
{
	if (pUnit->dwUnitType == UNIT_OBJECT && pUnit->pObjectData->pObjectTxt->nSubClass & OBJSUBCLASS_SHRINE)
	{
		return TRUE;
	}

	return FALSE;
}

//1.10f: D2Common.0x6FDBFCB0 (#10394)
// 1.13c D2Common+0x2ECF0
D2ObjectsTxt* __stdcall UNITS_GetObjectTxtRecordFromObject(D2UnitStrc* pUnit)
{
	D2_ASSERT(pUnit);

	if (pUnit->dwUnitType != UNIT_OBJECT)
	{
		FOG_DisplayHalt(0, __FILE__, __LINE__);
		exit(-1);
	}

	return pUnit->pObjectData->pObjectTxt;
}

//D2Common.0x6FDBFD00 (#10395)
// 1.13c D2Common+0x2ECB0 (fatal codes 0xE4E/0xE55; the read is identical)
D2ShrinesTxt* __stdcall UNITS_GetShrineTxtRecordFromObject(D2UnitStrc* pUnit)
{
	D2_ASSERT(pUnit);


	if (pUnit->dwUnitType != UNIT_OBJECT)
	{
		FOG_DisplayHalt(0, __FILE__, __LINE__);
		exit(-1);
	}

	return pUnit->pObjectData->pShrineTxt;
}

//D2Common.0x6FDBFD50 (#10396)
// 1.13c D2Common+0x2EC60: null/type guards use fatal codes 0xE60/0xE67; the store is identical.
void __stdcall UNITS_SetShrineTxtRecordInObjectData(D2UnitStrc* pUnit, D2ShrinesTxt* pShrinesTxtRecord)
{
	D2_ASSERT(pUnit);

	if (pUnit->dwUnitType != UNIT_OBJECT)
	{
		FOG_DisplayHalt(0, __FILE__, __LINE__);
		exit(-1);
	}

	pUnit->pObjectData->pShrineTxt = pShrinesTxtRecord;
}

//D2Common.0x6FDBFDB0 (#10413)
// 1.13c D2Common+0x317F0 (unit-level wrapper: same body, probe d2common_317f0)
void __stdcall UNITS_UpdateDirectionAndSpeed(D2UnitStrc* pUnit, int nX, int nY)
{
	if (pUnit->pDynamicPath)
	{
		D2Common_10216(pUnit->pDynamicPath, nX, nY, 0);
	}
}

//D2Common.0x6FDBFDD0 (#10414)
// 1.13c D2Common+0x2FA20 (fatal code 0xE83 on a null unit; the unit's own position is compared with the path target)
int __stdcall UNITS_GetNewDirection(D2UnitStrc* pUnit)
{
	int nX = 0;
	int nY = 0;
	D2_ASSERT(pUnit);

	if (pUnit->dwUnitType == UNIT_OBJECT || pUnit->dwUnitType == UNIT_ITEM || pUnit->dwUnitType == UNIT_TILE)
	{
		nX = pUnit->pStaticPath->tGameCoords.nX;
		nY = pUnit->pStaticPath->tGameCoords.nY;
	}
	else if (pUnit->pDynamicPath)
	{
		nX = pUnit->pDynamicPath->tGameCoords.wPosX;
		nY = pUnit->pDynamicPath->tGameCoords.wPosY;
	}

	if (pUnit->pDynamicPath->tTargetCoord.X == nX && pUnit->pDynamicPath->tTargetCoord.Y == nY)
	{
		return pUnit->pDynamicPath->nNewDirection;
	}

	return UNITS_GetDirectionToCoords(pUnit, pUnit->pDynamicPath->tTargetCoord.X, pUnit->pDynamicPath->tTargetCoord.Y);
}

//D2Common.0x6FDBFF20 (#10416)
// 1.13c D2Common+0x330F0 (forwards to the StoreOwnerInfo body)
void __stdcall UNITS_StoreOwnerTypeAndGUID(D2UnitStrc* pUnit, int nOwnerType, D2UnitGUID nOwnerId)
{
	UNITS_StoreOwnerInfo(pUnit, nOwnerType, nOwnerId);
}

//D2Common.0x6FDBFF40
// 1.13c D2Common+0x32CA0 (unit in EDI; the state toggle includes the state-count gate and the room refresh)
void __fastcall UNITS_StoreOwnerInfo(D2UnitStrc* pUnit, int nOwnerType, int nOwnerId)
{
	D2StatListStrc* pStatList = NULL;

	if (pUnit)
	{
		pUnit->dwOwnerType = nOwnerType;
		pUnit->dwOwnerGUID = nOwnerId;

		if (pUnit->pStatListEx)
		{
			STATES_ToggleState(pUnit, STATE_SOURCEUNIT, TRUE);
			pStatList = STATLIST_GetStatListFromUnitAndState(pUnit, STATE_SOURCEUNIT);
			if (!pStatList)
			{
				pStatList = STATLIST_AllocStatList(pUnit->pMemoryPool, 0, 0, pUnit->dwUnitType, pUnit->dwUnitId);
				if (!pStatList)
				{
					return;
				}

				STATLIST_SetState(pStatList, STATE_SOURCEUNIT);
				D2COMMON_10475_PostStatToStatList(pUnit, pStatList, 1);
			}

			STATLIST_SetStatIfListIsValid(pStatList, STAT_SOURCE_UNIT_TYPE, nOwnerType, 0);
			STATLIST_SetStatIfListIsValid(pStatList, STAT_SOURCE_UNIT_ID, nOwnerId, 0);
		}

		pUnit->dwFlagEx |= UNITFLAGEX_STOREOWNERINFO;
	}
}

//D2Common.0x6FDBFFE0 (#10415)
// 1.13c D2Common+0x33020
void __stdcall UNITS_StoreOwner(D2UnitStrc* pUnit, D2UnitStrc* pOwner)
{
	D2StatListStrc* pStatList = NULL;

	if (pOwner)
	{
		UNITS_StoreOwnerInfo(pUnit, pOwner->dwUnitType, pOwner->dwUnitId);
	}
	else
	{
		if (pUnit)
		{
			pUnit->dwOwnerType = 0;
			pUnit->dwOwnerGUID = 0;

			if (pUnit->pStatListEx)
			{
				STATES_ToggleState(pUnit, STATE_SOURCEUNIT, FALSE);

				pStatList = STATLIST_GetStatListFromUnitAndState(pUnit, STATE_SOURCEUNIT);
				if (pStatList)
				{
					D2Common_10474(pUnit, pStatList);
					STATLIST_FreeStatList(pStatList);
				}
			}

			pUnit->dwFlagEx &= ~UNITFLAGEX_STOREOWNERINFO;
		}
	}
}

// The retail offsets are x86 offsets. The wasm64 D2UnitStrc widens pointer fields;
// these assertions preserve the corresponding field positions after that widening.
static_assert(offsetof(D2UnitStrc, dwUnitType) == 0x00);
static_assert(offsetof(D2UnitStrc, dwUnitId) == 0x0C + (sizeof(void*) - 4));
static_assert(offsetof(D2UnitStrc, dwKillerType) == 0x9C + 15 * (sizeof(void*) - 4));
static_assert(offsetof(D2UnitStrc, dwKillerGUID) == 0xA0 + 15 * (sizeof(void*) - 4));
static_assert(offsetof(D2UnitStrc, dwFlagEx) == 0xC8 + 18 * (sizeof(void*) - 4));
static_assert(sizeof(D2UnitGUID) == 4);

//D2Common.0x6FDC0060 (#10417)
// 1.13c D2Common+0x2F9B0 (fatal code 0xEE7)
void __stdcall UNITS_StoreLastAttacker(D2UnitStrc* pUnit, D2UnitStrc* pKiller)
{
	D2_ASSERT(pUnit);

	if (pKiller)
	{
		pUnit->dwKillerType = pKiller->dwUnitType;
		pUnit->dwKillerGUID = pKiller->dwUnitId;
		pUnit->dwFlagEx |= UNITFLAGEX_STORELASTATTACKER;
	}
	else
	{
		pUnit->dwKillerType = 0;
		pUnit->dwKillerGUID = 0;
		pUnit->dwFlagEx &= ~UNITFLAGEX_STORELASTATTACKER;
	}
}

//D2Common.0x6FDC00E0 (#10418)
// 1.13c D2Common+0x2F910 (no null-unit fatal; static path x/y for object/item/tile, else the dynamic path's 16-bit position, 0 without a path)
int __stdcall UNITS_GetDirectionToCoords(D2UnitStrc* pUnit, int nNewX, int nNewY)
{
	int nX = 0;
	int nY = 0;

	if (pUnit->dwUnitType == UNIT_OBJECT || pUnit->dwUnitType == UNIT_ITEM || pUnit->dwUnitType == UNIT_TILE)
	{
		nX = pUnit->pStaticPath->tGameCoords.nX;
		nY = pUnit->pStaticPath->tGameCoords.nY;
	}
	else if (pUnit->pDynamicPath)
	{
		nX = pUnit->pDynamicPath->tGameCoords.wPosX;
		nY = pUnit->pDynamicPath->tGameCoords.wPosY;
	}

	return PATH_ComputeDirection(nX, nY, nNewX, nNewY);
}

// d2-web: the session client observes the overlay the server sets (its packet 0x11).
void (*g_d2webOverlayHook)(D2UnitStrc* pUnit, int nOverlay) = nullptr;

//D2Common.0x6FDC0160 (#10437)
// 1.13c D2Common+0x32C10 (no null-unit branch: retail reads the unit's pool directly). The hook call is not-1.13c: web session observer.
void __stdcall UNITS_SetOverlay(D2UnitStrc* pUnit, int nOverlay, int nUnused)
{
	if (g_d2webOverlayHook) g_d2webOverlayHook(pUnit, nOverlay);
	D2StatListStrc* pStatList = NULL;

	if (nOverlay >= 0 && nOverlay < sgptDataTables->nOverlayTxtRecordCount)
	{
		pStatList = STATLIST_GetStatListFromUnitAndFlag(pUnit, 0x80);
		if (!pStatList)
		{
			pStatList = STATLIST_AllocStatList(pUnit->pMemoryPool, 0x80, 0, pUnit->dwUnitType, pUnit->dwUnitId);

			D2COMMON_10475_PostStatToStatList(pUnit, pStatList, 1);
		}

		D2Common_10534(pUnit);
		D2Common_10531_SetStatInStatListLayer0(pStatList, STAT_UNIT_DOOVERLAY, (uint16_t)nOverlay, nUnused);
		UNITROOM_RefreshUnit(pUnit);
	}
}

//D2Common.0x6FDC01F0 (#10367)
// 1.13c D2Common+0x2F8A0 (fatal codes 0xF20/0xF24)
int __stdcall UNITS_GetBeltType(D2UnitStrc* pUnit)
{
	D2ItemsTxt* pItemsTxtRecord = NULL;

	D2_ASSERT(pUnit && pUnit->dwUnitType == UNIT_ITEM);

	pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pUnit->dwClassId);
	D2_ASSERT(pItemsTxtRecord);

	return pItemsTxtRecord->nBelt;
}

//D2Common.0x6FDC0260 (#10368)
// 1.13c D2Common+0x32330 (stats 6 and 7 as fixed point, >>8 each, then *100 / max)
int __stdcall UNITS_GetCurrentLifePercentage(D2UnitStrc* pUnit)
{
	const int nHp = STAT_HitPointsFixedPointToInt(STATLIST_UnitGetStatValue(pUnit, STAT_HITPOINTS, 0));
	if (const int nMaxHp = STAT_HitPointsFixedPointToInt(STATLIST_GetMaxLifeFromUnit(pUnit)))
	{
		return (100 * nHp) / nMaxHp;
	}

	return 0;
}

//D2Common.0x6FDC02A0 (#10359)
// 1.13c D2Common+0x317C0
BOOL __stdcall UNITS_IsSoftMonster(D2UnitStrc* pUnit)
{
	if (pUnit && pUnit->dwUnitType == UNIT_MONSTER)
	{
		if (D2MonStats2Txt* pMonStats2TxtRecord = UNITS_GetMonStats2TxtRecordFromMonsterId(pUnit->dwClassId))
		{
			return (pMonStats2TxtRecord->dwFlags & MONSTATS2FLAG_SOFT) != 0;
		}
	}
	return FALSE;
}

// not-1.13c: d2-web callee table that binds the 1.13c player-data alloc/free ports to the 1.10f source functions; retail has no such table (it calls them directly)
const D2UnitsPlayerDataSeams& UNITS_PlayerDataSeams()
{
	static const D2UnitsPlayerDataSeams kSeams = {
		[](void* pool, size_t n, int nLine) -> void* { return FOG_AllocPool(pool, n, __FILE__, nLine, 0); },
		[](void* pool, void* p, int nLine) { FOG_FreePool(pool, p, __FILE__, nLine, 0); },
		[](D2BitBufferStrc* b, uint8_t* p, size_t n) { BITMANIP_Initialize(b, p, n); },
		1, 1,   // the town waypoint's {word, bits}: WAYPOINTS_ActivateWaypoint's {0 / 16 + 1, 1 << (0 & 15)}
	};
	return kSeams;
}

//1.10f: D2Common.0x6FDC0320 (#10420)
// 1.13c D2Common+0x30970 (#10356). 1.13c deltas: QUESTRECORD_AllocRecord and WAYPOINTS_AllocWaypointData are inlined
// (record QuestRecord.cpp:0x50, its zeroed 0x60-byte stream :0x51, BITMANIP_Initialize; waypoint block Waypoint.cpp:0xFC,
// zeroed, word 0 = 0x102, then the town entry of the {word, bits} table OR-ed in); the player data comes uncleared from
// Units.cpp:0xF52 and is zeroed in place; DUNGEON_GetPortalFlagFromLevelId is inlined. Fatal: null unit 0xF4F, player
// data already set 0xF51.
void UNITS_AllocPlayerDataWith(D2UnitStrc* pUnit, const D2UnitsPlayerDataSeams& s)
{
	D2_ASSERT(pUnit);
	D2_ASSERT(!pUnit->pPlayerData);

	pUnit->pPlayerData = (D2PlayerDataStrc*)s.alloc(pUnit->pMemoryPool, sizeof(D2PlayerDataStrc), 0xF52);
	memset(pUnit->pPlayerData, 0x00, sizeof(D2PlayerDataStrc));
	pUnit->pPlayerData->szName[0] = 0;

	for (int i = 0; i < ARRAY_SIZE(pUnit->pPlayerData->pQuestData); ++i)
	{
		D2BitBufferStrc* pRecord = (D2BitBufferStrc*)s.alloc(pUnit->pMemoryPool, sizeof(D2BitBufferStrc), 0x50);
		uint8_t* pStream = (uint8_t*)s.alloc(pUnit->pMemoryPool, sizeof(uint16_t) * NUM_QUEST_WORDS, 0x51);
		memset(pStream, 0x00, sizeof(uint16_t) * NUM_QUEST_WORDS);
		s.bitInit(pRecord, pStream, sizeof(uint16_t) * NUM_QUEST_WORDS);
		pUnit->pPlayerData->pQuestData[i] = pRecord;

		D2WaypointDataStrc* pWaypoint = (D2WaypointDataStrc*)s.alloc(pUnit->pMemoryPool, sizeof(D2WaypointDataStrc), 0xFC);
		memset(pWaypoint, 0x00, sizeof(D2WaypointDataStrc));
		pWaypoint->nFlags[0] = 0x102;
		// The table's word index is not bounded by the 8-word block; retail ORs at that word wherever it lies.
		reinterpret_cast<uint16_t*>(pWaypoint)[s.nTownWpWord] |= s.nTownWpBits;
		pUnit->pPlayerData->pWaypointData[i] = pWaypoint;
	}

	pUnit->pPlayerData->nPortalFlags = DUNGEON_GetPortalFlagFromLevelId(LEVEL_ROGUEENCAMPMENT);
	pUnit->pPlayerData->bBusy = 0;
}

// 1.13c D2Common+0x30970
void __stdcall UNITS_AllocPlayerData(D2UnitStrc* pUnit)
{
	UNITS_AllocPlayerDataWith(pUnit, UNITS_PlayerDataSeams());
}

//1.10f: D2Common.0x6FDC03F0 (#10421)
// 1.13c D2Common+0x30880 (#11002). 1.13c deltas: QUESTRECORD_FreeRecord and WAYPOINTS_FreeWaypointData are inlined
// (stream QuestRecord.cpp:0x60, record :0x61, waypoint Waypoint.cpp:0x121, its state word 0 / 0x101 set to 0x102 and any
// other value fatal 0x113); the waypoint pointer is not null-checked. Fatal: null unit 0xF6B, no player data 0xF6D.
void UNITS_FreePlayerDataWith(void* pMemPool, D2UnitStrc* pPlayer, const D2UnitsPlayerDataSeams& s)
{
	D2_ASSERT(pPlayer);
	D2_ASSERT(pPlayer->pPlayerData);

	for (int i = 0; i < ARRAY_SIZE(pPlayer->pPlayerData->pQuestData); ++i)
	{
		if (D2BitBufferStrc* pRecord = pPlayer->pPlayerData->pQuestData[i])
		{
			s.free(pMemPool, pRecord->pBuffer, 0x60);
			s.free(pMemPool, pRecord, 0x61);
		}

		D2WaypointDataStrc* pWaypoint = pPlayer->pPlayerData->pWaypointData[i];
		if (pWaypoint->nFlags[0] != 0x102)
		{
			D2_ASSERT(pWaypoint->nFlags[0] == 0x00 || pWaypoint->nFlags[0] == 0x101);
			pWaypoint->nFlags[0] = 0x102;
		}
		s.free(pMemPool, pWaypoint, 0x121);
	}

	s.free(pMemPool, pPlayer->pPlayerData, 0xF76);
	pPlayer->pPlayerData = NULL;
}

// 1.13c D2Common+0x30880
void __stdcall UNITS_FreePlayerData(void* pMemPool, D2UnitStrc* pPlayer)
{
	UNITS_FreePlayerDataWith(pMemPool, pPlayer, UNITS_PlayerDataSeams());
}

//D2Common.0x6FDC04A0 (#10422)
// 1.13c D2Common+0x2EC00: retail calls SStrCopy(playerData, name, 0x7FFFFFFF); the 16-byte
// szName buffer makes strcpy_s truncate at 15, which no legal player name reaches.
void __stdcall UNITS_SetNameInPlayerData(D2UnitStrc* pUnit, char* szName)
{
	D2_ASSERT(pUnit);
	D2_ASSERT(pUnit->pPlayerData);
	D2_ASSERT(pUnit->dwUnitType == UNIT_PLAYER);

	// Note: Game actually uses Storm.dll SStrCopy
	strcpy_s(pUnit->pPlayerData->szName, szName);
}

//1.10f: D2Common.0x6FDC0530 (#10423)
// 1.13c D2Common+0x2EBB0 (#10746): asserts the unit (line 0xFA5), its player data (+0x14, 0xFA7) and
// unit type 0 (0xFA8) in that order, then returns the player data pointer, whose name is at +0.
char* __stdcall UNITS_GetPlayerName(D2UnitStrc* pUnit)
{
	D2_ASSERT(pUnit);
	D2_ASSERT(pUnit->pPlayerData);
	D2_ASSERT(pUnit->dwUnitType == UNIT_PLAYER);

	static_assert(offsetof(D2PlayerDataStrc, szName) == 0x00, "1.13c returns the player data pointer itself");
	return pUnit->pPlayerData->szName;
}

//1.10f: D2Common.0x6FDC05B0 (#10424)
// 1.13c D2Common+0x2EB70
D2PlayerDataStrc* __stdcall UNITS_GetPlayerData(D2UnitStrc* pUnit)
{
	D2_ASSERT(pUnit);

	D2_ASSERT(pUnit->dwUnitType == UNIT_PLAYER);

	return pUnit->pPlayerData;
}

//1.10f: D2Common.0x6FDC0600 (#10425)
// 1.13c D2Common+0x2EB30
void __stdcall UNITS_SetPlayerPortalFlags(D2UnitStrc* pUnit, int nPortalFlags)
{
	D2_ASSERT(pUnit);
	D2_ASSERT(pUnit->dwUnitType == UNIT_PLAYER);

	pUnit->pPlayerData->nPortalFlags = nPortalFlags;
}

//1.10f: D2Common.0x6FDC0660 (#10426)
// 1.13c D2Common+0x2EAF0
int __stdcall UNITS_GetPlayerPortalFlags(D2UnitStrc* pUnit)
{
	D2_ASSERT(pUnit);
	D2_ASSERT(pUnit->dwUnitType == UNIT_PLAYER);

	return pUnit->pPlayerData->nPortalFlags;
}

//D2Common.0x6FDC06C0 (#10353)
// 1.13c D2Common+0x2eab0
uint32_t __stdcall UNITS_GetNameOffsetFromObject(D2UnitStrc* pUnit)
{
	D2_ASSERT(pUnit);

	if (pUnit->dwUnitType == UNIT_OBJECT)
	{
		return pUnit->pObjectData->pObjectTxt->dwNameOffset;
	}

	return 0;
}

//1.10f: D2Common.0x6FDC0700 (#10427)
// 1.13c D2Common+0x2EA70
uint8_t __stdcall UNITS_GetObjectPortalFlags(D2UnitStrc* pUnit)
{
	D2_ASSERT(pUnit);
	D2_ASSERT(pUnit->dwUnitType == UNIT_OBJECT);
	return pUnit->pObjectData->nPortalFlags;
}

//1.10f: D2Common.0x6FDC0760 (#10428)
// 1.13c D2Common+0x2EA30
void __stdcall UNITS_SetObjectPortalFlags(D2UnitStrc* pUnit, uint8_t nPortalFlag)
{
	D2_ASSERT(pUnit);
	D2_ASSERT(pUnit->dwUnitType == UNIT_OBJECT);
	pUnit->pObjectData->nPortalFlags = nPortalFlag;
}

// 1.13c D2Common+0x2E9E0
//D2Common.0x6FDC07C0 (#10429)
BOOL __stdcall UNITS_CheckObjectPortalFlag(D2UnitStrc* pUnit, uint8_t nFlag)
{
	D2_ASSERT(pUnit);
	D2_ASSERT(pUnit->dwUnitType == UNIT_OBJECT);

	return (nFlag & pUnit->pObjectData->nPortalFlags) != 0;
}

//D2Common.0x6FDC0820 (#10430)
// 1.13c D2Common+0x30830
int __stdcall UNITS_GetOverlayHeight(D2UnitStrc* pUnit)
{
	D2MonStats2Txt* pMonStats2TxtRecord = NULL;

	if (pUnit)
	{
		if (pUnit->dwUnitType == UNIT_PLAYER)
		{
			return 1;
		}
		else if (pUnit->dwUnitType == UNIT_MONSTER)
		{
			pMonStats2TxtRecord = UNITS_GetMonStats2TxtRecordFromMonsterId(pUnit->dwClassId);
			if (pMonStats2TxtRecord)
			{
				return pMonStats2TxtRecord->nOverlayHeight - 1;
			}
		}
	}

	return 0;
}

// not-1.13c: d2-web callee table that binds the 1.13c defense port to the 1.10f source functions; retail has no such table (it calls them directly)
const D2UnitsDefenseSeams& UNITS_DefenseSeams()
{
	static const D2UnitsDefenseSeams kSeams = {
		[](D2UnitStrc* u, int n) -> BOOL { return STATES_CheckState(u, n); },
		[](int n) -> D2SkillsTxt* { return DATATBLS_GetSkillsTxtRecord(n); },
		[](D2InventoryStrc* i, D2UnitStrc** pp) -> BOOL { return INVENTORY_GetEquippedShield(i, pp); },
		[](D2UnitStrc* u, unsigned int c, int n, int l) -> int { return SKILLS_EvaluateSkillFormula(u, c, n, l); },
	};
	return kSeams;
}

//1.10f: D2Common.0x6FDC08B0 (#10431)
// 1.13c D2Common+0x32890 (#10672). 1.13c deltas: the holy-shield gate is inline (unit type 0, 1 or 3, states.txt
// count above 101, then the state bit) before the state's list is fetched; the skill and level must both be positive
// before the skills.txt record is fetched (1.10f fetched it for any id); the defense products wrap at 32 bits; the
// armor-override percent takes retail's branches: 32-bit product when total <= 0x100000 and percent <= 0x10000,
// (percent / 100) * total or (total / 100) * percent when the larger one is at least 0x640, else the 64-bit product.
int UNITS_GetDefenseWith(D2UnitStrc* pUnit, const D2UnitsDefenseSeams& s)
{
	int nDefense = STATLIST_UnitGetStatValue(pUnit, STAT_ARMORCLASS, 0) + STATLIST_UnitGetStatValue(pUnit, STAT_DEXTERITY, 0) / 4;
	int nArmorPercent = STATLIST_UnitGetItemStatOrSkillStatValue(pUnit, STAT_SKILL_ARMOR_PERCENT, 0) + STATLIST_UnitGetItemStatOrSkillStatValue(pUnit, STAT_ITEM_ARMOR_PERCENT, 0);

	if ((pUnit->dwUnitType == UNIT_PLAYER || pUnit->dwUnitType == UNIT_MONSTER || pUnit->dwUnitType == UNIT_MISSILE)
		&& sgptDataTables->nStatesTxtRecordCount > STATE_HOLYSHIELD && s.checkState(pUnit, STATE_HOLYSHIELD))
	{
		if (D2StatListStrc* pStatList = STATLIST_GetStatListFromUnitAndState(pUnit, STATE_HOLYSHIELD))
		{
			const int nSkillId = STATLIST_GetStatValue(pStatList, STAT_MODIFIERLIST_SKILL, 0);
			const int nSkillLevel = STATLIST_GetStatValue(pStatList, STAT_MODIFIERLIST_LEVEL, 0);
			if (nSkillId > 0 && nSkillLevel > 0)
			{
				if (D2SkillsTxt* pSkillsTxtRecord = s.getSkillsTxt(nSkillId))
				{
					D2UnitStrc* pShieldItem = NULL;
					s.getShield(pUnit->pInventory, &pShieldItem);
					if (pShieldItem)
					{
						nArmorPercent += s.evalFormula(pUnit, pSkillsTxtRecord->dwCalc[0], nSkillId, nSkillLevel);
					}
				}
			}
		}
	}

	const int32_t nProduct = (int32_t)((uint32_t)nArmorPercent * (uint32_t)nDefense);
	const int32_t nTotal = nDefense + (nDefense <= 0 ? nProduct / -100 : nProduct / 100);

	const int32_t nOverride = STATLIST_UnitGetStatValue(pUnit, STAT_ARMOR_OVERRIDE_PERCENT, 0);
	if (!nOverride)
	{
		return nTotal;
	}

	const uint32_t uTotal = (uint32_t)nTotal;
	const uint32_t uOverride = (uint32_t)nOverride;
	if (nTotal <= 0x100000)
	{
		if (nOverride <= 0x10000)
		{
			return (int)(uTotal + (uint32_t)((int32_t)(uOverride * uTotal) / 100));
		}
		if ((int32_t)(uOverride & 0xFFFFFFF0u) > 0x63F)
		{
			return (int)(uTotal + (uint32_t)(nOverride / 100) * uTotal);
		}
	}
	else if ((int32_t)(uTotal & 0xFFFFFFF0u) > 0x63F)
	{
		return (int)(uTotal + (uint32_t)(nTotal / 100) * uOverride);
	}
	return (int)(uTotal + (uint32_t)((int64_t)nOverride * (int64_t)nTotal / 100));
}

// 1.13c D2Common+0x32890
int __stdcall UNITS_GetDefense(D2UnitStrc* pUnit)
{
	return UNITS_GetDefenseWith(pUnit, UNITS_DefenseSeams());
}

//1.10f: D2Common.0x6FDC0AC0 (#10432)
// 1.13c D2Common+0x31EA0: 5 * dexterity + to-hit - 35, plus the class's charstats ToHitFactor (+0x3C) when the class is a row.
int __stdcall UNITS_GetAttackRate(D2UnitStrc* pAttacker)
{
	D2CharStatsTxt* pCharStatsTxtRecord = NULL;
	int nAttackRate = 0;
	int nDexterity = 0;
	int nToHit = 0;

	D2_ASSERT(pAttacker && pAttacker->dwUnitType == UNIT_PLAYER);

	nToHit = STATLIST_UnitGetStatValue(pAttacker, STAT_TOHIT, 0);
	nDexterity = STATLIST_UnitGetStatValue(pAttacker, STAT_DEXTERITY, 0);
	nAttackRate = nToHit + 5 * (nDexterity - 7);

	if (pAttacker->dwUnitType == UNIT_PLAYER && pAttacker->dwClassId >= 0 && pAttacker->dwClassId < sgptDataTables->nCharStatsTxtRecordCount)
	{
		pCharStatsTxtRecord = &sgptDataTables->pCharStatsTxt[pAttacker->dwClassId];
		if (pCharStatsTxtRecord)
		{
			return nAttackRate + pCharStatsTxtRecord->dwToHitFactor;
		}
	}

	return nAttackRate;
}

// 1.13c D2Common+0x31680: does a monster carry a blocking shield. Factored out of the 1.10f
// UNITS_GetBlockRate monster branch; retail passes the unit in EAX. The class id (out of the monstats
// range -> -1) picks Diablo, DoomKnight1 and DiabloClone (1) and Act3Hire (0); otherwise the
// component slot 7 code (unit type 1 with monster data: its nComponent[7], else 0) is read, an empty
// code or one in the reject list (the torch) is 0, and the code qualifies when its items.txt row's
// first type is a shield.
static BOOL UNITS_MonsterHasBlockingShield(D2UnitStrc* pUnit)
{
	static const uint32_t gdwNonShieldCodes[] =
	{
		' hct'
	};

	int nClassId = pUnit ? pUnit->dwClassId : -1;
	if (nClassId < 0 || nClassId >= sgptDataTables->nMonStatsTxtRecordCount)
	{
		nClassId = -1;
	}

	switch (nClassId)
	{
	case MONSTER_DIABLO:
	case MONSTER_DOOMKNIGHT1:
	case MONSTER_DIABLOCLONE:
		return TRUE;

	case MONSTER_ACT3HIRE:
		return FALSE;

	default:
		break;
	}

	const uint8_t nComponent = (pUnit && pUnit->dwUnitType == UNIT_MONSTER && pUnit->pMonsterData) ? pUnit->pMonsterData->nComponent[7] : 0;
	const uint32_t dwCode = D2Common_11069(pUnit, 7, nComponent);
	if (!dwCode)
	{
		return FALSE;
	}

	for (uint32_t dwRejected : gdwNonShieldCodes)
	{
		if (dwCode == dwRejected)
		{
			return FALSE;
		}
	}

	int nItemId = 0;
	D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemRecordFromItemCode(dwCode, &nItemId);
	return pItemsTxtRecord && pItemsTxtRecord->wType[0] == ITEMTYPE_SHIELD;
}

//1.10f: D2Common.0x6FDC0B60 (#10433)
// 1.13c D2Common+0x31D20 (#10212). A null unit counts as type 6 (0). Player: no equipped shield
// (inventory +0x60) -> 0; else the unit's item_toblock total (stat list +0x5C, ItemStatCost row 20,
// inlined) plus charstats nBlockFactor (+0x49); with bExpansion it becomes
// (dexterity - 15) * total / (2 * level), dexterity read before level and level floored at 1.
// Monster: a monstats NoShldBlock flag (byte +0xF, bit 2) goes straight to item_toblock; otherwise
// 0x31680 must accept the unit or the answer is 0. Every other type is 0. The result is capped at 75.
int __stdcall UNITS_GetBlockRate(D2UnitStrc* pUnit, BOOL bExpansion)
{
	const int nUnitType = pUnit ? pUnit->dwUnitType : UNIT_TYPES_COUNT;
	int nBlockChance = 0;

	if (nUnitType == UNIT_PLAYER)
	{
		if (!INVENTORY_GetEquippedShield(pUnit->pInventory, NULL))
		{
			return 0;
		}

		nBlockChance = STATLIST_UnitGetStatValue(pUnit, STAT_TOBLOCK, 0);

		// Retail indexes the charstats row without a fallback (an out-of-range class reads through a
		// null row); a player class id is always a row, and the guard keeps the 1.10f answer of 0.
		if (pUnit->dwClassId < 0 || pUnit->dwClassId >= sgptDataTables->nCharStatsTxtRecordCount)
		{
			return 0;
		}
		nBlockChance += sgptDataTables->pCharStatsTxt[pUnit->dwClassId].nBlockFactor;

		if (bExpansion)
		{
			const int nDexterity = STATLIST_UnitGetStatValue(pUnit, STAT_DEXTERITY, 0);
			int nLevel = STATLIST_UnitGetStatValue(pUnit, STAT_LEVEL, 0);
			if (nLevel < 2)
			{
				nLevel = 1;
			}

			nBlockChance = (nDexterity - 15) * nBlockChance / (2 * nLevel);
		}
	}
	else if (nUnitType == UNIT_MONSTER)
	{
		D2MonStatsTxt* pMonStatsTxtRecord = DATATBLS_GetMonStatsTxtRecord(pUnit->dwClassId);
		if (!pMonStatsTxtRecord || !(pMonStatsTxtRecord->dwMonStatsFlags & gdwBitMasks[MONSTATSFLAGINDEX_NOSHLDBLOCK]))
		{
			if (!UNITS_MonsterHasBlockingShield(pUnit))
			{
				return 0;
			}
		}

		nBlockChance = STATLIST_UnitGetStatValue(pUnit, STAT_TOBLOCK, 0);
	}
	else
	{
		return 0;
	}

	if (nBlockChance >= 75)
	{
		return 75;
	}
	return nBlockChance;
}

//D2Common.0x6FDC0DA0 (#10434)
//TODO: Find a name, v14
// 1.13c D2Common+0x31B90
D2UnitStrc* __stdcall D2Common_10434(D2UnitStrc* pUnit, BOOL a2)
{
	D2SkillsTxt* pSkillsTxtRecord = NULL;
	D2SkillStrc* pSkill = NULL;
	D2UnitStrc* pHandItem = NULL;
	D2UnitStrc* pWeapon = NULL;
	D2UnitStrc* pItem = NULL;
	int nBodyLoc = 0;
	int nFrame = 0;
	int v14; // ecx@22
	BOOL bIsLeftHandItem = FALSE;


	if (!pUnit->pInventory)
	{
		return NULL;
	}

	INVENTORY_GetEquippedWeapon(pUnit->pInventory, &pItem, &nBodyLoc, &bIsLeftHandItem);

	if (!UNITS_CanDualWield(pUnit))
	{
		return pItem;
	}

	// 1.13c reads the skill list's used-skill slot inline (null list gives no skill)
	pSkill = pUnit->pSkills ? pUnit->pSkills->pUsedSkill : NULL;
	if (!pSkill)
	{
		return pItem;
	}

	pSkillsTxtRecord = SKILLS_GetSkillsTxtRecordFromSkill(pSkill);
	if (!pSkillsTxtRecord)
	{
		return pItem;
	}

	pWeapon = NULL;
	switch (pSkillsTxtRecord->nWeapSel)
	{
	case 1:
		pWeapon = INVENTORY_GetCompositItem(pUnit->pInventory, COMPOSIT_LEFTHAND);
		break;
	
	case 2:
		pWeapon = INVENTORY_GetCompositItem(pUnit->pInventory, COMPOSIT_RIGHTHAND);
		pHandItem = INVENTORY_GetCompositItem(pUnit->pInventory, COMPOSIT_LEFTHAND);

		if (!pWeapon || !ITEMS_CheckItemTypeId(pWeapon, ITEMTYPE_WEAPON) || pHandItem && ITEMS_CheckItemTypeId(pHandItem, ITEMTYPE_WEAPON) && SKILLS_GetFlags(pSkill) & 0x2000)
		{
			pWeapon = pHandItem;
		}

		break;

	case 3:
		nFrame = pUnit->dwSeqFrame >> 8;

		if (pUnit->dwFlagEx & UNITFLAGEX_SERVERUNIT)
		{
			v14 = nFrame % 2;
		}
		else
		{
			v14 = nFrame > 5;
		}

		if (nFrame && (!a2 || v14))
		{
			pWeapon = INVENTORY_GetCompositItem(pUnit->pInventory, COMPOSIT_LEFTHAND);
		}
		else
		{
			pWeapon = INVENTORY_GetCompositItem(pUnit->pInventory, COMPOSIT_RIGHTHAND);
		}
		break;

	case 4:
		pItem = NULL;
		break;

	default:
		pWeapon = INVENTORY_GetCompositItem(pUnit->pInventory, COMPOSIT_RIGHTHAND);
		if (!pWeapon || !ITEMS_CheckItemTypeId(pWeapon, ITEMTYPE_WEAPON))
		{
			pWeapon = INVENTORY_GetCompositItem(pUnit->pInventory, COMPOSIT_LEFTHAND);
		}

		break;
	}

	if (!ITEMS_CheckItemTypeId(pWeapon, ITEMTYPE_WEAPON))
	{
		return pItem;
	}

	if (!ITEMS_CanBeEquipped(pWeapon))
	{
		return pItem;
	}

	return pWeapon;
}

//D2Common.0x6FDC0F70 (#10435)
// 1.13c D2Common+0x31B40
// 1.13c body: the monster must be UNIT_MONSTER with an inventory whose signature is valid; the candidate is
// the 1.13c equipped-hand rule of D2Common+0x20700 (not INVENTORY_GetEquippedWeapon): body-loc cell 4 (right
// hand) when it is not an item or its item flags have neither 0x100 nor 0x4000, and it is a weapon (0x2D);
// else cell 5 (left hand) when it can be equipped and is a weapon. The body-loc grid is the one 1.13c reads
// through 0x1E1B0 with the static size pair DAT_6FDED378 = {13,1}, which is gBodyLocInventoryGridInfo here.
D2UnitStrc* __stdcall UNITS_GetEquippedWeaponFromMonster(D2UnitStrc* pMonster)
{
	if (!pMonster || pMonster->dwUnitType != UNIT_MONSTER || !pMonster->pInventory)
	{
		return NULL;
	}

	if (!INVENTORY_GetPtrIfValid(pMonster->pInventory))
	{
		return NULL;
	}

	D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pMonster->pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
	if (!pInventoryGrid)
	{
		return NULL;
	}

	D2UnitStrc* pRightHand = pInventoryGrid->ppItems[BODYLOC_RARM];
	if (pRightHand)
	{
		// Item flags are read from the item data only for an item unit (0x100 and 0x4000 are the 1.13c masks).
		D2ItemDataStrc* pItemData = ITEMS_GetItemData(pRightHand);
		BOOL bFlagsClear = !pItemData || (!(pItemData->dwItemFlags & 0x100) && !(pItemData->dwItemFlags & 0x4000));
		if ((pRightHand->dwUnitType != UNIT_ITEM || bFlagsClear) && ITEMS_CheckItemTypeId(pRightHand, ITEMTYPE_WEAPON))
		{
			return pRightHand;
		}
	}

	D2UnitStrc* pLeftHand = pInventoryGrid->ppItems[BODYLOC_LARM];
	if (pLeftHand && ITEMS_CanBeEquipped(pLeftHand) && ITEMS_CheckItemTypeId(pLeftHand, ITEMTYPE_WEAPON))
	{
		return pLeftHand;
	}

	return NULL;
}

//D2Common.0x6FDC0FC0 (#10436)
// 1.13c D2Common+0x32220
int __stdcall UNITS_GetFrameBonus(D2UnitStrc* pUnit)
{
	static const int gaClassesWeaponFrameBonus_6FDD3078[NUM_WEAPON_CLASSES][NUMBER_OF_PLAYERCLASSES] =
	{
		//AMA,SOR,NEC,PAL,BAR,DRU,ASS
		{   1,  1,  0,  0,  0,  0,  0}, // WEAPONCLASS_NONE
		{   0,  0,  0,  0,  0,  0,  0}, // WEAPONCLASS_BOW
		{   2,  2,  0,  0,  0,  0,  0}, // WEAPONCLASS_1HS
		{   2,  2,  0,  0,  0,  0,  0}, // WEAPONCLASS_1HT
		{   2,  2,  0,  0,  0,  0,  0}, // WEAPONCLASS_STF
		{   2,  2,  0,  0,  0,  0,  0}, // WEAPONCLASS_2HS
		{   2,  2,  0,  0,  0,  0,  0}, // WEAPONCLASS_2HT
		{   0,  0,  0,  0,  0,  0,  0}, // WEAPONCLASS_XBW
		{   0,  0,  0,  0,  0,  0,  0}, // WEAPONCLASS_1JS
		{   0,  0,  0,  0,  0,  0,  0}, // WEAPONCLASS_1JT
		{   0,  0,  0,  0,  0,  0,  0}, // WEAPONCLASS_1SS
		{   0,  0,  0,  0,  0,  0,  0}, // WEAPONCLASS_1ST
		{   0,  0,  0,  0,  0,  0,  0}, // WEAPONCLASS_HT1
		{   0,  0,  0,  0,  0,  0,  0}  // WEAPONCLASS_HT2
	};
	
	int nClassId = -1;
	int nType = UNIT_TYPES_COUNT;
	int nMode = 0;
	if (pUnit)
	{
		nClassId = pUnit->dwClassId;
		nType = pUnit->dwUnitType;
		nMode = pUnit->dwAnimMode;
	}		

	D2COMMON_11013_ConvertMode(pUnit, &nType, &nClassId, &nMode, __FILE__, __LINE__);

	if (nType != UNIT_PLAYER)
	{
		return 0;
	}


	int nWeaponClass = WEAPONCLASS_HTH;
	switch (nMode)
	{
	case PLRMODE_ATTACK1:
	case PLRMODE_ATTACK2:
		if (D2UnitStrc* pItem = D2Common_10434(pUnit, 1))
		{
			nWeaponClass = ITEMS_GetWeaponClassId(pItem);
		}
		break;
	case PLRMODE_SPECIAL3:
	case PLRMODE_SPECIAL4:
		if (!UNITS_CanDualWield(pUnit))
		{
			return 0;
		}

		if (D2UnitStrc* pItem = D2Common_10434(pUnit, 1))
		{
			nWeaponClass = ITEMS_GetWeaponClassId(pItem);
		}
		break;
	default:
		return 0;
	}

	D2_ASSERT(0 <= nClassId && nClassId < NUMBER_OF_PLAYERCLASSES);
	D2_ASSERT(0 <= nWeaponClass && nWeaponClass < NUM_WEAPON_CLASSES);
	return gaClassesWeaponFrameBonus_6FDD3078[nWeaponClass][nClassId];
}

//D2Common.0x6FDC1120 (#10360)
// 1.13c D2Common+0x325C0 (fatal code 0x11BE)
int __stdcall UNITS_GetMeleeRange(D2UnitStrc* pUnit)
{
	D2MonStats2Txt* pMonStats2TxtRecord = NULL;
	BOOL bIsLeftHandItem = FALSE;
	int nWeaponClass = 0;
	int nBodyLoc = 0;

	D2_ASSERT(pUnit);

	if (pUnit->dwUnitType == UNIT_PLAYER)
	{
		if (INVENTORY_GetEquippedWeapon(pUnit->pInventory, &pUnit, &nBodyLoc, &bIsLeftHandItem))
		{
			return ITEMS_GetMeleeRange(pUnit);
		}		
	}
	else if (pUnit->dwUnitType == UNIT_MONSTER)
	{
		pMonStats2TxtRecord = UNITS_GetMonStats2TxtRecordFromMonsterId(pUnit->dwClassId);
		if (pMonStats2TxtRecord)
		{
			if (pMonStats2TxtRecord->nMeleeRng == 255)
			{
				COMPOSIT_GetWeaponClassId(pUnit, pUnit->pInventory, &nWeaponClass, -1, 1);
				return nWeaponClass == WEAPONCLASS_2HT ? 2 : 0;
			}

			return pMonStats2TxtRecord->nMeleeRng;
		}
	}

	return 0;
}

//D2Common.0x6FDC1230 (#10364)
// 1.13c D2Common+0x315E0
BOOL __stdcall UNITS_TestCollisionByCoordinates(D2UnitStrc* pUnit, int nX, int nY, int nFlags)
{
	D2ActiveRoomStrc* pRoom = NULL;
	D2CoordStrc pCoords = {};

	if (!pUnit)
	{
		return FALSE;
	}

	pRoom = UNITS_GetRoom(pUnit);
	if (!pRoom)
	{
		return FALSE;
	}

	UNITS_GetCoords(pUnit, &pCoords);

	return UNITS_TestCollision(pCoords.nX, pCoords.nY, UNITS_GetUnitSizeX(pUnit), nX, nY, 2, pRoom, nFlags);
}

//D2Common.0x6FDC13D0
// 1.13c D2Common+0x30730
BOOL __fastcall UNITS_TestCollision(int nX1, int nY1, int nSize1, int nX2, int nY2, int nSize2, D2ActiveRoomStrc* pRoom, int nCollisionMask)
{
	D2CoordStrc pCoords1 = {};
	D2CoordStrc pCoords2 = {};
	int nXDiff = 0;
	int nYDiff = 0;

	if (nSize1 >= 3)
	{
		nSize1 = 2;
	}

	if (nSize2 >= 3)
	{
		nSize2 = 2;
	}

	nXDiff = nX2 - nX1;
	if (nXDiff < 0)
	{
		nXDiff = -nXDiff;
	}

	nYDiff = nY2 - nY1;
	if (nYDiff < 0)
	{
		nYDiff = -nYDiff;
	}

	if (nXDiff + nYDiff < nSize1 + nSize2)
	{
		return FALSE;
	}

	if (nSize1 || nSize2)
	{
		if (nXDiff >= nYDiff)
		{
			if (nX2 <= nX1)
			{
				nX1 -= nSize1;
				nX2 += nSize2;
			}
			else
			{
				nX1 += nSize1;
				nX2 -= nSize2;
			}
		}

		if (nYDiff >= nXDiff)
		{
			if (nY2 <= nY1)
			{
				nY1 -= nSize1;
				nY2 += nSize2;
			}
			else
			{
				nY1 += nSize1;
				nY2 -= nSize2;
			}
		}
	}

	pCoords1.nX = nX1;
	pCoords1.nY = nY1;
	pCoords2.nX = nX2;
	pCoords2.nY = nY2;

	return COLLISION_RayTrace(pRoom, &pCoords1, &pCoords2, nCollisionMask);
}

//D2Common.0x6FDC14C0 (#10362)
// 1.13c D2Common+0x314A0 (fatal 0x1290 on a null first unit; a null second unit returns FALSE)
BOOL __stdcall UNITS_TestCollisionWithUnit(D2UnitStrc* pUnit1, D2UnitStrc* pUnit2, int nCollisionMask)
{
	D2ActiveRoomStrc* pRoom = NULL;
	D2CoordStrc pCoords1 = {};
	D2CoordStrc pCoords2 = {};

	D2_ASSERT(pUnit1);

	if (!pUnit2)
	{
		return FALSE;
	}

	pRoom = UNITS_GetRoom(pUnit1);
	if (!pRoom)
	{
		return FALSE;
	}

	UNITS_GetCoords(pUnit1, &pCoords1);
	UNITS_GetCoords(pUnit2, &pCoords2);

	return UNITS_TestCollision(pCoords1.nX, pCoords1.nY, UNITS_GetUnitSizeX(pUnit1), pCoords2.nX, pCoords2.nY, UNITS_GetUnitSizeX(pUnit2), pRoom, nCollisionMask);
}

//D2Common.0x6FDC1790 (#10363)
// 1.13c D2Common+0x319C0
//TODO: v4 + v26
BOOL __stdcall UNITS_TestCollisionBetweenInteractingUnits(D2UnitStrc* pUnit1, D2UnitStrc* pUnit2, int nCollisionMask)
{
	D2CoordStrc pCoords1 = {};
	D2CoordStrc pCoords2 = {};
	D2ActiveRoomStrc* pRoom = NULL;
	BOOL bResult = FALSE;
	int v4 = 0;
	int v26 = 0;

	if (!pUnit1 || !pUnit2)
	{
		return FALSE;
	}

	pRoom = UNITS_GetRoom(pUnit1);
	if (!pRoom)
	{
		return FALSE;
	}

	UNITS_GetCoords(pUnit1, &pCoords1);
	UNITS_GetCoords(pUnit2, &pCoords2);

	if (UNITS_GetCollisionMask(pUnit1) & nCollisionMask)
	{
		v4 = PATH_RemoveCollisionFootprintForUnit(pUnit1, 0);
	}

	if (UNITS_GetCollisionMask(pUnit2) & nCollisionMask)
	{
		v26 = PATH_RemoveCollisionFootprintForUnit(pUnit2, 0);
	}

	bResult = UNITS_TestCollision(pCoords1.nX, pCoords1.nY, UNITS_GetUnitSizeX(pUnit1), pCoords2.nX, pCoords2.nY, UNITS_GetUnitSizeX(pUnit2), pRoom, nCollisionMask);

	if (v4)
	{
		PATH_AddCollisionFootprintForUnit(pUnit1);
	}

	if (v26)
	{
		PATH_AddCollisionFootprintForUnit(pUnit2);
	}

	return bResult;
}

//D2Common.0x6FDC1A70 (#10361)
// 1.13c D2Common+0x326C0
BOOL __stdcall UNITS_IsInMeleeRange(D2UnitStrc* pUnit1, D2UnitStrc* pUnit2, int nRangeBonus)
{
	int nDistance = 0;
	int nClassId = 0;

	if (pUnit1 && pUnit2)
	{
		if (pUnit2->dwUnitType == UNIT_MONSTER)
		{
			nClassId = MONSTERS_GetBaseIdFromMonsterId(pUnit2->dwClassId);
			if (nClassId < 0 || nClassId >= sgptDataTables->nMonStatsTxtRecordCount)	// d2-web: retail clamps an out-of-table base id to -1 before the tentacle test
			{
				nClassId = -1;
			}

			if (nClassId == MONSTER_TENTACLE1 || nClassId == MONSTER_TENTACLEHEAD1)
			{
				if (UNITS_GetMeleeRange(pUnit1) + 8 > D2Common_10399(pUnit1, pUnit2))
				{
					return TRUE;
				}
			}
		}

		nDistance = D2Common_10399(pUnit1, pUnit2);
		if (nDistance <= 0)
		{
			return 1;
		}

		if (UNITS_GetMeleeRange(pUnit1) + nRangeBonus + 1 >= nDistance)
		{
			return UNITS_TestCollisionWithUnit(pUnit1, pUnit2, COLLIDE_MASK_PLAYER_FLYING) == 0;
		}
	}

	return FALSE;
}

//D2Common.0x6FDC1B40 (#10318)
// 1.13c D2Common+0x313F0
BOOL __stdcall UNITS_IsInMovingMode(D2UnitStrc* pUnit)
{
	if (pUnit == nullptr)
	{
		return false;
	}
	switch (pUnit->dwUnitType)
	{
	case UNIT_PLAYER:
		switch (const int nPlayerMode = pUnit->pAnimSeq ? pUnit->dwSeqMode : pUnit->dwAnimMode)
		{
		case PLRMODE_WALK:
		case PLRMODE_RUN:
		case PLRMODE_TOWNWALK:
			return TRUE;
		default:
			return FALSE;
		}
	case UNIT_MONSTER:
		switch (pUnit->dwAnimMode)
		{
		case MONMODE_WALK:
		case MONMODE_RUN:
			return TRUE;
		case MONMODE_SKILL1:
		case MONMODE_SKILL2:
			return MONSTERS_GetBaseIdFromMonsterId(pUnit->dwClassId) == MONSTER_VULTURE1;
		default:
			return FALSE;
		}
	default:
		D2_ASSERT(pUnit->dwUnitType != UNIT_MISSILE);
		return FALSE;
	}
}

//D2Common.0x6FDC1C30 (#10319)
// 1.13c D2Common+0x319A0
BOOL __stdcall UNITS_IsInMovingModeEx(D2UnitStrc* pUnit)
{
	return UNITS_IsInMovingMode(pUnit) != 0;
}

//D2Common.0x6FDC1C50 (#10365)
// 1.13c D2Common+0x321E0
int __fastcall UNITS_GetHitClass(D2UnitStrc* pUnit)
{
	if (pUnit)
	{
		if (pUnit->dwUnitType == UNIT_PLAYER)
		{
			if (D2UnitStrc* pItem = D2Common_10434(pUnit, FALSE))
			{
				return ITEMS_GetHitClassFromItem(pItem);
			}
			else
			{
				return HITCLASS_HandToHand;
			}
		}
		else if (pUnit->dwUnitType == UNIT_MONSTER)
		{
			if (D2MonStats2Txt* pMonStats2TxtRecord = UNITS_GetMonStats2TxtRecordFromMonsterId(pUnit->dwClassId))
			{
				return pMonStats2TxtRecord->nHitClass;
			}
		}
	}

	return HITCLASS_None;
}

//D2Common.0x6FDC1CE0 (#10366)
// 1.13c D2Common+0x321C0
int __fastcall UNITS_GetWeaponClass(D2UnitStrc* pUnit)
{
	if (D2UnitStrc* pItem = D2Common_10434(pUnit, TRUE))
	{
		return ITEMS_GetWeaponClassId(pItem);
	}

	return WEAPONCLASS_HTH;
}

// 1.13c D2Common+0x32040
unsigned int __stdcall UNITS_GetHealingCost(D2UnitStrc* pUnit)
{
	unsigned int nShiftedLifeDifference = 0;
	unsigned int nShiftedManaDifference = 0;

	const unsigned int nMaxLife = (unsigned int)STATLIST_GetMaxLifeFromUnit(pUnit);
	const unsigned int nMaxMana = (unsigned int)STATLIST_GetMaxManaFromUnit(pUnit);
	const unsigned int nLife = (unsigned int)STATLIST_UnitGetStatValue(pUnit, STAT_HITPOINTS, 0);
	const unsigned int nMana = (unsigned int)STATLIST_UnitGetStatValue(pUnit, STAT_MANA, 0);

	if (nLife < nMaxLife)
	{
		nShiftedLifeDifference = (nMaxLife - nLife) >> 8;
	}

	if (nMana < nMaxMana)
	{
		nShiftedManaDifference = (nMaxMana - nMana) >> 8;
	}

	if (nShiftedLifeDifference == 0 && nShiftedManaDifference == 0)
	{
		return 0;
	}

	return ((nShiftedManaDifference + nShiftedLifeDifference) * (unsigned int)STATLIST_UnitGetStatValue(pUnit, STAT_LEVEL, 0)) >> 2;
}

// 1.13c D2Common+0x31940
unsigned int __stdcall UNITS_GetInventoryGoldLimit(D2UnitStrc* pUnit)
{
	if (pUnit == nullptr)
	{
		return 0;
	}

	if (pUnit->pStatListEx != nullptr && sgptDataTables->nItemStatCostTxtRecordCount > STAT_LEVEL && &sgptDataTables->pItemStatCostTxt[STAT_LEVEL] != nullptr)
	{
		// The level stat total (ItemStatCost row 12, packed id 0xC0000) times 10000.
		const int nLevel = STATLIST_GetTotalStat_6FDB63E0(pUnit->pStatListEx, 0xC0000, &sgptDataTables->pItemStatCostTxt[STAT_LEVEL]);
		return (unsigned int)(nLevel * 10000);
	}

	return 0;
}

// 1.13c D2Common+0x32EF0
//TODO: v5
void __stdcall UNITS_MergeDualWieldWeaponStatLists(D2UnitStrc* pUnit, int a2)
{
	D2UnitStrc* v5 = NULL;
	D2UnitStrc* pPrimaryWeapon = NULL;
	D2UnitStrc* pSecondaryWeapon = NULL;

	if (!UNITS_CanDualWield(pUnit))
	{
		return;
	}

	v5 = D2Common_10434(pUnit, a2);
	pPrimaryWeapon = INVENTORY_GetLeftHandWeapon(pUnit->pInventory);
	if (v5)
	{
		if (v5 == pPrimaryWeapon)
		{
			pSecondaryWeapon = INVENTORY_GetSecondaryWeapon(pUnit->pInventory);
			if (pPrimaryWeapon && STATLIST_GetOwner(pPrimaryWeapon, 0))
			{
				STATLIST_MergeStatLists(pUnit, pPrimaryWeapon, 1);
			}

			if (pSecondaryWeapon && STATLIST_GetOwner(pSecondaryWeapon, 0))
			{
				STATLIST_MergeStatLists(pUnit, pSecondaryWeapon, 0);
			}
		}
		else
		{
			if (STATLIST_GetOwner(v5, 0))
			{
				STATLIST_MergeStatLists(pUnit, v5, 1);
			}

			if (pPrimaryWeapon && STATLIST_GetOwner(pPrimaryWeapon, 0))
			{
				STATLIST_MergeStatLists(pUnit, pPrimaryWeapon, 0);
			}
		}
	}
	else
	{
		if (pPrimaryWeapon && STATLIST_GetOwner(pPrimaryWeapon, 0))
		{
			STATLIST_MergeStatLists(pUnit, pPrimaryWeapon, 1);
		}
	}
}

// Retail fatal: Ordinal_10024(&DAT_6fdda728, Ordinal_10265(code)); _exit(-1). Seams are weak, as in d2common_batch_1a330.
__attribute__((weak)) uint32_t Ordinal_10265(uint32_t);
__attribute__((weak)) void Ordinal_10024(const void*, uint32_t);
__attribute__((weak)) char DAT_6fdda728[1];
[[noreturn]] static void UNITS_FatalCode(uint32_t code)
{
	if (Ordinal_10265 && Ordinal_10024)
		Ordinal_10024(DAT_6fdda728, Ordinal_10265(code));
	__builtin_trap();
}

// not-1.13c: d2-web callee table that binds the 1.13c item-component port to the 1.10f source functions; retail has no such table (it calls them directly)
const D2UnitsComponentSeams& UNITS_ComponentSeams()
{
	static const D2UnitsComponentSeams kSeams = {
		[](D2InventoryStrc* i, int g, D2InventoryGridInfoStrc* p) -> D2InventoryGridStrc* { return INVENTORY_GetGrid(i, g, p); },
		[](int n) -> uint32_t { return ITEMS_CheckWeaponIfTwoHandedByItemId(n); },
		[](D2UnitStrc* u) -> BOOL { return UNITS_CanDualWield(u); },
		[](D2InventoryStrc* i) -> D2UnitStrc* { return INVENTORY_GetLeftHandWeapon(i); },
		[](int n) -> D2ItemsTxt* { return DATATBLS_GetItemsTxtRecord(n); },
	};
	return kSeams;
}

extern D2InventoryGridInfoStrc gBodyLocInventoryGridInfo;   // D2Inventory.cpp

// The equipped item at a body location as 1.13c reads it inline: a valid inventory, the body-location grid, the slot,
// and an item that is broken or flagged unequippable counts as none.
static D2UnitStrc* UNITS_GetEquippedForComponent(D2InventoryStrc* pInventory, int nBodyLoc, const D2UnitsComponentSeams& s)
{
	if (!pInventory || !INVENTORY_CheckSignature(pInventory))
	{
		return NULL;
	}
	D2InventoryGridStrc* pGrid = s.getGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
	if (!pGrid)
	{
		return NULL;
	}
	D2UnitStrc* pItem = pGrid->ppItems[nBodyLoc];
	return ITEMS_CanBeEquipped(pItem) ? pItem : NULL;
}

//1.10f: D2Common.0x6FDC1F10 (#10442)
// 1.13c D2Common+0x31260 (#10921). 1.13c deltas: the hands are read from the body-location grid inline (one grid lookup
// per hand); the two-handed test is by class id and only for items; the class gate (barbarian, assassin, shadow
// warrior/master) is UNITS_CanDualWield; ITEMS_GetBodyLocation and ITEMS_GetComponent are inline, the latter fatal on a null
// item (0x635), a non-item (0x636) or a missing items.txt row (0x639).
uint8_t UNITS_GetItemComponentIdWith(D2UnitStrc* pUnit, D2UnitStrc* pItem, const D2UnitsComponentSeams& s)
{
	const uint8_t nBodyLoc = (pItem && pItem->dwUnitType == UNIT_ITEM && pItem->pItemData) ? pItem->pItemData->nBodyLoc : 0;
	if (nBodyLoc == BODYLOC_LARM || nBodyLoc == BODYLOC_RARM)
	{
		D2InventoryStrc* pInventory = pUnit->pInventory;
		D2UnitStrc* pRightHandItem = UNITS_GetEquippedForComponent(pInventory, BODYLOC_RARM, s);
		D2UnitStrc* pLeftHandItem = UNITS_GetEquippedForComponent(pInventory, BODYLOC_LARM, s);
		if (pRightHandItem && pLeftHandItem
			&& (pRightHandItem->dwUnitType != UNIT_ITEM || !s.twoHanded(pRightHandItem->dwClassId))
			&& (pLeftHandItem->dwUnitType != UNIT_ITEM || !s.twoHanded(pLeftHandItem->dwClassId))
			&& s.canDualWield(pUnit) && s.leftHandWeapon(pInventory) != pItem)
		{
			return 6;
		}
	}

	if (!pItem)
	{
		UNITS_FatalCode(0x635);
	}
	if (pItem->dwUnitType != UNIT_ITEM)
	{
		UNITS_FatalCode(0x636);
	}
	D2ItemsTxt* pItemsTxtRecord = s.itemsTxt(pItem->dwClassId);
	if (!pItemsTxtRecord)
	{
		UNITS_FatalCode(0x639);
	}
	return pItemsTxtRecord->nComponent;
}

// 1.13c D2Common+0x31260
uint8_t __stdcall UNITS_GetItemComponentId(D2UnitStrc* pUnit, D2UnitStrc* pItem)
{
	return UNITS_GetItemComponentIdWith(pUnit, pItem, UNITS_ComponentSeams());
}

//D2Common.0x6FDC1FE0
// 1.13c D2Common+0x13B0
D2MonStats2Txt* __fastcall UNITS_GetMonStats2TxtRecordFromMonsterId(int nMonsterId)
{
	// D2Common 1.13c +0x13B0: out-of-range monster id returns NULL
	D2MonStatsTxt* pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(nMonsterId);
	if (!pMonStatsTxt)
	{
		return NULL;
	}
	int nMonStatsEx = pMonStatsTxt->wMonStatsEx;

	if (nMonStatsEx >= 0 && nMonStatsEx < sgptDataTables->nMonStats2TxtRecordCount)
	{
		return &sgptDataTables->pMonStats2Txt[nMonStatsEx];
	}

	return NULL;
}

// 1.13c D2Common+0x306D0
void __stdcall UNITS_InitRightSkill(D2UnitStrc* pUnit)
{
	if (pUnit && pUnit->dwUnitType == UNIT_PLAYER)
	{
		if (pUnit->pPlayerData == nullptr)
			UNITS_FatalCode(0x13F6);

		D2SkillStrc* pSkill = UNITS_GetRightSkill(pUnit);
		pUnit->pPlayerData->nRightSkillId = pSkill ? (int32_t)pSkill->pSkillsTxt->nSkillId : 0;
		if (pSkill == nullptr)
			UNITS_FatalCode(0x638);
		pUnit->pPlayerData->nRightSkillFlags = pSkill->nOwnerGUID;
	}
}

// 1.13c D2Common+0x30670
void __stdcall UNITS_InitLeftSkill(D2UnitStrc* pUnit)
{
	if (pUnit && pUnit->dwUnitType == UNIT_PLAYER)
	{
		if (pUnit->pPlayerData == nullptr)
			UNITS_FatalCode(0x1405);

		D2SkillStrc* pSkill = UNITS_GetLeftSkill(pUnit);
		pUnit->pPlayerData->nLeftSkillId = pSkill ? (int32_t)pSkill->pSkillsTxt->nSkillId : 0;
		if (pSkill == nullptr)
			UNITS_FatalCode(0x638);
		pUnit->pPlayerData->nLeftSkillFlags = pSkill->nOwnerGUID;
	}
}

// 1.13c D2Common+0x30600
void __stdcall UNITS_InitSwitchRightSkill(D2UnitStrc* pUnit)
{
	if (pUnit && pUnit->dwUnitType == UNIT_PLAYER)
	{
		if (pUnit->pPlayerData == nullptr)
			UNITS_FatalCode(0x1414);

		D2SkillStrc* pSkill = UNITS_GetRightSkill(pUnit);
		pUnit->pPlayerData->nSwitchRightSkillId = pSkill ? (int32_t)pSkill->pSkillsTxt->nSkillId : 0;
		if (pSkill == nullptr)
			UNITS_FatalCode(0x638);
		pUnit->pPlayerData->nSwitchRightSkillFlags = pSkill->nOwnerGUID;
	}
}

// 1.13c D2Common+0x30590
void __stdcall UNITS_InitSwitchLeftSkill(D2UnitStrc* pUnit)
{
	if (pUnit && pUnit->dwUnitType == UNIT_PLAYER)
	{
		if (pUnit->pPlayerData == nullptr)
			UNITS_FatalCode(0x1424);

		D2SkillStrc* pSkill = UNITS_GetLeftSkill(pUnit);
		pUnit->pPlayerData->nSwitchLeftSkillId = pSkill ? (int32_t)pSkill->pSkillsTxt->nSkillId : 0;
		if (pSkill == nullptr)
			UNITS_FatalCode(0x638);
		pUnit->pPlayerData->nSwitchLeftSkillFlags = pSkill->nOwnerGUID;
	}
}

//D2Common.0x6FDC21F0 (#10447)
// 1.13c D2Common+0x2F800: non-player/missing data writes (0, -1); player reads nRightSkillId/nRightSkillFlags.
void __stdcall UNITS_GetRightSkillData(D2UnitStrc* pUnit, int* pRightSkillId, int* pRightSkillFlags)
{
	if (pUnit && pUnit->dwUnitType == UNIT_PLAYER)
	{
		D2_ASSERT(pUnit->pPlayerData);

		*pRightSkillId = pUnit->pPlayerData->nRightSkillId;
		*pRightSkillFlags = pUnit->pPlayerData->nRightSkillFlags;
	}
	else
	{
		*pRightSkillId = 0;
		*pRightSkillFlags = -1;
	}
}

// 1.13c D2Common+0x2F7A0
void __stdcall UNITS_GetLeftSkillData(D2UnitStrc* pUnit, int* pLeftSkillId, int* pLeftSkillFlags)
{
	if (pUnit && pUnit->dwUnitType == UNIT_PLAYER)
	{
		if (pUnit->pPlayerData == nullptr)
			UNITS_FatalCode(0x1449);

		*pLeftSkillId = pUnit->pPlayerData->nLeftSkillId;
		*pLeftSkillFlags = pUnit->pPlayerData->nLeftSkillFlags;
	}
	else
	{
		*pLeftSkillId = 0;
		*pLeftSkillFlags = -1;
	}
}

// 1.13c D2Common+0x2F720
void __stdcall UNITS_GetSwitchRightSkillDataResetRightSkill(D2UnitStrc* pUnit, int* pSwitchRightSkillId, int* pSwitchRightSkillFlags)
{
	if (pUnit && pUnit->dwUnitType == UNIT_PLAYER)
	{
		if (pUnit->pPlayerData == nullptr)
			UNITS_FatalCode(0x145A);

		*pSwitchRightSkillId = pUnit->pPlayerData->nSwitchRightSkillId;
		*pSwitchRightSkillFlags = pUnit->pPlayerData->nSwitchRightSkillFlags;

		pUnit->pPlayerData->nRightSkillId = 0;
		pUnit->pPlayerData->nRightSkillFlags = -1;
		pUnit->pPlayerData->nWeaponGUID = D2UnitInvalidGUID;
		pUnit->pPlayerData->unk0x94 = 0;
	}
	else
	{
		*pSwitchRightSkillId = 0;
		*pSwitchRightSkillFlags = -1;
	}
}

// 1.13c D2Common+0x2F6A0
void __stdcall UNITS_GetSwitchLeftSkillDataResetLeftSkill(D2UnitStrc* pUnit, int* pSwitchLeftSkillId, int* pSwitchLeftSkillFlags)
{
	if (pUnit && pUnit->dwUnitType == UNIT_PLAYER)
	{
		if (pUnit->pPlayerData == nullptr)
			UNITS_FatalCode(0x1470);

		*pSwitchLeftSkillId = pUnit->pPlayerData->nSwitchLeftSkillId;
		*pSwitchLeftSkillFlags = pUnit->pPlayerData->nSwitchLeftSkillFlags;

		pUnit->pPlayerData->nLeftSkillId = 0;
		pUnit->pPlayerData->nLeftSkillFlags = -1;
		pUnit->pPlayerData->nWeaponGUID = D2UnitInvalidGUID;
		pUnit->pPlayerData->unk0x94 = 0;
	}
	else
	{
		*pSwitchLeftSkillId = 0;
		*pSwitchLeftSkillFlags = -1;
	}
}

//D2Common.0x6FDC23B0 (#10451)
// 1.13c D2Common+0x2F630
void __stdcall UNITS_GetSwitchLeftSkillData(D2UnitStrc* pUnit, int* pSwitchLeftSkillId, int* pSwitchLeftSkillFlags)
{
	if (pUnit && pUnit->dwUnitType == UNIT_PLAYER)
	{
		if (pUnit->pPlayerData == nullptr)
			UNITS_FatalCode(0x1487);

		*pSwitchLeftSkillId = pUnit->pPlayerData->nSwitchLeftSkillId;
		*pSwitchLeftSkillFlags = pUnit->pPlayerData->nSwitchLeftSkillFlags;
	}
	else
	{
		*pSwitchLeftSkillId = 0;
		*pSwitchLeftSkillFlags = -1;
	}
}

// 1.13c D2Common+0x2F5C0 (#10756)
//D2Common.0x6FDC2420 (#10452)
void __stdcall UNITS_GetSwitchRightSkillData(D2UnitStrc* pUnit, int* pSwitchRightSkillId, int* pSwitchRightSkillFlags)
{
	if (pUnit && pUnit->dwUnitType == UNIT_PLAYER)
	{
		if (pUnit->pPlayerData == nullptr)
			UNITS_FatalCode(0x1499);

		*pSwitchRightSkillId = pUnit->pPlayerData->nSwitchRightSkillId;
		*pSwitchRightSkillFlags = pUnit->pPlayerData->nSwitchRightSkillFlags;
	}
	else
	{
		*pSwitchRightSkillId = 0;
		*pSwitchRightSkillFlags = -1;
	}
}

// 1.13c D2Common+0x2F570
void __stdcall UNITS_SetSwitchLeftSkill(D2UnitStrc* pUnit, int nSwitchLeftSkillId, int nSwitchLeftSkillFlags)
{
	if (pUnit && pUnit->dwUnitType == UNIT_PLAYER)
	{
		if (pUnit->pPlayerData == nullptr)
			UNITS_FatalCode(0x14A8);

		pUnit->pPlayerData->nSwitchLeftSkillId = nSwitchLeftSkillId;
		pUnit->pPlayerData->nSwitchLeftSkillFlags = nSwitchLeftSkillFlags;
	}
}

// 1.13c D2Common+0x2F520
void __stdcall UNITS_SetSwitchRightSkill(D2UnitStrc* pUnit, int nSwitchRightSkillId, int nSwitchRightSkillFlags)
{
	if (pUnit && pUnit->dwUnitType == UNIT_PLAYER)
	{
		if (pUnit->pPlayerData == nullptr)
			UNITS_FatalCode(0x14B7);

		pUnit->pPlayerData->nSwitchRightSkillId = nSwitchRightSkillId;
		pUnit->pPlayerData->nSwitchRightSkillFlags = nSwitchRightSkillFlags;
	}
}

// 1.13c D2Common+0x2F4B0
void __stdcall UNITS_SetWeaponGUID(D2UnitStrc* pUnit, D2UnitStrc* pWeapon)
{
	if (pUnit && pUnit->dwUnitType == UNIT_PLAYER)
	{
		if (pUnit->pPlayerData == nullptr)
			UNITS_FatalCode(0x14C5);

		if (pWeapon)
		{
			if (pWeapon->dwUnitType != UNIT_ITEM)
				UNITS_FatalCode(0x14C9);

			pUnit->pPlayerData->nWeaponGUID = pWeapon->dwUnitId;
		}
		else
		{
			pUnit->pPlayerData->nWeaponGUID = 0;
		}
	}
}

// 1.13c D2Common+0x2F460
D2UnitGUID __stdcall UNITS_GetWeaponGUID(D2UnitStrc* pUnit)
{
	if (pUnit == nullptr)
		UNITS_FatalCode(0xFB4);
	if (pUnit->dwUnitType != UNIT_PLAYER)
		UNITS_FatalCode(0xFB6);
	if (pUnit->pPlayerData == nullptr)
		UNITS_FatalCode(0x14D7);

	return pUnit->pPlayerData->nWeaponGUID;
}

// 1.13c D2Common+0x2E9C0
unsigned int __stdcall UNITS_GetStashGoldLimit(D2UnitStrc* pUnit)
{
	// 1.13c: the stash limit is a constant (the 1.10 level-scaled rule is gone).
	(void)pUnit;
	return 2500000;
}

// 1.13c D2Common+0x31200
BOOL __fastcall UNITS_CanSwitchAI(int nMonsterId)
{
	if (nMonsterId > -1 && nMonsterId < sgptDataTables->nMonStatsTxtRecordCount)
	{
		D2MonStatsTxt* pMonStatsTxtRecord = &sgptDataTables->pMonStatsTxt[nMonsterId];
		if (pMonStatsTxtRecord)
		{
			// Retail inlines the MonStats2 lookup (bounds-checked wMonStatsEx) inside its mode-flag helper (0x1400).
			uint32_t nModeFlag = 0;
			const int32_t nMonStatsEx = pMonStatsTxtRecord->wMonStatsEx;
			if (nMonStatsEx > -1 && nMonStatsEx < sgptDataTables->nMonStats2TxtRecordCount)
			{
				nModeFlag = sgptDataTables->pMonStats2Txt[nMonStatsEx].dwModeFlags & MONSTATS2MODEFLAG_WL;
			}

			// Can switch only if walking, not a boss and has the SwitchAI flag.
			if (nModeFlag != 0 && 0 == (pMonStatsTxtRecord->dwMonStatsFlags & MONSTATSFLAG_BOSS))
			{
				return 0 != (pMonStatsTxtRecord->dwMonStatsFlags & MONSTATSFLAG_SWITCHAI);
			}
		}
	}

	return FALSE;
}

//1.10f: D2Common.0x6FDC2720 (#10458)
// 1.13c D2Common+0x2E990
void __fastcall UNITS_SetTimerArg(D2UnitStrc* pUnit, D2TimerArgStrc* pTimerArg)
{
	if (pUnit == nullptr)
		UNITS_FatalCode(0x150B);

	pUnit->pTimerParams = pTimerArg;
}

// 1.13c D2Common+0x2E960
D2TimerArgStrc* __fastcall UNITS_GetTimerArg(D2UnitStrc* pUnit)
{
	if (pUnit == nullptr)
		UNITS_FatalCode(0x1513);

	return (D2TimerArgStrc*)pUnit->pTimerParams;
}

// 1.13c D2Common+0x2F410
void __stdcall UNITS_AllocStaticPath(D2UnitStrc* pUnit)
{
	if (!pUnit->pStaticPath)
	{
		pUnit->pStaticPath = D2_CALLOC_STRC_POOL(pUnit->pMemoryPool, D2StaticPathStrc);
	}
}

// 1.13c D2Common+0x2F3E0
//D2Common.0x6FDC27C0 (#10461)
void __stdcall UNITS_FreeStaticPath(D2UnitStrc* pUnit)
{
	if (pUnit->pStaticPath)
	{
		D2_FREE_POOL(pUnit->pMemoryPool, pUnit->pStaticPath);
	}
}

//D2Common.0x6FDC27F0 (#10462)
// 1.13c D2Common+0x30530
BOOL __stdcall UNITS_CanDualWield(D2UnitStrc* pUnit)
{
	if (pUnit)
	{
		switch (pUnit->dwUnitType)
		{
		case UNIT_PLAYER:
			if (pUnit->dwClassId == PCLASS_BARBARIAN || pUnit->dwClassId == PCLASS_ASSASSIN)
			{
				return TRUE;
			}
			break;

		case UNIT_MONSTER:
			if (pUnit->dwClassId == MONSTER_SHADOWWARRIOR || pUnit->dwClassId == MONSTER_SHADOWMASTER)
			{
				return TRUE;
			}
			break;
		default:
			break;
		}
	}

	return FALSE;
}

//1.10f: D2Common.0x6FDC2860 (#11238)
// 1.13c D2Common+0x311B0 (#10965): a monster (type 1) in mode 12 (dead) whose monstats2 row (0x13B0,
// class id in EAX) is present, has DeadCol clear (flag byte +6 tested against gdwBitMasks[3]) and
// CorpseSel set (flag byte +4 tested against gdwBitMasks[7]) is useable when 0x33D60 (the udead
// state mask) finds no such state on it. Retail tests single flag bytes; bits 19 and 7 of dwFlags.
BOOL __stdcall UNITS_IsCorpseUseable(D2UnitStrc* pUnit)
{
	if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER || pUnit->dwAnimMode != MONMODE_DEAD)
	{
		return FALSE;
	}

	D2MonStats2Txt* pMonStats2TxtRecord = UNITS_GetMonStats2TxtRecordFromMonsterId(pUnit->dwClassId);
	if (!pMonStats2TxtRecord
		|| (pMonStats2TxtRecord->nFlags[2] & (uint8_t)gdwBitMasks[MONSTATS2FLAGINDEX_DEADCOL - 16])
		|| !(pMonStats2TxtRecord->nFlags[0] & (uint8_t)gdwBitMasks[MONSTATS2FLAGINDEX_CORPSESEL]))
	{
		return FALSE;
	}

	return STATES_CheckStateMaskUdeadOnUnit(pUnit) == 0;
}

// not-1.13c: D2Common+0x7FCD0 with the two positions passed in (retail reads them inline from the units at +0x2C); same table, same branches
// D2Common_10399 with an explicit pair of game-coordinate positions. Keeping the size-adjusted table
// in one place lets the ordinary predicate and candidate enumeration share the same native rule.
static int D2Common_10399AtCoords(D2UnitStrc* pUnit1, D2UnitStrc* pUnit2, const D2CoordStrc& pCoords1, const D2CoordStrc& pCoords2)
{
	static const int dword_6FDD3200[] =
	{
		-1, -1, -1, 0, 2, 4, 6, 8,
		-1, -1, 0, 1, 2, 4, 6, 8,
		-1, 0, 0, 2, 3, 5, 7, 8,
		0, 1, 2, 2, 4, 5, 7, 8,
		2, 2, 3, 4, 5, 6, 7, 9,
		4, 4, 5, 5, 6, 7, 8, 9,
		6, 6, 7, 7, 7, 8, 10, 10,
		8, 8, 8, 8, 9, 9, 10, 11,
	};

	int nAbsoluteXDiff = abs(pCoords2.nX - pCoords1.nX);
	int nAbsoluteYDiff = abs(pCoords2.nY - pCoords1.nY);
	const int nSizeX1 = UNITS_GetUnitSizeX(pUnit1);
	const int nSizeX2 = UNITS_GetUnitSizeX(pUnit2);
	int nDist = 0;
	if (nAbsoluteXDiff >= 8 || nAbsoluteYDiff >= 8 || nSizeX1 >= 4 || nSizeX2 >= 4)
	{
		int nSizeDiff = ((unsigned int)nSizeX2 >> 1) + ((unsigned int)nSizeX1 >> 1);
		if (nSizeDiff < 0) nSizeDiff = -nSizeDiff;
		int nX = nAbsoluteXDiff - nSizeDiff;
		int nY = nAbsoluteYDiff - nSizeDiff;
		if (nX < 0) nX = 0;
		if (nY < 0) nY = 0;
		nDist = nY + 2 * nX;
		if (nX <= nY) nDist = nX + 2 * nY;
	}
	else
	{
		nDist = dword_6FDD3200[nAbsoluteXDiff + 8 * nAbsoluteYDiff];
		if (nDist >= 0)
		{
			if (nSizeX1 == 3 || nSizeX2 == 3) --nDist;
			if (nDist < 0) nDist = 0;
			if (nSizeX1 <= 1 || nSizeX2 <= 1) ++nDist;
		}
		else
		{
			return 0;
		}
	}
	return nDist;
}

// not-1.13c: the shared body of D2Common+0x31000 with the mover's coordinates passed in, because retail reads them from the unit at +0x2C (session candidate enumeration needs explicit coordinates)
static BOOL UNITS_IsObjectInInteractRangeAt(D2UnitStrc* pUnit, D2UnitStrc* pObject, const D2CoordStrc& pUnitCoords)
{
	if (!pObject || pObject->dwUnitType != UNIT_OBJECT) return FALSE;

	D2CoordStrc objectCoords = {};
	objectCoords.nX = UNITS_GetXPosition(pObject);
	objectCoords.nY = UNITS_GetYPosition(pObject);
	if (!D2Common_10399AtCoords(pUnit, pObject, pUnitCoords, objectCoords)) return TRUE;

	const int nSizeX2 = UNITS_GetUnitSizeX(pObject);
	const int nSizeY2 = UNITS_GetUnitSizeY(pObject);
	const int nX = objectCoords.nX - nSizeX2 / 2;
	const int nY = objectCoords.nY - nSizeY2 / 2;
	if (nSizeX2 >= 1 && nSizeY2 >= 1)
	{
		if (pUnitCoords.nX >= nX - 2 && pUnitCoords.nX <= nSizeX2 + nX + 2 &&
			pUnitCoords.nY >= nY - 2 && pUnitCoords.nY <= nSizeY2 + nY + 2)
		{
			// Retail reads the mover's SizeX here (D2Common+0x31000 calls 0x30F80, not 0x30EE0).
			const int nSizeX1 = UNITS_GetUnitSizeX(pUnit);
			if (nSizeX1 > 2) return TRUE;
			if (pUnitCoords.nY >= nY - 1)
			{
				if (pUnitCoords.nY <= nSizeY2 + nY + 1) return TRUE;
				if (pUnitCoords.nX >= nX - 1)
				{
					if (pUnitCoords.nX > nSizeX2 + nX + 1) return FALSE;
					return TRUE;
				}
			}
			else if (pUnitCoords.nX >= nX - 1 && pUnitCoords.nX <= nSizeX2 + nX + 1)
			{
				return TRUE;
			}
		}
		return FALSE;
	}
	if (pUnitCoords.nX < nX - 1 || pUnitCoords.nX > nX + 1 ||
		pUnitCoords.nY < nY - 1 || pUnitCoords.nY > nY + 1) return FALSE;
	return TRUE;
}

// not-1.13c: exported wrapper of the explicit-coordinate form of D2Common+0x31000 for the session candidate enumeration (monspawn_check.cpp)
BOOL __stdcall UNITS_IsObjectInInteractRangeAt(D2UnitStrc* pUnit, D2UnitStrc* pObject, int32_t nX, int32_t nY)
{
	if (!pObject || pObject->dwUnitType != UNIT_OBJECT) return FALSE;
	D2CoordStrc unitCoords = { nX, nY };
	return UNITS_IsObjectInInteractRangeAt(pUnit, pObject, unitCoords);
}

// 1.13c D2Common+0x31000
BOOL __stdcall UNITS_IsObjectInInteractRange(D2UnitStrc* pUnit, D2UnitStrc* pObject)
{
	if (!pObject || pObject->dwUnitType != UNIT_OBJECT) return FALSE;
	D2CoordStrc unitCoords = {};
	unitCoords.nX = UNITS_GetXPosition(pUnit);
	unitCoords.nY = UNITS_GetYPosition(pUnit);
	return UNITS_IsObjectInInteractRangeAt(pUnit, pObject, unitCoords.nX, unitCoords.nY);
}

//1.10f: D2Common.0x6FDC2C80
// 1.13c D2Common+0x12D0 (record id in EAX)
D2CharStatsTxt* __fastcall UNITS_GetCharStatsTxtRecord(int nRecordId)
{
	if (nRecordId >= 0 && nRecordId < sgptDataTables->nCharStatsTxtRecordCount)
	{
		return &sgptDataTables->pCharStatsTxt[nRecordId];
	}

	return NULL;
}

//1.10f: D2Common.0x6FDC2CB0 (#10399)
// 1.13c D2Common+0x7FCD0 (#10407)
//TODO: Find a name
int __stdcall D2Common_10399(D2UnitStrc* pUnit1, D2UnitStrc* pUnit2)
{
	D2CoordStrc coords1 = {}, coords2 = {};
	UNITS_GetCoords(pUnit1, &coords1);
	UNITS_GetCoords(pUnit2, &coords2);
	return D2Common_10399AtCoords(pUnit1, pUnit2, coords1, coords2);
}

//D2Common.0x6FDC2E40 (#10397)
// 1.13c D2Common+0x7FBD0
int __stdcall UNITS_GetDistanceToOtherUnit(D2UnitStrc* pUnit1, D2UnitStrc* pUnit2)
{
	D2CoordStrc pCoords1 = {};
	D2CoordStrc pCoords2 = {};
	int nDistanceX = 0;
	int nDistanceY = 0;
	int nSize = 0;
	int nX = 0;
	int nY = 0;

	UNITS_GetCoords(pUnit1, &pCoords1);
	UNITS_GetCoords(pUnit2, &pCoords2);

	nDistanceX = pCoords2.nX - pCoords1.nX;
	if (nDistanceX < 0)
	{
		nDistanceX = -nDistanceX;
	}

	nDistanceY = pCoords2.nY - pCoords1.nY;
	if (nDistanceY < 0)
	{
		nDistanceY = -nDistanceY;
	}

	nSize = UNITS_GetUnitSizeX(pUnit1) / -2 - UNITS_GetUnitSizeX(pUnit2) / 2;

	nX = nSize + nDistanceX;
	nY = nSize + nDistanceY;

	if (nX < 0)
	{
		nX = 0;
	}

	if (nY < 0)
	{
		nY = 0;
	}

	if (nX <= nY)
	{
		return (nX + 2 * nY) / 2;
	}
	else
	{
		return (nY + 2 * nX) / 2;
	}
}

//D2Common.0x6FDC2F50 (#10398)
// 1.13c D2Common+0x7F5E0 (same body as 0x2F1E0; a second copy in retail)
unsigned int __stdcall UNITS_GetDistanceToCoordinates(D2UnitStrc* pUnit, int nX, int nY)
{
	D2CoordStrc pCoords = {};
	int nDistanceX = 0;
	int nDistanceY = 0;

	UNITS_GetCoords(pUnit, &pCoords);

	nDistanceX = nX - pCoords.nX;
	if (nDistanceX < 0)
	{
		nDistanceX = -nDistanceX;
	}

	nDistanceY = nY - pCoords.nY;
	if (nDistanceY < 0)
	{
		nDistanceY = -nDistanceY;
	}

	if (nDistanceX < 0)
	{
		nDistanceX = 0;
	}
	
	if (nDistanceY < 0)
	{
		nDistanceY = 0;
	}

	if (nDistanceX <= nDistanceY)
	{
		return (nDistanceX + 2 * nDistanceY) / 2;
	}
	else
	{
		return (nDistanceY + 2 * nDistanceX) / 2;
	}
}

//D2Common.0x6FDC2FF0 (#10400)
// 1.13c D2Common+0x7F550
BOOL __stdcall UNITS_IsInRange(D2UnitStrc* pUnit, D2CoordStrc* pCoord, int nDistance)
{
	D2CoordStrc pCoords = {};
	int nDistanceX = 0;
	int nDistanceY = 0;

	UNITS_GetCoords(pUnit, &pCoords);

	nDistanceX = pCoord->nX - pCoords.nX;
	if (nDistanceX < 0)
	{
		nDistanceX = -nDistanceX;
	}

	nDistanceY = pCoord->nY - pCoords.nY;
	if (nDistanceY < 0)
	{
		nDistanceY = -nDistanceY;
	}

	return nDistanceX * nDistanceX + nDistanceY * nDistanceY <= nDistance * nDistance;
}

// not-1.13c: shared body of D2Common+0x7F6A0 (no excluded unit) and D2Common+0x7F910 (excludes a7); retail has two copies
//Used in D2Common.#10406 and D2Common.#10407
D2UnitStrc* __stdcall D2Common_10407_Impl(D2ActiveRoomStrc* pRoom, int nX, int nY, int(__fastcall* pCallback)(D2UnitStrc*, void*), void* a5, int a6, D2UnitStrc* a7)
{
	D2DrlgCoordsStrc pDrlgCoords = {};
	D2ActiveRoomStrc** ppRoomList = NULL;
	D2UnitStrc* pUnit = NULL;
	int nNumRooms = 0;
	int nAbsDiffX = 0;
	int nAbsDiffY = 0;
	int nUnitX = 0;
	int nUnitY = 0;
	int nSize = 0;

	if (!pRoom || a6 <= 0 || a6 >= 4)
	{
		return NULL;
	}

	DUNGEON_GetAdjacentRoomsListFromRoom(pRoom, &ppRoomList, &nNumRooms);

	if (nNumRooms <= 0)
	{
		return NULL;
	}

	for (int i = 0; i < nNumRooms; ++i)
	{
		DUNGEON_GetRoomCoordinates(ppRoomList[i], &pDrlgCoords);

		if (nX + 2 >= pDrlgCoords.nSubtileX || nX - 2 <= pDrlgCoords.nSubtileX + pDrlgCoords.nSubtileWidth)
		{
			if (nY + 2 >= pDrlgCoords.nSubtileY || nY - 2 <= pDrlgCoords.nSubtileY + pDrlgCoords.nSubtileHeight)
			{
				pUnit = ppRoomList[i]->pUnitFirst;
				while (pUnit)
				{
					if ((pUnit->dwUnitType == UNIT_PLAYER && pUnit->dwAnimMode != PLRMODE_DEATH && pUnit->dwAnimMode != PLRMODE_DEAD)
						|| (pUnit->dwUnitType == UNIT_MONSTER && pUnit->dwAnimMode != MONMODE_DEATH && pUnit->dwAnimMode != MONMODE_DEAD)
						|| pUnit->dwUnitType == UNIT_MISSILE)
					{
						nSize = UNITS_GetUnitSizeX(pUnit);

						if (nSize > 0)
						{
							if (nSize > 3)
							{
								nSize = 3;
							}

							nUnitX = UNITS_GetXPosition(pUnit);
							nUnitY = UNITS_GetYPosition(pUnit);

							switch (nSize + 3 * (a6 - 1))
							{
							case 1:
								if (nX == nUnitX && nY == nUnitY)
								{
									if (pUnit != a7 && pCallback(pUnit, a5))
									{
										return pUnit;
									}
								}
								break;

							case 2:
							case 4:
								nAbsDiffX = nX - nUnitX;
								if (nAbsDiffX < 0)
								{
									nAbsDiffX = -nAbsDiffX;
								}

								nAbsDiffY = nY - nUnitY;
								if (nY - nUnitY < 0)
								{
									nAbsDiffY = -nAbsDiffY;
								}

								if (nAbsDiffX + nAbsDiffY <= 1)
								{
									if (pUnit != a7 && pCallback(pUnit, a5))
									{
										return pUnit;
									}
								}
								break;

							case 5:
								nAbsDiffX = nX - nUnitX;
								if (nAbsDiffX < 0)
								{
									nAbsDiffX = -nAbsDiffX;
								}

								nAbsDiffY = nY - nUnitY;
								if (nAbsDiffY < 0)
								{
									nAbsDiffY = -nAbsDiffY;
								}

								if (nAbsDiffX + nAbsDiffY <= 2)
								{
									if (pUnit != a7 && pCallback(pUnit, a5))
									{
										return pUnit;
									}
								}
								break;

							case 3:
							case 7:
								if (nX >= nUnitX - 1 && nX <= nUnitX + 1 && nY >= nUnitY - 1 && nY <= nUnitY + 1)
								{
									if (pUnit != a7 && pCallback(pUnit, a5))
									{
										return pUnit;
									}
								}
								break;

							case 6:
							case 8:
								nAbsDiffX = nX - nUnitX;
								if (nAbsDiffX < 0)
								{
									nAbsDiffX = -nAbsDiffX;
								}

								nAbsDiffY = nY - nUnitY;
								if (nAbsDiffY < 0)
								{
									nAbsDiffY = -nAbsDiffY;
								}

								if ((nAbsDiffX <= 2 && nAbsDiffY <= 1) || (nAbsDiffY <= 2 && nAbsDiffX <= 1))
								{
									if (pUnit != a7 && pCallback(pUnit, a5))
									{
										return pUnit;
									}
								}
								break;

							case 9:
								if (nX >= nUnitX - 2 && nX <= nUnitX + 2 && nY >= nUnitY - 2 && nY <= nUnitY + 2)
								{
									if (pUnit != a7 && pCallback(pUnit, a5))
									{
										return pUnit;
									}
								}
								break;

							default:
								break;
							}
						}
					}

					pUnit = pUnit->pRoomNext;
				}
			}
		}
	}

	return NULL;
}

//D2Common.0x6FDC3090 (#10406)
// 1.13c D2Common+0x7F910
//TODO: Find a name
D2UnitStrc* __stdcall D2Common_10406(D2UnitStrc* pUnit, int (__fastcall* pCallback)(D2UnitStrc*, void*), void* a3)
{
	return D2Common_10407_Impl(UNITS_GetRoom(pUnit), UNITS_GetXPosition(pUnit), UNITS_GetYPosition(pUnit), pCallback, a3, UNITS_GetUnitSizeX(pUnit), pUnit);
}

//D2Common.0x6FDC33C0 (#10407)
// 1.13c D2Common+0x7F6A0
//TODO: Find a name
D2UnitStrc* __stdcall D2Common_10407(D2ActiveRoomStrc* pRoom, int nX, int nY, int (__fastcall* pCallback)(D2UnitStrc*, void*), void* a5, int a6)
{
	return D2Common_10407_Impl(pRoom, nX, nY, pCallback, a5, a6, NULL);
}

// 1.13c D2Common+0x1000: the unit in EAX, a/b/c on the stack. Re-applies the player guards and stores the notice
// record (1, a, b, c) at playerdata+0x150..0x15C (image offsets; unk0xA8[42..45] here). Nothing on any early exit.
void D2Common_1000_SetPlayerNotice(D2UnitStrc* pUnit, uint32_t a, uint32_t b, uint32_t c)
{
	if (!pUnit || pUnit->dwUnitType != UNIT_PLAYER || !pUnit->pPlayerData) return;
	uint32_t* p = &pUnit->pPlayerData->unk0xA8[42];
	p[0] = 1; p[1] = a; p[2] = b; p[3] = c;
}

// 1.13c D2Common+0x7F670 (sets the first notice word, then branches into D2Common+0x1000 for the record)
void __fastcall UNITS_SetInteractData(D2UnitStrc* pUnit, int nSkillId, int nUnitType, D2UnitGUID nUnitGUID)
{
	if (pUnit && pUnit->dwUnitType == UNIT_PLAYER)
	{
		if (pUnit->pPlayerData) pUnit->pPlayerData->unk0xA8[42] = 1;
		D2Common_1000_SetPlayerNotice(pUnit, nSkillId, nUnitType, nUnitGUID);
	}
}

// 1.13c D2Common+0x2F100 (unit in ESI, frame point on the stack): re-derive the sequence mode, frame and event
// from the unit's current sequence point; true when the mode changed.
BOOL __fastcall UNITS_RecomputeSeqMode(D2UnitStrc* pUnit, unsigned int nFramePoint)
{
	const unsigned int nOldMode = pUnit->dwSeqMode;
	unsigned int nMode = nFramePoint;
	unsigned int nFrame = 0;
	int nDirection = 0;
	int nEvent = 0;
	DATATBLS_ComputeSequenceAnimation(pUnit->pAnimSeq, pUnit->dwSeqFrame, (int)nFramePoint, &nMode, &nFrame, &nDirection, &nEvent);
	pUnit->nSeqCurrentFramePrecise = nFrame << 8;
	pUnit->dwSeqMode = nMode;
	pUnit->nActionFrame = (uint8_t)nEvent;
	return nMode != nOldMode;
}
