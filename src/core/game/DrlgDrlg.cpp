#include "Drlg/D2DrlgDrlg.h"

#include "D2DataTbls.h"
#include "Drlg/D2DrlgDrlgAnim.h"
#include "Drlg/D2DrlgDrlgRoom.h"
#include "Drlg/D2DrlgDrlgWarp.h"
#include "Drlg/D2DrlgActivate.h"
#include "Drlg/D2DrlgMaze.h"
#include "Drlg/D2DrlgOutdoors.h"
#include "Drlg/D2DrlgOutPlace.h"
#include "Drlg/D2DrlgPreset.h"
#include "Drlg/D2DrlgRoomTile.h"
#include "D2Dungeon.h"
#include "D2Seed.h"
#include <test_seam.h>
#include <D2CMP.h>
#include <DataTbls/LevelsIds.h>

static_assert(DRLGROOMFLAG_HAS_WARP_0 == (1 << DRLGROOMFLAG_HAS_WARP_FIRST_BIT), "Warp first bit must match of DRLGROOMFLAG_HAS_WARP_0");
static_assert(DRLGROOMFLAG_SUBSHRINE_ROW1 == (1 << DRLGROOMFLAG_SUBSHRINE_ROWS_FIRST_BIT), "Subshrines first bit must match of DRLGROOMFLAG_SUBSHRINE_ROW1");
static_assert(DRLGROOMFLAG_HAS_WAYPOINT == (1 << DRLGROOMFLAG_HAS_WAYPOINT_FIRST_BIT), "Waypoint first bit must match of DRLGROOMFLAG_HAS_WAYPOINT");

//1.10f: D2Common.0x6FD74120 (#10014)
//1.11:  D2Common.0x6FDC7AA0 (#11155)
// 1.13c D2Common+0x2E3E0 (retail has no archive assert; the tile library path is built by 0x2D4E0, inlined below)
D2DrlgStrc* __fastcall DRLG_AllocDrlg(D2DrlgActStrc* pAct, uint8_t nActNo, HD2ARCHIVE hArchive, uint32_t nInitSeed, int nTownLevelId, uint32_t nFlags, D2GameStrc* pGame, uint8_t nDifficulty, AUTOMAPFN pfAutoMap, TOWNAUTOMAPFN pfTownAutoMap)
{
	D2DrlgStrc* pDrlg = D2_CALLOC_STRC_POOL(pAct->pMemPool, D2DrlgStrc);

	pDrlg->pAct = pAct;
	pDrlg->pMempool = pAct->pMemPool;
	pDrlg->hArchive = hArchive; // Always nullptr in the game
	pDrlg->nAct = nActNo;

	SEED_InitLowSeed(&pDrlg->pSeed, nInitSeed);

	pDrlg->dwStartSeed = (unsigned int)SEED_RollRandomNumber(&pDrlg->pSeed);
	pDrlg->dwFlags = nFlags;
	pDrlg->pGame = pGame;
	pDrlg->dwGameLowSeed = nInitSeed;
	pDrlg->nDifficulty = nDifficulty;
	pDrlg->pfAutomap = pfAutoMap;
	pDrlg->pfTownAutomap = pfTownAutoMap;

	char szPath[MAX_PATH] = {};

	switch (nActNo)
	{
	case ACT_I:
		sprintf(szPath, "%s\\Tiles\\Act1\\Town\\Floor.dt1", "DATA\\GLOBAL");
		D2CMP_10087_LoadTileLibrarySlot(pDrlg->pTiles, szPath);
		break;
	
	case ACT_II:
	{
		unsigned int nStaffLevelOffset = 0;
		unsigned int nBossLevelOffset = 0;
		do
		{
			nStaffLevelOffset = (unsigned int)SEED_RollRandomNumber(&pDrlg->pSeed) % 7;
			nBossLevelOffset = (unsigned int)SEED_RollRandomNumber(&pDrlg->pSeed) % 7;
		} while (nStaffLevelOffset == nBossLevelOffset);

		pDrlg->nStaffTombLevel = LEVEL_TALRASHASTOMB1 + nStaffLevelOffset;
		pDrlg->nBossTombLevel = LEVEL_TALRASHASTOMB1 + nBossLevelOffset;

		sprintf(szPath, "%s\\Tiles\\Act2\\Town\\Ground.dt1", "DATA\\GLOBAL");
		D2CMP_10087_LoadTileLibrarySlot(pDrlg->pTiles, szPath);
		break;
	}
	case ACT_III:
		pDrlg->bJungleInterlink = SEED_RollRandomNumber(&pDrlg->pSeed) & 1;

		sprintf(szPath, "%s\\Tiles\\ACT3\\Kurast\\sets.dt1", "DATA\\GLOBAL");
		D2CMP_10087_LoadTileLibrarySlot(pDrlg->pTiles, szPath);
		break;
	default:
		break;
	}

	DRLGACTIVATE_InitializeRoomExStatusLists(pDrlg);
	DRLGOUTPLACE_CreateLevelConnections(pDrlg, nActNo);

	if (nTownLevelId != LEVEL_NONE)
	{
		DRLG_InitLevel(DRLG_GetLevel(pDrlg, nTownLevelId));
	}

	return pDrlg;
}

//D2Common.0x6FD743B0 (#10012)
// 1.13c D2Common+0x2E020
void __fastcall DRLG_FreeDrlg(D2DrlgStrc* pDrlg)
{
	D2DrlgLevelStrc* pNextLevel = NULL;
	D2DrlgLevelStrc* pLevel = NULL;
	D2DrlgWarpStrc* pNextWarp = NULL;
	D2DrlgWarpStrc* pWarp = NULL;

	if (pDrlg->pLevel)
	{
		pLevel = pDrlg->pLevel;

		do
		{
			pNextLevel = pLevel->pNextLevel;
			DRLG_FreeLevel(pDrlg->pMempool, pLevel, FALSE);
			D2_FREE_POOL(pDrlg->pMempool, pLevel);
			pLevel = pNextLevel;
		}
		while (pLevel);

		pDrlg->pLevel = NULL;
	}

	if (pDrlg->pWarp)
	{
		pWarp = pDrlg->pWarp;

		do
		{
			pNextWarp = pWarp->pNext;
			D2_FREE_POOL(pDrlg->pMempool, pWarp);
			pWarp = pNextWarp;
		}
		while (pWarp);

		pDrlg->pWarp = NULL;
	}

	D2_FREE_POOL(pDrlg->pMempool, pDrlg);
}

//D2Common.0x6FD74440
// 1.13c D2Common+0x2DAF0: a level of any other drlg type is left alone (no unreachable)
void __fastcall DRLG_FreeLevel(void* pMemPool, D2DrlgLevelStrc* pLevel, BOOL bAlloc)
{
	D2DrlgBuildStrc* pNextDrlgBuild = NULL;
	D2DrlgBuildStrc* pDrlgBuild = NULL;
	D2DrlgRoomStrc* pNextRoomEx = NULL;
	D2DrlgRoomStrc* pDrlgRoom = NULL;
	int nCounter = 0;

	if (bAlloc)
	{
		if (!pLevel->pPresetMaps && pLevel->nRooms)
		{
			pLevel->pPresetMaps = (int*)D2_CALLOC_POOL(pMemPool, sizeof(int) * pLevel->nRooms);
		}
	}
	else
	{
		if (pLevel->pPresetMaps)
		{
			D2_FREE_POOL(pMemPool, pLevel->pPresetMaps);
			pLevel->pPresetMaps = NULL;
		}
	}

	pNextRoomEx = pLevel->pFirstRoomEx;
	if (pNextRoomEx)
	{
		nCounter = 0;
		do
		{
			pDrlgRoom = pNextRoomEx;
			pNextRoomEx = pNextRoomEx->pDrlgRoomNext;
			if (pLevel->pPresetMaps)
			{
				pLevel->pPresetMaps[nCounter] = pDrlgRoom->dwOtherFlags & 1;
				++nCounter;
			}

			DRLGROOM_FreeRoomEx(pDrlgRoom);
		}
		while (pNextRoomEx);

		pLevel->pFirstRoomEx = NULL;
		pLevel->nRooms = 0;
	}

	if (pLevel->pCurrentMap)
	{
		DRLGPRESET_FreeDrlgMap(pMemPool, pLevel->pCurrentMap);
		pLevel->pCurrentMap = NULL;
	}

	switch (pLevel->nDrlgType)
	{
	case DRLGTYPE_MAZE:
		if (!bAlloc)
		{
			pLevel->pMaze = NULL;
		}
		break;

	case DRLGTYPE_PRESET:
		pLevel->pPreset->pDrlgMap = NULL;
		if (!bAlloc)
		{
			D2_FREE_POOL(pMemPool, pLevel->pPreset);
			pLevel->pPreset = NULL;
		}
		break;

	case DRLGTYPE_OUTDOOR:
		DRLGOUTDOORS_FreeOutdoorInfo(pLevel, bAlloc);
		break;
	default:
		break;
	}

	memset(pLevel->pTileInfo, 0x00, sizeof(pLevel->pTileInfo));
	pLevel->nTileInfo = 0;

	memset(pLevel->nRoom_Center_Warp_X, 0x00, sizeof(pLevel->nRoom_Center_Warp_X) + sizeof(pLevel->nRoom_Center_Warp_Y));
	pLevel->nRoomCoords = 0;

	if (!bAlloc && pLevel->pJungleDefs)
	{
		D2_FREE_POOL(pMemPool, pLevel->pJungleDefs);
		pLevel->pJungleDefs = NULL;
	}

	pNextDrlgBuild = pLevel->pBuild;
	if (pNextDrlgBuild)
	{
		do
		{
			pDrlgBuild = pNextDrlgBuild;
			pNextDrlgBuild = pNextDrlgBuild->pNext;
			D2_FREE_POOL(pMemPool, pDrlgBuild);
		}
		while (pNextDrlgBuild);

		pLevel->pBuild = NULL;
	}
}

// 1.13c D2Common+0x2DF80: add nDelta to a level's active count and to that of every level it sees (the vis levels from
// the level's own row, created when missing), arm the inactive-frame delay on each, and clamp the counts at zero.
void DRLG_AdjustLevelActiveCount(D2DrlgLevelStrc* pLevel, int nDelta)   // not static: the 0x3C810 proof probe stands in for it through D2Seam
{
	pLevel->bActive += nDelta;
	pLevel->dwInactiveFrames = 10;
	pLevel->bActive = pLevel->bActive > 0 ? pLevel->bActive : 0;

	int* pVisLevelIds = DRLGROOM_GetVisArrayFromLevelId(pLevel->pDrlg, pLevel->nLevelId);
	for (int i = 0; i < 8; ++i)
	{
		if (pVisLevelIds[i])
		{
			D2DrlgLevelStrc* pVisLevel = DRLG_GetLevel(pLevel->pDrlg, pVisLevelIds[i]);
			pVisLevel->bActive += nDelta;
			pVisLevel->dwInactiveFrames = 10;
			pVisLevel->bActive = pVisLevel->bActive > 0 ? pVisLevel->bActive : 0;
		}
	}
}

// 1.13c D2Common+0x3C810 (retail's export body; the per-level step is 0x2DF80, DRLG_AdjustLevelActiveCount above)
void __fastcall sub_6FD745C0(D2DrlgRoomStrc* pDrlgRoom1, D2DrlgRoomStrc* pDrlgRoom2)
{
	D2DrlgLevelStrc* pLevel1 = pDrlgRoom1 ? pDrlgRoom1->pLevel : NULL;
	D2DrlgLevelStrc* pLevel2 = pDrlgRoom2 ? pDrlgRoom2->pLevel : NULL;

	if (pLevel1 != pLevel2)
	{
		if (pLevel1)
		{
			D2_CALL(DRLG_AdjustLevelActiveCount, pLevel1, -1);
		}

		if (pLevel2)
		{
			D2_CALL(DRLG_AdjustLevelActiveCount, pLevel2, 1);
		}
	}
}

// 1.13c D2Common+0x2DDB0: whether an inactive level can be freed. Any of its rooms still needing its status or holding a
// room blocks it (return 0, nothing freed); otherwise the near-room data of every room of its vis levels that faces back at
// it is released as each vis level is passed, and a later vis level may still block after earlier ones were released.
BOOL DRLG_CanDiscardLevel(D2DrlgLevelStrc* pLevel)
{
	D2DrlgStrc* pDrlg = pLevel->pDrlg;
	void* pMemPool = pDrlg->pMempool;

	for (D2DrlgRoomStrc* pRoom = pLevel->pFirstRoomEx; pRoom; pRoom = pRoom->pDrlgRoomNext)
	{
		if (pRoom->fRoomStatus < 4 || (pRoom->dwFlags & DRLGROOMFLAG_HAS_ROOM))
		{
			return FALSE;
		}
	}

	int* pVisLevelIds = D2_CALL(DRLGROOM_GetVisArrayFromLevelId, pDrlg, pLevel->nLevelId);
	for (int j = 0; j < 8; ++j)
	{
		if (!pVisLevelIds[j])
		{
			continue;
		}

		D2DrlgLevelStrc* pVisLevel = DRLG_GetLevel(pDrlg, pVisLevelIds[j]);
		if (!pVisLevel->pFirstRoomEx)
		{
			continue;
		}

		int* pBackIds = D2_CALL(DRLGROOM_GetVisArrayFromLevelId, pVisLevel->pDrlg, pVisLevel->nLevelId);
		uint32_t dwMask = 0;
		for (int i = 0; i < 8; ++i)
		{
			if (pBackIds[i] == pLevel->nLevelId)
			{
				dwMask |= 0x10u << i;
			}
		}

		for (D2DrlgRoomStrc* pRoom = pVisLevel->pFirstRoomEx; pRoom; pRoom = pRoom->pDrlgRoomNext)
		{
			if ((dwMask & pRoom->dwFlags) && (pRoom->fRoomStatus < 4 || (pRoom->dwFlags & DRLGROOMFLAG_HAS_ROOM)))
			{
				return FALSE;
			}
		}

		for (D2DrlgRoomStrc* pRoom = pVisLevel->pFirstRoomEx; pRoom; pRoom = pRoom->pDrlgRoomNext)
		{
			if (pRoom->dwFlags & dwMask)
			{
				DRLGROOM_FreeRoomTiles(pMemPool, pRoom);

				if (pRoom->ppRoomsNear)
				{
					D2_FREE_POOL(pMemPool, pRoom->ppRoomsNear);
					pRoom->ppRoomsNear = NULL;
					pRoom->nRoomsNear = 0;
				}
			}
		}
	}

	return TRUE;
}

// 1.13c D2Common+0x2E2C0
void __fastcall DRLG_UpdateAndFreeInactiveRooms(D2DrlgStrc* pDrlg)
{
	void* pMemPool = pDrlg->pMempool;

	for (D2DrlgLevelStrc* pLevel = pDrlg->pLevel; pLevel; pLevel = pLevel->pNextLevel)
	{
		if (!pLevel->bActive && pLevel->pFirstRoomEx)
		{
			if (pLevel->dwInactiveFrames)
			{
				--pLevel->dwInactiveFrames;
			}
			else if (DRLG_CanDiscardLevel(pLevel))
			{
				DRLG_FreeLevel(pMemPool, pLevel, TRUE);
			}
			else
			{
				pLevel->dwInactiveFrames = 10;
			}
		}
	}
}

//1.10f: D2Common.0x6FD748D0 (#10013)
//1.11:  D2Common.0x6FDC74A0 (#10204)
// 1.13c D2Common+0x2D9B0
D2DrlgLevelStrc* __fastcall DRLG_AllocLevel(D2DrlgStrc* pDrlg, int nLevelId)
{
	if (D2Seam<&DRLG_AllocLevel>::hook) return D2Seam<&DRLG_AllocLevel>::hook(pDrlg, nLevelId);
	D2DrlgLevelStrc* pLevel = D2_CALLOC_STRC_POOL(pDrlg->pMempool, D2DrlgLevelStrc);

	pLevel->pDrlg = pDrlg;
	pLevel->nLevelId = nLevelId;
	pLevel->nLevelType = DATATBLS_GetLevelDefRecord(nLevelId)->dwLevelType;
	pLevel->nDrlgType = DATATBLS_GetLevelDefRecord(nLevelId)->dwDrlgType;

	if (pDrlg->dwFlags & DRLGFLAG_ONCLIENT)
	{
		pLevel->dwFlags |= DRLGLEVELFLAG_AUTOMAP_REVEAL;
	}

	SEED_InitLowSeed(&pLevel->pSeed, pLevel->nLevelId + pLevel->pDrlg->dwStartSeed);

	switch (pLevel->nDrlgType)
	{
	case DRLGTYPE_MAZE:
		pLevel->pMaze = DATATBLS_GetLvlMazeTxtRecordFromLevelId(pLevel->nLevelId);
		DRLG_SetLevelPositionAndSize(pLevel->pDrlg, pLevel);
		break;
	
	case DRLGTYPE_PRESET:
		DRLGPRESET_InitLevelData(pLevel);
		break;
		
	case DRLGTYPE_OUTDOOR:
		pLevel->pOutdoors = D2_CALLOC_STRC_POOL(pLevel->pDrlg->pMempool, D2DrlgOutdoorInfoStrc);
		break;
	default:
		D2_UNREACHABLE;
	}

	pLevel->pNextLevel = pDrlg->pLevel;
	pDrlg->pLevel = pLevel;

	return pLevel;
}

//D2Common.0x6FD749A0 (#10005)
// 1.13c D2Common+0x2DD80
D2DrlgLevelStrc* __stdcall DRLG_GetLevel(D2DrlgStrc* pDrlg, int nLevelId)
{
	for (D2DrlgLevelStrc* pLevel = pDrlg->pLevel; pLevel; pLevel = pLevel->pNextLevel)
	{
		if (pLevel->nLevelId == nLevelId)
		{
			return pLevel;
		}
	}

	return D2_CALL(DRLG_AllocLevel, pDrlg, nLevelId);
}

//D2Common.0x6FD749E0
// 1.13c D2Common+0x2D470
int __fastcall DRLG_GetDirectionFromCoordinates(D2DrlgCoordStrc* pDrlgCoord1, D2DrlgCoordStrc* pDrlgCoord2)
{
	if (pDrlgCoord1->nPosX <= pDrlgCoord2->nPosX)
	{
		if (pDrlgCoord2->nPosX == pDrlgCoord1->nPosX + pDrlgCoord1->nWidth)
		{
			return DIRECTION_SOUTHEAST;
		}
	}
	else
	{
		if (pDrlgCoord1->nPosX == pDrlgCoord2->nPosX + pDrlgCoord2->nWidth)
		{
			return DIRECTION_SOUTHWEST;
		}
	}

	if (pDrlgCoord1->nPosY <= pDrlgCoord2->nPosY)
	{
		if (pDrlgCoord2->nPosY == pDrlgCoord1->nPosY + pDrlgCoord1->nHeight)
		{
			return DIRECTION_NORTHEAST;
		}
	}
	else
	{
		if (pDrlgCoord1->nPosY == pDrlgCoord2->nPosY + pDrlgCoord2->nHeight)
		{
			return DIRECTION_NORTHWEST;
		}
	}

	return DIRECTION_INVALID;
}

// 1.13c D2Common+0x2E1F0
//D2Common.0x6FD74A40
// 1.13c D2Common+0x2E1F0 (ESI = room): same body -- rect x5, seed step only when the tile grid has walls or floors,
// flags 4 / 1 / 0 from the room flags; the two extern notes are d2-web diagnostics, not retail
void __fastcall DRLG_CreateRoomForRoomEx(D2DrlgStrc* pDrlg, D2DrlgRoomStrc* pDrlgRoom)
{
	D2DrlgCoordsStrc pDrlgCoords = {};
	uint32_t dwFlags = 0;

	pDrlgCoords.nTileXPos = pDrlgRoom->nTileXPos;
	pDrlgCoords.nSubtileX = pDrlgRoom->nTileXPos;
	pDrlgCoords.nTileYPos = pDrlgRoom->nTileYPos;
	pDrlgCoords.nSubtileY = pDrlgRoom->nTileYPos;
	pDrlgCoords.nTileWidth = pDrlgRoom->nTileWidth;
	pDrlgCoords.nSubtileWidth = pDrlgRoom->nTileWidth;
	pDrlgCoords.nTileHeight = pDrlgRoom->nTileHeight;
	pDrlgCoords.nSubtileHeight = pDrlgRoom->nTileHeight;

	DUNGEON_GameTileToSubtileCoords(&pDrlgCoords.nSubtileX, &pDrlgCoords.nSubtileY);
	DUNGEON_GameTileToSubtileCoords(&pDrlgCoords.nSubtileWidth, &pDrlgCoords.nSubtileHeight);

	if (pDrlgRoom->pTileGrid->pTiles.nWalls || pDrlgRoom->pTileGrid->pTiles.nFloors)
	{
		if (pDrlgRoom->dwFlags & DRLGROOMFLAG_AUTOMAP_REVEAL)
		{
			dwFlags = 4;
		}
		else if (pDrlgRoom->dwOtherFlags & 1)
		{
			dwFlags = 1;
		}
		else
		{
			dwFlags = 0;
		}

		const int nD2webRoomSeed = (int)SEED_RollRandomNumber(&pDrlgRoom->pSeed);
		// d2-web diagnosis (not retail): the order rooms are created in, and the seed each active room gets.
		{ extern void d2_note_room_create(int, int, int, int); d2_note_room_create(pDrlgRoom->pLevel ? pDrlgRoom->pLevel->nLevelId : -1, pDrlgRoom->nTileXPos * 5, pDrlgRoom->nTileYPos * 5, nD2webRoomSeed); }
		pDrlgRoom->pRoom = DUNGEON_AllocRoom(pDrlg->pAct, pDrlgRoom, &pDrlgCoords, &pDrlgRoom->pTileGrid->pTiles, nD2webRoomSeed, dwFlags);
		{ extern void d2client_unitseed_room(const D2ActiveRoomStrc*, uint32_t); d2client_unitseed_room(pDrlgRoom->pRoom, (uint32_t)nD2webRoomSeed); }   // d2-web: the client room's seed
	}
}

//D2Common.0x6FD74B40
// 1.13c D2Common+0x2DC90 (ESI = level): same walk; retail's per-room warp flag is only ever set just before it is used
void __fastcall DRLG_ComputeLevelWarpInfo(D2DrlgLevelStrc* pLevel)
{
	for (D2DrlgRoomStrc* pDrlgRoom = pLevel->pFirstRoomEx; pDrlgRoom; pDrlgRoom = pDrlgRoom->pDrlgRoomNext)
	{
		// First check if we have a waypoint
		bool bHasWarp = (pDrlgRoom->dwFlags & DRLGROOMFLAG_HAS_WAYPOINT_MASK) != 0;

		// Then check for additional warps
		if ((pDrlgRoom->dwFlags & DRLGROOMFLAG_HAS_WARP_MASK) != 0 && !bHasWarp)
		{
			int nWarpIndex = 0;
			for (int warpMask = DRLGROOMFLAG_HAS_WARP_0; (warpMask & DRLGROOMFLAG_HAS_WARP_MASK) != 0; warpMask <<= 1)
			{
				if (pDrlgRoom->dwFlags & warpMask && DRLGWARP_GetWarpDestinationFromArray(pLevel, nWarpIndex) != -1)
				{
					bHasWarp = true;
				}
				nWarpIndex++;
			}
		}

		if (bHasWarp)
		{
			int* pX = &pLevel->nRoom_Center_Warp_X[pLevel->nRoomCoords];
			int* pY = &pLevel->nRoom_Center_Warp_Y[pLevel->nRoomCoords];

			// Put warp in the center of the tile
			*pX = pDrlgRoom->nTileXPos + pDrlgRoom->nTileWidth / 2;
			*pY = pDrlgRoom->nTileYPos + pDrlgRoom->nTileHeight / 2;

			DUNGEON_GameTileToSubtileCoords(pX, pY);

			++pLevel->nRoomCoords;
		}
	}
}

//D2Common.0x6FD74C10 (#10006)
// 1.13c D2Common+0x2E360 (a level of an unknown type skips the generator but still runs the preset-map flag step and the warp info)
void __stdcall DRLG_InitLevel(D2DrlgLevelStrc* pLevel)
{
	SEED_InitLowSeed(&pLevel->pSeed, pLevel->nLevelId + pLevel->pDrlg->dwStartSeed);

	switch (pLevel->nDrlgType)
	{
	case DRLGTYPE_MAZE:
		DRLGMAZE_GenerateLevel(pLevel);
		break;

	case DRLGTYPE_PRESET:
		DRLGPRESET_GenerateLevel(pLevel);
		break;

	case DRLGTYPE_OUTDOOR:
		DRLGOUTDOORS_GenerateLevel(pLevel);
		break;

	default:
		break;
	}

	if (pLevel->nRooms && pLevel->pPresetMaps)
	{
		int nCounter = 0;
		for (D2DrlgRoomStrc* pDrlgRoom = pLevel->pFirstRoomEx; pDrlgRoom; pDrlgRoom = pDrlgRoom->pDrlgRoomNext)
		{
			if (pLevel->pPresetMaps[nCounter])
			{
				pDrlgRoom->dwOtherFlags |= 1;
			}

			++nCounter;
		}
	}

	DRLG_ComputeLevelWarpInfo(pLevel);
}

//D2Common.0x6FD74D50
// 1.13c D2Common+0x2E1A0: counts the level's rooms without the population-zero flag (0x800000)
int __fastcall DRLG_GetNumberOfPopulatedRoomsInLevel(D2DrlgStrc* pDrlg, int nLevelId)
{
	D2DrlgLevelStrc* pLevel = DRLG_GetLevel(pDrlg, nLevelId);
	int nCounter = 0;

	for (D2DrlgRoomStrc* i = pLevel->pFirstRoomEx; i; i = i->pDrlgRoomNext)
	{
		if (!(i->dwFlags & DRLGROOMFLAG_POPULATION_ZERO))
		{
			++nCounter;
		}
	}

	return nCounter;
}

//D2Common.0x6FD74D90
// 1.13c D2Common+0x2D3C0
void __fastcall DRLG_GetMinAndMaxCoordinatesFromLevel(D2DrlgLevelStrc* pLevel, int* pTileMinX, int* pTileMinY, int* pTileMaxX, int* pTileMaxY)
{
	D2DrlgRoomStrc* pDrlgRoom = pLevel->pFirstRoomEx;

	*pTileMinX = pDrlgRoom->nTileXPos;
	*pTileMinY = pDrlgRoom->nTileYPos;
	*pTileMaxX = pDrlgRoom->nTileXPos + pDrlgRoom->nTileWidth;
	*pTileMaxY = pDrlgRoom->nTileYPos + pDrlgRoom->nTileHeight;

	while (pDrlgRoom)
	{
		if (pDrlgRoom->nTileXPos + pDrlgRoom->nTileWidth > *pTileMaxX)
		{
			*pTileMaxX = pDrlgRoom->nTileXPos + pDrlgRoom->nTileWidth;
		}

		if (pDrlgRoom->nTileXPos < *pTileMinX)
		{
			*pTileMinX = pDrlgRoom->nTileXPos;
		}

		if (pDrlgRoom->nTileYPos + pDrlgRoom->nTileHeight > *pTileMaxY)
		{
			*pTileMaxY = pDrlgRoom->nTileYPos + pDrlgRoom->nTileHeight;
		}

		if (pDrlgRoom->nTileYPos < *pTileMinY)
		{
			*pTileMinY = pDrlgRoom->nTileYPos;
		}

		pDrlgRoom = pDrlgRoom->pDrlgRoomNext;
	}
}

//D2Common.0x6FD74E10
// 1.13c D2Common+0x2D660
void __fastcall DRLG_UpdateRoomExCoordinates(D2DrlgLevelStrc* pLevel)
{
	D2DrlgRoomStrc* pDrlgRoom = NULL;
	int nTileMaxX = 0;
	int nTileMinX = 0;
	int nTileMaxY = 0;
	int nTileMinY = 0;

	if (!pLevel)
	{
		FOG_DisplayWarning("ptDrlgLevel", __FILE__, __LINE__);
		return;
	}

	if (!pLevel->pFirstRoomEx)
	{
		FOG_DisplayWarning("ptDrlgLevel->ptRoomFirst", __FILE__, __LINE__);
		return;
	}

	DRLG_GetMinAndMaxCoordinatesFromLevel(pLevel, &nTileMinX, &nTileMinY, &nTileMaxX, &nTileMaxY);

	if (pLevel->nWidth < nTileMaxX - nTileMinX)
	{
		FOG_DisplayWarning("ptCoordsLevel->nSizeTileX >= nTileMaxX - nTileMinX", __FILE__, __LINE__);
		return;
	}

	if (pLevel->nHeight < nTileMaxY - nTileMinY)
	{
		FOG_DisplayWarning("ptCoordsLevel->nSizeTileY >= nTileMaxY - nTileMinY", __FILE__, __LINE__);
		return;
	}

	pDrlgRoom = pLevel->pFirstRoomEx;
	while (pDrlgRoom)
	{
		pDrlgRoom->nTileXPos += pLevel->nPosX - nTileMinX;
		pDrlgRoom->nTileYPos += pLevel->nPosY - nTileMinY;

		pDrlgRoom = pDrlgRoom->pDrlgRoomNext;
	}
}

//D2Common.0x6FD74EF0
// 1.13c D2Common+0x2D600 (retail ends in its fatal exit, assert lines 0x2FD / 0x2FE, on a null level or an empty room list)
D2DrlgRoomStrc* __fastcall DRLG_GetRoomExFromLevelAndCoordinates(D2DrlgLevelStrc* pLevel, int nX, int nY)
{
	D2_ASSERT(pLevel);
	D2_ASSERT(pLevel->pFirstRoomEx);

	for (D2DrlgRoomStrc* pDrlgRoom = pLevel->pFirstRoomEx; pDrlgRoom; pDrlgRoom = pDrlgRoom->pDrlgRoomNext)
	{
		if (DRLGROOM_AreXYInsideCoordinates(&pDrlgRoom->pDrlgCoord, nX, nY))
		{
			return pDrlgRoom;
		}
	}

	return NULL;
}

//D2Common.0x6FD74F70
// 1.13c D2Common+0x2E5C0
D2DrlgRoomStrc* __fastcall DRLG_GetRoomExFromCoordinates(int nX, int nY, D2DrlgStrc* pDrlg, D2DrlgRoomStrc* pDrlgRoomHint, D2DrlgLevelStrc* pLevel)
{
	if (D2Seam<&DRLG_GetRoomExFromCoordinates>::hook) return D2Seam<&DRLG_GetRoomExFromCoordinates>::hook(nX, nY, pDrlg, pDrlgRoomHint, pLevel);

	if (pDrlgRoomHint)
	{
		if (DRLGROOM_AreXYInsideCoordinates(&pDrlgRoomHint->pDrlgCoord, nX, nY))
		{
			return pDrlgRoomHint;
		}

		for (int i = 0; i < pDrlgRoomHint->nRoomsNear; ++i)
		{
			if (pDrlgRoomHint != pDrlgRoomHint->ppRoomsNear[i])
			{
				if (DRLGROOM_AreXYInsideCoordinates(&pDrlgRoomHint->ppRoomsNear[i]->pDrlgCoord, nX, nY))
				{
					return pDrlgRoomHint->ppRoomsNear[i];
				}
			}
		}
	}

	if (!pLevel)
	{
		int nLevelId = 0;
		if (pDrlg->pLevel)
		{
			D2DrlgLevelStrc* pCurrentLevel = pDrlg->pLevel;

			while (!DRLGROOM_AreXYInsideCoordinates(&pCurrentLevel->pLevelCoords, nX, nY))
			{
				pCurrentLevel = pCurrentLevel->pNextLevel;
				if (!pCurrentLevel)
				{
					break;
				}
			}

			if (pCurrentLevel)
			{
				nLevelId = pCurrentLevel->nLevelId;
			}
		}

		pLevel = DRLG_GetLevel(pDrlg, nLevelId);
	}

	if (!pLevel->pFirstRoomEx)
	{
		DRLG_InitLevel(pLevel);
	}

	return DRLG_GetRoomExFromLevelAndCoordinates(pLevel, nX, nY);
}

//1.10f: D2Common.0x6FD751C0
//1.13c: D2Common.0x6FD7D320
// 1.13c D2Common+0x2D320
BOOL __fastcall DRLG_IsTownLevel(int nLevelId)
{
	switch (nLevelId)
	{
	case LEVEL_ROGUEENCAMPMENT:
	case LEVEL_LUTGHOLEIN:
	case LEVEL_KURASTDOCKTOWN:
	case LEVEL_THEPANDEMONIUMFORTRESS:
	case LEVEL_HARROGATH:
		return TRUE;

	default:
		return FALSE;
	}
}

//D2Common.0x6FD75260 (#10000)
// 1.13c D2Common+0x2D970: the levels.txt LevelType column of the row
int __stdcall DRLG_GetLevelTypeFromLevelId(int nLevelId)
{
	return DATATBLS_GetLevelDefRecord(nLevelId)->dwLevelType;
}

//D2Common.0x6FD75270
// 1.13c D2Common+0x2D890
void __fastcall DRLG_SetLevelPositionAndSize(D2DrlgStrc* pDrlg, D2DrlgLevelStrc* pLevel)
{
	D2LevelDefBin* pLevelDefBin = DATATBLS_GetLevelDefRecord(pLevel->nLevelId);
	D2DrlgLevelStrc* pDependLevel = NULL;
	int nX = 0;
	int nY = 0;

	pLevel->nWidth = pLevelDefBin->dwSizeX[pDrlg->nDifficulty];
	pLevel->nHeight = pLevelDefBin->dwSizeY[pDrlg->nDifficulty];

	if (pLevelDefBin->dwDepend)
	{
		pDependLevel = DRLG_GetLevel(pDrlg, pLevelDefBin->dwDepend);

		nX = pDependLevel->nPosX;
		nY = pDependLevel->nPosY;
	}

	pLevel->nPosX = nX + pLevelDefBin->dwOffsetX;
	pLevel->nPosY = nY + pLevelDefBin->dwOffsetY;
}

//D2Common.0x6FD75300 (#10001)
// 1.13c D2Common+0x2D2C0
uint8_t __stdcall DRLG_GetActNoFromLevelId(int nLevelId)
{
	if (D2Seam<&DRLG_GetActNoFromLevelId>::hook) return D2Seam<&DRLG_GetActNoFromLevelId>::hook(nLevelId);
	//IMPROVEMENT: Lookup the act from Levels.txt

	static const int gnTownIds[] = { LEVEL_ROGUEENCAMPMENT, LEVEL_LUTGHOLEIN, LEVEL_KURASTDOCKTOWN, LEVEL_THEPANDEMONIUMFORTRESS, LEVEL_HARROGATH, 1024 };

	int nAct = 1;

	while (nLevelId >= gnTownIds[nAct])
	{
		++nAct;
		if (nAct > 5)
		{
			return 0;
		}
	}

	return nAct - 1;
}

//D2Common.0x6FD75330 (#10004)
// 1.13c D2Common+0x2D850: the levels.txt SaveMonsters column of the row
int __stdcall DRLG_GetSaveMonstersFromLevelId(int nLevelId)
{
	if (D2Seam<&DRLG_GetSaveMonstersFromLevelId>::hook) return D2Seam<&DRLG_GetSaveMonstersFromLevelId>::hook(nLevelId);
	return DATATBLS_GetLevelDefRecord(nLevelId)->dwSaveMonsters;
}

//D2Common.0x6FD75350 (#10002)
// 1.13c D2Common+0x2D810
int __stdcall DRLG_GetLOSDrawFromLevelId(int nLevelId)
{
	return DATATBLS_GetLevelDefRecord(nLevelId)->dwLOSDraw;
}

//D2Common.0x6FD75370
// 1.13c D2Common+0x2D730
D2DrlgWarpStrc* __fastcall DRLG_GetDrlgWarpFromLevelId(D2DrlgStrc* pDrlg, int nLevelId)
{
	D2LevelDefBin* pLevelDefBin = NULL;
	D2DrlgWarpStrc* pDrlgWarp = NULL;

	for (D2DrlgWarpStrc* i = pDrlg->pWarp; i; i = i->pNext)
	{
		if (i->nLevel == nLevelId)
		{
			return i;
		}
	}

	pDrlgWarp = D2_ALLOC_STRC_POOL(pDrlg->pMempool, D2DrlgWarpStrc);
	pDrlgWarp->nLevel = nLevelId;

	pLevelDefBin = DATATBLS_GetLevelDefRecord(nLevelId);
	for (int i = 0; i < 8; ++i)
	{
		pDrlgWarp->nVis[i] = pLevelDefBin->dwVis[i];
		pDrlgWarp->nWarp[i] = pLevelDefBin->dwWarp[i];
	}

	pDrlgWarp->pNext = pDrlg->pWarp;
	pDrlg->pWarp = pDrlgWarp;

	return pDrlgWarp;
}

//D2Common.0x6FD753F0
// 1.13c D2Common+0x2D210
void __fastcall DRLG_SetWarpId(D2DrlgWarpStrc* pDrlgWarp, int nVis, int nWarp, int nId)
{
	for (int i = 0; i < 8; ++i)
	{
		if (pDrlgWarp->nVis[i] == nVis)
		{
			pDrlgWarp->nWarp[i] = nWarp;
			return;
		}
	}

	if (nId == -1)
	{
		for (int i = 0; i < 8; ++i)
		{
			if (!pDrlgWarp->nVis[i] && pDrlgWarp->nWarp[i] == -1)
			{
				pDrlgWarp->nVis[i] = nVis;
				pDrlgWarp->nWarp[i] = nWarp;
				return;
			}
		}
		// 1.13c has no fatal here: with no free slot nId stays -1 and falls through to the writes below
	}

	// 1.13c writes the id at word nId + 1 and the value at word nId + 9 with no range check (nId -1 hits words 0 and 8)
	int32_t* pWords = reinterpret_cast<int32_t*>(pDrlgWarp);
	pWords[nId + 1] = nVis;
	pWords[nId + 9] = nWarp;
}

// not-1.13c: retail has no such function; each caller tests bit 0 of the DRLG flag word (drlg+0x8C) in place, e.g. 0x7B8D0
int __fastcall DRLG_IsOnClient(D2DrlgStrc* pDrlg)
{
	return pDrlg->dwFlags & DRLGFLAG_ONCLIENT;
}
