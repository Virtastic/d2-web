#include "Units/UnitRoom.h"
#include <test_seam.h>

#include "D2Dungeon.h"
#include "D2StatList.h"
#include "Units/Units.h"
#include "Path/Path.h"


// 1.13c D2Common +0x0D540 reads DAT_6fddd458/45c. Both addresses are in
// .rdata and contain zero in the 1.13c DLL; +0x3C870 reads the same pair.
constexpr int dword_6FDDD458 = 0;
constexpr int dword_6FDDD45C = 0;

// 1.13c D2Common+0x66630: the unit's tile must lie in the room (fatal 0x2E otherwise); a unit already on the room's
// list answers 0, or is fatal (0x36) when the caller passes 1. The default room check of UNITROOM_AddUnitToRoomEx.
static BOOL UNITROOM_CheckUnitForAdd(D2ActiveRoomStrc* pRoom, D2UnitStrc* pUnit, int nOne)
{
	D2CoordStrc tCoord = {};
	UNITS_GetCoords(pUnit, &tCoord);
	D2_ASSERT(DungeonTestRoomGame(pRoom, tCoord.nX, tCoord.nY));

	for (D2UnitStrc* i = pRoom->pUnitFirst; i; i = i->pRoomNext)
	{
		if (i == pUnit)
		{
			D2_ASSERTM(!nOne, "Unit being added to a room it is already in");
			return FALSE;
		}
	}
	return TRUE;
}

// not-1.13c: d2-web callee table that binds the 1.13c add-to-room port (D2Common+0x66800) to the 1.10f source functions; retail calls them directly
const D2UnitRoomAddSeams& UNITROOM_AddSeams()
{
	static const D2UnitRoomAddSeams kSeams = {
		[](D2ActiveRoomStrc* r, D2UnitStrc* u, int n) -> BOOL { return UNITROOM_CheckUnitForAdd(r, u, n); },
		[](D2UnitStrc* u) { UNITROOM_RefreshUnit(u); },
		[](D2UnitStrc* u) -> int { return STATLIST_GetUnitAlignment(u); },
	};
	return kSeams;
}

//1.10f: D2Common.0x6FDBCF10 (#11279)
// 1.13c D2Common+0x66800 (#10531). 1.13c deltas: the room check is one call (0x66630 with 1: box test and the
// "already in" walk) whose 0 answer is fatal 0x4C; the list head is the room's own field (no DUNGEON_GetUnitListFromRoom);
// the allied count is bumped inline. Fatal codes: null unit 0x42, null room 0x43.
int UNITROOM_AddUnitToRoomWith(D2UnitStrc* pUnit, D2ActiveRoomStrc* pRoom, const D2UnitRoomAddSeams& s)
{
	D2_ASSERT(pUnit);
	D2_ASSERT(pRoom);
	D2_ASSERT(s.check(pRoom, pUnit, 1));

	pUnit->pRoomNext = pRoom->pUnitFirst;
	pRoom->pUnitFirst = pUnit;
	s.refresh(pUnit);

	if (pUnit->dwUnitType == UNIT_PLAYER || (pUnit->dwUnitType == UNIT_MONSTER && s.alignment(pUnit) == UNIT_ALIGNMENT_GOOD))
	{
		++pRoom->nAllies;
	}

	return 1;
}

// 1.13c D2Common+0x66800
int __stdcall UNITROOM_AddUnitToRoomEx(D2UnitStrc* pUnit, D2ActiveRoomStrc* pRoom, int nUnused)
{
	D2_MAYBE_UNUSED(nUnused);
	return UNITROOM_AddUnitToRoomWith(pUnit, pRoom, UNITROOM_AddSeams());
}

//D2Common.0x6FDBD100 (#10384)
// 1.13c D2Common+0x66900
int __stdcall UNITROOM_AddUnitToRoom(D2UnitStrc* pUnit, D2ActiveRoomStrc* pRoom)
{
	if (D2Seam<&UNITROOM_AddUnitToRoom>::hook) return D2Seam<&UNITROOM_AddUnitToRoom>::hook(pUnit, pRoom);
	return UNITROOM_AddUnitToRoomEx(pUnit, pRoom, 1);
}

// 1.13c D2Common+0x665C0 (#10158)
void __stdcall UNITROOM_RefreshUnit(D2UnitStrc* pUnit)
{
	if (D2Seam<&UNITROOM_RefreshUnit>::hook) { D2Seam<&UNITROOM_RefreshUnit>::hook(pUnit); return; }

	D2_ASSERT(pUnit);

	D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pUnit);
	if (pRoom && !D2Common_10084(pRoom) && !(pUnit->dwFlags & UNITFLAG_ISLINKREFRESHMSG))
	{
		pUnit->dwFlags |= UNITFLAG_ISLINKREFRESHMSG;

		D2UnitStrc** ppUnitFirst = DUNGEON_GetUnitUpdateListFromRoom(pRoom, TRUE);
		D2_ASSERT(ppUnitFirst);

		pUnit->pChangeNextUnit = *ppUnitFirst;
		*ppUnitFirst = pUnit;
	}
}

//1.10f: D2Common.0x6FDBD1B0 (#10388)
// 1.13c D2Common+0x664B0 (#10384): one pass over the room's unit list, moving a unit ahead of a lower client-Y predecessor.
void __stdcall UNITROOM_SortUnitListByTargetY(D2ActiveRoomStrc* pRoom)
{
	D2UnitStrc** ppUnitFirst = NULL;
	D2UnitStrc** ppUnit = NULL;
	D2UnitStrc* pPreviousUnit = NULL;
	D2UnitStrc* pNextUnit = NULL;
	D2UnitStrc* pUnit = NULL;
	bool bContinue = false;

	ppUnit = DUNGEON_GetUnitListFromRoom(pRoom);
	pUnit = *ppUnit;
	ppUnitFirst = ppUnit;

	if (pUnit)
	{
		pNextUnit = pUnit->pRoomNext;
		do
		{
			bContinue = false;

			while (pNextUnit)
			{
				if (UNITS_GetClientCoordY(pUnit) <= UNITS_GetClientCoordY(pNextUnit))
				{
					pPreviousUnit = pUnit;
					pUnit = pNextUnit;
					pNextUnit = pNextUnit->pRoomNext;
				}
				else
				{
					if (pPreviousUnit)
					{
						pPreviousUnit->pRoomNext = pNextUnit;
					}
					else
					{
						*ppUnitFirst = pNextUnit;
					}

					pPreviousUnit = pNextUnit;
					pUnit->pRoomNext = pNextUnit->pRoomNext;
					pNextUnit->pRoomNext = pUnit;
					pNextUnit = pUnit->pRoomNext;
					bContinue = true;
				}
			}
		}
		while (bContinue);
	}
}

// 1.13c D2Common+0x0D540 decompile offsets pinned (pointer-bearing fields are native-width on wasm64, so only the leading scalars) for the ResetDynamicPath body.
#include <cstddef>
static_assert(offsetof(D2DynamicPathStrc, tGameCoords.dwPrecisionX) == 0x00, "D540 [0]");
static_assert(offsetof(D2DynamicPathStrc, tGameCoords.dwPrecisionY) == 0x04, "D540 [1]");
static_assert(offsetof(D2DynamicPathStrc, dwClientCoordX) == 0x08, "D540 [2]");
static_assert(offsetof(D2DynamicPathStrc, dwClientCoordY) == 0x0C, "D540 [3]");
static_assert(PATH_CURRENT_ROOM_INVALID == 2, "D540 flag |2");
// Retail passes the path in ESI; room and unit fields remain native pointers.
// Not static: the function probe (src/core/fn/d2common_d540.cpp) calls it.
// 1.13c D2Common+0xD540
void UNITROOM_ResetDynamicPath(D2DynamicPathStrc* pPath)
{
	pPath->tGameCoords.dwPrecisionX = dword_6FDDD458;
	pPath->tGameCoords.dwPrecisionY = dword_6FDDD45C;

	pPath->dwClientCoordX = dword_6FDDD458;
	pPath->dwClientCoordY = dword_6FDDD45C;

	pPath->dwPathPoints = 0;

	if (pPath->pRoom)
	{
		pPath->pPreviousRoom = pPath->pRoom;
		UNITROOM_RemoveUnitFromRoom(pPath->pUnit);
		pPath->dwFlags |= PATH_CURRENT_ROOM_INVALID;
	}
}

//1.00:  D2Common.0x1006D1E0 (#10385)
//1.10f: D2Common.0x6FDBD250 (#10390)
// 1.13c D2Common+0x668A0
void __stdcall UNITROOM_UpdatePath(D2UnitStrc* pUnit)
{
	if (D2Seam<&UNITROOM_UpdatePath>::hook) { D2Seam<&UNITROOM_UpdatePath>::hook(pUnit); return; }
	D2_ASSERT(pUnit);

	switch (pUnit->dwUnitType)
	{
	case UNIT_PLAYER:
	case UNIT_MONSTER:
	case UNIT_MISSILE:
		UNITROOM_ResetDynamicPath(pUnit->pDynamicPath);
		break;

	case UNIT_OBJECT:
	case UNIT_ITEM:
	case UNIT_TILE:
		if (pUnit->pStaticPath->pRoom)
		{
			UNITROOM_RemoveUnitFromRoom(pUnit);
			pUnit->pStaticPath->pRoom = NULL;
		}
		break;
	default:
		break;
	}
}

//D2Common.0x6FDBD2B0 (#10391)
// 1.13c D2Common+0x663B0 (#10127). Delta: a null room is retail-fatal (0x2AD).
void __stdcall UNITROOM_ClearUpdateQueue(D2ActiveRoomStrc* pRoom)
{
	D2UnitStrc** ppChangeUnit = NULL;
	D2UnitStrc* pNextChangeUnit = NULL;

	D2_ASSERT(pRoom);

	ppChangeUnit = DUNGEON_GetUnitUpdateListFromRoom(pRoom, FALSE);
	while (*ppChangeUnit)
	{
		pNextChangeUnit = (*ppChangeUnit)->pChangeNextUnit;
		(*ppChangeUnit)->dwFlags &= ~UNITFLAG_ISLINKREFRESHMSG;
		(*ppChangeUnit)->pChangeNextUnit = NULL;
		*ppChangeUnit = pNextChangeUnit;
	}

	*ppChangeUnit = NULL;
}

// 1.13c D2Common+0x66700 (#11016). Delta: with a room, retail checks nothing after the unlink; the "room link must be
// clear" fatal (0xF7) is only for a unit that has no room (1.10f asserted it on every path).
//D2Common.0x6FDBD300 (#10386)
void __stdcall UNITROOM_RemoveUnitFromRoom(D2UnitStrc* pUnit)
{
	if (D2Seam<&UNITROOM_RemoveUnitFromRoom>::hook) { D2Seam<&UNITROOM_RemoveUnitFromRoom>::hook(pUnit); return; }
	D2UnitStrc** ppRoomUnit = NULL;
	D2UnitStrc* pPreviousUnit = NULL;
	D2UnitStrc* pRoomUnit = NULL;
	D2ActiveRoomStrc* pRoom = NULL;
	bool bReset = false;

	D2_ASSERT(pUnit);

	pRoom = UNITS_GetRoom(pUnit);
	if (pRoom)
	{
		ppRoomUnit = DUNGEON_GetUnitListFromRoom(pRoom);
		pRoomUnit = *ppRoomUnit;

		if (pRoomUnit)
		{
			while (pRoomUnit != pUnit)
			{
				pPreviousUnit = pRoomUnit;
				pRoomUnit = pRoomUnit->pRoomNext;
				if (!pRoomUnit)
				{
					break;
				}
			}

			if (pRoomUnit)
			{
				bReset = true;
				if (pPreviousUnit)
				{
					pPreviousUnit->pRoomNext = pRoomUnit->pRoomNext;
				}
				else
				{
					*ppRoomUnit = pRoomUnit->pRoomNext;
				}

				pRoomUnit->pRoomNext = NULL;

				if (pUnit->dwUnitType == UNIT_PLAYER || pUnit->dwUnitType == UNIT_MONSTER && STATLIST_GetUnitAlignment(pUnit) == UNIT_ALIGNMENT_GOOD)
				{
					DUNGEON_DecreaseAlliedCountOfRoom(pRoom);
				}
			}
		}

		UNITROOM_RemoveUnitFromUpdateQueue(pUnit);
		pUnit->dwFlags &= ~UNITFLAG_ISLINKREFRESHMSG;
		if (bReset)
		{
			UNITS_ResetRoom(pUnit);
		}
	}
	else
	{
		D2_ASSERT(!pUnit->pRoomNext);
	}
}

//1.10f: D2Common.0x6FDBD400 (#10387)
// 1.13c D2Common+0x66410 (#10681): retail checks for a stale update link only when the unit has no room; with a room it
// unlinks the unit if it is on the update list and checks nothing after (1.10f checked the link on every path).
void __stdcall UNITROOM_RemoveUnitFromUpdateQueue(D2UnitStrc* pUnit)
{
	D2UnitStrc** ppChangeUnit = NULL;
	D2UnitStrc* pPreviousChangeUnit = NULL;
	D2UnitStrc* pChangeUnit = NULL;
	D2ActiveRoomStrc* pRoom = NULL;

	D2_ASSERT(pUnit);

	pRoom = UNITS_GetRoom(pUnit);
	if (!pRoom)
	{
		D2_ASSERT(!pUnit->pChangeNextUnit);
		return;
	}
	{
		ppChangeUnit = DUNGEON_GetUnitUpdateListFromRoom(pRoom, FALSE);
		pChangeUnit = *ppChangeUnit;

		if (pChangeUnit)
		{
			while (pChangeUnit != pUnit)
			{
				pPreviousChangeUnit = pChangeUnit;
				pChangeUnit = pChangeUnit->pChangeNextUnit;
				if (!pChangeUnit)
				{
					return;
				}
			}

			if (pChangeUnit)
			{
				if (pPreviousChangeUnit)
				{
					pPreviousChangeUnit->pChangeNextUnit = pChangeUnit->pChangeNextUnit;
					pChangeUnit->pChangeNextUnit = NULL;
				}
				else
				{
					*ppChangeUnit = pChangeUnit->pChangeNextUnit;
					pChangeUnit->pChangeNextUnit = NULL;
				}
			}
		}
	}
}

// 1.13c D2Common+0x66350 (#10821): same walk of the room's unit list; retail's do-while reads the head's link without a
// null check, so an empty room faults there, this answers FALSE
//D2Common.0x6FDBD4C0 (#10389)
BOOL __stdcall UNITROOM_IsUnitInRoom(D2ActiveRoomStrc* pRoom, D2UnitStrc* pUnit)
{
	D2UnitStrc* pRoomUnit = NULL;

	D2_ASSERT(pUnit);

	if (!pRoom)
	{
		return FALSE;
	}

	pRoomUnit = *DUNGEON_GetUnitListFromRoom(pRoom);
	if (pRoomUnit != pUnit)
	{
		while (pRoomUnit)
		{
			pRoomUnit = pRoomUnit->pRoomNext;

			if (pRoomUnit == pUnit)
			{
				return TRUE;
			}
		}

		return FALSE;
	}

	return TRUE;
}
