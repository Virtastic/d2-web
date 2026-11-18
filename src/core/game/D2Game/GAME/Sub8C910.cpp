#include <D2Game/oc20g11.h>

#include <Fog.h>
#include <Units/Units.h>
#include <Units/Player.h>
#include <Units/Monster.h>
#include "../../../compat/test_seam.h"

#include "AI/AiGeneral.h"
#include "UNIT/SUnit.h"

// 1.13c D2Game+0x8A330 (eax = node, edx = value)
// The shared tail of the three entry points below: a node {+4 value, +0xC counter}. An unchanged value answers 0. A value that moved
// by more than 8, or the 7th small move in a row, resets the counter, stores the value and answers 1.
int32_t __fastcall sub_8A330(D2UnkMonsterDataStrc* pNode, int32_t nValue)
{
	int32_t nDiff = nValue - pNode->nLifePercentage;
	if (nDiff != 0)
	{
		if (nDiff < 0)
		{
			nDiff = -nDiff;
		}

		if (nDiff > 8 || ++pNode->nCount > 6)
		{
			pNode->nCount = 0;
			pNode->nLifePercentage = nValue;
			return 1;
		}
	}
	return 0;
}

// 1.13c D2Game+0x8C910 (eax = unit; stack = key unit, value, tag)
// Per-owner list on the player data (+0x98) of 20-byte nodes {key, value, tag, counter, next}, keyed by the key unit's id (-1 for none).
// Both units must belong to the same party. A missing node is allocated at the head. A node whose tag differs takes (value, tag) and
// answers 1; otherwise the shared tail 0x8A330 decides.
int32_t __fastcall sub_8C910(D2UnitStrc* pUnit, D2UnitStrc* pKeyUnit, int32_t nValue, int32_t nTag)
{
	D2_ASSERT(pUnit);   // fatal 0xA2A

	if (!pUnit->pGame)
	{
		return 0;
	}

	const int16_t nPartyId = D2_CALL(PARTY_GetPartyIdForUnitOwner, pUnit->pGame, pUnit);
	if (nPartyId == -1)
	{
		return 0;
	}

	if (nPartyId != D2_CALL(PARTY_GetPartyIdForUnitOwner, pUnit->pGame, pKeyUnit))
	{
		return 0;
	}

	D2PlayerDataStrc* pPlayerData = D2_CALL(UNITS_GetPlayerData, pUnit);
	if (!pPlayerData)
	{
		return 0;
	}

	const int32_t nKey = pKeyUnit ? (int32_t)pKeyUnit->dwUnitId : -1;
	D2UnkPlayerDataStrc* pNode = pPlayerData->unk0x98;
	while (pNode && pNode->nUnitGUID != nKey)
	{
		pNode = pNode->pNext;
	}

	if (!pNode)
	{
		pNode = (D2UnkPlayerDataStrc*)D2_ALLOC(sizeof(D2UnkPlayerDataStrc));   // SCmd.cpp line 0x6FA
		if (!pNode)
		{
			return 0;
		}

		pNode->nUnitGUID = 0;
		pNode->unk0x04 = 0;
		pNode->unk0x08 = 0;
		pNode->unk0x0C = 0;
		pNode->pNext = pPlayerData->unk0x98;
		pPlayerData->unk0x98 = pNode;
		pNode->nUnitGUID = nKey;
	}

	if (pNode->unk0x08 != nTag)
	{
		pNode->unk0x04 = nValue;
		pNode->unk0x08 = nTag;
		return 1;
	}

	return D2_CALL(sub_8A330, reinterpret_cast<D2UnkMonsterDataStrc*>(pNode), nValue);
}

// 1.13c D2Game+0x8D500 (eax = unit, edi = value; stack = owner)
// A monster whose AI control names an owner (type and GUID at +0x30/+0x2C of the control, game at +0x28): when the owner's unit is the
// given owner and the monster's tracked value (+0x3C data, value at +4) differs from `nValue`, the shared tail 0x8A330 decides.
int32_t __fastcall sub_8D500(D2UnitStrc* pUnit, D2UnitStrc* pOwner, int32_t nValue)
{
	D2MonsterDataStrc* pMonsterData = nullptr;
	D2UnitStrc* pFound = nullptr;

	if (pUnit && pUnit->dwUnitType == UNIT_MONSTER)
	{
		pMonsterData = pUnit->pMonsterData;
		if (pMonsterData && pMonsterData->pAiControl && pMonsterData->pAiControl->pGame)
		{
			D2AiControlStrc* pAiControl = pMonsterData->pAiControl;
			pFound = D2_CALL(SUNIT_GetServerUnit, pAiControl->pGame, (int32_t)pAiControl->dwOwnerTypeEx, (int32_t)pAiControl->dwOwnerGUIDEx);
		}
	}

	if (pFound != pOwner || !pMonsterData || pMonsterData->unk0x3C.nLifePercentage == nValue)
	{
		return 0;
	}

	return D2_CALL(sub_8A330, &pMonsterData->unk0x3C, nValue);
}

// 1.13c D2Game+0x8E110 (ecx = object, eax = unit, edx = value; stack = tag; edi = the caller's value for 0x8D500)
// Dispatches on the unit's type: a player goes to 0x8C910 (the object leading), a monster to 0x8D500; anything else answers 0.
int32_t __fastcall sub_8E110(D2UnitStrc* pObject, D2UnitStrc* pUnit, int32_t nValue, int32_t nTag, int32_t nCallerValue)
{
	D2_ASSERT(pObject);   // fatal 0x712
	D2_ASSERT(pUnit);     // fatal 0x713

	if (pUnit->dwUnitType == UNIT_PLAYER)
	{
		return D2_CALL(sub_8C910, pObject, pUnit, nValue, nTag);
	}

	if (pUnit->dwUnitType == UNIT_MONSTER)
	{
		return D2_CALL(sub_8D500, pUnit, pObject, nCallerValue);
	}

	return 0;
}
