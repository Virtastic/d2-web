#include "D2Dungeon.h"

#include "D2Collision.h"
#include "D2DataTbls.h"
#include <DataTbls/LevelsIds.h>
#include "Drlg/D2DrlgActivate.h"
#include "Drlg/D2DrlgDrlg.h"
#include "Drlg/D2DrlgDrlgAnim.h"
#include "Drlg/D2DrlgDrlgLogic.h"
#include "Drlg/D2DrlgDrlgRoom.h"
#include "Drlg/D2DrlgDrlgWarp.h"
#include "Drlg/D2DrlgPreset.h"
#include "D2Environment.h"
#include "Units/UnitRoom.h"
#include "Units/Units.h"
#include "D2Seed.h"
#include <test_seam.h>


//D2Common.0x6FD8B8A0 (#10038)
// 1.13c D2Common+0x3CB30
D2DrlgActStrc* __stdcall DUNGEON_AllocAct(uint8_t nAct, uint32_t nInitSeed, BOOL bClient, D2GameStrc* pGame, uint8_t nDifficulty, void* pMemPool, int nTownLevelId, AUTOMAPFN pfAutoMap, TOWNAUTOMAPFN pfTownAutoMap)
{
	D2DrlgActStrc* pAct = D2_CALLOC_STRC_POOL(pMemPool, D2DrlgActStrc);

	pAct->dwInitSeed = nInitSeed;
	pAct->bClient = bClient;
	pAct->nAct = nAct;
	pAct->pMemPool = pMemPool;

	if (!bClient)
	{
		pAct->nTownId = nTownLevelId;
	}

	pAct->pDrlg = DRLG_AllocDrlg(pAct, nAct, 0, nInitSeed, (bClient ? LEVEL_NONE : nTownLevelId), (bClient ? DRLGFLAG_ONCLIENT : 0), pGame, nDifficulty, pfAutoMap, pfTownAutoMap);
	pAct->pEnvironment = ENVIRONMENT_AllocDrlgEnvironment(pMemPool);

	DRLGANIM_InitCache(pAct->pDrlg, &pAct->pTileData);

	return pAct;
}

//D2Common.0x6FD8B950 (#10039)
// 1.13c D2Common+0x3C990
void __stdcall DUNGEON_FreeAct(D2DrlgActStrc* pAct)
{
	D2ActiveRoomStrc* pRoom = NULL;
	D2ActiveRoomStrc* pNext = NULL;

	D2_ASSERT(pAct);

	if (pAct->pDrlg)
	{
		DRLG_FreeDrlg(pAct->pDrlg);
		pAct->pDrlg = NULL;
	}

	pRoom = pAct->pRoom;
	while (pRoom)
	{
		pNext = pRoom->pRoomNext;
		DUNGEON_FreeRoom(pAct->pMemPool, pRoom);
		pRoom = pNext;
	}

	if (pAct->pEnvironment)
	{
		D2_FREE_POOL(pAct->pMemPool, pAct->pEnvironment);
		pAct->pEnvironment = NULL;
	}

	D2_FREE_POOL(pAct->pMemPool, pAct);
}

//D2Common.0x6FD8B9D0
// 1.13c D2Common+0x3BBC0 (act in EAX, returns act +0x5C)
void* __fastcall DUNGEON_GetMemPoolFromAct(D2DrlgActStrc* pAct)
{
	return pAct->pMemPool;
}

//D2Common.0x6FD8B9E0 (#10026)
// 1.13c D2Common+0x3BB80 (the room-tile loop is inlined there)
void __stdcall DUNGEON_ToggleRoomTilesEnableFlag(D2ActiveRoomStrc* pRoom, BOOL bEnabled)
{
	D2_ASSERT(pRoom);

	DRLGWARP_ToggleRoomTilesEnableFlag(pRoom->pDrlgRoom, bEnabled);
}

//D2Common.0x6FD8BA20 (#10027)
// 1.13c D2Common+0x3CD00
D2UnitStrc* __stdcall DUNGEON_GetWarpTileFromRoomAndSourceLevelId(D2ActiveRoomStrc* pRoom, int nSourceLevel, D2LvlWarpTxt** ppLvlWarpTxtRecord)
{
	D2_ASSERT(pRoom);

	int nDestinationLevel = 0;
	D2ActiveRoomStrc* pDestRoom = DRLGWARP_GetDestinationRoom(nSourceLevel, pRoom->pDrlgRoom, &nDestinationLevel, ppLvlWarpTxtRecord);
	D2_ASSERT(pDestRoom);

	for (D2UnitStrc* pTile = pDestRoom->pUnitFirst; pTile; pTile = pTile->pRoomNext)
	{
		if (pTile->dwUnitType == UNIT_TILE && pTile->dwClassId == nDestinationLevel)
		{
			return pTile;
		}
	}

	return nullptr;
}

//D2Common.0x6FD8BAB0 (#10028)
// 1.13c D2Common+0x3C280 (the room-tile lookup is inlined there)
D2LvlWarpTxt* __stdcall DUNGEON_GetLvlWarpTxtRecordFromRoomAndUnit(D2ActiveRoomStrc* pRoom, D2UnitStrc* pUnit)
{
	D2_ASSERT(pRoom);

	return DRLGWARP_GetLvlWarpTxtRecordFromUnit(pRoom->pDrlgRoom, pUnit);
}

//D2Common.0x6FD8BAF0 (#10030)
// 1.13c D2Common+0x3BB50
D2DrlgTileDataStrc* __stdcall DUNGEON_GetFloorTilesFromRoom(D2ActiveRoomStrc* pRoom, int* pFloorCount)
{
	if (pRoom && pRoom->pRoomTiles)
	{
		*pFloorCount = pRoom->pRoomTiles->nFloors;
		return pRoom->pRoomTiles->pFloorTiles;
	}

	*pFloorCount = 0;
	return NULL;
}

//D2Common.0x6FD8BB20 (#10031)
// 1.13c D2Common+0x3BB10
D2DrlgTileDataStrc* __stdcall DUNGEON_GetWallTilesFromRoom(D2ActiveRoomStrc* pRoom, int* pWallCount)
{
	D2_ASSERT(pRoom);

	*pWallCount = pRoom->pRoomTiles->nWalls;
	return pRoom->pRoomTiles->pWallTiles;
}

// 1.13c room-tile block (retail 32-bit offsets): pWallTiles/nWalls at 0x00/0x04, pFloorTiles/nFloors at
// 0x08/0x0C, pRoofTiles/nRoofs at 0x10/0x14. This build widens pointers, so pin the field order instead.
static_assert(offsetof(D2ActiveRoomStrc, pRoomTiles) == sizeof(D2DrlgCoordsStrc), "room-tiles block follows the coords");
// The struct is `#pragma pack(1)`: each widened pointer adds sizeof(void*) - 4 bytes before the field.
static_assert(offsetof(D2DrlgRoomTilesStrc, pRoofTiles) == 0x10 + 2 * (sizeof(void*) - 4), "1.13c room-tiles block+0x10");
static_assert(offsetof(D2DrlgRoomTilesStrc, nRoofs)     == 0x14 + 3 * (sizeof(void*) - 4), "1.13c room-tiles block+0x14");

//D2Common.0x6FD8BB60 (#10032)
// 1.13c D2Common+0x3BAD0
D2DrlgTileDataStrc* __stdcall DUNGEON_GetRoofTilesFromRoom(D2ActiveRoomStrc* pRoom, int* pRoofCount)
{
	D2_ASSERT(pRoom);

	*pRoofCount = pRoom->pRoomTiles->nRoofs;
	return pRoom->pRoomTiles->pRoofTiles;
}

//D2Common.0x6FD8BBA0 (#10033)
// 1.13c D2Common+0x3BAC0
D2DrlgTileDataStrc* __stdcall DUNGEON_GetTileDataFromAct(D2DrlgActStrc* pAct)
{
	return &pAct->pTileData;
}

//D2Common.0x6FD8BBB0 (#10034)
// 1.13c D2Common+0x3BA60: asserts the out pointer first; a null room zeroes all eight dwords
void __stdcall DUNGEON_GetRoomCoordinates(D2ActiveRoomStrc* pRoom, D2DrlgCoordsStrc* pCoords)
{
	if (D2Seam<&DUNGEON_GetRoomCoordinates>::hook) { D2Seam<&DUNGEON_GetRoomCoordinates>::hook(pRoom, pCoords); return; }
	D2_ASSERT(pCoords);

	if (pRoom)
	{
		memcpy(pCoords, &pRoom->tCoords, sizeof(D2DrlgCoordsStrc));
	}
	else
	{
		memset(pCoords, 0x00, sizeof(D2DrlgCoordsStrc));
	}
}

//D2Common.0x6FD8BC10 (#10035)
// 1.13c D2Common+0x3BA20: the count (room +0x24) goes through the third argument first, then the list head (room +0x00) through the second
void __stdcall DUNGEON_GetAdjacentRoomsListFromRoom(D2ActiveRoomStrc* pRoom, D2ActiveRoomStrc*** pppRoomList, int* pNumRooms)
{
	if (D2Seam<&DUNGEON_GetAdjacentRoomsListFromRoom>::hook) { D2Seam<&DUNGEON_GetAdjacentRoomsListFromRoom>::hook(pRoom, pppRoomList, pNumRooms); return; }
	D2_ASSERT(pRoom);

	*pNumRooms = pRoom->nNumRooms;
	*pppRoomList = pRoom->ppRoomList;
}

// 1.13c D2Common+0x3C200
// Prior art: the 1.10f source Dungeon.cpp's AllocRoom adjacency block. Retail factors
// allocation/reordering into this helper; pointers remain native-width here.
static void DUNGEON_InitializeAdjacentRooms(D2ActiveRoomStrc* pRoom)
{
	pRoom->ppRoomList = (D2ActiveRoomStrc**)D2_ALLOC_POOL(pRoom->pAct->pMemPool, sizeof(D2ActiveRoomStrc*) * pRoom->pDrlgRoom->nRoomsNear);
	pRoom->nNumRooms = DRLGROOM_ReorderNearRoomList(pRoom->pDrlgRoom, pRoom->ppRoomList);

	for (int i = 0; i < pRoom->nNumRooms; ++i)
	{
		D2ActiveRoomStrc* pAdjacentRoom = pRoom->ppRoomList[i];
		D2_ASSERT(pAdjacentRoom);

		if (pAdjacentRoom != pRoom)
		{
			pAdjacentRoom->nNumRooms = DRLGROOM_ReorderNearRoomList(pAdjacentRoom->pDrlgRoom, pAdjacentRoom->ppRoomList);
		}
	}
}

//D2Common.0x6FD8BC50
// 1.13c D2Common+0x3C720 (retail's body from the assert on: the low-seed init and the stack/EBP
// inputs come from its caller-side prologue at 0x3C6xx; fields, list-link order and the
// 3C200 -> 4D010 -> callback tail match)
D2ActiveRoomStrc* __fastcall DUNGEON_AllocRoom(D2DrlgActStrc* pAct, D2DrlgRoomStrc* pDrlgRoom, D2DrlgCoordsStrc* pDrlgCoords, D2DrlgRoomTilesStrc* pRoomTiles, int nLowSeed, uint32_t dwFlags)
{
	D2_ASSERT(pAct);

	D2ActiveRoomStrc* pRoom = D2_CALLOC_STRC_POOL(pAct->pMemPool, D2ActiveRoomStrc);

	pRoom->dwFlags = dwFlags;

	SEED_InitLowSeed(&pRoom->pSeed, nLowSeed);

	pRoom->pDrlgRoom = pDrlgRoom;
	memcpy(&pRoom->tCoords, pDrlgCoords, sizeof(D2DrlgCoordsStrc));
	pRoom->pRoomTiles = pRoomTiles;
	pRoom->pRoomNext = pAct->pRoom;
	pAct->pRoom = pRoom;
	pAct->bHasPendingRoomsUpdates = TRUE;
	pRoom->pAct = pAct;

	DRLGROOM_SetRoom(pDrlgRoom, pRoom);

	DUNGEON_InitializeAdjacentRooms(pRoom);

	COLLISION_AllocRoomCollisionGrid(pAct->pMemPool, pRoom);

	if (pAct->pfnActCallBack)
	{
		pAct->pfnActCallBack(pRoom);
	}

	return pRoom;
}

//D2Common.0x6FD8BD90 (#10040)
// 1.13c D2Common+0x3b970
BOOL __stdcall DUNGEON_DoRoomsTouchOrOverlap(D2ActiveRoomStrc* ptFirst, D2ActiveRoomStrc* ptSecond)
{
	D2_ASSERT(ptFirst);
	D2_ASSERT(ptSecond);

	return ptFirst->tCoords.nTileXPos <= ptSecond->tCoords.nTileXPos + ptSecond->tCoords.nTileWidth
		&& ptFirst->tCoords.nTileXPos + ptFirst->tCoords.nTileWidth >= ptSecond->tCoords.nTileXPos
		&& ptFirst->tCoords.nTileYPos <= ptSecond->tCoords.nTileYPos + ptSecond->tCoords.nTileHeight
		&& ptFirst->tCoords.nTileYPos + ptFirst->tCoords.nTileHeight >= ptSecond->tCoords.nTileYPos;
}

//D2Common.0x6FD8BE30 (#10043)
// 1.13c D2Common+0x3B910 (room tile rect is at +0x5C.. in retail; fields read by name here)
BOOL __stdcall DUNGEON_AreTileCoordinatesInsideRoom(D2ActiveRoomStrc* pRoom, int nX, int nY)
{
	D2_ASSERT(pRoom);

	return nX >= pRoom->tCoords.nTileXPos && nX < pRoom->tCoords.nTileXPos + pRoom->tCoords.nTileWidth
		&& nY >= pRoom->tCoords.nTileYPos && nY < pRoom->tCoords.nTileYPos + pRoom->tCoords.nTileHeight;
}

//D2Common.0x6FD8BE90 (#10048)
// 1.13c D2Common+0x3B860
int __stdcall DUNGEON_CheckRoomsOverlapping_BROKEN(D2ActiveRoomStrc* pPrimary, D2ActiveRoomStrc* pSecondary)
{
	//This was probably meant to check if 2 rooms are overlaping / adjacent, but pSecondary is unused.
	//In the end, this always returns 4 unless width or height is 0
	//Probably why this is not used since it's broken.
	D2_MAYBE_UNUSED(pSecondary);
	D2_ASSERT(pPrimary); // pRoom is pPrimary in original code

	if (pPrimary->tCoords.nTileWidth + pPrimary->tCoords.nTileXPos >= pPrimary->tCoords.nTileXPos
		&& pPrimary->tCoords.nTileYPos + pPrimary->tCoords.nTileHeight >= pPrimary->tCoords.nTileYPos)
	{
		if (pPrimary->tCoords.nTileXPos == pPrimary->tCoords.nTileWidth + pPrimary->tCoords.nTileXPos)
		{
			return 1;
		}
		else if (pPrimary->tCoords.nTileYPos == pPrimary->tCoords.nTileYPos + pPrimary->tCoords.nTileHeight)
		{
			return 3;
		}
		else
		{
			return 4;
		}
	}

	return 0;
}

//D2Commmon.0x6FD8BF00 (#10051)
// 1.13c D2Common+0x3C1B0 (no act test; the tile-rect test is inlined)
D2ActiveRoomStrc* __stdcall DUNGEON_FindRoomByTileCoordinates(D2DrlgActStrc* pAct, int nX, int nY)
{
	for (D2ActiveRoomStrc* pRoom = pAct->pRoom; pRoom; pRoom = pRoom->pRoomNext)
	{
		if(DUNGEON_AreTileCoordinatesInsideRoom(pRoom, nX, nY))
		{
			return pRoom;
		}
	}
	return nullptr;
}

//D2Common.0x6FD8BF50 (#10050)
// 1.13c D2Common+0x3C120
D2ActiveRoomStrc* __stdcall DUNGEON_GetAdjacentRoomByTileCoordinates(D2ActiveRoomStrc* pRoom, int nX, int nY)
{
	D2ActiveRoomStrc* pTemp = NULL;

	D2_ASSERT(pRoom);

	for (int i = 0; i < pRoom->nNumRooms; ++i)
	{
		pTemp = pRoom->ppRoomList[i];
		D2_ASSERT(pTemp);
		if (DUNGEON_AreTileCoordinatesInsideRoom(pTemp, nX, nY))
		{
			return pTemp;
		}
	}

	return NULL;
}

//D2Common.0x6FD8BFF0 (#10049)
// 1.13c D2Common+0x3B7D0
void __stdcall DUNGEON_CallRoomCallback(D2ActiveRoomStrc* pRoom, ROOMCALLBACKFN pfnRoomCallback, void* pArgs)
{
	D2_ASSERT(pRoom);

	D2_ASSERT(pfnRoomCallback); // d2-web: IsBadCodePtr is Win32-only

	for (int i = 0; i < pRoom->nNumRooms; ++i)
	{
		if (!pfnRoomCallback(pRoom->ppRoomList[i], pArgs))
		{
			break;
		}
	}
}

//D2Common.0x6FD8C080 (#10052)
// 1.13c D2Common+0x3C070 (the four draw-position conversions are inlined there)
void __stdcall D2Common_10052(D2ActiveRoomStrc* pRoom, RECT* pRect)
{
	int nTemp = 0;

	D2_ASSERT(pRoom);

	DUNGEON_GameToClientTileDrawPositionCoords(pRoom->tCoords.nTileXPos                            , pRoom->tCoords.nTileYPos                             , &nTemp             , (int*)&pRect->top);
	DUNGEON_GameToClientTileDrawPositionCoords(pRoom->tCoords.nTileXPos                            , pRoom->tCoords.nTileHeight + pRoom->tCoords.nTileYPos, (int*)&pRect->left , &nTemp);
	DUNGEON_GameToClientTileDrawPositionCoords(pRoom->tCoords.nTileXPos + pRoom->tCoords.nTileWidth, pRoom->tCoords.nTileYPos                             , (int*)&pRect->right, &nTemp);
	DUNGEON_GameToClientTileDrawPositionCoords(pRoom->tCoords.nTileXPos + pRoom->tCoords.nTileWidth, pRoom->tCoords.nTileHeight + pRoom->tCoords.nTileYPos, &nTemp             , (int*)&pRect->bottom);

	D2_ASSERT(pRect->top <= pRect->bottom);
	D2_ASSERT(pRect->left <= pRect->right);
}

//D2Common.0x6FD8C170 (#10053)
// 1.13c D2Common+0x3B760
void __stdcall DUNGEON_GetSubtileRect(D2ActiveRoomStrc* pRoom, RECT* pRect)
{
	D2_ASSERT(pRoom);

	pRect->top = pRoom->tCoords.nSubtileY;
	pRect->bottom = pRoom->tCoords.nSubtileY + pRoom->tCoords.nSubtileHeight;
	pRect->left = pRoom->tCoords.nSubtileX;
	pRect->right = pRoom->tCoords.nSubtileWidth + pRoom->tCoords.nSubtileX;

	D2_ASSERT(pRect->top <= pRect->bottom);
	D2_ASSERT(pRect->left <= pRect->right);
}

//D2Common.0x6FD8C210 (#10054)
// 1.13c D2Common+0x3C6A0
void __stdcall DUNGEON_GetRGB_IntensityFromRoom(D2ActiveRoomStrc* pRoom, uint8_t* pIntensity, uint8_t* pRed, uint8_t* pGreen, uint8_t* pBlue)
{
	return D2_CALL(DRLGROOM_GetRGB_IntensityFromRoomEx, pRoom->pDrlgRoom, pIntensity, pRed, pGreen, pBlue);
}

//D2Common.0x6FD8C240 (#10041)
// 1.13c D2Common+0x3B710
// Cached retail: null act, linked-list order, inclusive lower/exclusive upper bounds.
D2ActiveRoomStrc* __stdcall DUNGEON_FindRoomBySubtileCoordinates(D2DrlgActStrc* pAct, int nX, int nY)
{
	if (D2Seam<&DUNGEON_FindRoomBySubtileCoordinates>::hook) return D2Seam<&DUNGEON_FindRoomBySubtileCoordinates>::hook(pAct, nX, nY);
	if (pAct)
	{
		for (D2ActiveRoomStrc* pRoom = pAct->pRoom; pRoom; pRoom = pRoom->pRoomNext)
		{
			if (DungeonTestRoomGame(pRoom, nX, nY))
			{
				return pRoom;
			}
		}
	}

	return NULL;
}

//D2Common.0x6FD8C2F0 (#10046)
// 1.13c D2Common+0x3CEE0
D2ActiveRoomStrc* __stdcall DUNGEON_FindActSpawnLocation(D2DrlgActStrc* pAct, int nLevelId, int nTileIndex, int* pX, int* pY)
{
	D2_ASSERT(pAct);

	return sub_6FD788D0(pAct->pDrlg, nLevelId, nTileIndex, pX, pY);
}

//D2Common.0x6FD8C340 (#10045)
// 1.13c D2Common+0x3CDF0: the final room pick is the 0x1330 search, which skips null neighbours
D2ActiveRoomStrc* __stdcall DUNGEON_FindActSpawnLocationEx(D2DrlgActStrc* pAct, int nLevelId, int nTileIndex, int* pX, int* pY, int nUnitSize)
{
	D2ActiveRoomStrc* pRoom = NULL;
	D2CoordStrc pCoord = {};
	int nX = 0;
	int nY = 0;

	D2_ASSERT(pAct);

	pRoom = sub_6FD788D0(pAct->pDrlg, nLevelId, nTileIndex, &nX, &nY);
	if (pRoom)
	{
		DUNGEON_GameTileToSubtileCoords(&nX, &nY);

		pCoord.nX = nX + 3;
		pCoord.nY = nY + 3;

		if (!COLLISION_GetFreeCoordinates(pRoom, &pCoord, nUnitSize, 0x1C09, 0))
		{
			FOG_DisplayAssert("Unable to find spawn location for act start", __FILE__, __LINE__);
			exit(-1);
		}

		*pX = pCoord.nX;
		*pY = pCoord.nY;

		return DUNGEON_GetRoomAtPosition(pRoom, *pX, *pY);
	}

	return NULL;
}

//D2Common.0x6FD8C4A0 (#10029)
// 1.13c D2Common+0x3C640
D2UnitStrc* __stdcall DUNGEON_GetFirstUnitInRoom(D2ActiveRoomStrc* pRoom)
{
	D2_ASSERT(pRoom);

	UNITROOM_SortUnitListByTargetY(pRoom);

	return pRoom->pUnitFirst;
}

//D2Common.0x6FD8C4E0 (#10100)
// 1.13c D2Common+0x3B6B0
void __stdcall DUNGEON_IncreaseAlliedCountOfRoom(D2ActiveRoomStrc* pRoom)
{
	if (pRoom)
	{
		++pRoom->nAllies;
	}
}

//D2Comon.0x6FD8C4F0 (#10036)
// 1.13c D2Common+0x3B690
int __stdcall DUNGEON_GetAlliedCountFromRoom(D2ActiveRoomStrc* pRoom)
{
	if (pRoom)
	{
		return pRoom->nAllies;
	}

	return 0;
}

//D2Common.0x6FD8C510 (#10101)
// 1.13c D2Common+0x3B650
void __stdcall DUNGEON_DecreaseAlliedCountOfRoom(D2ActiveRoomStrc* pRoom)
{
	if (pRoom)
	{
		D2_ASSERT(pRoom->nAllies > 0);

		--pRoom->nAllies;
	}
}

//D2Common.0x6FD8C550
// 1.13c D2Common+0x3B620 (room in EAX, assert on null, returns room +0x74)
D2UnitStrc** __fastcall DUNGEON_GetUnitListFromRoom(D2ActiveRoomStrc* pRoom)
{
	D2_ASSERT(pRoom);

	return &pRoom->pUnitFirst;
}

//D2Common.0x6FD8C580
// 1.13c D2Common+0x3B5E0
D2UnitStrc** __fastcall DUNGEON_GetUnitUpdateListFromRoom(D2ActiveRoomStrc* pRoom, BOOL bUpdate)
{
	D2_ASSERT(pRoom);

	if (bUpdate)
	{
		pRoom->pAct->bHasPendingUnitListUpdates = TRUE;
	}

	return &pRoom->pUnitUpdate;
}

//D2Common.0x6FD8C5C0 (#10055)
// 1.13c D2Common+0x3C020 (the preset-unit read is inlined there)
D2PresetUnitStrc* __stdcall DUNGEON_GetPresetUnitsFromRoom(D2ActiveRoomStrc* pRoom)
{
	D2_ASSERT(pRoom);

	return DRLGROOM_GetPresetUnits(pRoom->pDrlgRoom);
}

//D2Common.0x6FD8C600
// 1.13c D2Common+0x3B5B0 (room in EAX, assert on null, returns room +0x20)
D2RoomCollisionGridStrc* __fastcall DUNGEON_GetCollisionGridFromRoom(D2ActiveRoomStrc* pRoom)
{
	D2_ASSERT(pRoom);

	return pRoom->pCollisionGrid;
}

//D2Common.0x6FD8C630
// 1.13c D2Common+0x3B580
void __fastcall DUNGEON_SetCollisionGridInRoom(D2ActiveRoomStrc* pRoom, D2RoomCollisionGridStrc* pCollisionGrid)
{
	D2_ASSERT(pRoom);

	pRoom->pCollisionGrid = pCollisionGrid;
}

//D2Common.0x6FD8C660 (#10063)
// 1.13c D2Common+0x3CCA0 (its callee 0x7C300 sets the status only when the room's count is zero)
void __stdcall DUNGEON_SetClientIsInSight(D2DrlgActStrc* pAct, int nLevelId, int nX, int nY, D2ActiveRoomStrc* pRoom)
{
	D2_ASSERT(pAct);

	DRLGACTIVATE_SetClientIsInSight(pAct->pDrlg, nLevelId, nX, nY, pRoom ? pRoom->pDrlgRoom : nullptr);
}

//D2Common.0x6FD8C6B0 (#10064)
// 1.13c D2Common+0x3CBE0 (its callee 0x7BF20 is the unset-and-propagate path)
void __stdcall DUNGEON_UnsetClientIsInSight(D2DrlgActStrc* pAct, int nLevelId, int nX, int nY, D2ActiveRoomStrc* pRoom)
{
	D2_ASSERT(pAct);

	DRLGACTIVATE_UnsetClientIsInSight(pAct->pDrlg, nLevelId, nX, nY, pRoom ? pRoom->pDrlgRoom : nullptr);
}

//D2Common.0x6FD8C700 (#10062)
// 1.13c D2Common+0x3CC40
void __stdcall DUNGEON_ChangeClientRoom(D2ActiveRoomStrc* pRoom1, D2ActiveRoomStrc* pRoom2)
{
	DRLGACTIVATE_ChangeClientRoom(pRoom1 ? pRoom1->pDrlgRoom : nullptr, pRoom2 ? pRoom2->pDrlgRoom : nullptr);
}

//D2Common.0x6FD8C730 (#10065)
// 1.13c D2Common+0x3CDA0
D2ActiveRoomStrc* __stdcall DUNGEON_StreamRoomAtCoords(D2DrlgActStrc* pAct, int nX, int nY)
{
	if (D2Seam<&DUNGEON_StreamRoomAtCoords>::hook) return D2Seam<&DUNGEON_StreamRoomAtCoords>::hook(pAct, nX, nY);
	D2_ASSERT(pAct);

	return DRLGACTIVATE_StreamRoomAtCoords(pAct->pDrlg, nX, nY);
}

//D2Common.0x6FD8C770 (#10056)
// 1.13c D2Common+0x3B550
D2ActiveRoomStrc* __stdcall DUNGEON_GetRoomFromAct(D2DrlgActStrc* pAct)
{
	if (D2Seam<&DUNGEON_GetRoomFromAct>::hook) return D2Seam<&DUNGEON_GetRoomFromAct>::hook(pAct);
	D2_ASSERT(pAct);

	return pAct->pRoom;
}

//D2Common.0x6FD8C7A0 (#10057)
// 1.13c D2Common+0x3C000 (null room -> 0, else room->pDrlgRoom->pLevel->nLevelId with no further null test)
int __stdcall DUNGEON_GetLevelIdFromRoom(D2ActiveRoomStrc* pRoom)
{
	if (D2Seam<&DUNGEON_GetLevelIdFromRoom>::hook) return D2Seam<&DUNGEON_GetLevelIdFromRoom>::hook(pRoom);
	if (pRoom)
	{
		return DRLGROOM_GetLevelId(pRoom->pDrlgRoom);
	}

	return 0;
}

//D2Common.0x6FD8C7C0 (#10058)
// 1.13c D2Common+0x3CD80
int __stdcall DUNGEON_GetWarpDestinationLevel(D2ActiveRoomStrc* pRoom, int nSourceLevel)
{
	if (pRoom)
	{
		return D2_CALL(DRLGROOM_GetWarpDestinationLevel, pRoom->pDrlgRoom, nSourceLevel);
	}

	return 0;
}

//D2Common.0x6FD8C7E0 (#10059)
// 1.13c D2Common+0x3BFB0
// Cached retail inlines the populated-room check and aborts on a missing
// room-ex. Keep the 0x800000 flag and level field through native pointers.
int __stdcall DUNGEON_GetLevelIdFromPopulatedRoom(D2ActiveRoomStrc* pRoom)
{
	if (D2Seam<&DUNGEON_GetLevelIdFromPopulatedRoom>::hook) return D2Seam<&DUNGEON_GetLevelIdFromPopulatedRoom>::hook(pRoom);
	if (pRoom)
	{
		D2DrlgRoomStrc* pDrlgRoom = pRoom->pDrlgRoom;
		D2_ASSERT(pDrlgRoom);
		if (!(pDrlgRoom->dwFlags & DRLGROOMFLAG_POPULATION_ZERO))
		{
			return pDrlgRoom->pLevel->nLevelId;
		}
	}

	return 0;
}

// Layout note (function list 0x3BFB0): retail 1.13c reads room+0x10 (D2DrlgRoomStrc), +0x28 flags, +0x58 pLevel,
// level+0x1D0 nLevelId. Those are retail 32-bit offsets of a reordered struct; this build keeps the 1.10f
// the 1.10f source layouts, widened to native pointers, and every reader/writer of them uses the same headers, so the
// fields are consistent among themselves. Pin the semantic facts the function relies on.
static_assert(DRLGROOMFLAG_POPULATION_ZERO == 0x800000, "1.13c D2Common+0x3BFB0 tests flag 0x800000");
static_assert(offsetof(D2DrlgRoomStrc, pLevel) == 0x00, "our D2DrlgRoomStrc.pLevel (retail +0x58)");
static_assert(offsetof(D2DrlgRoomStrc, dwFlags) == sizeof(void*) + 0x10, "our D2DrlgRoomStrc.dwFlags (retail +0x28)");
static_assert(offsetof(D2DrlgLevelStrc, nLevelId) == sizeof(void*), "our D2DrlgLevelStrc.nLevelId (retail +0x1D0)");

//D2Common.0x6FD8C800 (#10060)
BOOL __stdcall DUNGEON_HasWaypoint(D2ActiveRoomStrc* pRoom)
{
	D2_ASSERT(pRoom);

	return DRLGROOM_HasWaypoint(pRoom->pDrlgRoom);
}

//D2Common.0x6FD8C840 (#10061)
// 1.13c D2Common+0x3BF20
const char* __stdcall DUNGEON_GetPickedLevelPrestFilePathFromRoom(D2ActiveRoomStrc* pRoom)
{
	if (pRoom)
	{
		return DRLGROOM_GetPickedLevelPrestFilePathFromRoomEx(pRoom->pDrlgRoom);
	}

	return NULL;
}

//D2Common.0x6FD8C860 (#10066)
// 1.13c D2Common+0x3B500
void __stdcall DUNGEON_AllocDrlgDelete(D2ActiveRoomStrc* pRoom, int nUnitType, D2UnitGUID nUnitGUID)
{
	if (D2Seam<&DUNGEON_AllocDrlgDelete>::hook) { D2Seam<&DUNGEON_AllocDrlgDelete>::hook(pRoom, nUnitType, nUnitGUID); return; }
	D2DrlgDeleteStrc* pDrlgDelete = NULL;

	if (pRoom)
	{
		pDrlgDelete = D2_ALLOC_STRC_POOL(pRoom->pAct->pMemPool, D2DrlgDeleteStrc);
		pDrlgDelete->nUnitType = nUnitType;
		pDrlgDelete->nUnitGUID = nUnitGUID;
		pDrlgDelete->pNext = pRoom->pDrlgDelete;
		pRoom->pDrlgDelete = pDrlgDelete;
		pRoom->pAct->bHasPendingRoomDeletions = TRUE;
	}
}

//D2Common.0x6FD8C8B0 (#10067)
// 1.13c D2Common+0x3B4A0
void __stdcall DUNGEON_FreeDrlgDelete(D2ActiveRoomStrc* pRoom)
{
	D2DrlgDeleteStrc* pNext = NULL;

	D2_ASSERT(pRoom);

	for (D2DrlgDeleteStrc* pDrlgDelete = pRoom->pDrlgDelete; pDrlgDelete; pDrlgDelete = pNext)
	{
		pNext = pDrlgDelete->pNext;
		D2_FREE_POOL(pRoom->pAct->pMemPool, pDrlgDelete);
	}

	pRoom->pDrlgDelete = NULL;
}

//D2Common.0x6FD8C910 (#10068)
// 1.13c D2Common+0x3B470
D2DrlgDeleteStrc* __stdcall DUNGEON_GetDrlgDeleteFromRoom(D2ActiveRoomStrc* pRoom)
{
	D2_ASSERT(pRoom);

	return pRoom->pDrlgDelete;
}

//D2Common.0x6FD8C940 (#10069)
// 1.13c D2Common+0x3C5E0
D2ActiveRoomStrc* __stdcall DUNGEON_GetARoomInClientSight(D2DrlgActStrc* pAct)
{
	D2_ASSERT(pAct);

	return DRLGACTIVATE_GetARoomInClientSight(pAct->pDrlg);
}

//D2Common.0x6FD8C980 (#10070)
// 1.13c D2Common+0x3C570
D2ActiveRoomStrc* __stdcall DUNGEON_GetARoomInSightButWithoutClient(D2DrlgActStrc* pAct, D2ActiveRoomStrc* pRoom)
{
	D2_ASSERT(pAct);

	D2_ASSERT(pRoom);

	return DRLGACTIVATE_GetARoomInSightButWithoutClient(pAct->pDrlg, pRoom->pDrlgRoom);
}

//D2Common.0x6FD8C9E0 (#10071)
// 1.13c D2Common+0x3C520
BOOL __stdcall DUNGEON_TestRoomCanUnTile(D2DrlgActStrc* pAct, D2ActiveRoomStrc* pRoom)
{
	D2_ASSERT(pAct);
	D2_ASSERTM(!pAct->bClient, "Only servers should use DungeonTestRoomCanUnTile.");
	D2_ASSERT(pRoom);

	return DRLGACTIVATE_TestRoomCanUnTile(pRoom->pDrlgRoom);
}

//D2Common.0x6FD8CA60 (#10072)
// 1.13c D2Common+0x3BF00
bool __stdcall DUNGEON_GetRoomStatusFlags(D2ActiveRoomStrc* pRoom)
{
	return DRLGACTIVATE_GetRoomStatusFlags(pRoom->pDrlgRoom) >= 3;
}

//D2Common.0x6FD8CA80 (#10073)
// 1.13c D2Common+0x3BE90
BOOL __stdcall D2Common_10073(D2ActiveRoomStrc* pRoom)
{
	D2_ASSERT(pRoom);

	if (pRoom->nNumRooms == pRoom->pDrlgRoom->nRoomsNear)
	{
		for (int i = 0; i < pRoom->nNumRooms; ++i)
		{
			if (!(pRoom->ppRoomList[i]->dwFlags & 1))
			{
				return FALSE;
			}
		}

		return TRUE;
	}

	return FALSE;
}

//D2Common.0x6FD8CAE0 (#10074)
// 1.13c D2Common+0x3B440
BOOL __stdcall D2Common_10074(D2ActiveRoomStrc* pRoom)
{
	D2_ASSERT(pRoom);

	return pRoom->dwFlags & 2;
}

//D2Common.0x6FD8CB10 (#10075)
// 1.13c D2Common+0x3B3F0
void __stdcall D2Common_10075(D2ActiveRoomStrc* pRoom, BOOL bSet)
{
	D2_ASSERT(pRoom);

	if (bSet)
	{
		pRoom->dwFlags |= 2;
	}
	else
	{
		pRoom->dwFlags &= 0xFFFFFFFD;
	}
}

// 1.13c D2Common+0x3B3A0 (room in ECX): every entry of the room's client list must be set (1.13c shares one body between
// its three callers; the assert reports and exits on a null entry)
BOOL __fastcall DUNGEON_ValidateClientList(D2ActiveRoomStrc* pRoom)
{
	for (uint32_t i = 0; i < (uint32_t)pRoom->nNumClients; ++i)
	{
		D2_ASSERT(pRoom->ppClients[i]);
	}
	return TRUE;
}

//D2Common.0x6FD8CB60 (#10079)
// 1.13c D2Common+0x3C460
void __stdcall DUNGEON_AddClientToRoom(D2ActiveRoomStrc* pRoom, D2ClientStrc* pClient)
{
	D2_ASSERT(pRoom);

	D2_ASSERT(pClient);

	DUNGEON_ValidateClientList(pRoom);

	D2_ASSERT(pRoom->nNumClients <= pRoom->nMaxClients);

	if (pRoom->nNumClients == pRoom->nMaxClients)
	{
		pRoom->nMaxClients += 4;
		pRoom->ppClients = (D2ClientStrc**)D2_REALLOC_POOL(pRoom->pAct->pMemPool, pRoom->ppClients, sizeof(D2ClientStrc*) * pRoom->nMaxClients);
	}

	pRoom->ppClients[pRoom->nNumClients] = pClient;
	++pRoom->nNumClients;

	DUNGEON_UpdateClientListOfRoom(pRoom);
}

//D2Common.0x6FD8CC50
// 1.13c D2Common+0x3BDD0 (room in EAX)
void __fastcall DUNGEON_UpdateClientListOfRoom(D2ActiveRoomStrc* pRoom)
{
	D2ClientStrc* pClient1 = NULL;
	D2ClientStrc* pClient2 = NULL;
	BOOL bContinue = FALSE;

	DUNGEON_ValidateClientList(pRoom);

	for (int nClients = pRoom->nNumClients - 1; nClients > 0; --nClients)
	{
		bContinue = FALSE;

		for (int i = 0; i < nClients; ++i)
		{
			pClient1 = pRoom->ppClients[i];
			pClient2 = pRoom->ppClients[i + 1];

			if (pClient1 > pClient2)
			{
				pRoom->ppClients[i] = pClient2;
				pRoom->ppClients[i + 1] = pClient1;
				bContinue = TRUE;
			}
		}

		if (!bContinue)
		{
			return;
		}
	}

	for (int i = 0; i < pRoom->nNumClients; ++i)
	{
		D2_ASSERT(pRoom->ppClients[i]);
	}
}

//D2Common.0x6FD8CD10 (#10080)
// 1.13c D2Common+0x3C3B0
void __stdcall DUNGEON_RemoveClientFromRoom(D2ActiveRoomStrc* pRoom, D2ClientStrc* pClient)
{
	int i = 0;

	D2_ASSERT(pRoom);

	D2_ASSERT(pClient);

	for (i = 0; i < pRoom->nNumClients; ++i)
	{
		D2_ASSERT(pRoom->ppClients[i]);
	}

	for (i = 0; i < pRoom->nNumClients; ++i)
	{
		if (pRoom->ppClients[i] == pClient)
		{
			break;
		}
	}

	D2_ASSERT(i < pRoom->nNumClients);

	--pRoom->nNumClients;
	pRoom->ppClients[i] = pRoom->ppClients[pRoom->nNumClients];

	D2_CALL(DUNGEON_UpdateClientListOfRoom, pRoom);
}

//D2Common.0x6FD8CDF0 (#10081)
// 1.13c D2Common+0x3B350
int __stdcall D2Common_10081_GetTileCountFromRoom(D2ActiveRoomStrc* pRoom)
{
	D2_ASSERT(pRoom);

	if (pRoom->nNumClients)
	{
		pRoom->nTileCount = 0;
		return 0;
	}
	else
	{
		++pRoom->nTileCount;
		return pRoom->nTileCount;
	}
}

//D2Common.0x6FD8CE40
// 1.13c D2Common+0x3C870 (register-arg room teardown, inlined here)
// The session's client critters (src/core/d2client/session_critter.cpp) live outside the room's unit list; retail's teardown
// detaches every unit in the room, client units too, so they are told here. Weak: builds without the session have none.
__attribute__((weak)) void d2client_critter_room_freed(D2ActiveRoomStrc* pRoom);
void __fastcall DUNGEON_FreeRoom(void* pMemPool, D2ActiveRoomStrc* pRoom)
{
	if (d2client_critter_room_freed) d2client_critter_room_freed(pRoom);
	for (D2UnitStrc* i = pRoom->pUnitFirst; i; i = pRoom->pUnitFirst)
	{
		if (!(i->dwFlags & UNITFLAG_ISCLIENTUNIT))
		{
			i->dwFlagEx |= 0x20;
		}

		i->dwFlags |= 0x800000;
		UNITROOM_UpdatePath(i);
	}

	COLLISION_FreeRoomCollisionGrid(pMemPool, pRoom);

	DUNGEON_FreeDrlgDelete(pRoom);

	if (pRoom->ppClients)
	{
		D2_FREE_POOL(pRoom->pAct->pMemPool, pRoom->ppClients);
	}

	D2_FREE_POOL(pMemPool, pRoom->ppRoomList);
	D2_FREE_POOL(pMemPool, pRoom);
}

//D2Common.0x6FD8CF10 (#10076)
// 1.13c D2Common+0x3CA20
void __stdcall DUNGEON_RemoveRoomFromAct(D2DrlgActStrc* pAct, D2ActiveRoomStrc* pRoom)
{
	D2ActiveRoomStrc* pPreviousRoom = NULL;
	D2ActiveRoomStrc* pCurrentRoom = NULL;
	D2ActiveRoomStrc* pNearRoom = NULL;
	int nCounter = 0;

	D2_ASSERT(pRoom);
	D2_ASSERT(pAct->pRoom);

	pCurrentRoom = pAct->pRoom;
	while (pCurrentRoom != pRoom)
	{
		pPreviousRoom = pCurrentRoom;
		pCurrentRoom = pCurrentRoom->pRoomNext;
		if (!pCurrentRoom)
		{
			break;
		}
	}

	D2_ASSERT(pCurrentRoom);

	if (pPreviousRoom)
	{
		pPreviousRoom->pRoomNext = pCurrentRoom->pRoomNext;
	}
	else
	{
		pAct->pRoom = pCurrentRoom->pRoomNext;
	}

	for (int i = 0; i < pRoom->nNumRooms; ++i)
	{
		pNearRoom = pRoom->ppRoomList[i];
		if (pNearRoom != pRoom)
		{
			nCounter = 0;

			while (nCounter < pNearRoom->nNumRooms && pNearRoom->ppRoomList[nCounter] != pRoom)
			{
				++nCounter;
			}

			if (nCounter < pNearRoom->nNumRooms)
			{
				--pNearRoom->nNumRooms;
				pNearRoom->ppRoomList[nCounter] = pNearRoom->ppRoomList[pNearRoom->nNumRooms];
			}
		}
	}

	sub_6FD77280(pRoom->pDrlgRoom, pAct->bClient, pRoom->dwFlags);
	DUNGEON_FreeRoom(pAct->pMemPool, pRoom);
}

//D2Common.0x6FD8D000 (#10077)
// 1.13c D2Common+0x3C810
void __stdcall D2Common_10077(D2ActiveRoomStrc* pRoom1, D2ActiveRoomStrc* pRoom2)
{
	sub_6FD745C0(pRoom1 ? pRoom1->pDrlgRoom : NULL, pRoom2 ? pRoom2->pDrlgRoom : NULL);
}

//D2Common.0x6FD8D030 (#10078)
// 1.13c D2Common+0x3C800: reads act +0x48 (the drlg) and runs the inactive-room sweep on it, no null test
void __stdcall DUNGEON_UpdateAndFreeInactiveRooms(D2DrlgActStrc* pAct)
{
	DRLG_UpdateAndFreeInactiveRooms(pAct->pDrlg);
}

//D2Common.0x6FD8D040 (#10044)
// 1.13c D2Common+0x3BDA0
int __stdcall DUNGEON_CheckLOSDraw(D2ActiveRoomStrc* pRoom)
{
	if (pRoom)
	{
		return DRLGROOM_CheckLOSDraw(pRoom->pDrlgRoom);
	}

	return 0;
}

//D2Common.0x6FD8D060
// 1.13c D2Common+0x3B2A0 (act in EAX, assert on null, returns act +4)
D2DrlgEnvironmentStrc* __fastcall DUNGEON_GetEnvironmentFromAct(D2DrlgActStrc* pAct)
{
	D2_ASSERT(pAct);

	return pAct->pEnvironment;
}

//D2Common.0x6FD8D090 (#10088)
// 1.13c D2Common+0x3B270
D2DrlgStrc* __stdcall DUNGEON_GetDrlgFromAct(D2DrlgActStrc* pAct)
{
	D2_ASSERT(pAct);

	return pAct->pDrlg;
}

//D2Common.0x6FD912D0 (#10089)
// 1.13c D2Common+0x3B250
int __stdcall DUNGEON_GetInitSeedFromAct(D2DrlgActStrc* pAct)
{
	if (pAct)
	{
		return pAct->dwInitSeed;
	}

	return 0;
}

//D2Common.0x6FD8D0C0 (#10007)
// 1.13c D2Common+0x3B240
D2DrlgRoomStrc* __fastcall DUNGEON_GetRoomExFromRoom(D2ActiveRoomStrc* pRoom)
{
	return pRoom->pDrlgRoom;
}

//D2Common.0x6FD8D0D0 (#10086)
// 1.13c D2Common+0x3B230
BOOL __stdcall DUNGEON_IsTownLevelId(int nLevelId)
{
	return DRLG_IsTownLevel(nLevelId);
}

//D2Common.0x6FD8D0E0 (#10082)
// 1.13c D2Common+0x3C390
BOOL __stdcall DUNGEON_IsRoomInTown(D2ActiveRoomStrc* pRoom)
{
	if (D2Seam<&DUNGEON_IsRoomInTown>::hook) return D2Seam<&DUNGEON_IsRoomInTown>::hook(pRoom);
	if (pRoom)
	{
		return DRLG_IsTownLevel(DRLGROOM_GetLevelId(pRoom->pDrlgRoom));
	}

	return FALSE;
}

//D2Common.0x6FD8D100 (#10083)
// 1.13c D2Common+0x3C380 (returns 0, pops its one argument)
int __stdcall D2COMMON_10083_Return0(D2ActiveRoomStrc* pRoom)
{
	//if (pRoom)
	//{
	//	DRLGROOM_GetLevelId(pRoom->pDrlgRoom);
	//}

	//return D2COMMON_10018_Return0();

	return 0;
}

//D2Common.0x6FD8D130 (#10084)
// 1.13c D2Common+0x3B220
int __stdcall D2Common_10084(D2ActiveRoomStrc* pRoom)
{
	return pRoom->dwFlags & 4;
}

//D2Common.0x6FD8D140 (#10085)
// 1.13c D2Common+0x3B1E0
int __stdcall DUNGEON_GetTownLevelIdFromActNo(uint8_t nAct)
{
	if (D2Seam<&DUNGEON_GetTownLevelIdFromActNo>::hook) return D2Seam<&DUNGEON_GetTownLevelIdFromActNo>::hook(nAct);
	static const int gnTownLevelIds[] = { LEVEL_ROGUEENCAMPMENT, LEVEL_LUTGHOLEIN, LEVEL_KURASTDOCKTOWN, LEVEL_THEPANDEMONIUMFORTRESS, LEVEL_HARROGATH };

	D2_ASSERT(nAct < ARRAY_SIZE(gnTownLevelIds));

	return gnTownLevelIds[nAct];
}

//D2Common.0x6FD8D180 (#10087)
// 1.13c D2Common+0x3C330
int __stdcall D2Common_10087(D2ActiveRoomStrc* pRoom)
{
	D2_ASSERT(pRoom);

	return sub_6FD779F0(pRoom->pDrlgRoom);
}

//D2Common.0x6FD8D1C0 (#10090)
// 1.13c D2Common+0x3C7E0
int __stdcall DUNGEON_GetNumberOfPopulatedRoomsInLevel(D2DrlgActStrc* pAct, int nLevelId)
{
	if (D2Seam<&DUNGEON_GetNumberOfPopulatedRoomsInLevel>::hook) return D2Seam<&DUNGEON_GetNumberOfPopulatedRoomsInLevel>::hook(pAct, nLevelId);
	return DRLG_GetNumberOfPopulatedRoomsInLevel(pAct->pDrlg, nLevelId);
}

//D2Common.0x6FD8D1E0 (#10025)
// 1.13c D2Common+0x3B1A0
int* __stdcall DUNGEON_GetWarpCoordinatesFromRoom(D2ActiveRoomStrc* pRoom)
{
	D2_ASSERT(pRoom);

	return pRoom->pDrlgRoom->pLevel->nRoom_Center_Warp_X;
}

//D2Common.0x6FD8D220 (#10091)
// 1.13c D2Common+0x3B120
void __stdcall DUNGEON_UpdateWarpRoomSelect(D2ActiveRoomStrc* pRoom, int nLevelId)
{
	D2_ASSERT(pRoom);

	DRLGWARP_UpdateWarpRoomSelect(pRoom->pDrlgRoom, nLevelId);
}

//D2Common.0x6FD8D260 (#10092)
// 1.13c D2Common+0x3B160
void __stdcall DUNGEON_UpdateWarpRoomDeselect(D2ActiveRoomStrc* pRoom, int nLevelId)
{
	D2_ASSERT(pRoom);

	DRLGWARP_UpdateWarpRoomDeselect(pRoom->pDrlgRoom, nLevelId);
}

//D2Common.0x6FD8D2A0 (#10093)
// 1.13c D2Common+0x3C2F0
void __stdcall DUNGEON_UpdatePops(D2ActiveRoomStrc* pRoom, int nX, int nY, BOOL bOtherRoom)
{
	D2_ASSERT(pRoom);

	DRLGPRESET_UpdatePops(pRoom->pDrlgRoom, nX, nY, bOtherRoom);
}

//D2Common.0x6FD8D2E0 (#10094)
// 1.13c D2Common+0x3B0D0
void __stdcall DUNGEON_GetTombStoneTileCoords(D2ActiveRoomStrc* pRoom, D2CoordStrc** ppTombStoneTiles, int* pnTombStoneTiles)
{
	return DRLGPRESET_GetTombStoneTileCoords(pRoom->pDrlgRoom, ppTombStoneTiles, pnTombStoneTiles);
}

//D2Common.0x6FD8D300 (#10095)
// 1.13c D2Common+0x3C6D0: the room pick is the 0x1330 search, which skips null neighbours
int __stdcall D2Common_10095(D2ActiveRoomStrc* pRoom, int nX, int nY)
{
	D2ActiveRoomStrc* pFound = DUNGEON_GetRoomAtPosition(pRoom, nX, nY);
	if (!pFound)
	{
		return 0;
	}

	return DRLGLOGIC_GetRoomCoordListIndex(pFound->pDrlgRoom, nX / 5, nY / 5);
}

D2RoomCoordListStrc* __fastcall DUNGEON_GetRoomCoordListFromSubtile_81950(D2DrlgRoomStrc* pDrlgRoom, int nX, int nY);

//D2Common.0x6FD8D3A0 (#10096)
// 1.13c D2Common+0x3C680
D2RoomCoordListStrc* __stdcall D2Common_10096(D2ActiveRoomStrc* pRoom, int nX, int nY)
{
	if (D2Seam<&D2Common_10096>::hook) return D2Seam<&D2Common_10096>::hook(pRoom, nX, nY);
	return D2_CALL(DUNGEON_GetRoomCoordListFromSubtile_81950, pRoom->pDrlgRoom, nX, nY);
}

// 1.13c D2Common+0x81950 (ESI = drlg room, stack = subtile x and y): the cell lookup that 1.10f's caller did after dividing by 5
D2RoomCoordListStrc* __fastcall DUNGEON_GetRoomCoordListFromSubtile_81950(D2DrlgRoomStrc* pDrlgRoom, int nX, int nY)
{
	return sub_6FD77110(pDrlgRoom, nX / 5, nY / 5);
}

//D2Common.0x6FD8D3C0 (#10097)
// 1.13c D2Common+0x3BD60
D2RoomCoordListStrc* __stdcall DUNGEON_GetRoomCoordList(D2ActiveRoomStrc* pRoom)
{
	return DRLGLOGIC_GetRoomCoordList(pRoom->pDrlgRoom);
}

//D2Common.0x6FD8D3D0 (#10098)
// 1.13c D2Common+0x3BC80: the portal table and its count are the module globals at 0xA1420 / 0xA1424; a count above 0x20 is fatal (0x60A).
// Each set bit of nFlags, from bit 0 up, takes the table entry at that index; a non-empty list is copied into a pool allocation.
void __stdcall DUNGEON_GetPortalLevelArrayFromPortalFlags(void* pMemPool, int nFlags, int** ppLevels, int* pnLevels)
{
	int nPortalLevels = 0;
	int* pPortalLevels = D2_CALL(DATATBLS_GetPortalLevels, &nPortalLevels);

	if (nPortalLevels > 0x20)
	{
		FOG_DisplayAssert("portal level count", __FILE__, __LINE__);
		exit(-1);
	}

	int pLevels[32];
	int nCount = 0;
	for (int i = 0; i < nPortalLevels; ++i)
	{
		if (nFlags & (1 << i))
		{
			pLevels[nCount++] = pPortalLevels[i];
		}
	}

	if (nCount)
	{
		int* pCopy = (int*)D2_ALLOC_POOL(pMemPool, nCount * sizeof(int));
		for (int i = 0; i < nCount; ++i)
		{
			pCopy[i] = pLevels[i];
		}
		*ppLevels = pCopy;
	}
	else
	{
		*ppLevels = NULL;
	}

	*pnLevels = nCount;
}

//D2Common.0x6FD8D4B0 (#10099)
// 1.13c D2Common+0x3BC40
int __stdcall DUNGEON_GetPortalFlagFromLevelId(int nPortalLevelId)
{
	int* pPortalLevels = NULL;
	int nPortalLevels = 0;

	pPortalLevels = DATATBLS_GetPortalLevels(&nPortalLevels);

	for (int i = 0; i < nPortalLevels; ++i)
	{
		if (nPortalLevelId == pPortalLevels[i])
		{
			return 1 << i;
		}
	}

	return 0;
}

//D2Common.0x6FD8D4F0 (#10037)
// 1.13c D2Common+0x3B0A0
int __stdcall DUNGEON_GetTownLevelIdFromAct(D2DrlgActStrc* pAct)
{
	D2_ASSERT(pAct);

	return pAct->nTownId;
}

//D2Common.0x6FD8D520 (#10047)
// 1.13c D2Common+0x3B080 (#10518): a null act or a null drlg gives 0
int __stdcall DUNGEON_GetHoradricStaffTombLevelId(D2DrlgActStrc* pAct)
{
	if (pAct && pAct->pDrlg)
	{
		return pAct->pDrlg->nStaffTombLevel;
	}

	return 0;
}

//D2Common.0x6FD8D540 (#10102)
// 1.13c D2Common+0x3BC10 (#10346): room +0x10 is the drlg room, its flags +0x28; no null test on the drlg room.
void __stdcall DUNGEON_ToggleHasPortalFlag(D2ActiveRoomStrc* pRoom, BOOL bReset)
{
	if (D2Seam<&DUNGEON_ToggleHasPortalFlag>::hook) { D2Seam<&DUNGEON_ToggleHasPortalFlag>::hook(pRoom, bReset); return; }
	if (pRoom)
	{
		DRLGACTIVATE_ToggleHasPortalFlag(pRoom->pDrlgRoom, bReset);
	}
}

//D2Common.0x6FD8D560 (#10104)
// 1.13c D2Common+0x3BBD0
void __stdcall DUNGEON_AnimateTiles(D2ActiveRoomStrc* pRoom)
{
	if (pRoom)
	{
		DRLGANIM_AnimateTiles(pRoom->pDrlgRoom);
	}
}

// function probe hook (src/core/fn/d2common_3b040.cpp): the capture recorded the callee's two arguments with a stub; null in the game.
void (__fastcall* g_pfnInitRoomTileAnimCallee)(D2DrlgRoomStrc*, D2DrlgRoomStrc*) = NULL;
#define DRLGANIM_UpdateFrameInAdjacentRooms(a, b) (g_pfnInitRoomTileAnimCallee ? g_pfnInitRoomTileAnimCallee(a, b) : DRLGANIM_UpdateFrameInAdjacentRooms(a, b))
//D2Common.0x6FD8D580 (#10105)
// 1.13c D2Common+0x3B040
void __stdcall DUNGEON_InitRoomTileAnimation(D2DrlgActStrc* pAct, D2ActiveRoomStrc* pRoom1, D2ActiveRoomStrc* pRoom2)
{
	D2DrlgRoomStrc* pDrlgRoom1 = NULL;
	D2ActiveRoomStrc* pRoom = NULL;

	if (pRoom1)
	{
		pRoom = pAct->pRoom;
		if (pRoom)
		{
			while (pRoom != pRoom1)
			{
				pRoom = pRoom->pRoomNext;
				if (!pRoom)
				{
					DRLGANIM_UpdateFrameInAdjacentRooms(NULL, pRoom2 ? pRoom2->pDrlgRoom : NULL);
					return;
				}
			}

			pDrlgRoom1 = pRoom1->pDrlgRoom;
		}
	}

	DRLGANIM_UpdateFrameInAdjacentRooms(pDrlgRoom1, pRoom2 ? pRoom2->pDrlgRoom : NULL);
}

//D2Common.0x6FD8D5C0 (#10103)
// 1.13c D2Common+0x3B000: a null act is a fatal assert (1.10f only warned)
void __stdcall DUNGEON_SetActCallbackFunc(D2DrlgActStrc* pAct, ACTCALLBACKFN pActCallbackFunction)
{
	D2_ASSERT(pAct);
	pAct->pfnActCallBack = pActCallbackFunction;
}

//D2Common.0x6FD8D600 (#10106)
// 1.13c D2Common+0x3AFD0
void __stdcall DUNGEON_SaveKilledUnitGUID(D2ActiveRoomStrc* pRoom, D2UnitGUID nUnitGUID)
{
	if (pRoom)
	{
		pRoom->nLastDeadGUIDs[pRoom->nCurrentDeathIndex] = nUnitGUID;
		pRoom->nCurrentDeathIndex = (pRoom->nCurrentDeathIndex + 1) & 3;
	}
}

//D2Common.0x6FD8D690 (#10107)
// 1.13c D2Common+0x4daf0
int __stdcall DUNGEON_ClientToGameTileCoords(int* pX, int* pY)
{
	const int nNumY = 2 * *pY - *pX;
	const int nOutX = (2 * *pY + *pX) / 160;
	const int nOutY = nNumY / 160;

	*pX = nOutX;
	*pY = nOutY;

	// 1.13c returns the Y numerator times the divide-by-160 multiplier in EAX (not a value the caller reads).
	return (int)((uint32_t)nNumY * 0x66666667u);
}

//D2Common.0x6FD8D870 (#10108)
// 1.13c D2Common+0x4D8E0
void __stdcall DUNGEON_ClientToGameSubtileCoords(int* pX, int* pY)
{
	// retail shifts arithmetically (floor); 1.10f divided (truncate): differs for negatives
	const int nOutX = (2 * *pY + *pX) >> 5;
	const int nOutY = (2 * *pY - *pX) >> 5;

	*pX = nOutX;
	*pY = nOutY;
}

//D2Common.0x6FD8D8A0 (#10109)
// 1.13c D2Common+0x4d8c0
void __stdcall DUNGEON_ClientToGameCoords(int* pX, int* pY)
{
	int nX = 2 * *pY + *pX;
	int nY = 2 * *pY - *pX;

	*pX = nX;
	*pY = nY;
}

//D2Common.0x6FD8D6E0 (#10110)
// 1.13c D2Common+0x4DAC0
void __stdcall DUNGEON_GameTileToClientCoords(int* pX, int* pY)
{
	int nX = *pX;
	int nY = *pY;

	*pX = 80 * (nX - nY);
	*pY = (80 * (nX + nY)) / 2;
}

//D2Common.0x6FD8D630 (#10111)
// 1.13c D2Common+0x4DB70
void __stdcall DUNGEON_GameSubtileToClientCoords(int* pX, int* pY)
{
	const int nOutX = 16 * (*pX - *pY);
	const int nOutY =  8 * (*pX + *pY);

	*pX = nOutX;
	*pY = nOutY;
}

//D2Common.0x6FD8D660 (#10112)
// 1.13c D2Common+0x4DB40 (#10132): arithmetic shifts, floor for negatives (1.10f divided, truncating toward zero)
void __stdcall DUNGEON_GameToClientCoords(int* pX, int* pY)
{
	const int nOutX = (*pX - *pY) >> 1;
	const int nOutY = (*pX + *pY) >> 2;

	*pX = nOutX;
	*pY = nOutY;
}

//D2Common.0x6FD8D8C0 (#10113)
// 1.13c D2Common+0x4D8A0
void __stdcall DUNGEON_GameTileToSubtileCoords(int* pX, int* pY)
{
	*pX *= 5;
	*pY *= 5;
}

//D2Common.0x6FD8D710 (#10114)
// 1.13c D2Common+0x4DA40
int __stdcall DUNGEON_ClientTileDrawPositionToGameCoords(int nX, int nY, int* pX, int* pY)
{
	const int nNumY = 2 * nY - nX;
	*pX = 2 * nY + nX;

	if (*pX >= 0)
	{
		*pX /= 160;
	}
	else
	{
		*pX = *pX / 160 - 1;
	}

	if (nNumY >= 0)
	{
		*pY = nNumY / 160;
		return *pY;
	}

	*pY = nNumY / 160 - 1;
	// 1.13c returns the Y numerator times the divide-by-160 multiplier in EAX for negative Y.
	return (int)((uint32_t)nNumY * 0x66666667u);
}

//D2Common.0x6FD8D790 (#10115)
// 1.13c D2Common+0x4DA00
void __stdcall DUNGEON_GameToClientTileDrawPositionCoords(int nX, int nY, int* pX, int* pY)
{
	*pX = 80 * (nX - nY);
	*pY = 40 * (nX + nY);
	*pX -= 80;
	*pY += 80;
}

//D2Common.0x6FD8D7D0 (#10116)
// 1.13c D2Common+0x4d9a0
void __stdcall DUNGEON_ClientSubileDrawPositionToGameCoords(int nX, int nY, int* pX, int* pY)
{
	*pX = 2 * nY + nX;
	*pY = 2 * nY - nX;

	if (*pX >= 0)
	{
		*pX /= 32;
	}
	else
	{
		*pX = (*pX / 32) - 1;
	}

	if (*pY >= 0)
	{
		*pY /= 32;
	}
	else
	{
		*pY = (*pY / 32) - 1;
	}
}

//D2Common.0x6FD8D830 (#10117)
// 1.13c D2Common+0x4D970
void __stdcall DUNGEON_GameToClientSubtileDrawPositionCoords(int nX, int nY, int* pX, int* pY)
{
	*pX = 16 * (nX - nY);
	*pY = 8 * (nX + nY);
	*pX -= 16;
	*pY += 16;
}

// 1.13c D2Common+0x3D040: sort comparator over two 16-bit keys: 1 when the first is larger, -1 when smaller, 0 when equal (unsigned).
int32_t d2common_3d040(const uint16_t* a, const uint16_t* b)
{
	return *a > *b ? 1 : (*a < *b ? -1 : 0);
}

// 1.13c D2Common+0x3CF30: stable insertion sort of 12-byte records (count is the u16 in EAX). Only a record's word at +0 and
// dword at +4 move (the dword at +8 stays in its slot). The comparator takes the held record copy and the candidate copy, both
// {u16, dword}; a positive answer means the held record sorts after the candidate. The held copy is the largest record so far
// and only changes when the candidate is not smaller.
namespace
{
struct D2SortRec3CF30 { uint16_t w; uint32_t d; };
inline D2SortRec3CF30 SortRecLoad3CF30(uint8_t* a, uint32_t i) { D2SortRec3CF30 r; memcpy(&r.w, a + i * 12, 2); memcpy(&r.d, a + i * 12 + 4, 4); return r; }
inline void SortRecStore3CF30(uint8_t* a, uint32_t i, const D2SortRec3CF30& r) { memcpy(a + i * 12, &r.w, 2); memcpy(a + i * 12 + 4, &r.d, 4); }
}

extern "C" void D2Common_3CF30_Raw(uint16_t count, void* pArray, int32_t (*cmp)(const void* held, const void* cand))
{
	if (count <= 1)
	{
		return;
	}
	uint8_t* a = static_cast<uint8_t*>(pArray);
	D2SortRec3CF30 held = SortRecLoad3CF30(a, 0);
	for (uint32_t i = 1; i < count; i++)
	{
		D2SortRec3CF30 key = SortRecLoad3CF30(a, i);
		if (cmp(&held, &key) > 0)
		{
			SortRecStore3CF30(a, i, held);
			uint32_t j = i - 1;
			while (j > 0)
			{
				D2SortRec3CF30 t = SortRecLoad3CF30(a, j - 1);
				if (cmp(&t, &key) <= 0)
				{
					break;
				}
				SortRecStore3CF30(a, j, t);
				j--;
			}
			SortRecStore3CF30(a, j, key);
		}
		else
		{
			held = key;
		}
	}
}
