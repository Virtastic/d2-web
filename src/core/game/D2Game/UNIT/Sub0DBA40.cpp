#include "oc20g17.h"

#include <test_seam.h>
#include <D2DataTbls.h>
#include <D2Monsters.h>
#include <D2Skills.h>
#include <Units/Monster.h>
#include "MONSTER/MonsterUnique.h"
#include "ITEMS/Items.h"

// 1.13c D2Game+0xDBA40 (EAX = damage record, ECX = defender, EDX = game, stack = attacker, the unit that leeches; ret 4)
// Applies the life and mana leech parts of a damage record to the attacker: a defender that is a monster scales the
// leech by its drain percentage (monstats Drain per difficulty), the attacker's own kind picks the difficulty divisors
// (a player) or leaves them (a hireling), and a non-player/non-hireling attacker takes the pool-drain route (defender mana
// and stamina converted to attacker life). EDI of the callee calls is the attacker.
void __fastcall sub_DBA40(D2DamageStrc* pDamage, D2UnitStrc* pDefender, D2GameStrc* pGame, D2UnitStrc* pAttacker)
{
	if (pDamage->dwLifeLeech == 0 && pDamage->dwManaLeech == 0)
	{
		return;
	}

	int32_t nPercent = 100;
	if (pDefender && pDefender->dwUnitType == UNIT_MONSTER)
	{
		D2MonsterDataStrc* pMonsterData = pDefender->pMonsterData;
		if (!pMonsterData || !pMonsterData->pMonstatsTxt)
		{
			return;
		}

		nPercent = pMonsterData->pMonstatsTxt->nDrain[pGame->nDifficulty];
		if (nPercent <= 0)
		{
			return;
		}
	}

	pDamage->dwLifeLeech = (int32_t)((uint32_t)pDamage->dwLifeLeech << 6);
	pDamage->dwManaLeech = (int32_t)((uint32_t)pDamage->dwManaLeech << 6);

	int32_t nUnitType = 0;
	int32_t nClassId = 0;
	int32_t nAnimMode = 0;
	bool bSkipPools = false;
	if (!pAttacker)
	{
		nUnitType = 6;
	}
	else if (pAttacker->dwUnitType == UNIT_PLAYER)
	{
		D2DifficultyLevelsTxt* pDifficultyLevelsTxtRecord = D2_CALL(DATATBLS_GetDifficultyLevelsTxtRecord, pAttacker->pGame->nDifficulty);
		if (pDifficultyLevelsTxtRecord->dwLifeStealDiv)
		{
			pDamage->dwLifeLeech /= (int32_t)pDifficultyLevelsTxtRecord->dwLifeStealDiv;
		}
		if (pDifficultyLevelsTxtRecord->dwManaStealDiv)
		{
			pDamage->dwManaLeech /= (int32_t)pDifficultyLevelsTxtRecord->dwManaStealDiv;
		}
		bSkipPools = true;
	}
	else if (pAttacker->dwUnitType == UNIT_MONSTER && D2_CALL(MONSTERS_GetHirelingTypeId, pAttacker))
	{
		bSkipPools = true;
	}
	else
	{
		nUnitType = pAttacker->dwUnitType;
		nClassId = pAttacker->dwClassId;
		nAnimMode = pAttacker->dwAnimMode;
	}

	if (!bSkipPools)
	{
		if (!pAttacker)
		{
			nClassId = -1;
			nAnimMode = 0;
		}

		D2_CALL(D2COMMON_11013_ConvertMode, pAttacker, &nUnitType, &nClassId, &nAnimMode, "..\\Source\\D2Game\\UNIT\\SUnitDmg.cpp", 0x648);
		if (nUnitType != 0)
		{
			const int32_t nMana = D2_CALL(STATLIST_UnitGetStatValue, pDefender, STAT_MANA, 0);
			const int32_t nStamina = D2_CALL(STATLIST_UnitGetStatValue, pDefender, STAT_STAMINA, 0);
			int32_t nLife = (pDamage->dwLifeLeech + ((pDamage->dwLifeLeech >> 31) & 0x3F)) >> 6;
			int32_t nManaPart = (pDamage->dwManaLeech + ((pDamage->dwManaLeech >> 31) & 0x3F)) >> 6;
			pDamage->dwLifeLeech = nLife;
			pDamage->dwManaLeech = nManaPart;
			if (nLife >= pDamage->dwPhysDamage)
			{
				nLife = pDamage->dwPhysDamage;
			}
			if (nManaPart >= nMana)
			{
				nManaPart = nMana;
			}
			int32_t nStam = nStamina;
			if (pDamage->dwStamLeech < nStam)
			{
				nStam = pDamage->dwStamLeech;
			}

			int32_t nTotal = nLife + nManaPart + nStam;
			if (nTotal == 0)
			{
				return;
			}
			if (nPercent != 100)
			{
				nTotal = D2_CALL(MONSTERUNIQUE_CalculatePercentage, nPercent, nTotal, 100);
			}

			const int32_t nMaxLife = D2_CALL(STATLIST_GetMaxLifeFromUnit, pAttacker);
			const int32_t nCurLife = D2_CALL(STATLIST_UnitGetStatValue, pAttacker, STAT_HITPOINTS, 0);
			if (nTotal >= nMaxLife - nCurLife)
			{
				nTotal = nMaxLife - nCurLife;
			}
			if (nTotal <= 0)
			{
				return;
			}

			D2_CALL(STATLIST_AddUnitStat, pAttacker, STAT_HITPOINTS, nTotal, 0);
			D2_CALL(UNITS_SetOverlay, pAttacker, 0x97, 0);
			return;
		}
	}

	const int32_t nPhys = pDamage->dwPhysDamage;
	if (nPhys <= 0)
	{
		return;
	}

	bool bManaDone = false;
	if (pDamage->dwManaLeech != 0)
	{
		int32_t nLeeched = D2_CALL(MONSTERUNIQUE_CalculatePercentage, pDamage->dwManaLeech, nPhys, 100);
		if (nPercent != 100)
		{
			nLeeched = D2_CALL(MONSTERUNIQUE_CalculatePercentage, nPercent, nLeeched, 100);
		}
		D2_CALL(SUNITDMG_AddLeechedMana, pAttacker, (nLeeched + ((nLeeched >> 31) & 0x3F)) >> 6);
		bManaDone = true;
	}

	if (pDamage->dwLifeLeech == 0)
	{
		if (bManaDone)
		{
			D2_CALL(UNITS_SetOverlay, pAttacker, 0x98, 0);
		}
		return;
	}

	int32_t nLeeched = D2_CALL(MONSTERUNIQUE_CalculatePercentage, pDamage->dwLifeLeech, nPhys, 100);
	if (nPercent != 100)
	{
		nLeeched = D2_CALL(MONSTERUNIQUE_CalculatePercentage, nPercent, nLeeched, 100);
	}
	D2_CALL(SUNITDMG_AddLeechedLife, pAttacker, (nLeeched + ((nLeeched >> 31) & 0x3F)) >> 6);

	if (!bManaDone || D2_CALL(ITEMS_RollLimitedRandomNumber, &pAttacker->pSeed, 2) == 0)
	{
		D2_CALL(UNITS_SetOverlay, pAttacker, 0x97, 0);
		return;
	}
	D2_CALL(UNITS_SetOverlay, pAttacker, 0x98, 0);
}
