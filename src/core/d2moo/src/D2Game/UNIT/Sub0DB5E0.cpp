#include "oc20g17.h"

#include <test_seam.h>
#include <D2States.h>
#include <D2StatList.h>
#include <Units/Monster.h>
#include "AI/AiGeneral.h"
#include "UNIT/SUnit.h"

// 1.13c D2Game+0xDB5E0 (EBX = unit, stack = game, target unit; ret 8)
// Resolves the player a target stands for: a player stands for itself; a monster stands for its minion owner,
// or, when it carries a state with an owner statlist (flag 0x800), for that statlist's owner. Returns the found
// unit only when it is a player.
D2UnitStrc* __fastcall sub_DB5E0(D2UnitStrc* pUnit, D2GameStrc* pGame, D2UnitStrc* pTarget)
{
	const int32_t nType = pUnit ? (int32_t)pUnit->dwUnitType : 6;
	if (nType == UNIT_PLAYER)
	{
		return pUnit;
	}
	if (nType != UNIT_MONSTER)
	{
		return nullptr;
	}

	D2UnitStrc* pOwner = nullptr;
	if (pUnit && pUnit->dwUnitType == UNIT_MONSTER)
	{
		D2AiControlStrc* pAiControl = pUnit->pMonsterData ? pUnit->pMonsterData->pAiControl : nullptr;
		if (pAiControl && pAiControl->pGame)
		{
			pOwner = D2_CALL(SUNIT_GetServerUnit, pAiControl->pGame, pAiControl->dwOwnerTypeEx, pAiControl->dwOwnerGUIDEx);
		}
	}

	D2StatListStrc* pStatList = nullptr;
	if (D2_CALL(STATES_CheckStateMaskExpOnUnit, pUnit))
	{
		pStatList = D2_CALL(STATLIST_GetStatListFromUnitAndFlag, pUnit, 0x800);
	}
	if (pTarget && D2_CALL(STATES_CheckStateMaskExpOnUnit, pTarget))
	{
		pStatList = D2_CALL(STATLIST_GetStatListFromUnitAndFlag, pTarget, 0x800);
	}
	if (pStatList)
	{
		pOwner = D2_CALL(SUNIT_GetServerUnit, pGame, D2_CALL(STATLIST_GetOwnerType, pStatList), D2_CALL(STATLIST_GetOwnerGUID, pStatList));
	}

	if (pOwner && pOwner->dwUnitType == UNIT_PLAYER)
	{
		return pOwner;
	}
	return nullptr;
}
