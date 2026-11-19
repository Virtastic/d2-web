#include "Drlg/D2DrlgActivate.h"
#include <test_seam.h>

#include "Drlg/D2DrlgDrlg.h"
#include "Drlg/D2DrlgDrlgRoom.h"
#include "Drlg/D2DrlgPreset.h"
#include "Drlg/D2DrlgRoomTile.h"
#include <DataTbls/LevelsIds.h>

static int gStatsClientFreedRooms;
static int gStatsClientAllocatedRooms;
static int gStatsFreedRooms;
static int gStatsAllocatedRooms;

// D2Common.0x6FDE07B0
// not-1.13c: function-pointer table, not code; retail keeps the same four setters at data rva 0x99BE8
static void (__fastcall* gRoomExSetStatus[ROOMSTATUS_COUNT])(D2DrlgRoomStrc*) =
{
	DRLGACTIVATE_RoomExSetStatus_ClientInRoom,
	DRLGACTIVATE_RoomExSetStatus_ClientInSight,
	DRLGACTIVATE_RoomExSetStatus_ClientOutOfSight,
	DRLGACTIVATE_RoomExSetStatus_Untile
};

// D2Common.0x6FDE07C0
// not-1.13c: function-pointer table, not code; retail data rva 0x99BF8 holds three byte-identical identify
// copies (0x7BE90, 0x7BE50, 0x7BE10) for slots 0-2, one body here
static void (__fastcall* gRoomExUnsetStatus[ROOMSTATUS_COUNT])(D2DrlgRoomStrc*) =
{
	DRLGACTIVATE_RoomExIdentifyRealStatus,
	DRLGACTIVATE_RoomExIdentifyRealStatus,
	DRLGACTIVATE_RoomExIdentifyRealStatus,
	DRLGACTIVATE_RoomExStatusUnset_Untile
};

//Helper functions
// not-1.13c: list insert inlined into every 1.13c status setter
static void DRLGACTIVATE_RoomExStatuslink(D2DrlgRoomStrc* pStatusRoomsListHead, D2DrlgRoomStrc* pDrlgRoom)
{
	pDrlgRoom->pStatusNext = pStatusRoomsListHead;
	pDrlgRoom->pStatusPrev = pStatusRoomsListHead->pStatusPrev;

	pStatusRoomsListHead->pStatusPrev->pStatusNext = pDrlgRoom;
	pStatusRoomsListHead->pStatusPrev = pDrlgRoom;
}
// not-1.13c: list unlink inlined into every 1.13c status setter
static void DRLGACTIVATE_RoomExStatusUnlink(D2DrlgRoomStrc* pDrlgRoom)
{
	if (pDrlgRoom->pStatusPrev && pDrlgRoom->pStatusNext)
	{
		pDrlgRoom->pStatusPrev->pStatusNext = pDrlgRoom->pStatusNext;
		pDrlgRoom->pStatusNext->pStatusPrev = pDrlgRoom->pStatusPrev;
		pDrlgRoom->pStatusPrev = nullptr;
		pDrlgRoom->pStatusNext = nullptr;
	}
}
// 1.13c D2Common+0x7BB70
static void DRLGACTIVATE_ForceRoomExStatusImpl(D2DrlgRoomStrc* pDrlgRoom, D2DrlgRoomStatus nStatus)
{
	if (pDrlgRoom->fRoomStatus == nStatus)
	{
		return;
	}
	DRLGACTIVATE_RoomExStatusUnlink(pDrlgRoom);
	// 1.13c: a status outside the four lists leaves the room unlinked and its status unchanged
	if (nStatus < ROOMSTATUS_COUNT)
	{
		DRLGACTIVATE_RoomExStatuslink(&pDrlgRoom->pLevel->pDrlg->tStatusRoomsLists[nStatus], pDrlgRoom);
		pDrlgRoom->fRoomStatus = nStatus;
	}
}
// Returns true if status changed
// not-1.13c: priority test inlined into each 1.13c status setter
static bool DRLGACTIVATE_UpdateRoomExStatusImpl(D2DrlgRoomStrc* pDrlgRoom, D2DrlgRoomStatus nStatus)
{
	// Note: Lower value has priority over others
	if (pDrlgRoom->fRoomStatus > nStatus)
	{
		DRLGACTIVATE_ForceRoomExStatusImpl(pDrlgRoom, nStatus);
		return true;
	}
	return false;
}
// Returns -1 if no status up to nMaxStatus is referenced
// not-1.13c: ref-count scan inlined into the 1.13c propagate and identify functions
static int DRLGACTIVATE_RoomExFindFirstStatusWithRefCount(D2DrlgRoomStrc* pDrlgRoom, D2DrlgRoomStatus nMaxStatus)
{
	for (int nFirstNonEmptyListStatus = 0; nFirstNonEmptyListStatus <= nMaxStatus && nFirstNonEmptyListStatus < ROOMSTATUS_COUNT; nFirstNonEmptyListStatus++)
	{
		if (pDrlgRoom->wRoomsInList[nFirstNonEmptyListStatus] != 0)
		{
			return nFirstNonEmptyListStatus;
		}
	}
	return -1;
}

//D2Common.0x6FD733D0
// 1.13c D2Common+0x7BED0
void __fastcall DRLGACTIVATE_RoomExSetStatus_ClientInRoom(D2DrlgRoomStrc* pDrlgRoom)
{
	DRLGACTIVATE_UpdateRoomExStatusImpl(pDrlgRoom, ROOMSTATUS_CLIENT_IN_ROOM);
}

//Helper function
// not-1.13c: timeout store inlined into the 1.13c client-in-sight setter
static void DRLGACTIVATE_InitRoomsInitTimeout(D2DrlgStrc* pDrlg)
{
	pDrlg->nRoomsInitTimeout = 2 * (DRLG_IsOnClient(pDrlg) == 0) + 5;
}
// 1.13c D2Common+0x7C0E0
static void DRLGACTIVATE_RoomEx_CreateRoom(D2DrlgRoomStrc* pDrlgRoom)
{
	D2DrlgStrc* pDrlg = pDrlgRoom->pLevel->pDrlg;
	if (!pDrlgRoom->nRoomsNear)
	{
		sub_6FD77BB0(pDrlg->pMempool, pDrlgRoom);
	}

	DRLGROOMTILE_InitRoomGrids(pDrlgRoom);
	DRLGROOMTILE_AddRoomMapTiles(pDrlgRoom);
	DRLG_CreateRoomForRoomEx(pDrlg, pDrlgRoom);
	++pDrlg->nRoomsInitSinceLastUpdate;
	++pDrlg->nAllocatedRooms;
}
// not-1.13c: the guard and timeout are inlined into the 1.13c client-in-sight setter
void DRLGACTIVATE_RoomEx_EnsureHasRoom(D2DrlgRoomStrc* pDrlgRoom, bool bInitTimeoutCounter)
{
	if (pDrlgRoom->pRoom == nullptr && !(pDrlgRoom->dwFlags & DRLGROOMFLAG_HAS_ROOM))
	{
		DRLGACTIVATE_RoomEx_CreateRoom(pDrlgRoom);
		if (bInitTimeoutCounter)
		{
			DRLGACTIVATE_InitRoomsInitTimeout(pDrlgRoom->pLevel->pDrlg);
		}
	}
}

//D2Common.0x6FD73450
// 1.13c D2Common+0x7C370
void __fastcall DRLGACTIVATE_RoomExSetStatus_ClientInSight(D2DrlgRoomStrc* pDrlgRoom)
{
	DRLGACTIVATE_RoomEx_EnsureHasRoom(pDrlgRoom, true);
	DRLGACTIVATE_UpdateRoomExStatusImpl(pDrlgRoom, ROOMSTATUS_CLIENT_IN_SIGHT);
}

//D2Common.0x6FD73550
// 1.13c D2Common+0x7C470
void __fastcall DRLGACTIVATE_RoomExSetStatus_ClientOutOfSight(D2DrlgRoomStrc* pDrlgRoom)
{
	if (pDrlgRoom->dwFlags & DRLGROOMFLAG_TILELIB_LOADED 
		&& (pDrlgRoom->nType != DRLGTYPE_PRESET || (pDrlgRoom->dwFlags & DRLGROOMFLAG_PRESET_UNITS_ADDED) != 0))
	{

		if (DRLGACTIVATE_UpdateRoomExStatusImpl(pDrlgRoom, ROOMSTATUS_CLIENT_OUT_OF_SIGHT)
			&& pDrlgRoom->wRoomsInList[ROOMSTATUS_CLIENT_IN_SIGHT] != 0)
		{
			DRLGACTIVATE_RoomExSetStatus_ClientInSight(pDrlgRoom);
		}
	}
}

//D2Common.0x6FD736F0
// 1.13c D2Common+0x7BEE0
void __fastcall DRLGACTIVATE_RoomExSetStatus_Untile(D2DrlgRoomStrc* pDrlgRoom)
{
	if (!(pDrlgRoom->dwFlags & DRLGROOMFLAG_TILELIB_LOADED))
	{
		DRLGROOMTILE_LoadDT1FilesForRoom(pDrlgRoom);
	}

	if (pDrlgRoom->nType == DRLGTYPE_PRESET && !(pDrlgRoom->dwFlags & DRLGROOMFLAG_PRESET_UNITS_ADDED))
	{
		DRLGPRESET_SpawnHardcodedPresetUnits(pDrlgRoom);
	}
	DRLGACTIVATE_UpdateRoomExStatusImpl(pDrlgRoom, ROOMSTATUS_UNTILE);
}

//D2Common.0x6FD73790
// 1.13c D2Common+0x7BE90
// 1.13c D2Common+0x7BE50
// 1.13c D2Common+0x7BE10
// 1.13c D2Common+0x7BC20
// (four byte-identical retail copies: unset slots 0, 1, 2 and the untile unsetter's register-call copy)
void __fastcall DRLGACTIVATE_RoomExIdentifyRealStatus(D2DrlgRoomStrc* pDrlgRoom)
{
	if (pDrlgRoom->fRoomStatus >= ROOMSTATUS_COUNT || pDrlgRoom->wRoomsInList[pDrlgRoom->fRoomStatus] == 0)
	{
		uint8_t nStatus = 0;
		while (nStatus < ROOMSTATUS_COUNT && pDrlgRoom->wRoomsInList[nStatus] == 0)
		{
			++nStatus;
		}
		DRLGACTIVATE_ForceRoomExStatusImpl(pDrlgRoom, D2DrlgRoomStatus(nStatus));
		// 1.13c: the status byte is stored after the move even when no list takes the room (4)
		pDrlgRoom->fRoomStatus = nStatus;
	}
}

//D2Common.0x6FD73880
// 1.13c D2Common+0x7BDB0
void __fastcall DRLGACTIVATE_RoomExStatusUnset_Untile(D2DrlgRoomStrc* pDrlgRoom)
{
	if (pDrlgRoom->fRoomStatus != ROOMSTATUS_COUNT)
	{
		DRLGACTIVATE_RoomExIdentifyRealStatus(pDrlgRoom);
		if (pDrlgRoom->fRoomStatus == ROOMSTATUS_COUNT)
		{
			D2DrlgStrc* pDrlg = pDrlgRoom->pLevel->pDrlg;
			D2_ASSERT(pDrlg);
			if (DRLG_IsOnClient(pDrlg))
			{
				DRLGROOMTILE_FreeRoom(pDrlgRoom, false);
			}
		}
	}
}

// F500: `sub_6FD77F00` can put a room of the warp DESTINATION level into ppRoomsNear. Live play follows
// retail's cross-level status propagation; the session bulk-town-create window is gated in
// d2_cross_level_sight_on because it sights every town room in one call. The explicit switch remains
// a probe counterfactual. See F500.
extern "C" int d2_cross_level_sight_on(void);

//D2Common.0x6FD73A30
// 1.13c D2Common+0x7BFA0
void __fastcall DRLGACTIVATE_RoomExPropagateSetStatus(void* pMemPool, D2DrlgRoomStrc* pDrlgRoom, uint8_t nStatus)
{
	if (pDrlgRoom->nRoomsNear == 0)
	{
		sub_6FD77BB0(pMemPool, pDrlgRoom);
	}

	for (int i = 0; i < pDrlgRoom->nRoomsNear; ++i)
	{
		D2DrlgRoomStrc* pNearRoom = pDrlgRoom->ppRoomsNear[i];
		if (pNearRoom->pLevel != pDrlgRoom->pLevel && !d2_cross_level_sight_on())
		{
			continue;
		}
		if (nStatus + 1 < ROOMSTATUS_COUNT)
		{
			DRLGACTIVATE_RoomExPropagateSetStatus(pMemPool, pNearRoom, nStatus + 1);
		}

		if (pNearRoom->fRoomStatus >= nStatus)
		{
			if (DRLGACTIVATE_RoomExFindFirstStatusWithRefCount(pNearRoom, D2DrlgRoomStatus(nStatus)) < 0)
			{
				gRoomExSetStatus[nStatus](pNearRoom);
			}
		}
		++pNearRoom->wRoomsInList[nStatus];
	}
}

//D2Common.0x6FD73BE0
// 1.13c D2Common+0x7BB20
void __fastcall DRLGACTIVATE_RoomExPropagateUnsetStatus(D2DrlgRoomStrc* pDrlgRoom, uint8_t nStatus)
{
	for (int i = 0; i < pDrlgRoom->nRoomsNear; ++i)
	{
		if (pDrlgRoom->ppRoomsNear[i] && pDrlgRoom->ppRoomsNear[i]->pLevel != pDrlgRoom->pLevel && !d2_cross_level_sight_on())
		{
			continue;
		}
		if (pDrlgRoom->ppRoomsNear[i])
		{
			--pDrlgRoom->ppRoomsNear[i]->wRoomsInList[nStatus];

			gRoomExUnsetStatus[nStatus](pDrlgRoom->ppRoomsNear[i]);

			if (nStatus + 1 < ROOMSTATUS_COUNT)
			{
				DRLGACTIVATE_RoomExPropagateUnsetStatus(pDrlgRoom->ppRoomsNear[i], nStatus + 1);
			}
		}
	}
}

// Helper functions
// 1.13c D2Common+0x7C090
static void DRLGACTIVATE_RoomSetAndPropagateStatus(D2DrlgRoomStrc* pDrlgRoom, D2DrlgRoomStatus nStatus)
{
	DRLGACTIVATE_RoomExPropagateSetStatus(pDrlgRoom->pLevel->pDrlg->pMempool, pDrlgRoom, nStatus + 1);
	if (pDrlgRoom->fRoomStatus >= nStatus)
	{
		if (DRLGACTIVATE_RoomExFindFirstStatusWithRefCount(pDrlgRoom, nStatus) < 0)
		{
			gRoomExSetStatus[nStatus](pDrlgRoom);
		}
	}
	++pDrlgRoom->wRoomsInList[nStatus];
}

// not-1.13c: inlined into the 1.13c unset-in-sight (0x7BF20) and change-client-room export (0x3CC40)
static void DRLGACTIVATE_RoomUnsetAndPropagateStatus(D2DrlgRoomStrc* pDrlgRoom, D2DrlgRoomStatus nStatus)
{
	if (pDrlgRoom->wRoomsInList[nStatus] != 0)
	{
		--pDrlgRoom->wRoomsInList[nStatus];
		gRoomExUnsetStatus[nStatus](pDrlgRoom);
		DRLGACTIVATE_RoomExPropagateUnsetStatus(pDrlgRoom, nStatus + 1);
	}
}

//D2Common.0x6FD739A0
// 1.13c D2Common+0x7C300
void __fastcall DRLGACTIVATE_SetClientIsInSight(D2DrlgStrc* pDrlg, int nLevelId, int nX, int nY, D2DrlgRoomStrc* pDrlgRoomHint)
{
	D2DrlgLevelStrc* pLevel = DRLG_GetLevel(pDrlg, nLevelId);
	if (pDrlgRoomHint && pDrlgRoomHint->pLevel->nLevelId != nLevelId)
	{
		pDrlgRoomHint = nullptr;
	}
	D2DrlgRoomStrc* pDrlgRoom = DRLG_GetRoomExFromCoordinates(nX, nY, pDrlg, pDrlgRoomHint, pLevel);

	if (pDrlgRoom->wRoomsInList[ROOMSTATUS_CLIENT_IN_SIGHT] == 0)
	{
		DRLGACTIVATE_RoomSetAndPropagateStatus(pDrlgRoom, ROOMSTATUS_CLIENT_IN_SIGHT);
	}
}

//D2Common.0x6FD73B40
// 1.13c D2Common+0x7BF20
void __fastcall DRLGACTIVATE_UnsetClientIsInSight(D2DrlgStrc* pDrlg, int nLevelId, int nX, int nY, D2DrlgRoomStrc* pDrlgRoomHint)
{
	D2DrlgLevelStrc* pLevel = DRLG_GetLevel(pDrlg, nLevelId);
	if (pDrlgRoomHint && pDrlgRoomHint->pLevel->nLevelId != nLevelId)
	{
		pDrlgRoomHint = nullptr;
	}

	D2DrlgRoomStrc* pDrlgRoom = DRLG_GetRoomExFromCoordinates(nX, nY, pDrlg, pDrlgRoomHint, pLevel);
	if (pDrlgRoom && pDrlgRoom->wRoomsInList[ROOMSTATUS_CLIENT_IN_SIGHT] != 0)
	{
		--pDrlgRoom->wRoomsInList[ROOMSTATUS_CLIENT_IN_SIGHT];
		gRoomExUnsetStatus[ROOMSTATUS_CLIENT_IN_SIGHT](pDrlgRoom);
		DRLGACTIVATE_RoomExPropagateUnsetStatus(pDrlgRoom, ROOMSTATUS_CLIENT_OUT_OF_SIGHT);
	}
}


//D2Common.0x6FD73C40
// not-1.13c: inlined into the 1.13c change-client-room export, which takes active rooms
void __fastcall DRLGACTIVATE_ChangeClientRoom(D2DrlgRoomStrc* pPreviousRoom, D2DrlgRoomStrc* pNewRoom)
{
	if (pPreviousRoom == pNewRoom)
	{
		return;
	}

	if (pNewRoom)
	{
		DRLGACTIVATE_RoomSetAndPropagateStatus(pNewRoom, ROOMSTATUS_CLIENT_IN_ROOM);
	}

	if (pPreviousRoom)
	{
		DRLGACTIVATE_RoomUnsetAndPropagateStatus(pPreviousRoom, ROOMSTATUS_CLIENT_IN_ROOM);
	}
}

//D2Common.0x6FD73CF0
// 1.13c D2Common+0x7C270
void __fastcall DRLGACTIVATE_InitializeRoomEx(D2DrlgRoomStrc* pDrlgRoom)
{
	if (D2Seam<&DRLGACTIVATE_InitializeRoomEx>::hook) { D2Seam<&DRLGACTIVATE_InitializeRoomEx>::hook(pDrlgRoom); return; }
	if (!(pDrlgRoom->dwFlags & DRLGROOMFLAG_TILELIB_LOADED))
	{
		DRLGROOMTILE_LoadDT1FilesForRoom(pDrlgRoom);
	}

	if (!(pDrlgRoom->dwFlags & DRLGROOMFLAG_PRESET_UNITS_ADDED) && (pDrlgRoom->nType == DRLGTYPE_PRESET))
	{
		DRLGPRESET_SpawnHardcodedPresetUnits(pDrlgRoom);
	}

	// 1.13c: only the has-room flag gates the build, never the active-room pointer
	if (!(pDrlgRoom->dwFlags & DRLGROOMFLAG_HAS_ROOM))
	{
		DRLGACTIVATE_RoomEx_CreateRoom(pDrlgRoom);
	}
}

//D2Common.0x6FD73D80
// not-1.13c: inlined into the 1.13c stream-room export 0x3CDA0
D2ActiveRoomStrc* __fastcall DRLGACTIVATE_StreamRoomAtCoords(D2DrlgStrc* pDrlg, int nX, int nY)
{
	if (D2DrlgRoomStrc* pDrlgRoom = DRLG_GetRoomExFromCoordinates(nX, nY, pDrlg, 0, 0))
	{
		DRLGACTIVATE_InitializeRoomEx(pDrlgRoom);
		return pDrlgRoom->pRoom;
	}
	return nullptr;
}

//D2Common.0x6FD73E30
// 1.13c D2Common+0x7B8D0
void __fastcall DRLGACTIVATE_InitializeRoomExStatusLists(D2DrlgStrc* pDrlg)
{
	for (int nStatus = 0; nStatus < ROOMSTATUS_COUNT; ++nStatus)
	{
		D2DrlgRoomStrc* pDrlgRoomStatusList = &pDrlg->tStatusRoomsLists[nStatus];
		pDrlgRoomStatusList->fRoomStatus = nStatus;
		pDrlgRoomStatusList->pStatusNext = pDrlgRoomStatusList;
		pDrlgRoomStatusList->pStatusPrev = pDrlgRoomStatusList;
	}
}

//Helper function
// not-1.13c: list-head read inlined into the 1.13c exports 0x3C5E0 and 0x3C570
static D2DrlgRoomStrc* DRLGACTIVATE_RoomExStatusList_GetFirst(D2DrlgStrc* pDrlg, D2DrlgRoomStatus nStatus)
{
	const D2DrlgRoomStrc* pStatusListHead = &pDrlg->tStatusRoomsLists[nStatus];
	if (pStatusListHead->pStatusNext && pStatusListHead != pStatusListHead->pStatusNext) // If not empty
	{
		return pStatusListHead->pStatusNext;
	}
	return nullptr;
}

//D2Common.0x6FD73E60
// not-1.13c: inlined into the 1.13c export 0x3C5E0 (#10811)
D2ActiveRoomStrc* __fastcall DRLGACTIVATE_GetARoomInClientSight(D2DrlgStrc* pDrlg)
{
	if (D2DrlgRoomStrc* pDrlgRoom = DRLGACTIVATE_RoomExStatusList_GetFirst(pDrlg, ROOMSTATUS_CLIENT_IN_ROOM))
	{
		return pDrlgRoom->pRoom;
	}
	if (D2DrlgRoomStrc* pDrlgRoom = DRLGACTIVATE_RoomExStatusList_GetFirst(pDrlg, ROOMSTATUS_CLIENT_IN_SIGHT))
	{
		return pDrlgRoom->pRoom;
	}
	return nullptr;
}

//D2Common.0x6FD73E90
// not-1.13c: inlined into the 1.13c export 0x3C570 (#10547)
D2ActiveRoomStrc* __fastcall DRLGACTIVATE_GetARoomInSightButWithoutClient(D2DrlgStrc* pDrlg, D2DrlgRoomStrc* pDrlgRoom)
{
	if (D2DrlgRoomStrc* pNextStatusRoom = pDrlgRoom->pStatusNext)
	{
		if (pDrlgRoom->fRoomStatus != ROOMSTATUS_CLIENT_IN_ROOM || pNextStatusRoom != &pDrlg->tStatusRoomsLists[ROOMSTATUS_CLIENT_IN_ROOM])
		{
			return pNextStatusRoom->pRoom;
		}

		if (D2DrlgRoomStrc* pFirstRoomInSight = DRLGACTIVATE_RoomExStatusList_GetFirst(pDrlg, ROOMSTATUS_CLIENT_IN_SIGHT))
		{
			return pFirstRoomInSight->pRoom;
		}
	}

	return nullptr;
}

//D2Common.0x6FD73EF0 (#10015)
// 1.13c D2Common+0x7B810
void __fastcall DRLGACTIVATE_GetRoomsAllocationStats(int* pOutStatsClientAllocatedRooms, int* pOutStatsClientFreedRooms, int* pOutStatsAllocatedRooms, int* pOutStatsFreedRooms)
{
	*pOutStatsClientAllocatedRooms = gStatsClientAllocatedRooms;
	*pOutStatsClientFreedRooms = gStatsClientFreedRooms;
	*pOutStatsAllocatedRooms = gStatsAllocatedRooms;
	*pOutStatsFreedRooms = gStatsFreedRooms;
}

//D2Common.0x6FD73F20 (#10003)
// 1.13c D2Common+0x7C170
void __stdcall DRLGACTIVATE_Update(D2DrlgStrc* pDrlg)
{
	if (DRLG_IsOnClient(pDrlg))
	{
		gStatsClientAllocatedRooms = pDrlg->nAllocatedRooms;
		gStatsClientFreedRooms = pDrlg->nFreedRooms;
	}
	else
	{
		gStatsAllocatedRooms = pDrlg->nAllocatedRooms;
		gStatsFreedRooms = pDrlg->nFreedRooms;
	}

	if (pDrlg->nRoomsInitSinceLastUpdate > 1)
	{
		pDrlg->nRoomsInitSinceLastUpdate = 0;
		return;
	}

	--pDrlg->nRoomsInitTimeout;

	if (pDrlg->nRoomsInitTimeout == 0)
	{
		DRLGACTIVATE_InitRoomsInitTimeout(pDrlg);

		if (!pDrlg->pDrlgRoom || pDrlg->pDrlgRoom->fRoomStatus != ROOMSTATUS_CLIENT_OUT_OF_SIGHT)
		{
			pDrlg->pDrlgRoom = pDrlg->tStatusRoomsLists[ROOMSTATUS_CLIENT_OUT_OF_SIGHT].pStatusNext;
		}


		if (D2DrlgRoomStrc* pDrlgRoomExListHead = pDrlg->pDrlgRoom)
		{
			D2DrlgRoomStrc* pCurRoomEx = pDrlgRoomExListHead;
			do
			{
				if (pCurRoomEx != &pDrlg->tStatusRoomsLists[ROOMSTATUS_CLIENT_OUT_OF_SIGHT])
				{
					DRLGACTIVATE_RoomEx_EnsureHasRoom(pCurRoomEx, false);
				}
				pCurRoomEx = pCurRoomEx->pStatusNext;
			} while (pCurRoomEx != pDrlgRoomExListHead && pDrlg->nRoomsInitSinceLastUpdate == 0);

			pDrlg->pDrlgRoom = pCurRoomEx;
			pDrlg->nRoomsInitSinceLastUpdate = 0;
		}
	}
}

//D2Common.0x6FD74060
// 1.13c D2Common+0x7BA00
BOOL __fastcall DRLGACTIVATE_TestRoomCanUnTile(D2DrlgRoomStrc* pDrlgRoom)
{
	if (pDrlgRoom->dwFlags & DRLGROOMFLAG_HASPORTAL)
	{
		return FALSE;
	}

	D2_ASSERT(pDrlgRoom->pLevel);
	D2_ASSERT(pDrlgRoom->pLevel->pDrlg);
	D2_ASSERT(!DRLG_IsOnClient(pDrlgRoom->pLevel->pDrlg));

	if (pDrlgRoom->fRoomStatus <= 1)
	{
		return FALSE;
	}

	if (DRLG_IsTownLevel(pDrlgRoom->pLevel->nLevelId) || pDrlgRoom->pLevel->nLevelId == LEVEL_ROCKYSUMMIT)
	{
		for (D2DrlgRoomStrc* pCurrentRoomEx = pDrlgRoom->pLevel->pFirstRoomEx; pCurrentRoomEx; pCurrentRoomEx = pCurrentRoomEx->pDrlgRoomNext)
		{
			if (pCurrentRoomEx->fRoomStatus <= 1)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

//D2Common.0x6FD740F0
// not-1.13c: inlined into the 1.13c export 0x3BC10 (#10346)
void __fastcall DRLGACTIVATE_ToggleHasPortalFlag(D2DrlgRoomStrc* pDrlgRoom, BOOL bReset)
{
	if (bReset)
	{
		pDrlgRoom->dwFlags &= ~DRLGROOMFLAG_HASPORTAL;
	}
	else
	{
		pDrlgRoom->dwFlags |= DRLGROOMFLAG_HASPORTAL;
	}
}

//D2Common.0x6FD74110
// not-1.13c: inlined into the 1.13c export 0x3BF00 (#10969)
uint8_t __fastcall DRLGACTIVATE_GetRoomStatusFlags(D2DrlgRoomStrc* pDrlgRoom)
{
	return pDrlgRoom->fRoomStatus;
}
