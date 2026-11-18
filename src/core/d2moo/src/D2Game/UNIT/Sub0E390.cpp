#include "oc20g17.h"

#include <cstdlib>
#include <test_seam.h>

// 1.13c D2Game+0x6FD2F880: dword count, three entries, then the fallback value (retail reads the fallback at +0x10).
uint32_t gdwE390Table[5] = {};

// 1.13c D2Game+0xE390 (EAX = unit)
// Picks a value from a small table by the act the unit stands in, two entries further when the unit's game runs at a
// non-normal difficulty; an index past the count returns the fallback. A unit without a room or a null unit is fatal.
uint32_t __fastcall sub_E390(D2UnitStrc* pUnit)
{
	D2ActiveRoomStrc* pRoom = D2_CALL(UNITS_GetRoom, pUnit);
	if (!pRoom)
	{
		FOG_DisplayAssert("pRoom", __FILE__, __LINE__);
		exit(-1);
	}

	uint8_t nIndex = D2_CALL(DRLG_GetActNoFromLevelId, D2_CALL(DUNGEON_GetLevelIdFromRoom, pRoom));
	if (!pUnit)
	{
		FOG_DisplayAssert("pUnit", __FILE__, __LINE__);
		exit(-1);
	}

	if (pUnit->pGame && pUnit->pGame->nDifficulty)
	{
		nIndex = (uint8_t)(nIndex + 2);
	}
	if (gdwE390Table[0] <= nIndex)
	{
		return gdwE390Table[4];
	}
	return gdwE390Table[1 + nIndex];
}
