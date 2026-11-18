#include "oc20g17.h"

#include <test_seam.h>

// 1.13c D2Game+0xDAB00 (EAX = unit whose room is searched, EBX = reference unit, stack = object class; ret 4)
// Finds the object of the given class standing on the reference unit's tile in the room's unit list, puts it in
// mode 2 and frees its collision path.
void __fastcall sub_DAB00(D2UnitStrc* pRoomUnit, D2UnitStrc* pRefUnit, int32_t nClassId)
{
	D2ActiveRoomStrc* pRoom = D2_CALL(UNITS_GetRoom, pRoomUnit);
	if (!pRoom)
	{
		return;
	}

	D2UnitStrc* pUnit = pRoom->pUnitFirst;
	while (pUnit)
	{
		D2UnitStrc* pNext = pUnit->pRoomNext;
		if (pUnit->dwUnitType == UNIT_OBJECT && pUnit->dwClassId == (uint32_t)nClassId)
		{
			D2CoordStrc tUnitCoords = {};
			D2CoordStrc tRefCoords = {};
			D2_CALL(UNITS_GetCoords, pUnit, &tUnitCoords);
			D2_CALL(UNITS_GetCoords, pRefUnit, &tRefCoords);
			if (tUnitCoords.nX == tRefCoords.nX && tUnitCoords.nY == tRefCoords.nY)
			{
				D2_CALL(UNITS_ChangeAnimMode, pUnit, 2);
				D2_CALL(UNITS_FreeCollisionPath, pUnit);
				return;
			}
		}
		pUnit = pNext;
	}
}
