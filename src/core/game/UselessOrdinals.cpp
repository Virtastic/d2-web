#include "UselessOrdinals.h"
#include <test_seam.h>

#include "D2DataTbls.h"
#include "D2States.h"
#include "D2StatList.h"
#include "Units/Units.h"
#include <D2BitManip.h>
#include <D2Monsters.h>


// not-1.13c: 1.13c exports one expire-frame getter, D2Common+0x383A0 (STATLIST_GetExpireFrame); retail callers such as
// D2Game+0x99B60 reach it through the D2Game import thunk at +0xAB74. This 1.10f duplicate ordinal only forwards to it.
int __stdcall D2COMMON_10473(D2StatListStrc* pStatList)
{
	return STATLIST_GetExpireFrame(pStatList);
}

// not-1.13c: 1.13c exports one expire-frame setter, D2Common+0x38380 (STATLIST_SetExpireFrame); retail callers such as
// D2Game+0x99B60 reach it through the D2Game import thunk at +0xA7C6. This 1.10f duplicate ordinal only forwards to it.
void __stdcall D2COMMON_10476(D2StatListStrc* pStatList, int nExpireFrame)
{
	return STATLIST_SetExpireFrame(pStatList, nExpireFrame);
}

// A dead monster, in no state of the undead mask, whose MonStats2 row is both corpse-selectable and soft.
// Retail inlines the whole test; the same body is exported again at D2Common+0x50020.
// 1.13c D2Common+0x500E0
BOOL __stdcall D2COMMON_11018(D2UnitStrc* pUnit)
{
	if (D2Seam<&D2COMMON_11018>::hook) return D2Seam<&D2COMMON_11018>::hook(pUnit);
	if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && pUnit->dwAnimMode == MONMODE_DEAD && !STATES_CheckStateMaskOnUnit(pUnit, STATEMASK_UDEAD))
	{
		D2MonStats2Txt* pMonStats2TxtRecord = UNITS_GetMonStats2TxtRecordFromMonsterId(pUnit->dwClassId);
		if (pMonStats2TxtRecord && (pMonStats2TxtRecord->dwFlags & gdwBitMasks[MONSTATS2FLAGINDEX_CORPSESEL])
			&& (pMonStats2TxtRecord->dwFlags & gdwBitMasks[MONSTATS2FLAGINDEX_SOFT]))
		{
			return TRUE;
		}
	}

	return FALSE;
}

// A dead monster, in no state of the undead mask, whose MonStats2 row is corpse-selectable (retail inlines the test;
// identical bodies are exported at D2Common+0x4FF80 and +0x4FFD0).
// 1.13c D2Common+0x50090
BOOL __stdcall D2COMMON_11019(D2UnitStrc* pUnit)
{
	if (D2Seam<&D2COMMON_11019>::hook) return D2Seam<&D2COMMON_11019>::hook(pUnit);
	if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && pUnit->dwAnimMode == MONMODE_DEAD && !STATES_CheckStateMaskOnUnit(pUnit, STATEMASK_UDEAD))
	{
		D2MonStats2Txt* pMonStats2TxtRecord = UNITS_GetMonStats2TxtRecordFromMonsterId(pUnit->dwClassId);
		if (pMonStats2TxtRecord && (pMonStats2TxtRecord->dwFlags & gdwBitMasks[MONSTATS2FLAGINDEX_CORPSESEL]))
		{
			return TRUE;
		}
	}

	return FALSE;
}

// Same test as D2COMMON_11019, at its own 1.13c export.
// 1.13c D2Common+0x4FFD0
BOOL __stdcall D2COMMON_11021(D2UnitStrc* pUnit)
{
	if (D2Seam<&D2COMMON_11021>::hook) return D2Seam<&D2COMMON_11021>::hook(pUnit);
	if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && pUnit->dwAnimMode == MONMODE_DEAD && !STATES_CheckStateMaskOnUnit(pUnit, STATEMASK_UDEAD))
	{
		D2MonStats2Txt* pMonStats2TxtRecord = UNITS_GetMonStats2TxtRecordFromMonsterId(pUnit->dwClassId);
		if (pMonStats2TxtRecord && (pMonStats2TxtRecord->dwFlags & gdwBitMasks[MONSTATS2FLAGINDEX_CORPSESEL]))
		{
			return TRUE;
		}
	}

	return FALSE;
}
