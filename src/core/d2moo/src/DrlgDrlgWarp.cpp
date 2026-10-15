#include "Drlg/D2DrlgDrlgWarp.h"

#include "D2DataTbls.h"
#include "Drlg/D2DrlgActivate.h"
#include "Drlg/D2DrlgDrlg.h"
#include "Drlg/D2DrlgDrlgRoom.h"
#include "Drlg/D2DrlgPreset.h"
#include "D2Seed.h"
#include <Units/Units.h>

// 1.13c D2Common+0x1A0D0
//D2Common.0x6FD78780
D2ActiveRoomStrc* __fastcall DRLGWARP_GetDestinationRoom(int nSourceLevel, D2DrlgRoomStrc* pDrlgRoom, int* pDestinationLevel, D2LvlWarpTxt** ppLvlWarpTxtRecord)
{	
	for (D2RoomTileStrc* pSourceRoomTile = pDrlgRoom->pRoomTiles; pSourceRoomTile; pSourceRoomTile = pSourceRoomTile->pNext)
	{
		if (pSourceRoomTile->pLvlWarpTxtRecord->dwLevelId == nSourceLevel)
		{			
			for (D2RoomTileStrc* pDestinationRoomTile = pSourceRoomTile->pDrlgRoom->pRoomTiles; pDestinationRoomTile; pDestinationRoomTile = pDestinationRoomTile->pNext)
			{
				if (pDestinationRoomTile->pDrlgRoom == pDrlgRoom)
				{
					*pDestinationLevel = pDestinationRoomTile->pLvlWarpTxtRecord->dwLevelId;
					*ppLvlWarpTxtRecord = pDestinationRoomTile->pLvlWarpTxtRecord;

					if (!pSourceRoomTile->pDrlgRoom->pRoom)
					{
						DRLGACTIVATE_InitializeRoomEx(pSourceRoomTile->pDrlgRoom);
					}

					return pSourceRoomTile->pDrlgRoom->pRoom;
				}
			}
			return NULL; // 1.13c: first matching source tile decides; 1.10f kept scanning
		}
	}

	return NULL;
}

//D2Common.0x6FD787F0
void __fastcall DRLGWARP_ToggleRoomTilesEnableFlag(D2DrlgRoomStrc* pDrlgRoom, BOOL bEnabled)
{
	for (D2RoomTileStrc* i = pDrlgRoom->pRoomTiles; i; i = i->pNext)
	{
		i->bEnabled = bEnabled;
	}
}

//D2Common.0x6FD78810
void __fastcall DRLGWARP_UpdateWarpRoomSelect(D2DrlgRoomStrc* pDrlgRoom, int nLevelId)
{
	for (D2RoomTileStrc* pRoomTile = pDrlgRoom->pRoomTiles; pRoomTile; pRoomTile = pRoomTile->pNext)
	{
		if (pRoomTile->pLvlWarpTxtRecord->dwLevelId == nLevelId)
		{
			if (pRoomTile->unk0x0C)
			{
				for (D2DrlgTileDataStrc* i = pRoomTile->unk0x0C; i; i = i->unk0x20)
				{
					i->dwFlags &= 0xFFFFFFF7;
				}

				for (D2DrlgTileDataStrc* i = pRoomTile->unk0x10; i; i = i->unk0x20)
				{
					i->dwFlags |= 8;
				}
			}
		}
	}
}

//D2Common.0x6FD78870
void __fastcall DRLGWARP_UpdateWarpRoomDeselect(D2DrlgRoomStrc* pDrlgRoom, int nLevelId)
{
	for (D2RoomTileStrc* pRoomTile = pDrlgRoom->pRoomTiles; pRoomTile; pRoomTile = pRoomTile->pNext)
	{
		if (pRoomTile->pLvlWarpTxtRecord->dwLevelId == nLevelId)
		{
			if (pRoomTile->unk0x10)
			{
				for (D2DrlgTileDataStrc* i = pRoomTile->unk0x10; i; i = i->unk0x20)
				{
					i->dwFlags &= 0xFFFFFFF7;
				}

				for (D2DrlgTileDataStrc* i = pRoomTile->unk0x0C; i; i = i->unk0x20)
				{
					i->dwFlags |= 8;
				}
			}
		}
	}
}

//D2Common.0x6FD788D0
//TODO: Name
D2ActiveRoomStrc* __fastcall sub_6FD788D0(D2DrlgStrc* pDrlg, int nLevelId, int nTileIndex, int* pX, int* pY)
{
	// Sanitizer-flagged: nTileIndex
	// (this function's parameter) and pLevel->pTileInfo[i].nTileIndex both index this table with no
	// bounds check in retail (confirmed in Ghidra: D2Common.dll 1.13c's function at VA 0x6FD69B60
	// reads the table at 0x6FDE4F48 by index, 8 bytes an entry, with no guard). pLevel->pTileInfo[]'s
	// own nTileIndex stays small (DrlgPreset.cpp assigns it 0-12), so every larger index seen here is
	// the nTileIndex PARAMETER: retail's own Levels.txt "WarpN" columns are passed straight through
	// (DUNGEON_FindActSpawnLocationEx -> LEVEL_WarpUnit/LEVEL_ChangeAct) with no reshaping, and that
	// table's own values reach 32 (Warp0 for the Arcane Sanctuary and the Pandemonium Fortress) up to
	// 67 (Act 3 Mephisto levels' Warp0) and 82 (Act 5's Baal Temple/Throne Room levels' Warp1, the
	// table's overall maximum) -- so this is retail's own data driving retail's own unchecked read,
	// not a computation this port got wrong. Retail's reads past index 13 land in its own neighbouring
	// .rdata: the C-string literals that follow this table in D2Common.dll ("armtype", "composit(e)",
	// "objmode", "objtype", the "xx_Dir" tile-type name list) through roughly index 40, then bytes that
	// read as a plausible small coordinate table -- likely an unrelated static array the linker placed
	// right after this one -- through index 77, then more string-shaped data to 82. Whatever it
	// actually is, it is retail's literal answer, byte for byte, not meaningful D2CoordStrc data -- so
	// entries 14-82 below are those same bytes (found by byte-pattern match at D2Common.dll 1.13c file
	// offset 0x94f48, VA 0x6fde4f48, image base 0x6fd50000; ~/d2-retail/D2Common.dll, byte-identical to
	// retail/D2/D2SE/CORES/1.13c/D2Common.dll here; 82 is Levels.txt's own highest WarpN value across
	// every level, so no further entries are needed), so an in-bounds read of THIS array gives
	// retail's exact undefined-behaviour result instead of whatever wasm happens to keep at +664.
	static const D2CoordStrc stru_6FDCE6A0[] =
	{
		{ 1, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 1, 1 },
		{ 0, 1 },
		{ 0, 1 },
		{ 0, 1 },
		{ 0, 1 },
		{ 0, 2 },
		{ 0, 3 },
		{ 0, 4 },
		{ 0, 5 },
		// -- retail's own neighbouring .rdata past this point, not real coordinates; see above --
		{ 1953329761, 6647929 },
		{ 1886220131, 1953067887 },
		{ 0, 1835688559 },
		{ 6644847, 1953129071 },
		{ 6647929, 1147096658 },
		{ 29289, 1147097427 },
		{ 29289, 1147093579 },
		{ 29289, 1147094084 },
		{ 29289, 1147090003 },
		{ 29289, 1147089747 },
		{ 29289, 1147089491 },
		{ 29289, 1147089235 },
		{ 29289, 1147093843 },
		{ 29289, 1147096130 },
		{ 29289, 1147089473 },
		{ 29289, 1147089217 },
		{ 29289, 1147095111 },
		{ 29289, 1147096151 },
		{ 29289, 1147098446 },
		{ 29289, 1147098180 },
		{ 29289, 1398550062 },
		{ 1668445551, 843340901 },
		{ 1835888451, 1146908271 },
		{ 1413567553, 1548962882 },
		{ 1416326996, 779316322 },
		{ 7368803, 1953655920 },
		{ 6647929, 1701539700 },
		{ 110, 0 },
		{ 6, 0 },
		{ 0, 2 },
		{ 2, 4 },
		{ 4, 6 },
		{ 6, 0 },
		{ 0, 2 },
		{ 2, 4 },
		{ 4, 6 },
		{ 2, 2 },
		{ 4, 4 },
		{ 6, 6 },
		{ 0, 0 },
		{ 2, 2 },
		{ 4, 4 },
		{ 6, 6 },
		{ 0, 0 },
		{ 2, 2 },
		{ 4, 4 },
		{ 6, 6 },
		{ 0, 0 },
		{ 2, 2 },
		{ 4, 4 },
		{ 6, 6 },
		{ 0, 0 },
		{ 6, 6 },
		{ 0, 0 },
		{ 2, 2 },
		{ 4, 4 },
		{ 6, 6 },
		{ 0, 0 },
		{ 2, 2 },
		{ 4, 4 },
		{ 7, 0 },
		{ 1, 6 },
		{ -1, 2 },
		{ 5, 4 },
		{ 3, 1398550062 },
		{ 1668445551, 843340901 },
		{ 1835888451, 1130131055 },
		{ 1551130984, 1952540739 },
		{ 1886413614, 0 },
	};

	D2LevelDefBin* pLevelDefBinRecord = NULL;
	D2DrlgLevelStrc* pLevel = NULL;
	D2DrlgRoomStrc* pDrlgRoom = NULL;
	unsigned int nMatches = 0;
	int nCounter = 0;
	int nFlags = 0;
	int nRand = 0;
	
	pLevelDefBinRecord = DATATBLS_GetLevelDefRecord(nLevelId);
	*pX = -1;
	*pY = -1;

	pLevel = DRLG_GetLevel(pDrlg, nLevelId);
	if (!pLevel->pFirstRoomEx)
	{
		DRLG_InitLevel(pLevel);
	}

	if (pLevelDefBinRecord->dwPosition)
	{
		if (nTileIndex == 13)
		{
			pDrlgRoom = DRLGWARP_GetWaypointRoomExFromLevel(pLevel, pX, pY);
			DRLGACTIVATE_InitializeRoomEx(pDrlgRoom);
			return pDrlgRoom->pRoom;
		}

		nMatches = 0;
		nRand = 0;
		for (int i = 0; i < pLevel->nTileInfo; ++i)
		{
			if (nTileIndex == pLevel->pTileInfo[i].nTileIndex || stru_6FDCE6A0[nTileIndex].nX && stru_6FDCE6A0[nTileIndex].nY == stru_6FDCE6A0[pLevel->pTileInfo[i].nTileIndex].nY)
			{
				++nMatches;
			}
		}

		if (nMatches)
		{
			nRand = SEED_RollLimitedRandomNumber(&pLevel->pSeed, nMatches) + 1;
		}

		for (int i = 0; i < pLevel->nTileInfo; ++i)
		{
			if (nTileIndex == pLevel->pTileInfo[i].nTileIndex || stru_6FDCE6A0[nTileIndex].nX && stru_6FDCE6A0[nTileIndex].nY == stru_6FDCE6A0[pLevel->pTileInfo[i].nTileIndex].nY)
			{
				--nRand;
			}

			if (nRand <= 0)
			{
				*pX = pLevel->pTileInfo[i].nPosX;
				*pY = pLevel->pTileInfo[i].nPosY;

				pDrlgRoom = DRLG_GetRoomExFromCoordinates(*pX, *pY, pLevel->pDrlg, 0, pLevel);
				DRLGACTIVATE_InitializeRoomEx(pDrlgRoom);
				return pDrlgRoom->pRoom;
			}
		}

		*pX = pLevel->pTileInfo[pLevel->nTileInfo].nPosX;
		*pY = pLevel->pTileInfo[pLevel->nTileInfo].nPosY;

		pDrlgRoom = DRLG_GetRoomExFromCoordinates(*pX, *pY, pLevel->pDrlg, 0, pLevel);
		DRLGACTIVATE_InitializeRoomEx(pDrlgRoom);
		return pDrlgRoom->pRoom;
	}

	pDrlgRoom = DRLGWARP_GetWaypointRoomExFromLevel(pLevel, pX, pY);
	if (!pDrlgRoom)
	{		
		for (D2DrlgRoomStrc* i = pLevel->pFirstRoomEx; i; i = i->pDrlgRoomNext)
		{
			if (i->dwFlags & DRLGROOMFLAG_HAS_WARP_MASK)
			{
				nFlags = DRLGROOMFLAG_HAS_WARP_0;
				nCounter = 0;
				do
				{
					if (nFlags & i->dwFlags && DRLGWARP_GetWarpIdArrayFromLevelId(pLevel->pDrlg, pLevel->nLevelId)[nCounter] != -1)
					{
						if (*pX == -1 || *pY == -1)
						{
							*pX = i->nTileXPos + i->nTileWidth / 2;
							*pY = i->nTileYPos + i->nTileHeight / 2;
						}

						DRLGACTIVATE_InitializeRoomEx(i);
						return i->pRoom;
					}

					nFlags *= 2;
					++nCounter;
				}
				while (nFlags & DRLGROOMFLAG_HAS_WARP_MASK);
			}
		}

		pDrlgRoom = DRLG_GetRoomExFromLevelAndCoordinates(pLevel, pLevel->nWidth / 2 + pLevel->nPosX - 2, pLevel->nHeight / 2 + pLevel->nPosY - 2);
		if (!pDrlgRoom)
		{
			D2_CHECK(pLevel->pFirstRoomEx);

			pDrlgRoom = pLevel->pFirstRoomEx;

			for (int i = SEED_RollLimitedRandomNumber(&pLevel->pSeed, pLevel->nRooms); i; --i)
			{
				pDrlgRoom = pDrlgRoom->pDrlgRoomNext;
			}
		}
	}

	if (pDrlgRoom)
	{
		if (*pX == -1 || *pY == -1)
		{
			*pX = pDrlgRoom->nTileXPos + pDrlgRoom->nTileWidth / 2;
			*pY = pDrlgRoom->nTileYPos + pDrlgRoom->nTileHeight / 2;
		}

		DRLGACTIVATE_InitializeRoomEx(pDrlgRoom);
		return pDrlgRoom->pRoom;
	}

	return NULL;
}

// Helper function
static D2DrlgRoomStrc* DRLG_FindWaypointRoom(D2DrlgLevelStrc* pLevel)
{
	for (D2DrlgRoomStrc* pDrlgRoom = pLevel->pFirstRoomEx; pDrlgRoom; pDrlgRoom = pDrlgRoom->pDrlgRoomNext)
	{
		if (DRLGROOM_CheckWaypointFlags(pDrlgRoom))
		{
			return pDrlgRoom;
		}
	}
	return nullptr;
}

//D2Common.0x6FD78C10
D2DrlgRoomStrc* __fastcall DRLGWARP_GetWaypointRoomExFromLevel(D2DrlgLevelStrc* pLevel, int* pX, int* pY)
{
	D2DrlgRoomStrc* pWaypointRoom = DRLG_FindWaypointRoom(pLevel);
	if (pWaypointRoom)
	{
		DRLGACTIVATE_InitializeRoomEx(pWaypointRoom);

		for (D2PresetUnitStrc* pPresetUnit = pWaypointRoom->pPresetUnits; pPresetUnit; pPresetUnit = pPresetUnit->pNext)
		{
			if (pPresetUnit->nUnitType == UNIT_OBJECT && pPresetUnit->nIndex < 573 && (DATATBLS_GetObjectsTxtRecord(pPresetUnit->nIndex)->nSubClass & OBJSUBCLASS_WAYPOINT))
			{
				*pX = pWaypointRoom->nTileXPos + pPresetUnit->nXpos / 5;
				*pY = pWaypointRoom->nTileYPos + pPresetUnit->nYpos / 5;
				return pWaypointRoom;
			}
		}
	}
	return pWaypointRoom;
}

//D2Common.0x6FD78CC0
int* __fastcall DRLGWARP_GetWarpIdArrayFromLevelId(D2DrlgStrc* pDrlg, int nLevelId)
{
	for (D2DrlgWarpStrc* pDrlgWarp = pDrlg->pWarp; pDrlgWarp; pDrlgWarp = pDrlgWarp->pNext)
	{
		if (!pDrlgWarp->nLevel)
		{
			FOG_DisplayWarning("ptVisInfo->eLevelId != LEVEL_ID_NONE", __FILE__, __LINE__);
		}

		if (nLevelId == pDrlgWarp->nLevel)
		{
			return pDrlgWarp->nWarp;
		}
	}

	return (int*)DATATBLS_GetLevelDefRecord(nLevelId)->dwWarp;
}

// 1.10f:D2Common.0x6FD78D10
// 1.11 :D2Common.0x6FD77400
int __fastcall DRLGWARP_GetWarpDestinationFromArray(D2DrlgLevelStrc* pLevel, uint8_t nArrayId)
{
	return DRLGWARP_GetWarpIdArrayFromLevelId(pLevel->pDrlg, pLevel->nLevelId)[nArrayId];
}

//D2Common.0x6FD78D80
D2LvlWarpTxt* __fastcall DRLGWARP_GetLvlWarpTxtRecordFromWarpIdAndDirection(D2DrlgLevelStrc* pLevel, uint8_t nWarpId, char szDirection)
{
	return DATATBLS_GetLvlWarpTxtRecordFromLevelIdAndDirection(DRLGWARP_GetWarpDestinationFromArray(pLevel, nWarpId), szDirection);
}

//D2Common.0x6FD78DF0
D2LvlWarpTxt* __fastcall DRLGWARP_GetLvlWarpTxtRecordFromUnit(D2DrlgRoomStrc* pDrlgRoom, D2UnitStrc* pUnit)
{
	D2RoomTileStrc* pRoomTile = pDrlgRoom->pRoomTiles;
	int nClassId = 0;

	if (pUnit)
	{
		nClassId = pUnit->dwClassId;
	}
	else
	{
		nClassId = -1;
	}

	while (pRoomTile)
	{
		if (pRoomTile->pLvlWarpTxtRecord->dwLevelId == nClassId && pRoomTile->bEnabled)
		{
			return pRoomTile->pLvlWarpTxtRecord;
		}

		pRoomTile = pRoomTile->pNext;
	}

	return NULL;
}
