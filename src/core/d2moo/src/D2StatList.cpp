#include "D2StatList.h"

#include <D2BitManip.h>
#include "D2DataTbls.h"
#include "D2Environment.h"
#include "D2ItemMods.h"
#include "D2Items.h"
#include "D2States.h"
#include "Units/Units.h"
//TODO: Find names


//D2Common.0x6FDB57C0 (#10563)
// 1.13c D2Common+0x84D30
BOOL __stdcall STATLIST_AreUnitsAligned(D2UnitStrc* pUnit1, D2UnitStrc* pUnit2)
{
	if (pUnit1 && pUnit2)
	{
		if (pUnit1 == pUnit2)
		{
			return TRUE;
		}

		int nAlignment1 = STATLIST_GetUnitAlignment(pUnit1);
		int nAlignment2 = STATLIST_GetUnitAlignment(pUnit2);

		if (nAlignment1 == UNIT_ALIGNMENT_EVIL && nAlignment2 == UNIT_ALIGNMENT_EVIL
			|| nAlignment1 == UNIT_ALIGNMENT_GOOD && nAlignment2 == UNIT_ALIGNMENT_GOOD)
		{
			return TRUE;
		}
	}

	return FALSE;
}


// Helper function
template<class T>
// 1.13c D2Common+0x38270
static int __fastcall StatArray_FindInsertionIndex(const T* pStatsArray, D2SLayerStatIdStrc::PackedType nLayer_StatId, bool* pAlreadyInArray)
{
	if (pAlreadyInArray)
	{
		*pAlreadyInArray = false;
	}
	// Find by dichotomic search, stats are sorted by nPackedValue
	int nMin = 0;
	int nMax = pStatsArray->nStatCount;

	while (nMin < nMax)
	{
		const int nMidPoint = nMin + (nMax - nMin) / 2;

		if (nLayer_StatId < pStatsArray->pStat[nMidPoint].nPackedValue)
		{
			nMax = nMin + (nMax - nMin) / 2;
		}
		else if (nLayer_StatId > pStatsArray->pStat[nMidPoint].nPackedValue)
		{
			nMin = nMidPoint + 1;
		}
		else // found it
		{
			if (pAlreadyInArray)
			{
				*pAlreadyInArray = true;
			}
			return nMidPoint;
		}
	}

	return nMin;
}

template<class T>
int StatArray_DichotomicSearch(const T* pStatArray, D2SLayerStatIdStrc::PackedType nLayer_StatId)
{
	bool alreadyInArray = false;
	int insertionIdx = StatArray_FindInsertionIndex(pStatArray, nLayer_StatId, &alreadyInArray);
	if (alreadyInArray)
		return insertionIdx;
	return -1;
}

// Helper function
template<class T>
// D2Common 1.13c +0x38640 (StatArray_InsertStat)
decltype(T::pStat) __fastcall StatArray_InsertStat(void* pMemPool, T* pStatsArray, D2SLayerStatIdStrc::PackedType nLayer_StatId, int insertionIdx)
{
	typedef decltype(T::pStat) StatType;

	if (pStatsArray->nStatCount >= pStatsArray->nCapacity)
	{
		pStatsArray->nCapacity += T::nGrowthAmount;
		pStatsArray->pStat = (StatType)D2_REALLOC_POOL(pMemPool, pStatsArray->pStat, pStatsArray->nCapacity * sizeof(*pStatsArray->pStat));
	}

	if (insertionIdx < pStatsArray->nStatCount)
	{
		// Move memory by 1 slot so that we can insert our stat. The element size is the array's
		// own (D2ModStatsArrayStrc holds 4-byte layer/stat ids, not 8-byte D2StatStrc): D2MOO's
		// sizeof(D2StatStrc) here moved twice the bytes and wrote past the end of the mod-stat
		// array whenever a saved stat was inserted before existing ones (heap corruption seen as
		// a realloc abort in PLAYERSTATS_LevelUp).
		memmove(&pStatsArray->pStat[insertionIdx + 1], &pStatsArray->pStat[insertionIdx], (pStatsArray->nStatCount - insertionIdx) * sizeof(*pStatsArray->pStat));
	}

	pStatsArray->pStat[insertionIdx] = {}; // Default init to 0
	pStatsArray->pStat[insertionIdx].nPackedValue = nLayer_StatId;
	++pStatsArray->nStatCount;

	return &pStatsArray->pStat[insertionIdx];
}

// Helper function
static D2StatStrc* __fastcall STATLIST_GetOrInsertStat(void* pMemPool, D2StatsArrayStrc* pStatsArray, D2SLayerStatIdStrc::PackedType nLayer_StatId)
{
	bool bFoundStatInArray = false;
	const int insertionIdx = StatArray_FindInsertionIndex(pStatsArray, nLayer_StatId, &bFoundStatInArray);

	if (bFoundStatInArray)
	{
		return &pStatsArray->pStat[insertionIdx];
	}
	else
	{
		return StatArray_InsertStat(pMemPool, pStatsArray, nLayer_StatId, insertionIdx);
	}
}

// Helper function
static void STATLIST_NotifyUnitOfStatValueChange(D2ItemStatCostTxt* pItemStatCostTxtRecord, D2StatListExStrc* pStatListEx, D2UnitStrc* pUnit, D2SLayerStatIdStrc::PackedType nLayer_StatId, int nPreviousValue, int nNewValue)
{
	if (pStatListEx->pfOnValueChanged && (pItemStatCostTxtRecord->dwItemStatFlags & gdwBitMasks[ITEMSTATCOSTFLAGINDEX_FCALLBACK]))
	{
		pStatListEx->pfOnValueChanged(pStatListEx->pGame, pStatListEx->pOwner, pUnit, nLayer_StatId, nPreviousValue, nNewValue);
	}
}

//Helper function
static void STATLIST_SetUnitStatNewValue(D2StatListExStrc* pStatListEx, D2StatsArrayStrc* pStatsArray, D2StatStrc* pStat, D2SLayerStatIdStrc::PackedType nLayer_StatId, int nNewValue, D2ItemStatCostTxt* pItemStatCostTxtRecord, D2UnitStrc* pUnit)
{
	int nPreviousValue = pStat->nValue;
	if (nNewValue != 0 || pItemStatCostTxtRecord->nKeepZero)
	{
		pStat->nValue = nNewValue;
		if (pItemStatCostTxtRecord->bHasOpApplyingToItem)
		{
			pStatListEx->dwFlags |= STATLIST_PERMANENT;
		}
	}
	else // We do not need to keep a value of 0, it's the default if not found
	{
		STATLIST_RemoveStat_6FDB6A30(pStatListEx->pMemPool, pStatsArray, pStat);
	}

	if (nPreviousValue != nNewValue)
	{
		STATLIST_NotifyUnitOfStatValueChange(pItemStatCostTxtRecord, pStatListEx, pUnit, nLayer_StatId, nPreviousValue, nNewValue);
	}
}

// Helper function
static int ComputeStatPercentage(int nValue, D2StatListExStrc* pStatListEx, uint16_t opStatId)
{
	int nOpStatValue = sub_6FDB5830(pStatListEx, D2SLayerStatIdStrc::MakeFromStatId(opStatId).nPackedValue);
	if (nOpStatValue)
	{
		return nValue * (nOpStatValue / 100.0);
	}
	return 0;
}

//D2Common.0x6FDB5830
int __fastcall sub_6FDB5830(D2StatListExStrc* pStatListEx, D2SLayerStatIdStrc::PackedType nLayer_StatId)
{
	int nAccumulatedValue = 0;

	const uint16_t nStatId = D2SLayerStatIdStrc::FromPackedType(nLayer_StatId).nStat;

	D2ItemStatCostTxt* pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(nStatId);
	if (!pItemStatCostTxtRecord)
	{
		return 0;
	}

	nAccumulatedValue = STATLIST_GetBaseStat_6FDB6340(pStatListEx, nLayer_StatId, 0);

	if (STATLIST_IsExtended(pStatListEx))
	{
		for (D2StatListStrc* i = pStatListEx->pMyLastList; i; i = i->pPrevLink)
		{
			if (!(pItemStatCostTxtRecord->dwItemStatFlags & gdwBitMasks[ITEMSTATCOSTFLAGINDEX_DAMAGERELATED]) || !(i->dwFlags & 0x40000000))
			{
				nAccumulatedValue += STATLIST_GetTotalStat_6FDB63E0((D2StatListExStrc*)i, nLayer_StatId, 0);
			}
		}
	}

	if (!pItemStatCostTxtRecord->bHasOpStatData)
	{
		return nAccumulatedValue;
	}

	int nPreviousOpStatValue = nAccumulatedValue;

	for (int i = 0; i < ARRAY_SIZE(pItemStatCostTxtRecord->pOpStatData); ++i)
	{
		D2OpStatDataStrc& opStatData = pItemStatCostTxtRecord->pOpStatData[i];
		switch (opStatData.nOp)
		{
		case 0:
			return nAccumulatedValue;

		case 1:

			if (pStatListEx->pOwner)
			{
				if (pStatListEx->pOwner->dwUnitType == UNIT_ITEM)
				{
					nPreviousOpStatValue = 0;
					if (pItemStatCostTxtRecord && pStatListEx->pOwner->pStatListEx)
					{
						nPreviousOpStatValue = STATLIST_GetBaseStat_6FDB6340(pStatListEx->pOwner->pStatListEx, D2SLayerStatIdStrc::MakeFromStatId(nStatId).nPackedValue, pItemStatCostTxtRecord);
					}
				}

				if (nPreviousOpStatValue)
				{
					nAccumulatedValue += ComputeStatPercentage(nPreviousOpStatValue, pStatListEx, opStatData.nStat);
				}
			}
			break;

		case 2:
			if (pStatListEx->dwOwnerType == UNIT_PLAYER || pStatListEx->dwOwnerType == UNIT_MONSTER)
			{
				uint16_t nOpBase = opStatData.nOpBase;
				if (nOpBase != 0xFFFF)
				{
					D2ItemStatCostTxt* pOpBaseItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(nOpBase);
					if (pOpBaseItemStatCostTxtRecord)
					{
						D2StatsArrayStrc* pStatsArray = STATLIST_IsExtended(pStatListEx) ? &pStatListEx->FullStats : &pStatListEx->Stats;
						
						if (D2StatStrc* pStat = STATLIST_FindStat_6FDB6920(pStatsArray, D2SLayerStatIdStrc::MakeFromStatId(nOpBase).nPackedValue))
						{
							int nShiftedVal = pStat->nValue >> pOpBaseItemStatCostTxtRecord->nValShift;
							if (nShiftedVal > 0)
							{
								int nRecursiveValue = sub_6FDB5830(pStatListEx, D2SLayerStatIdStrc::MakeFromStatId(opStatData.nStat).nPackedValue);
								if (nRecursiveValue)
								{
									nAccumulatedValue += (nShiftedVal * nRecursiveValue) >> opStatData.nOpParam;
								}
							}
						}
					}
				}
			}
			break;

		case 3:
			if (pStatListEx->dwOwnerType == UNIT_PLAYER || pStatListEx->dwOwnerType == UNIT_MONSTER)
			{
				uint16_t nOpBase = opStatData.nOpBase;
				if (nOpBase != 0xFFFF)
				{
					D2ItemStatCostTxt* pOpBaseItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(nOpBase);
					if (pOpBaseItemStatCostTxtRecord)
					{
						D2StatsArrayStrc* pStatsArray = STATLIST_IsExtended(pStatListEx) ? &pStatListEx->FullStats : &pStatListEx->Stats;

						if (D2StatStrc* pStat = STATLIST_FindStat_6FDB6920(pStatsArray, D2SLayerStatIdStrc::MakeFromStatId(nOpBase).nPackedValue))
						{
							int nShiftedVal = pStat->nValue >> pOpBaseItemStatCostTxtRecord->nValShift;
							if (nShiftedVal > 0)
							{
								int nRecursiveValue = sub_6FDB5830(pStatListEx, D2SLayerStatIdStrc::MakeFromStatId(opStatData.nStat).nPackedValue);
								if (nRecursiveValue)
								{
									int nTemp = (nShiftedVal * nRecursiveValue) >> opStatData.nOpParam;
									nAccumulatedValue += nTemp * nPreviousOpStatValue / 100.0;
								}
							}
						}
					}
				}
			}
			break;

		case 4:
			if (pStatListEx->pUnit && (pStatListEx->pUnit->dwUnitType == UNIT_PLAYER || pStatListEx->pUnit->dwUnitType == UNIT_MONSTER))
			{
				uint16_t nOpBase = opStatData.nOpBase;
				if (nOpBase != 0xFFFF)
				{
					D2ItemStatCostTxt* pOpBaseItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(nOpBase);
					if (pOpBaseItemStatCostTxtRecord && pStatListEx->pUnit->pStatListEx)
					{
						int nTmp = STATLIST_GetTotalStat_6FDB63E0(pStatListEx->pUnit->pStatListEx, D2SLayerStatIdStrc::MakeFromStatId(nOpBase).nPackedValue, pOpBaseItemStatCostTxtRecord) >> pOpBaseItemStatCostTxtRecord->nValShift;
						if (nTmp > 0)
						{
							int nRecursiveValue = sub_6FDB5830(pStatListEx, D2SLayerStatIdStrc::MakeFromStatId(opStatData.nStat).nPackedValue);
							if (nRecursiveValue)
							{
								nAccumulatedValue += (nTmp * nRecursiveValue) >> opStatData.nOpParam;
							}
						}
					}
				}
			}
			break;

		case 5:
			if (pStatListEx->pUnit && (pStatListEx->pUnit->dwUnitType == UNIT_PLAYER || pStatListEx->pUnit->dwUnitType == UNIT_MONSTER))
			{
				uint16_t nOpBase = opStatData.nOpBase;
				if (nOpBase != 0xFFFF)
				{
					D2ItemStatCostTxt* pOpBaseItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(nOpBase);
					if (pOpBaseItemStatCostTxtRecord && pStatListEx->pUnit->pStatListEx)
					{
						int nTmp = STATLIST_GetTotalStat_6FDB63E0(pStatListEx->pUnit->pStatListEx, D2SLayerStatIdStrc::MakeFromStatId(nOpBase).nPackedValue, pOpBaseItemStatCostTxtRecord) >> pOpBaseItemStatCostTxtRecord->nValShift;
						if (nTmp > 0)
						{
							int nRecursiveValue = sub_6FDB5830(pStatListEx, D2SLayerStatIdStrc::MakeFromStatId(opStatData.nStat).nPackedValue);
							if (nRecursiveValue)
							{
								int nTemp = (nTmp * nRecursiveValue) >> opStatData.nOpParam;
								nAccumulatedValue += nPreviousOpStatValue * nTemp / 100.0;
							}
						}
					}
				}
			}
			break;

		case 6:
			if (pStatListEx->pOwner && pStatListEx->pOwner->pDrlgAct)
			{
				if (int nTmp = sub_6FDB5830(pStatListEx, D2SLayerStatIdStrc::MakeFromStatId(opStatData.nStat).nPackedValue))
				{
					int nBaseTime = 0;
					int nPeriodOfDay = ENVIRONMENT_GetPeriodOfDayFromAct(pStatListEx->pOwner->pDrlgAct, &nBaseTime);
					nAccumulatedValue += ITEMMODS_GetByTimeAdjustment(nTmp, nPeriodOfDay, nBaseTime, NULL, NULL, NULL);
				}
			}
			break;

		case 7:
			if (pStatListEx->pOwner && pStatListEx->pOwner->pDrlgAct)
			{
				if (int nTmp = sub_6FDB5830(pStatListEx, D2SLayerStatIdStrc::MakeFromStatId(opStatData.nStat).nPackedValue))
				{
					int nBaseTime = 0;
					int nPeriodOfDay = ENVIRONMENT_GetPeriodOfDayFromAct(pStatListEx->pOwner->pDrlgAct, &nBaseTime);
					nAccumulatedValue += nAccumulatedValue * ITEMMODS_GetByTimeAdjustment(nTmp, nPeriodOfDay, nBaseTime, NULL, NULL, NULL) / 100.0;
				}
			}
			break;

		case 8:
			if (pStatListEx->pOwner && pStatListEx->pOwner->dwUnitType == UNIT_PLAYER)
			{
				if (D2CharStatsTxt* pCharStatsTxtRecord = UNITS_GetCharStatsTxtRecord(pStatListEx->pOwner->dwClassId))
				{
					int nTemp = 0;
					if (D2StatStrc* pStat = STATLIST_FindStat_6FDB6920(&pStatListEx->Stats, D2SLayerStatIdStrc::MakeFromStatId(opStatData.nStat).nPackedValue))
					{
						nTemp = pStat->nValue;
					}

					int nBaseValue = sub_6FDB5830(pStatListEx, D2SLayerStatIdStrc::MakeFromStatId(opStatData.nStat).nPackedValue) - nTemp;
					if (nBaseValue)
					{
						nAccumulatedValue += nBaseValue * pCharStatsTxtRecord->nManaPerMagic << 6;
					}
				}
			}
			break;

		case 9:
			if (pStatListEx->pOwner && pStatListEx->pOwner->dwUnitType == UNIT_PLAYER)
			{
				if (D2CharStatsTxt* pCharStatsTxtRecord = UNITS_GetCharStatsTxtRecord(pStatListEx->pOwner->dwClassId))
				{
					int nBaseStatValue = 0;
					if (D2StatStrc* pStat = STATLIST_FindStat_6FDB6920(&pStatListEx->Stats, D2SLayerStatIdStrc::MakeFromStatId(opStatData.nStat).nPackedValue))
					{
						nBaseStatValue = pStat->nValue;
					}
					
					int nBonusValue = sub_6FDB5830(pStatListEx, D2SLayerStatIdStrc::MakeFromStatId(opStatData.nStat).nPackedValue) - nBaseStatValue;
					if (nBonusValue)
					{
						if (nStatId == STAT_MAXSTAMINA)
						{
							nAccumulatedValue += nBonusValue * pCharStatsTxtRecord->nStaminaPerVitality << 6;
						}
						else
						{
							nAccumulatedValue += nBonusValue * pCharStatsTxtRecord->nLifePerVitality << 6;
						}
					}
				}
			}
			break;

		case 11:
			if (nPreviousOpStatValue)
			{
				if (pStatListEx->pOwner && (pStatListEx->pOwner->dwUnitType == UNIT_PLAYER || pStatListEx->pOwner->dwUnitType == UNIT_MONSTER))
				{
					nAccumulatedValue += ComputeStatPercentage(nPreviousOpStatValue, pStatListEx, opStatData.nStat);
				}
			}
			break;

		case 13:
			if (pStatListEx->pOwner && pStatListEx->pOwner->dwUnitType == UNIT_ITEM)
			{
				nPreviousOpStatValue = 0;
				if (pStatListEx->pOwner->pStatListEx)
				{
					nPreviousOpStatValue = STATLIST_GetBaseStat_6FDB6340(pStatListEx->pOwner->pStatListEx, D2SLayerStatIdStrc::MakeFromStatId(nStatId).nPackedValue, pItemStatCostTxtRecord);
					
					if (nPreviousOpStatValue)
					{
						nAccumulatedValue += ComputeStatPercentage(nPreviousOpStatValue, pStatListEx, opStatData.nStat);
					}
				}
			}
			break;

		default:
			break;
		}
	}

	return nAccumulatedValue;
}


//D2Common.0x6FDB6300
// 1.13c D2Common+0x382B0
int __fastcall STATLIST_FindStatIndex_6FDB6300(D2StatsArrayStrc* pStatArray, D2SLayerStatIdStrc::PackedType nLayer_StatId)
{
	return StatArray_DichotomicSearch(pStatArray, nLayer_StatId);
}

// Helper function
static int STATLIST_ApplyMinValue(int nValue, D2ItemStatCostTxt* pItemStatCostTxtRecord, D2StatListExStrc* pStatListEx)
{
	if (pItemStatCostTxtRecord && pItemStatCostTxtRecord->dwItemStatFlags & gdwBitMasks[ITEMSTATCOSTFLAGINDEX_FMIN])
	{
		if (pStatListEx && pStatListEx->pOwner && (pStatListEx->pOwner->dwUnitType == UNIT_PLAYER || pStatListEx->pOwner->dwUnitType == UNIT_MONSTER))
		{
			// Note: looks wrong but this is what the game acutally does.
			if (nValue < pItemStatCostTxtRecord->dwMinAccr)
			{
				nValue = pItemStatCostTxtRecord->dwMinAccr << pItemStatCostTxtRecord->nValShift;
			}
		}
	}
	return nValue;
}

//D2Common.0x6FDB6340
int __fastcall STATLIST_GetBaseStat_6FDB6340(D2StatListStrc* pStatList, D2SLayerStatIdStrc::PackedType nLayer_StatId, D2ItemStatCostTxt* pItemStatCostTxtRecord)
{
	if (D2StatStrc* pStat = STATLIST_FindStat_6FDB6920(&pStatList->Stats, nLayer_StatId))
	{
		return STATLIST_ApplyMinValue(pStat->nValue, pItemStatCostTxtRecord, STATLIST_StatListExCast(pStatList));
	}
	return 0;
}

//D2Common.0x6FDB63E0
int __fastcall STATLIST_GetTotalStat_6FDB63E0(D2StatListStrc* pStatList, D2SLayerStatIdStrc::PackedType nLayer_StatId, D2ItemStatCostTxt* pItemStatCostTxtRecord)
{
	D2_VERIFY(pStatList != nullptr);
	D2StatListExStrc* pStatListEx = STATLIST_StatListExCast(pStatList);
	D2StatsArrayStrc* pStatArray = pStatListEx ? &pStatListEx->FullStats : &pStatList->Stats;

	if (D2StatStrc* pStat = STATLIST_FindStat_6FDB6920(pStatArray, nLayer_StatId))
	{
		return STATLIST_ApplyMinValue(pStat->nValue, pItemStatCostTxtRecord, pStatListEx);
	}
	return 0;
}

//D2Common.0x6FDB64A0
int __fastcall sub_6FDB64A0(D2StatListExStrc* pStatListEx, D2SLayerStatIdStrc::PackedType nLayer_StatId, D2ItemStatCostTxt* pItemStatCostTxtRecord, D2UnitStrc* pUnit)
{
	int nNewValue = sub_6FDB5830(pStatListEx, nLayer_StatId);

	if (pItemStatCostTxtRecord->bIsBaseOfOtherStatOp)
	{
		if (nNewValue == 0)
		{
			if (D2StatStrc* pStat = STATLIST_FindStat_6FDB6920(&pStatListEx->FullStats, nLayer_StatId))
			{
				STATLIST_SetUnitStatNewValue(pStatListEx, &pStatListEx->FullStats, pStat, nLayer_StatId, nNewValue, pItemStatCostTxtRecord, pUnit);
			}
		}

		bool bUpdate = TRUE;
		
		for (int nCounter = 0; nCounter < 3; ++nCounter)
		{
			if (pItemStatCostTxtRecord->wOpStat[nCounter] == uint16_t(-1))
			{
				break;
			}

			const int nOpStat = pItemStatCostTxtRecord->wOpStat[nCounter];
			const D2OpStatDataStrc& rOpStatData = pItemStatCostTxtRecord->pOpStatData[nCounter];
			const D2SLayerStatIdStrc nOpStatLayer_StatId = D2SLayerStatIdStrc::MakeFromStatId(nOpStat);
			D2ItemStatCostTxt* pOpItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(nOpStat);
			D2StatsArrayStrc* pStatsArray = &pStatListEx->FullStats;
			int nOpStatNewValue = sub_6FDB64A0(pStatListEx, nOpStatLayer_StatId.nPackedValue, pOpItemStatCostTxtRecord, pUnit);
			
			D2StatStrc* pStat = STATLIST_FindStat_6FDB6920(pStatsArray, nOpStatLayer_StatId.nPackedValue);
			if (pStat == nullptr && nOpStatNewValue != 0)
			{
				pStat = STATLIST_InsertStatOrFail_6FDB6970(pStatListEx->pMemPool, pStatsArray, nOpStatLayer_StatId.nPackedValue);
			}

			if (pStat == nullptr)
				continue;

			STATLIST_SetUnitStatNewValue(pStatListEx, pStatsArray, pStat, nOpStatLayer_StatId.nPackedValue, nOpStatNewValue, pOpItemStatCostTxtRecord, pUnit);

			if (nOpStatNewValue)
			{
				switch (pItemStatCostTxtRecord->nOp)
				{
				case 2: // FALLTHROUGH
				case 3:
					if ((pStatListEx->dwOwnerType == UNIT_PLAYER || pStatListEx->dwOwnerType == UNIT_MONSTER) 
						&& rOpStatData.nOpBase != uint16_t(-1))
					{
						if (!STATLIST_IsExtended(pStatListEx))
						{
							pStatsArray = &pStatListEx->Stats;
						}

						const D2StatStrc* pOpBaseStat = STATLIST_FindStat_6FDB6920(pStatsArray, D2SLayerStatIdStrc::MakeFromStatId(rOpStatData.nOpBase).nPackedValue);
						if (pOpBaseStat && pOpBaseStat->nValue > 0)
						{
							bUpdate = FALSE;
						}
					}
					break;

				case STAT_OP_APPLY_TO_ITEM: // FALLTHROUGH
				case STAT_OP_APPLY_TO_ITEM_PCT:
					if (pStatListEx->pUnit && (pStatListEx->pUnit->dwUnitType == UNIT_PLAYER || pStatListEx->pUnit->dwUnitType == UNIT_MONSTER))
					{
						int nOpBase = rOpStatData.nOpBase;
						if (nOpBase != uint16_t(-1))
						{
							if (D2ItemStatCostTxt* pOpItemStatCost = ITEMS_GetItemStatCostTxtRecord(nOpBase))
							{
								int nOpStatValue = STATLIST_GetTotalStat_6FDB63E0(pStatListEx->pUnit->pStatListEx, D2SLayerStatIdStrc::MakeFromStatId(nOpBase).nPackedValue, pOpItemStatCost);
								bUpdate = nOpStatValue <= 0;
							}
						}
					}
					break;

				case STAT_OP_BY_TIME: // FALLTHROUGH
				case STAT_OP_BY_TIME_PCT:
					if (pStatListEx->dwOwnerType == UNIT_PLAYER || pStatListEx->dwOwnerType == UNIT_MONSTER)
					{
						bUpdate = FALSE;
					}
					break;

				case STAT_OP_ADD_ITEM_STAT_PCT:
					if (pStatListEx->dwOwnerType == UNIT_ITEM)
					{
						bUpdate = FALSE;
					}
					break;

				default:
					bUpdate = FALSE;
					break;

				case 0:
				case 8:
				case 9:
				case 10:
				case 11:
					break;
				}
			}
		}

		if (bUpdate && nNewValue)
		{
			STATLIST_UpdateUnitStat_6FDB6AB0(pStatListEx, nLayer_StatId, nNewValue, pItemStatCostTxtRecord, pUnit);

			for (int i = 0; i < ARRAY_SIZE(pItemStatCostTxtRecord->unk0x5E); ++i)
			{
				const int nStatId = pItemStatCostTxtRecord->unk0x5E[i];

				if (nStatId == uint16_t(-1))
				{
					break;
				}

				D2ItemStatCostTxt* pLayerStatCostRecord = ITEMS_GetItemStatCostTxtRecord(nStatId);
				const D2SLayerStatIdStrc layerStatId = D2SLayerStatIdStrc::MakeFromStatId(nStatId);
				const int nLayerStatNewValue = sub_6FDB64A0(pStatListEx, layerStatId.nPackedValue, pLayerStatCostRecord, pUnit);
				STATLIST_UpdateUnitStat_6FDB6AB0(pStatListEx, layerStatId.nPackedValue, nLayerStatNewValue, pLayerStatCostRecord, pUnit);
			}
		}
		return nNewValue;
	}

	D2StatStrc* pStat = STATLIST_FindStat_6FDB6920(&pStatListEx->FullStats, nLayer_StatId);
	if (pStat == nullptr)
	{
		if (nNewValue != 0)
		{
			pStat = STATLIST_InsertStatOrFail_6FDB6970(pStatListEx->pMemPool, &pStatListEx->FullStats, nLayer_StatId);
		}
		else
		{
			return 0;
		}
	}

	STATLIST_SetUnitStatNewValue(pStatListEx, &pStatListEx->FullStats, pStat, nLayer_StatId, nNewValue, pItemStatCostTxtRecord, pUnit);

	return nNewValue;
}

//D2Common.0x6FDB6920
D2StatStrc* __fastcall STATLIST_FindStat_6FDB6920(D2StatsArrayStrc* pStatArray, D2SLayerStatIdStrc::PackedType nLayer_StatId)
{
	const int nStatIndex = STATLIST_FindStatIndex_6FDB6300(pStatArray, nLayer_StatId);
	return (nStatIndex >= 0) ? &pStatArray->pStat[nStatIndex] : nullptr;
}

//D2Common.0x6FDB6970
D2StatStrc* __fastcall STATLIST_InsertStatOrFail_6FDB6970(void* pMemPool, D2StatsArrayStrc* pStatsArray, D2SLayerStatIdStrc::PackedType nLayer_StatId)
{
	bool bFoundStatInArray = false;
	int insertionIdx = StatArray_FindInsertionIndex(pStatsArray, nLayer_StatId, &bFoundStatInArray);
	if (bFoundStatInArray)
	{
		return nullptr;
	}
	return StatArray_InsertStat(pMemPool, pStatsArray, nLayer_StatId, insertionIdx);
}

//D2Common.0x6FDB6A30
// 1.13c D2Common+0x381A0
void __fastcall STATLIST_RemoveStat_6FDB6A30(void* pMemPool, D2StatsArrayStrc* pStatEx, D2StatStrc* pStat)
{
	int nIndex = pStat - pStatEx->pStat;

	if (nIndex < pStatEx->nStatCount)
	{
		if (nIndex < pStatEx->nStatCount - 1)
		{
			memmove(pStat, pStat + 1, (pStatEx->nStatCount - (nIndex + 1)) * sizeof(D2StatStrc));
		}

		--pStatEx->nStatCount;
		
		D2_ASSERT(pStatEx->nCapacity >= pStatEx->nStatCount);
		if ((pStatEx->nCapacity - pStatEx->nStatCount) > D2StatsArrayStrc::nShrinkThreshold)
		{
			pStatEx->nCapacity -= D2StatsArrayStrc::nGrowthAmount;
			pStatEx->pStat = (D2StatStrc*)D2_REALLOC_POOL(pMemPool, pStatEx->pStat, pStatEx->nCapacity * sizeof(D2StatStrc));
		}
	}
}

//D2Common.0x6FDB6AB0
void __fastcall STATLIST_UpdateUnitStat_6FDB6AB0(D2StatListExStrc* pStatListEx, D2SLayerStatIdStrc::PackedType nLayer_StatId, int nNewValue, D2ItemStatCostTxt* pItemStatCostTxtRecord, D2UnitStrc* pUnit)
{
	D2StatsArrayStrc* pStatsArray = &pStatListEx->FullStats;
	bool bFoundStatInArray = false;
	const int insertionIdx = StatArray_FindInsertionIndex(pStatsArray, nLayer_StatId, &bFoundStatInArray);

	D2StatStrc* pStat = nullptr;
	if (bFoundStatInArray)
	{
		pStat = &pStatsArray->pStat[insertionIdx];
	}
	else if (nNewValue != 0)
	{
		pStat = StatArray_InsertStat(pStatListEx->pMemPool, pStatsArray, nLayer_StatId, insertionIdx);
	}

	if (pStat == nullptr)
	{
		return;
	}
	STATLIST_SetUnitStatNewValue(pStatListEx, pStatsArray, pStat, nLayer_StatId, nNewValue, pItemStatCostTxtRecord, pUnit);
}

// Prior art used: D2MOO stat-array insertion/removal and callback helpers.
// Retail's additive FullStats path differs from the general setter: dirty only
// for positive results, and notify even when the additive delta is zero.
// 1.13c D2Common+0x389E0
int STATLIST_AddFullStatValue(D2StatListExStrc* list,
    D2SLayerStatIdStrc::PackedType id, int delta,
    D2ItemStatCostTxt* cost, D2UnitStrc* unit)
{
    D2StatStrc* stat = STATLIST_GetOrInsertStat(list->pMemPool, &list->FullStats, id);
    const int previous = stat->nValue;
    const int value = static_cast<int32_t>(static_cast<uint32_t>(previous) + static_cast<uint32_t>(delta));
    stat->nValue = value;
    if (value > 0 && cost->bHasOpApplyingToItem)
        list->dwFlags |= STATLIST_PERMANENT;
    if (value == 0 && !cost->nKeepZero)
        STATLIST_RemoveStat_6FDB6A30(list->pMemPool, &list->FullStats, stat);
    STATLIST_NotifyUnitOfStatValueChange(cost, list, unit, id, previous, value);
    return value;
}

//D2Common.0x6FDB6C10
// 1.13c D2Common+0x39F70
void __fastcall sub_6FDB6C10(D2StatListExStrc* pStatListEx, D2SLayerStatIdStrc::PackedType nLayer_StatId, int nValue, D2UnitStrc* pUnit)
{
	if (!nValue)
	{
		return;
	}

	const uint16_t nStatId = D2SLayerStatIdStrc::FromPackedType(nLayer_StatId).nStat;
	D2ItemStatCostTxt* pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(nStatId);
	if (!pItemStatCostTxtRecord || (pStatListEx->dwFlags & STATLIST_SET))
	{
		return;
	}
	const bool bStatIsDamageRelated = (pItemStatCostTxtRecord->dwItemStatFlags & gdwBitMasks[ITEMSTATCOSTFLAGINDEX_DAMAGERELATED]);

	D2StatListExStrc* pParentStatList = nullptr;
	if (!STATLIST_IsExtended(pStatListEx))
	{
		pParentStatList = static_cast<D2StatListExStrc*>(pStatListEx->pParent);
	}
	else
	{
		pParentStatList = pStatListEx;
	}

	while (pParentStatList)
	{
		if (pItemStatCostTxtRecord->bIsBaseOfOtherStatOp || pItemStatCostTxtRecord->bHasOpStatData)
		{
			sub_6FDB64A0(pParentStatList, nLayer_StatId, pItemStatCostTxtRecord, pUnit);
		}
		else
		{
			STATLIST_AddFullStatValue(pParentStatList, nLayer_StatId, nValue, pItemStatCostTxtRecord, pUnit);
		}
		if ((pParentStatList->dwFlags & STATLIST_SET) || (bStatIsDamageRelated && (pParentStatList->dwFlags & STATLIST_DYNAMIC)))
		{
			break;
		}

		pParentStatList = (D2StatListExStrc*)pParentStatList->pParent;
	}
}

//D2Common.0x6FDB6E30
// 1.13c D2Common+0x3A3B0
void __stdcall D2Common_ExpireStatList_6FDB6E30(D2StatListStrc* pStatList)
{
	if (!pStatList)
	{
		return;
	}
	D2StatListStrc* pParentStatList = pStatList->pParent;
	if (pParentStatList)
	{
		if (D2StatListExStrc* pParentStatListEx = STATLIST_StatListExCast(pParentStatList))
		{
			// Unlink the stat from the parent list
			if (pParentStatListEx->pMyLastList == pStatList)
			{
				pParentStatListEx->pMyLastList = pStatList->pPrevLink;
			}

			if (pParentStatListEx->pMyStats == pStatList)
			{
				pParentStatListEx->pMyStats = pStatList->pPrevLink;
			}
		}

		pStatList->pParent = NULL;
	}

	// Unlink the stat from the list
	if (pStatList->pNextLink)
	{
		pStatList->pNextLink->pPrevLink = pStatList->pPrevLink;
	}

	if (pStatList->pPrevLink)
	{
		pStatList->pPrevLink->pNextLink = pStatList->pNextLink;
	}

	pStatList->pPrevLink = NULL;
	pStatList->pNextLink = NULL;

	D2UnitStrc* pUnit = pStatList->pUnit;
	pStatList->pUnit = NULL;

	if (!(pStatList->dwFlags & STATLIST_SET))
	{
		D2StatListExStrc* pStatListEx = STATLIST_StatListExCast(pStatList);
		D2UnitStrc* pOwner = pStatListEx ? pStatListEx->pOwner : nullptr;
			
		if (pStatListEx && pStatList->dwFlags & STATLIST_PERMANENT)
		{
			D2SLayerStatIdStrc nLayer_StatIds[16] = {};
			int nCounter = 0;
			// Retail +0x3A44E sign-extends the stored count word.
			const int nFullStatCount = static_cast<int16_t>(pStatListEx->FullStats.nStatCount);
			for (int nStat = 0; nStat < nFullStatCount; ++nStat)
			{
				D2StatStrc* i = &pStatListEx->FullStats.pStat[nStat];
				D2ItemStatCostTxt* pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(i->nStat);
				if (pItemStatCostTxtRecord && pItemStatCostTxtRecord->bHasOpApplyingToItem)
				{
					nLayer_StatIds[nCounter] = *i;
					++nCounter;
				}
			}

			for (int i = 0; i < nCounter; ++i)
			{
				if (D2ItemStatCostTxt* pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(nLayer_StatIds[i].nStat))
				{
					sub_6FDB64A0(pStatListEx, nLayer_StatIds[i].nPackedValue, pItemStatCostTxtRecord, 0);
				}
			}
		}

		if (pParentStatList)
		{
			D2StatsArrayStrc* pStatsArray = STATLIST_IsExtended(pStatList) ? &pStatListEx->FullStats : &pStatList->Stats;

			// Retail passes the parent unchanged; +0x39F70 handles plain parents too.
			D2StatListExStrc* pParentStatListEx = reinterpret_cast<D2StatListExStrc*>(pParentStatList);
			// Retail +0x3A513 sign-extends both plain and extended count words.
			const int nStatCount = static_cast<int16_t>(pStatsArray->nStatCount);

			if (pStatList->dwFlags & STATLIST_DYNAMIC)
			{
				for (int nStat = 0; nStat < nStatCount; ++nStat)
				{
					D2StatStrc* i = &pStatsArray->pStat[nStat];
					D2ItemStatCostTxt* pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(i->nStat);
					if (pItemStatCostTxtRecord && !(pItemStatCostTxtRecord->dwItemStatFlags & gdwBitMasks[ITEMSTATCOSTFLAGINDEX_DAMAGERELATED]))
					{
						sub_6FDB6C10(pParentStatListEx, i->nPackedValue, -i->nValue, pOwner);
					}
				}
			}
			else
			{
				for (int nStat = 0; nStat < nStatCount; ++nStat)
				{
					D2StatStrc* i = &pStatsArray->pStat[nStat];
					sub_6FDB6C10(pParentStatListEx, i->nPackedValue, -i->nValue, pOwner);
				}
			}
		}

		if (pUnit && pStatList->fpStatRemove)
		{
			pStatList->fpStatRemove(pUnit, pStatList->dwStateNo, pStatList);
		}
	}
}

//D2Common.0x6FDB7030 (#10485)
// 1.13c D2Common+0x3AD40
void __stdcall STATLIST_FreeStatList(D2StatListStrc* pStatList)
{
	if (pStatList && !STATLIST_IsExtended(pStatList))
	{
		D2Common_STATLIST_FreeStatListImpl_6FDB7050(pStatList);
	}
}

//D2Common.0x6FDB7050
// 1.13c D2Common+0x3A9F0
void __fastcall D2Common_STATLIST_FreeStatListImpl_6FDB7050(D2StatListStrc* pStatList)
{

	D2Common_ExpireStatList_6FDB6E30(pStatList);

	if (pStatList->Stats.pStat)
	{
		D2_FREE_POOL(pStatList->pMemPool, pStatList->Stats.pStat);
	}

	if (D2StatListExStrc* pStatListEx = STATLIST_StatListExCast(pStatList))
	{
		D2StatListStrc* pPrevious;
		for (D2StatListStrc* pCurrent = pStatListEx->pMyLastList; pCurrent != nullptr; pCurrent = pPrevious)
		{
			pPrevious = pCurrent->pPrevLink;
			pCurrent->pParent = nullptr;
			pCurrent->pUnit = nullptr;
			if (STATLIST_IsExtended(pCurrent))
			{
				continue;
			}

			if (pStatListEx->pMyLastList == pCurrent)
			{
				pStatListEx->pMyLastList = pPrevious;
			}

			D2Common_STATLIST_FreeStatListImpl_6FDB7050(pCurrent);

			pPrevious = pStatListEx->pMyLastList;
		}

		if (pStatListEx->FullStats.pStat)
		{
			D2_FREE_POOL(pStatListEx->pMemPool, pStatListEx->FullStats.pStat);
		}

		if (pStatListEx->ModStats.pStat)
		{
			D2_FREE_POOL(pStatListEx->pMemPool, pStatListEx->ModStats.pStat);
		}

		D2_FREE_POOL(pStatListEx->pMemPool, pStatListEx->StatFlags);
	}

	D2_FREE_POOL(pStatList->pMemPool, pStatList);
}

//D2Common.0x6FDB7110 (#10527)
// 1.13c D2Common+0x3AED0
void __stdcall STATLIST_FreeStatListEx(D2UnitStrc* pUnit)
{
	if (pUnit->pStatListEx)
	{
		if (STATLIST_IsExtended(pUnit->pStatListEx))
		{
			D2Common_STATLIST_FreeStatListImpl_6FDB7050(pUnit->pStatListEx);
		}

		pUnit->pStatListEx = NULL;
	}
}

//D2Common.0x6FDB7140 (#10470)
// 1.13c D2Common+0x38100
D2StatListStrc* __stdcall STATLIST_AllocStatList(void* pMemPool, uint32_t fFilter, uint32_t dwTimeout, int nUnitType, D2UnitGUID nUnitGUID)
{
	D2StatListStrc* pStatList = D2_CALLOC_STRC_POOL(pMemPool, D2StatListStrc);

	pStatList->dwOwnerType = nUnitType;
	pStatList->dwOwnerId = nUnitGUID;
	pStatList->pMemPool = pMemPool;
	pStatList->dwFlags = fFilter;
	pStatList->dwExpireFrame = dwTimeout;

	return pStatList;
}

//D2Common.0x6FDB7190 (#10526)
// 1.13c D2Common+0x3AF00
void __stdcall STATLIST_AllocStatListEx(D2UnitStrc* pUnit, char nFlags, StatListValueChangeFunc pfOnValueChanged, D2GameStrc* pGame)
{
	STATLIST_FreeStatListEx(pUnit);

	D2StatListExStrc* pStatListEx = D2_CALLOC_STRC_POOL(pUnit->pMemoryPool, D2StatListExStrc);

	pStatListEx->pMemPool = pUnit->pMemoryPool;
	pStatListEx->dwOwnerType = pUnit->dwUnitType;
	pStatListEx->dwOwnerId = pUnit->dwUnitId;
	pStatListEx->pOwner = pUnit;
	pStatListEx->pfOnValueChanged = pfOnValueChanged;
	pStatListEx->pGame = pGame;
	pStatListEx->dwFlags = (nFlags & STATLIST_BASIC) | STATLIST_EXTENDED;
	pStatListEx->StatFlags = (uint32_t*)D2_CALLOC_POOL(pUnit->pMemoryPool, 2 * sizeof(uint32_t) * ((sgptDataTables->nStatesTxtRecordCount + 31) / 32) + 32); // +32: heap slack, not retail; the exact 1.13c size exposes a latent DRLG stale-link read (README)

	pUnit->pStatListEx = pStatListEx;
}

//D2Common.0x6FDB7260 (#10471)
int __stdcall STATLIST_GetOwnerType(D2StatListStrc* pStatList)
{
	if (pStatList)
	{
		return pStatList->dwOwnerType;
	}

	return 6;
}

//D2Common.0x6FD912D0 (#10472)
D2UnitGUID __stdcall STATLIST_GetOwnerGUID(D2StatListStrc* pStatList)
{
	if (pStatList)
	{
		return pStatList->dwOwnerId;
	}

	REMOVE_LATER_Trace("STATLIST_GetOwnerGUID: NULL pointer");
	return 0;
}

//D2Common.0x6FDB7280 (#11304)
// 1.13c D2Common+0x380C0
int __stdcall STATLIST_GetBaseStatsCount(D2StatListStrc* pStatList)
{
	if (pStatList)
	{
		return pStatList->Stats.nStatCount;
	}

	return 0;
}

//D2Common.0x6FDB72A0 (#11305)
int __stdcall STATLIST_GetFullStatsCountFromUnit(D2UnitStrc* pUnit)
{
	if (pUnit && pUnit->pStatListEx)
	{
		return pUnit->pStatListEx->FullStats.nStatCount;
	}

	return 0;
}

//D2Common.0x6FDB72C0 (#10478)
// 1.13c D2Common+0x38060
void __stdcall STATLIST_SetState(D2StatListStrc* pStatList, int nState)
{
	if (pStatList)
	{
		pStatList->dwStateNo = nState;
	}
}

//D2Common.0x6FDB72E0 (#10479)
// 1.13c D2Common+0x38040
int __stdcall STATLIST_GetState(D2StatListStrc* pStatList)
{
	if (pStatList)
	{
		return pStatList->dwStateNo;
	}

	return 0;
}

//D2Common.0x6FDB7300 (#10528)
void __stdcall STATLIST_SetExpireFrame(D2StatListStrc* pStatList, int nExpireFrame)
{
	if (pStatList)
	{
		if (nExpireFrame > 0)
		{
			pStatList->dwFlags |= STATLIST_NEWLENGTH;
		}

		pStatList->dwExpireFrame = nExpireFrame;
	}
}

//D2Common.0x6FDB7320 (#10529)
int __stdcall STATLIST_GetExpireFrame(D2StatListStrc* pStatList)
{
	if (pStatList)
	{
		return pStatList->dwExpireFrame;
	}

	REMOVE_LATER_Trace("STATLIST_GetExpireFrame: NULL pointer");
	return 0;
}

//D2Common.0x6FDB7340 (#10475)
void __stdcall D2COMMON_10475_PostStatToStatList(D2UnitStrc* pUnit, D2StatListStrc* pStatList, BOOL bResetFlag)
{
	if (D2StatListExStrc* pUnitStatListEx = STATLIST_StatListExCast(pUnit->pStatListEx))
	{
		D2Common_ExpireStatList_6FDB6E30((D2StatListExStrc*)pStatList);
		D2StatListExStrc* pCurrentStatList = pUnitStatListEx;

		while (pCurrentStatList && (pStatList != pCurrentStatList))
		{
			pCurrentStatList = (D2StatListExStrc*)pCurrentStatList->pParent;
		}
		if (pCurrentStatList)
		{
			// pCurStatList already present, skip
			return;
		}


		D2StatListExStrc* pStatListEx = STATLIST_StatListExCast(pStatList);
		if (pStatList->dwFlags & STATLIST_TEMPONLY)
		{
			pUnit->pStatListEx->dwFlags |= STATLIST_NEWLENGTH;
		}

		if (pStatList->dwFlags & STATLIST_SET)
		{
			pStatList->pPrevLink = pUnit->pStatListEx->pMyStats;

			if (pUnit->pStatListEx->pMyStats)
			{
				pUnit->pStatListEx->pMyStats->pNextLink = pStatList;
			}

			pStatList->pNextLink = NULL;
			pUnit->pStatListEx->pMyStats = pStatList;
			pStatList->pParent = (D2StatListStrc*)pUnit->pStatListEx;
			pStatList->pUnit = pUnit;
		}
		else
		{
			pStatList->pPrevLink = pUnit->pStatListEx->pMyLastList;
			if (pUnit->pStatListEx->pMyLastList)
			{
				pUnit->pStatListEx->pMyLastList->pNextLink = pStatList;
			}

			pStatList->pParent = (D2StatListStrc*)pUnit->pStatListEx;
			pUnit->pStatListEx->pMyLastList = pStatList;
			pStatList->pUnit = pUnit;

			D2UnitStrc* pOwner = nullptr;

			if (pStatListEx)
			{
				pOwner = pStatListEx->pOwner;

				int nCounter = 0;
				D2SLayerStatIdStrc nLayer_StatIds[16] = {};
				for (D2StatStrc* pCurStat = pStatListEx->FullStats.pStat; pCurStat < &pStatListEx->FullStats.pStat[pStatListEx->FullStats.nStatCount]; ++pCurStat)
				{
					D2ItemStatCostTxt* pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(pCurStat->nStat);
					if (pItemStatCostTxtRecord && pItemStatCostTxtRecord->bHasOpApplyingToItem)
					{
						nLayer_StatIds[nCounter] = *pCurStat;
						++nCounter;
					}
				}

				for (int i = 0; i < nCounter; ++i)
				{
					D2ItemStatCostTxt* pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(nLayer_StatIds[i].nStat);
					if (pItemStatCostTxtRecord)
					{
						sub_6FDB64A0(pStatListEx, nLayer_StatIds[i].nPackedValue, pItemStatCostTxtRecord, 0);
					}
				}
			}

			D2StatsArrayStrc* pStatsArray = pStatListEx ? &pStatListEx->FullStats : &pStatList->Stats;
			if (bResetFlag)
			{
				pStatList->dwFlags &= ~STATLIST_DYNAMIC;

				for (D2StatStrc* i = pStatsArray->pStat; i < &pStatsArray->pStat[pStatsArray->nStatCount]; ++i)
				{
					sub_6FDB6C10(pUnitStatListEx, i->nPackedValue, i->nValue, pOwner);
				}
			}
			else
			{
				pStatList->dwFlags |= STATLIST_DYNAMIC;

				for (D2StatStrc* i = pStatsArray->pStat; i < &pStatsArray->pStat[pStatsArray->nStatCount]; ++i)
				{
					D2ItemStatCostTxt* pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(i->nStat);
					if (pItemStatCostTxtRecord && !(pItemStatCostTxtRecord->dwItemStatFlags & gdwBitMasks[ITEMSTATCOSTFLAGINDEX_DAMAGERELATED]))
					{
						sub_6FDB6C10(pUnitStatListEx, i->nPackedValue, i->nValue, pOwner);
					}
				}
			}
		}
	}
}

//D2Common.0x6FDB7560 (#10464)
void __stdcall STATLIST_AddStat(D2StatListStrc* pStatList, int nStatId, int nValue, uint16_t nLayer)
{
	if (!pStatList || !nValue)
	{
		return;
	}

	D2StatsArrayStrc* pStatsArray = &pStatList->Stats;

	D2StatStrc* pStat = STATLIST_FindStat_6FDB6920(pStatsArray, D2SLayerStatIdStrc::Make(nLayer, nStatId).nPackedValue);

	if (!pStat)
	{
		pStat = STATLIST_InsertStatOrFail_6FDB6970(pStatList->pMemPool, pStatsArray, D2SLayerStatIdStrc::Make(nLayer, nStatId).nPackedValue);
	}

	pStat->nValue += nValue;

	if (pStat->nValue == 0)
	{
		STATLIST_RemoveStat_6FDB6A30(pStatList->pMemPool, pStatsArray, pStat);
	}

	sub_6FDB6C10((D2StatListExStrc*)pStatList, D2SLayerStatIdStrc::Make(nLayer, nStatId).nPackedValue, nValue, 0);
	if (D2StatListExStrc* pStatListEx = STATLIST_StatListExCast(pStatList))
	{
		if (pStatList->dwOwnerType == UNIT_PLAYER)
		{
			STATLIST_InsertStatModOrFail_6FDB7690(pStatListEx, D2SLayerStatIdStrc::Make(nLayer,nStatId).nPackedValue);
		}
	}
}

//D2Common.0x6FDB7690
// 1.13c D2Common+0x38550
void __fastcall STATLIST_InsertStatModOrFail_6FDB7690(D2StatListStrc* pStatList, D2SLayerStatIdStrc::PackedType nLayer_StatId)
{
	if (D2StatListExStrc* pStatListEx = STATLIST_StatListExCast(pStatList))
	{
		uint16_t nStatId = D2SLayerStatIdStrc::FromPackedType(nLayer_StatId).nStat;

		switch (nStatId)
		{
		case STAT_HITPOINTS:
		case STAT_MANA:
		case STAT_STAMINA:
		case STAT_EXPERIENCE:
		case STAT_GOLD:
			return;
		default:
			break;
		}

		D2ItemStatCostTxt* pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(nStatId);
		if (pItemStatCostTxtRecord && pItemStatCostTxtRecord->dwItemStatFlags & gdwBitMasks[ITEMSTATCOSTFLAGINDEX_SAVED])
		{
			bool alreadyInArray = false;
			int insertIdx = StatArray_FindInsertionIndex(&pStatListEx->ModStats, nLayer_StatId, &alreadyInArray);
			if (!alreadyInArray)
				StatArray_InsertStat(pStatListEx->pMemPool, &pStatListEx->ModStats, nLayer_StatId, insertIdx);
		}
	}
}

//D2Common.0x6FDB77B0 (#10463)
// 1.13c D2Common+0x3A280
BOOL __stdcall STATLIST_SetStat(D2StatListStrc* pStatList, int nStatId, int nValue, uint16_t nLayer)
{
	return STATLIST_SetBaseStat(pStatList, nStatId, nValue, nLayer, NULL);
}

//D2Common.0x6FDB7910 (#10465)
// 1.13c D2Common+0x3A7B0
void __stdcall STATLIST_SetStatIfListIsValid(D2StatListStrc* pStatList, int nStatId, int nValue, uint16_t nLayer)
{
	if (pStatList)
	{
		STATLIST_SetStat(pStatList, nStatId, nValue, nLayer);
	}
}

//D2Common.0x6FDB7930 (#11294)
// 1.13c D2Common+0x3A1D0
BOOL __stdcall STATLIST_SetBaseStat(D2StatListStrc* pStatList, int nStatId, int nValue, uint16_t nLayer, D2UnitStrc* pUnit)
{
	if (!pStatList)
	{
		return FALSE;
	}

	const D2SLayerStatIdStrc nLayer_StatId = D2SLayerStatIdStrc::Make(nLayer, nStatId);
	D2StatsArrayStrc* pBaseStatsArray = &pStatList->Stats;
	bool bFoundStatInArray = false;
	const int insertionIdx = StatArray_FindInsertionIndex(pBaseStatsArray, nLayer_StatId.nPackedValue, &bFoundStatInArray);
	if (!bFoundStatInArray)
	{
		if (nValue == 0) // Early exit, we do not keep stats with a value of 0
		{
			return FALSE;
		}
		StatArray_InsertStat(pStatList->pMemPool, pBaseStatsArray, nLayer_StatId.nPackedValue, insertionIdx);
	}
	D2StatStrc* pStat = &pBaseStatsArray->pStat[insertionIdx];

	const int nDiffValue = nValue - pStat->nValue;
	if (nDiffValue == 0)
	{
		return FALSE;
	}

	if (nValue != 0)
	{
		pStat->nValue = nValue;
	}
	else
	{
		STATLIST_RemoveStat_6FDB6A30(pStatList->pMemPool, pBaseStatsArray, pStat);
	}

	sub_6FDB6C10((D2StatListExStrc*)pStatList, nLayer_StatId.nPackedValue, nDiffValue, pUnit);
	if (D2StatListExStrc* pStatListEx = STATLIST_StatListExCast(pStatList))
	{
		if (pStatList->dwOwnerType == UNIT_PLAYER)
		{
			STATLIST_InsertStatModOrFail_6FDB7690(pStatListEx, nLayer_StatId.nPackedValue);
		}
	}

	return TRUE;

}

//D2Common.0x6FDB7A90 (#11295)
void __stdcall STATLIST_SetBaseStat2(D2StatListStrc* pStatList, int nStatId, int nValue, uint16_t nLayer, D2UnitStrc* pUnit)
{
	if (pStatList)
	{
		STATLIST_SetBaseStat(pStatList, nStatId, nValue, nLayer, pUnit);
	}
}

// 1.13c D2Common+0x3A740
//D2Common.0x6FDB7AB0 (#10517)
// FN-capture traces of the 0x3A280 / 0x38550 calls (checked against retail); null in the game.
BOOL (*gpfnStatListSetUnitStatSetTrace)(D2StatListStrc*, int, int, uint16_t) = nullptr;
void (*gpfnStatListSetUnitStatModTrace)(D2StatListStrc*, D2SLayerStatIdStrc::PackedType) = nullptr;

void __stdcall STATLIST_SetUnitStat(D2UnitStrc* pUnit, int nStatId, int nValue, uint16_t nLayer)
{
	const BOOL bSet = gpfnStatListSetUnitStatSetTrace ? gpfnStatListSetUnitStatSetTrace((D2StatListStrc*)pUnit->pStatListEx, nStatId, nValue, nLayer) : STATLIST_SetStat(pUnit->pStatListEx, nStatId, nValue, nLayer);
	if (bSet)
	{
		if (pUnit->dwUnitType == UNIT_PLAYER)
		{
			if (gpfnStatListSetUnitStatModTrace)
				gpfnStatListSetUnitStatModTrace((D2StatListStrc*)pUnit->pStatListEx, D2SLayerStatIdStrc::Make(nLayer, nStatId).nPackedValue);
			else
				STATLIST_InsertStatModOrFail_6FDB7690(pUnit->pStatListEx, D2SLayerStatIdStrc::Make(nLayer, nStatId).nPackedValue);
		}
	}
}

// 1.13c D2Common+0x3A710
//D2Common.0x6FDB7B00 (#10518)
// FN-capture trace of the 0x3A330 call (checked against retail); null in the game.
void (*gpfnStatListAddUnitStatTrace)(D2StatListStrc*, int, int, uint16_t) = nullptr;

void __stdcall STATLIST_AddUnitStat(D2UnitStrc* pUnit, int nStatId, int nValue, uint16_t nLayer)
{
	if (nValue)
	{
		if (gpfnStatListAddUnitStatTrace)
			gpfnStatListAddUnitStatTrace(pUnit->pStatListEx, nStatId, nValue, nLayer);
		STATLIST_AddStat(pUnit->pStatListEx, nStatId, nValue, nLayer);
	}
}

//D2Common.0x6FDB7B30 (#10521)
int __stdcall STATLIST_GetUnitBaseStat(D2UnitStrc* pUnit, int nStatId, uint16_t nLayer)
{
	if (!pUnit || !pUnit->pStatListEx)
	{
		return 0;
	}
	D2StatListStrc* pStatList = pUnit->pStatListEx;

	D2ItemStatCostTxt* pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(nStatId);
	if (!pItemStatCostTxtRecord)
	{
		return 0;
	}

	return STATLIST_GetBaseStat_6FDB6340(pStatList, D2SLayerStatIdStrc::Make(nLayer, nStatId).nPackedValue, pItemStatCostTxtRecord);
}

// Helper function
// Different than STATLIST_GetStatValue in that it may use FullStats
// The field order 1.13c D2Common+0x38BD0 and its callee depend on (`Stats` vs `FullStats`, the owner type,
// the flags). wasm64 widens the pointers, so these are the retail offsets plus one word per pointer.
static_assert(offsetof(D2StatStrc, nValue) == 4 && sizeof(D2StatStrc) == 8, "stat entry: packed layer/statId then value");
static_assert(offsetof(D2StatsArrayStrc, nStatCount) == sizeof(void*), "stat array: pStat then nStatCount");
static_assert(offsetof(D2StatListStrc, dwFlags) == 2 * sizeof(void*) + 8, "statlist: pMemPool, pUnit, owner type/id, flags");
static_assert(offsetof(D2StatListExStrc, FullStats) == offsetof(D2StatListExStrc, pOwner) + sizeof(void*), "statlist ex: full stats right after the owner");
// 1.13c D2Common+0x38BD0
int32_t __stdcall STATLIST_GetTotalStatValue(D2StatListStrc* pStatList, int nStatId, uint16_t nLayer)
{
	if (!pStatList)
	{
		return 0;
	}

	D2ItemStatCostTxt* pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(nStatId);
	if (!pItemStatCostTxtRecord)
	{
		return 0;
	}
	return STATLIST_GetTotalStat_6FDB63E0(pStatList, D2SLayerStatIdStrc::Make(nLayer, nStatId).nPackedValue, pItemStatCostTxtRecord);
}

//D2Common.0x6FDB7C30 (#10519)
int32_t __stdcall STATLIST_UnitGetStatValue(const D2UnitStrc* pUnit, int nStatId, uint16_t nLayer)
{
	return STATLIST_GetTotalStatValue(pUnit->pStatListEx, nStatId, nLayer);
}

//D2Common.0x6FDB7E30 (#10520)
// A little history for those wondering why it returns the exact same thing as STATLIST_UnitGetStatValue
// There have been a few reworks of the stat system over the different versions of the game.
// In the versions 1.08 and below, the "base" stats and "item/skill" stats were in two seperate lists.
// Then in 1.09, both lists were merged into the same system, resulting in both functions doing the same thing.
// This can easily be tracked by checking the function responsible for computing the defense rate and its usage of both functions.
// - 1.00 to 1.05 : UNITS_GetDefenseRate#10426. STATLIST_UnitGetStatValue#10497. STATLIST_UnitGetItemStatOrSkillStatValue#10498.
// - 1.08 to 1.10f: UNITS_GetDefenseRate#10431. STATLIST_UnitGetStatValue#10519. STATLIST_UnitGetItemStatOrSkillStatValue#10520.
int32_t __stdcall STATLIST_UnitGetItemStatOrSkillStatValue(D2UnitStrc* pUnit, int nStatId, uint16_t nLayer)
{
	return STATLIST_UnitGetStatValue(pUnit, nStatId, nLayer);
}


//D2Common.0x6FDB7D40 (#10466)
int __stdcall STATLIST_GetStatValue(D2StatListStrc* pStatList, int nStatId, uint16_t nLayer)
{
	if (!pStatList)
	{
		return 0;
	}

	D2ItemStatCostTxt* pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(nStatId);
	if (!pItemStatCostTxtRecord)
	{
		return 0;
	}

	if (D2StatStrc* pStat = STATLIST_FindStat_6FDB6920(&pStatList->Stats, D2SLayerStatIdStrc::Make(nLayer, nStatId).nPackedValue))
	{
		return STATLIST_ApplyMinValue(pStat->nValue, pItemStatCostTxtRecord, STATLIST_StatListExCast(pStatList));
	}

	return 0;
}


//D2Common.0x6FDB7F40 (#10522)
int __stdcall STATLIST_GetUnitStatBonus(D2UnitStrc* pUnit, int nStatId, uint16_t nLayer)
{


	if (!pUnit->pStatListEx)
	{
		return 0;
	}
	D2ItemStatCostTxt* pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(nStatId);

	if (!pItemStatCostTxtRecord)
	{
		return 0;
	}

	D2StatListStrc* pStatList = pUnit->pStatListEx;
	D2StatListExStrc* pStatListEx = STATLIST_StatListExCast(pStatList);

	int nValue = 0;
	D2StatsArrayStrc* pStatsArray = pStatListEx ? &pStatListEx->FullStats : &pStatList->Stats;
	if (const D2StatStrc* pStat = STATLIST_FindStat_6FDB6920(pStatsArray, D2SLayerStatIdStrc::Make(nLayer, nStatId).nPackedValue))
	{
		nValue = STATLIST_ApplyMinValue(pStat->nValue, pItemStatCostTxtRecord, pStatListEx);
	}

	D2StatsArrayStrc* pBaseStatsArray = &pStatList->Stats;
	if (const D2StatStrc* pStat = STATLIST_FindStat_6FDB6920(pBaseStatsArray, D2SLayerStatIdStrc::Make(nLayer, nStatId).nPackedValue))
	{
		int nBaseValue = STATLIST_ApplyMinValue(pStat->nValue, pItemStatCostTxtRecord, pStatListEx);
		return nValue - nBaseValue;
	}

	return nValue;
}

//D2Common.0x6FDB80C0 (#10515)
// 1.13c D2Common+0x3AE60
void __stdcall STATLIST_DeactivateTemporaryStates(D2UnitStrc* pUnit)
{
	if (pUnit->pStatListEx == nullptr || !(pUnit->pStatListEx->dwFlags & STATLIST_NEWLENGTH))
	{
		return;
	}

	D2StatListStrc* pPrevious;
	for (D2StatListStrc* pCurrent = pUnit->pStatListEx->pMyLastList; pCurrent != nullptr; pCurrent = pPrevious)
	{
		pPrevious = pCurrent->pPrevLink;
		if (!(pCurrent->dwFlags & STATLIST_TEMPONLY))
		{
			continue;
		}

		if (pCurrent->dwStateNo)
		{
			STATES_ToggleState(pUnit, pCurrent->dwStateNo, FALSE);
		}

		if (!STATLIST_IsExtended(pCurrent))
		{
			D2Common_STATLIST_FreeStatListImpl_6FDB7050(pCurrent);
		}

		pPrevious = pUnit->pStatListEx->pMyLastList;
	}

	pUnit->pStatListEx->dwFlags &= ~STATLIST_NEWLENGTH;
}

//D2Common.0x6FDB8120 (#10467)
// 1.13c D2Common+0x37FD0 (ordinal 10307 in 1.13c)
int __stdcall D2Common_10467(D2StatListStrc* pStatList, int nStat)
{
	if (pStatList && nStat < pStatList->Stats.nStatCount)
	{
		return pStatList->Stats.pStat[nStat].nValue;
	}

	return 0;
}

//D2Common.0x6FDB8150 (#10468)
void __stdcall STATLIST_RemoveAllStats(D2StatListStrc* pStatList)
{
	if (pStatList == nullptr)
	{
		return;
	}

	while (pStatList->Stats.nStatCount > 0)
	{
		STATLIST_SetStat(pStatList, pStatList->Stats.pStat->nStat, 0, pStatList->Stats.pStat->nLayer);
	}
}

//D2Common.0x6FDB8190
D2StatListStrc* __stdcall D2Common_GetStateFromStatListEx_6FDB8190(D2StatListExStrc* pStatListEx, int nStateId)
{
	if (pStatListEx && nStateId)
	{
		D2StatListStrc* pStatList = STATLIST_IsExtended(pStatListEx) ? pStatListEx->pMyLastList : nullptr;
		while (pStatList && pStatList->dwStateNo != nStateId)
		{
			pStatList = pStatList->pPrevLink;
		}

		if (pStatList)
		{
			return pStatList;
		}

		pStatList = pStatListEx->pMyStats;
		while (pStatList && pStatList->dwStateNo != nStateId)
		{
			pStatList = pStatList->pPrevLink;
		}
		return pStatList;
	}

	return NULL;
}

//D2Common.0x6FDB81E0 (#10480)
// 1.13c D2Common+0x38520
D2StatListStrc* __stdcall STATLIST_GetStatListFromUnitAndState(D2UnitStrc* pUnit, int nState)
{
	if (pUnit->pStatListEx && STATLIST_IsExtended(pUnit->pStatListEx))
	{
		return D2Common_GetStateFromStatListEx_6FDB8190(pUnit->pStatListEx, nState);
	}

	return nullptr;
}

//D2Common.0x6FDB8200 (#10482)
D2StatListStrc* __stdcall STATLIST_GetStatListFromFlag(D2StatListStrc* pStatList, int nFlag)
{
	if (!pStatList)
	{
		return nullptr;
	}

	pStatList = pStatList->pPrevLink;
	while (pStatList && !(nFlag & pStatList->dwFlags))
	{
		pStatList = pStatList->pPrevLink;
	}
	return pStatList;
}

// 1.13c D2Common+0x37F40: the walk 1.10f folded into STATLIST_GetStatListFromUnitAndFlag is a separate
// helper in 1.13c (pStatListEx in EAX, nFlag in ECX). Null -> null; a non-extended list starts the walk at
// its own header, an extended one at pMyStats when nFlag has STATLIST_SET else pMyLastList; the set bit is
// then cleared; walk pPrevLink to the first list whose flags overlap the rest of nFlag.
D2StatListStrc* __fastcall STATLIST_GetStatListFromStatListExAndFlag(D2StatListExStrc* pStatListEx, int nFlag)
{
	if (!pStatListEx)
	{
		return nullptr;
	}

	D2StatListStrc* pStatList;
	if (STATLIST_IsExtended(pStatListEx))
	{
		pStatList = nFlag & STATLIST_SET ? pStatListEx->pMyStats : pStatListEx->pMyLastList;
	}
	else
	{
		pStatList = pStatListEx;
	}

	nFlag &= ~STATLIST_SET;

	while (pStatList && !(nFlag & pStatList->dwFlags))
	{
		pStatList = pStatList->pPrevLink;
	}

	return pStatList;
}

//D2Common.0x6FDB8230 (#10481)
// 1.13c D2Common+0x384F0
D2StatListStrc* __stdcall STATLIST_GetStatListFromUnitAndFlag(D2UnitStrc* pUnit, int nFlag)
{
	if (pUnit->pStatListEx && STATLIST_IsExtended(pUnit->pStatListEx))
	{
		return STATLIST_GetStatListFromStatListExAndFlag(pUnit->pStatListEx, nFlag);
	}

	return nullptr;
}

//D2Common.0x6FDB8270 (#10483)
// 1.13c D2Common+0x384B0
D2StatListStrc* __stdcall STATLIST_GetStatListFromUnitStateOrFlag(D2UnitStrc* pUnit, int nState, int nFlag)
{
	if (pUnit->pStatListEx && STATLIST_IsExtended(pUnit->pStatListEx))
	{
		if (nState)
		{
			return D2Common_GetStateFromStatListEx_6FDB8190(pUnit->pStatListEx, nState);
		}

		return STATLIST_GetStatListFromUnitAndFlag(pUnit, nFlag);
	}

	return NULL;
}

//D2Common.0x6FDB82C0 (#10484)
D2StatListStrc* __stdcall STATLIST_GetStatListFromUnitStateAndFlag(D2UnitStrc* pUnit, int nState, int nFlag)
{
	if (pUnit->pStatListEx && STATLIST_IsExtended(pUnit->pStatListEx))
	{
		D2StatListStrc* pStatList = nFlag & STATLIST_SET ? pUnit->pStatListEx->pMyStats : pUnit->pStatListEx->pMyLastList;
		nFlag &= ~STATLIST_SET;
		while (pStatList && (pStatList->dwStateNo != nState || nFlag && !(nFlag & pStatList->dwFlags)))
		{
			pStatList = pStatList->pPrevLink;
		}

		return pStatList;
	}

	return NULL;
}

// 1.13c D2Common+0x3AC90
//D2Common.0x6FDB8310 (#10523)
void __stdcall STATLIST_MergeStatLists(D2UnitStrc* pTarget, D2UnitStrc* pUnit, BOOL bType)
{
	if (pTarget && pUnit && pUnit->pStatListEx)
	{
		if (pUnit->dwUnitType == UNIT_ITEM && (ITEMS_GetBodyLocation(pUnit) == BODYLOC_SWRARM || ITEMS_GetBodyLocation(pUnit) == BODYLOC_SWLARM))
		{
			D2Common_ExpireStatList_6FDB6E30(pUnit->pStatListEx);
		}
		else if (pUnit->pStatListEx->pUnit == pTarget)
		{
			if (bType && (pUnit->pStatListEx->dwFlags & STATLIST_DYNAMIC))
			{
				D2Common_11274(pTarget, pUnit);
			}
			else if (!bType && !(pUnit->pStatListEx->dwFlags & STATLIST_DYNAMIC))
			{
				D2Common_11275(pTarget, pUnit);
			}
		}
		else
		{
			D2COMMON_10475_PostStatToStatList(pTarget, pUnit->pStatListEx, bType);
			pUnit->pStatListEx->pUnit = pTarget;
		}
	}
}


//D2Common.0x6FDB83A0 (#10535)
D2UnitStrc* __stdcall STATLIST_GetOwner(D2UnitStrc* pUnit, BOOL* pStatNotDynamic)
{
	if (!pUnit || !pUnit->pStatListEx || !pUnit->pStatListEx->pParent || !STATLIST_IsExtended(pUnit->pStatListEx->pParent))
	{
		if (pStatNotDynamic)
		{
			*pStatNotDynamic = FALSE;
		}
		return NULL;
	}

	if (pStatNotDynamic)
	{
		*pStatNotDynamic = !(pUnit->pStatListEx->dwFlags & STATLIST_DYNAMIC);
	}
	return ((D2StatListExStrc*)pUnit->pStatListEx->pParent)->pOwner;
}

//D2Common.0x6FDB8420 (#10512)
void __stdcall D2Common_10512(D2UnitStrc* pUnit1, D2UnitStrc* pUnit2, int nStatId, void (__fastcall* pfCallback)(D2UnitStrc*, int, int, D2UnitStrc*))
{
	if (!pUnit1 || !pUnit1->pStatListEx || !STATLIST_IsExtended(pUnit1->pStatListEx))
	{
		return;
	}

	const D2SLayerStatIdStrc nLayerId = D2SLayerStatIdStrc::MakeFromStatId(nStatId);

	if (StatArray_DichotomicSearch(&pUnit1->pStatListEx->ModStats, nLayerId.nPackedValue) < 0)
	{
		return;
	}

	if (D2StatStrc* pBaseStat = STATLIST_FindStat_6FDB6920(&pUnit1->pStatListEx->Stats, nLayerId.nPackedValue))
	{
		pfCallback(pUnit1, nStatId, pBaseStat->nValue, pUnit2);
	}
}

//D2Common.0x6FDB84E0 (#10513)
void __stdcall D2Common_10513(D2UnitStrc* pUnit1, D2UnitStrc* pUnit2, void (__fastcall* pfCallback)(D2UnitStrc*, int, int, D2UnitStrc*))
{
	if (!pUnit1 || (!pUnit2 && pfCallback) || !pUnit1->pStatListEx || !STATLIST_IsExtended(pUnit1->pStatListEx))
	{
		return;
	}
	D2StatListExStrc* pUnit1StatList = pUnit1->pStatListEx;
	for (int i = 0; i < pUnit1StatList->ModStats.nStatCount; ++i)
	{
		const D2SLayerStatIdStrc* pModStat = &pUnit1StatList->ModStats.pStat[i];
		int nBaseStatValue = 0;
		if (D2StatStrc* pBaseStat = STATLIST_FindStat_6FDB6920(&pUnit1StatList->Stats, pModStat->nPackedValue))
		{
			nBaseStatValue = pBaseStat->nValue;
		}

		pfCallback(pUnit1, pModStat->nStat, nBaseStatValue, pUnit2);
	}
}

//D2Common.0x6FDB85D0 (#10511)
// 1.13c D2Common+0x37E20
void __stdcall STATLIST_FreeModStats(D2UnitStrc* pUnit)
{
	if (pUnit && pUnit->pStatListEx && STATLIST_IsExtended(pUnit->pStatListEx))
	{
		if (pUnit->pStatListEx->ModStats.pStat)
		{
			D2_FREE_POOL(pUnit->pStatListEx->pMemPool, pUnit->pStatListEx->ModStats.pStat);
			pUnit->pStatListEx->ModStats.pStat = NULL;
		}

		pUnit->pStatListEx->ModStats.nCapacity = 0;
		pUnit->pStatListEx->ModStats.nStatCount = 0;
	}
}

// 1.13c D2Common+0x391F0
//D2Common.0x6FDB8620 (#10562)
int __stdcall STATLIST_GetUnitAlignment(D2UnitStrc* pUnit)
{
	if (!pUnit || !pUnit->pStatListEx)
	{
		return UNIT_ALIGNMENT_EVIL;
	}

	if (pUnit->dwUnitType != UNIT_PLAYER && pUnit->dwUnitType != UNIT_MONSTER)
	{
		return UNIT_ALIGNMENT_GOOD;
	}

	if (D2StatListExStrc* pStatListEx = STATLIST_StatListExCast(pUnit->pStatListEx))
	{
		if (D2StatListStrc* pAlignmentStateList = D2Common_GetStateFromStatListEx_6FDB8190(pUnit->pStatListEx, STATE_ALIGNMENT))
		{
			return STATLIST_GetTotalStatValue_Layer0((D2StatListExStrc*)pAlignmentStateList, STAT_ALIGNMENT);
		}
	}

	return UNIT_ALIGNMENT_EVIL;
}

//D2Common.0x6FDB8750 (#10534)
void __stdcall D2Common_10534(D2UnitStrc* pUnit)
{
	if (pUnit && pUnit->pStatListEx)
	{
		pUnit->pStatListEx->dwFlags |= STATLIST_UNK_0x100;
	}
}

//D2Common.0x6FDB8770 (#10530)
BOOL __stdcall D2COMMON_10530_D2CheckStatlistFlagDMGRed(D2UnitStrc* pUnit)
{
	if (pUnit && pUnit->pStatListEx)
	{
		return pUnit->pStatListEx->dwFlags & STATLIST_UNK_0x100;
	}

	return FALSE;
}

//D2Common.0x6FDB87A0 (#10532)
int __stdcall STATLIST_GetTotalStatValue_Layer0(D2StatListStrc* pStatListEx, int nStatId)
{
	return STATLIST_GetTotalStatValue(pStatListEx, nStatId, 0);
}

//D2Common.0x6FDB8890 (#10533)
void __stdcall STATLIST_RemoveAllStatsFromOverlay(D2UnitStrc* pUnit)
{

	if (pUnit && pUnit->pStatListEx)
	{
		pUnit->pStatListEx->dwFlags &= ~STATLIST_UNK_0x100;

		if (D2StatListExStrc* pStatListEx = STATLIST_StatListExCast(pUnit->pStatListEx))
		{
			D2StatListStrc* pCurStatList = pStatListEx->pMyLastList;
			while (pCurStatList && !(pCurStatList->dwFlags & STATLIST_OVERLAY))
			{
				pCurStatList = pCurStatList->pPrevLink;
			}
			if(pCurStatList)
			{
				STATLIST_RemoveAllStats(pCurStatList);
			}
		}
	}
}

//D2Common.0x6FDB8900
void __stdcall D2Common_STATES_ToggleState_6FDB8900(D2UnitStrc* pUnit, int nState, BOOL bSet)
{
	
	if (pUnit && pUnit->pStatListEx && STATLIST_IsExtended(pUnit->pStatListEx))
	{
		const uint32_t dwStateBitMask = gdwBitMasks[nState % 32];
		const bool bStateBitChanged = BITMANIP_SetBitsValueForMask(pUnit->pStatListEx->StatFlags[nState / 32], dwStateBitMask, bSet);
		if (!bStateBitChanged)
		{
			return;
		}
		const size_t states2Offset = (sgptDataTables->nStatesTxtRecordCount + 31) / 32;
		pUnit->pStatListEx->StatFlags[nState / 32 + states2Offset] |= dwStateBitMask;

		if (D2StatesTxt* pStatesTxtRecord = DATATBLS_GetStatesTxtRecord(nState))
		{
			bool bStateMaskDisguise = pStatesTxtRecord->dwStateFlags & gdwBitMasks[STATEMASK_DISGUISE];
			if (bSet && bStateMaskDisguise)
			{
				pUnit->dwFlagEx |= UNITFLAGEX_ISSHAPESHIFTED;
			}
			else if (bStateMaskDisguise  && !STATES_CheckStateMaskOnUnit(pUnit, STATEMASK_DISGUISE))
			{
				pUnit->dwFlagEx &= ~UNITFLAGEX_ISSHAPESHIFTED;
			}
		}
	}
}

//D2Common.0x6FDB8A90
// 1.13c D2Common+0x33CA0: same body
uint32_t* __stdcall D2COMMON_STATES_GetStatFlags_6FDB8A90(D2UnitStrc* pUnit)
{
	if (pUnit && pUnit->pStatListEx && STATLIST_IsExtended(pUnit->pStatListEx))
	{
		return pUnit->pStatListEx->StatFlags;
	}

	return NULL;
}

//D2Common.0x6FDB8AC0
// 1.13c D2Common+0x342D0 (retail exits on a null unit; ours returns NULL)
uint32_t* __stdcall D2COMMON_STATES_GetListGfxFlags_6FDB8AC0(D2UnitStrc* pUnit)
{
	if (pUnit && pUnit->pStatListEx && STATLIST_IsExtended(pUnit->pStatListEx))
	{
		return &pUnit->pStatListEx->StatFlags[(sgptDataTables->nStatesTxtRecordCount + 31) / 32];
	}

	return NULL;
}

//D2Common.0x6FDB8B10 (#10516)
// 1.13c D2Common+0x3ADD0
void __stdcall STATLIST_UpdateStatListsExpiration(D2UnitStrc* pUnit, int nFrame)
{
	if (pUnit == nullptr || pUnit->pStatListEx == nullptr || !STATLIST_IsExtended(pUnit->pStatListEx))
	{
		return;
	}

	D2StatListStrc* pPrevious;
	for (D2StatListStrc* pCurrent = pUnit->pStatListEx->pMyLastList; pCurrent != nullptr; pCurrent = pPrevious)
	{
		pPrevious = pCurrent->pPrevLink;
		if (!(pCurrent->dwFlags & STATLIST_NEWLENGTH))
		{
			continue;
		}

		if (nFrame == 0)  // Client, the server only syncs
		{
			pCurrent->dwExpireFrame--;
		}

		if (pCurrent->dwExpireFrame <= nFrame)
		{
			if (!STATLIST_IsExtended(pCurrent))
			{
				D2Common_STATLIST_FreeStatListImpl_6FDB7050(pCurrent);
			}

			pPrevious = pUnit->pStatListEx->pMyLastList;
		}
	}
}


// Helper function
int __stdcall STATLIST_CopyStatsData(D2StatsArrayStrc* pStatsArray, D2StatStrc* pOutStatBuffer, int nBufferSize)
{
	int nStatCount = pStatsArray->nStatCount;
	if (nStatCount >= nBufferSize)
	{
		nStatCount = nBufferSize;
	}

	for(int nCounter = 0; nCounter < nStatCount; ++nCounter)
	{
		pOutStatBuffer[nCounter].nLayer = pStatsArray->pStat[nCounter].nLayer;
		pOutStatBuffer[nCounter].nStat  = pStatsArray->pStat[nCounter].nStat;
		pOutStatBuffer[nCounter].nValue = pStatsArray->pStat[nCounter].nValue;
	}

	return nStatCount;
}

//D2Common.0x6FDB8BA0 (#11268)
// 1.13c D2Common+0x38410
// Fields this function reads, wasm64-measured (retail 0x5C/0x48/0x04/8): widening the pointers is the only
// difference from the 32-bit layout.
static_assert(offsetof(D2UnitStrc, pStatListEx) == 124, "unit: statlist ex pointer (retail +0x5C, wasm64 widened)");
int __stdcall STATLIST_GetFullStatsDataFromUnit(D2UnitStrc* pUnit, D2StatStrc* pOutStatBuffer, int nBufferSize)
{
	if (pUnit && pUnit->pStatListEx && STATLIST_IsExtended(pUnit->pStatListEx))
	{
		return STATLIST_CopyStatsData(&pUnit->pStatListEx->FullStats, pOutStatBuffer, nBufferSize);
	}
	return 0;
}

//D2Common.0x6FDB8C00 (#11243)
// 1.13c D2Common+0x383C0
int __stdcall STATLIST_GetBaseStatsData(D2StatListStrc* pStatListEx, D2StatStrc* pOutStatBuffer, int nBufferSize)
{
	if (pStatListEx)
	{
		return STATLIST_CopyStatsData(&pStatListEx->Stats, pOutStatBuffer, nBufferSize);
	}
	return 0;
}

//D2Common.0x6FDB8C50 (#10573)
// 1.13c D2Common+0x3A600
void __stdcall STATLIST_MergeBaseStats(D2StatListStrc* pTargetStatList, D2StatListStrc* pSourceStatlist)
{
	if (pTargetStatList && pSourceStatlist)
	{
		for (D2StatStrc* i = pSourceStatlist->Stats.pStat; i < &pSourceStatlist->Stats.pStat[pSourceStatlist->Stats.nStatCount]; ++i)
		{
			STATLIST_AddStat(pTargetStatList, i->nStat, i->nValue, i->nLayer);
		}
	}
}

//D2Common.0x6FDB8CA0 (#10477)
// 1.13c D2Common+0x37D70
void __stdcall STATLIST_SetStatRemoveCallback(D2StatListStrc* pStatList, StatListRemoveCallback pfStatRemove)
{
	if (pStatList)
	{
		pStatList->fpStatRemove = pfStatRemove;
	}
}

//D2Common.0x6FDB8CC0 (#10469)
// 1.13c D2Common+0x3AD60
void __stdcall D2Common_10469(D2UnitStrc* pUnit)
{
	if (pUnit && pUnit->pStatListEx && STATLIST_IsExtended(pUnit->pStatListEx))
	{
		if (D2StatListStrc* pStatListEx = pUnit->pStatListEx->pMyLastList)
		{
			D2StatListStrc* pPrevious = NULL;
			do
			{
				pPrevious = pStatListEx->pPrevLink;
				if (pStatListEx->dwOwnerType != UNIT_ITEM 
					&& !(pStatListEx->dwFlags & (STATLIST_BASIC|STATLIST_OVERLAY|STATLIST_UNK_0x100)) 
					&& !STATES_CheckStateMaskStayDeathOnUnitByStateId(pUnit, pStatListEx->dwStateNo))
				{
					if (!STATLIST_IsExtended(pStatListEx))
					{
						D2Common_STATLIST_FreeStatListImpl_6FDB7050(pStatListEx);
					}

					pPrevious = pUnit->pStatListEx->pMyLastList;
				}
				pStatListEx = pPrevious;
			}
			while (pPrevious);
		}
	}
}

// Helper function
static void STATLIST_ClampStat(D2StatListExStrc* pStatListEx, int nStatId)
{
	int nCurrentValue = 0;
	if (D2ItemStatCostTxt* pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(nStatId))
	{
		nCurrentValue = STATLIST_GetTotalStat_6FDB63E0(pStatListEx, D2SLayerStatIdStrc::MakeFromStatId(nStatId).nPackedValue, pItemStatCostTxtRecord);
	}

	const int nMaxStatId = nStatId + 1; // Assumes the Max of the input stat is the next stat, for example STAT_MANA = 8 and STAT_MAXMANA = 9
	int nMaxValue = 0;
	if (D2ItemStatCostTxt* pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(nMaxStatId))
	{
		nMaxValue = STATLIST_GetTotalStat_6FDB63E0(pStatListEx, D2SLayerStatIdStrc::MakeFromStatId(nMaxStatId).nPackedValue, pItemStatCostTxtRecord);
	}

	if (nCurrentValue > nMaxValue)
	{
		STATLIST_AddStat(pStatListEx, nStatId, nMaxValue - nCurrentValue, 0);
	}
}

//D2Common.0x6FDB8D30 (#10514)
// 1.13c D2Common+0x3AB30
void __stdcall STATLIST_ClampStaminaManaHP(D2UnitStrc* pUnit)
{
	if (pUnit && pUnit->pStatListEx)
	{
		STATLIST_ClampStat(pUnit->pStatListEx, STAT_STAMINA);
		STATLIST_ClampStat(pUnit->pStatListEx, STAT_MANA);
		STATLIST_ClampStat(pUnit->pStatListEx, STAT_HITPOINTS);
	}
}

//D2Common.0x6FDB8EB0 (#10574)
BOOL __stdcall D2Common_10574(D2UnitStrc* pUnit, int nStateId, BOOL bSet)
{

	if (nStateId && pUnit->pStatListEx && STATLIST_IsExtended(pUnit->pStatListEx))
	{
		if (D2StatListStrc* pStatList = D2Common_GetStateFromStatListEx_6FDB8190(pUnit->pStatListEx, nStateId))
		{
			if (bSet)
			{
				if (!(pStatList->dwFlags & STATLIST_SET))
				{
					D2Common_ExpireStatList_6FDB6E30((D2StatListExStrc*)pStatList);
					pStatList->dwFlags |= STATLIST_SET;
					D2COMMON_10475_PostStatToStatList(pUnit, pStatList, TRUE);
				}
			}
			else
			{
				if (pStatList->dwFlags & STATLIST_SET)
				{
					D2Common_ExpireStatList_6FDB6E30((D2StatListExStrc*)pStatList);
					pStatList->dwFlags &= ~STATLIST_SET;
					D2COMMON_10475_PostStatToStatList(pUnit, pStatList, TRUE);
				}
			}

			return TRUE;
		}
	}

	return FALSE;
}

//D2Common.0x6FDB8F30 (#10525)
void __stdcall D2Common_10525(D2UnitStrc* pUnit1, D2UnitStrc* pUnit2)
{
	if (!pUnit1 || !pUnit2 || !(pUnit1->dwUnitType == UNIT_PLAYER || pUnit1->dwUnitType == UNIT_MONSTER))
	{
		return;
	}
	D2StatListExStrc* pStatListEx2 = pUnit2->pStatListEx ? STATLIST_StatListExCast(pUnit2->pStatListEx) : nullptr;
	if (!pStatListEx2)
	{
		return;
	}

	for (D2StatStrc* j = pStatListEx2->FullStats.pStat; j < &pStatListEx2->FullStats.pStat[pStatListEx2->FullStats.nStatCount]; ++j)
	{
		D2ItemStatCostTxt* pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(j->nStat);
		if (pItemStatCostTxtRecord && pItemStatCostTxtRecord->nOp >= 6 && pItemStatCostTxtRecord->nOp <= 7)
		{
			for (uint16_t wOpStat : pItemStatCostTxtRecord->wOpStat)
			{
				if (wOpStat == uint16_t(-1))
				{
					break;
				}

				D2SLayerStatIdStrc nOpLayer_StatId = D2SLayerStatIdStrc::MakeFromStatId(wOpStat);
				D2StatListExStrc* pUnit1StatListEx = pUnit1->pStatListEx;
				int nOpStatValue = sub_6FDB5830(pUnit1StatListEx, nOpLayer_StatId.nPackedValue);

				D2StatStrc* pUnit1Stat = STATLIST_FindStat_6FDB6920(&pUnit1StatListEx->FullStats, nOpLayer_StatId.nPackedValue);
				if (!pUnit1Stat && nOpStatValue != 0)
				{
					pUnit1Stat = STATLIST_InsertStatOrFail_6FDB6970(pUnit1StatListEx->pMemPool, &pUnit1StatListEx->FullStats, nOpLayer_StatId.nPackedValue);
				}
				if (pUnit1Stat)
				{
					STATLIST_SetUnitStatNewValue(pUnit1StatListEx, &pUnit1StatListEx->FullStats, pUnit1Stat, nOpLayer_StatId.nPackedValue, nOpStatValue, ITEMS_GetItemStatCostTxtRecord(wOpStat), pUnit1);
				}
			}
		}
	}
}

//D2Common.0x6FDB91C0 (#10474)
// 1.13c D2Common+0x3A5F0
void __stdcall D2Common_10474(D2UnitStrc* pUnused, D2StatListStrc* pStatList)
{
	D2_MAYBE_UNUSED(pUnused);
	D2Common_ExpireStatList_6FDB6E30((D2StatListExStrc*)pStatList);
}

//D2Common.0x6FDB91D0 (#10564)
// 1.13c D2Common+0x390F0
int __stdcall STATLIST_GetMaxLifeFromUnit(D2UnitStrc* pUnit)
{
	return D2Common_11248(NULL, pUnit, STAT_MAXHP);
}

//D2Common.0x6FDB92C0 (#10565)
int __stdcall STATLIST_GetMaxManaFromUnit(D2UnitStrc* pUnit)
{
	return D2Common_11248(NULL, pUnit, STAT_MAXMANA);
}

//D2Common.0x6FDB93B0 (#10566)
int __stdcall STATLIST_GetMaxStaminaFromUnit(D2UnitStrc* pUnit)
{
	return D2Common_11248(NULL, pUnit, STAT_MAXSTAMINA);
}

//D2Common.0x6FDB94A0 (#10567)
int __stdcall STATLIST_GetMaxDurabilityFromUnit(D2UnitStrc* pUnit)
{
	return D2Common_11248(NULL, pUnit, STAT_MAXDURABILITY);
}

//D2Common.0x6FDB95D0 (#10568)
int __stdcall STATLIST_GetMaxDamageFromUnit(D2UnitStrc* pUnit, BOOL b2Handed)
{
	if (b2Handed)
	{
		return D2Common_11248(NULL, pUnit, STAT_SECONDARY_MAXDAMAGE);
	}
	else
	{
		return D2Common_11248(NULL, pUnit, STAT_MAXDAMAGE);
	}
}

//D2Common.0x6FDB96F0 (#10569)
// 1.13c D2Common+0x38ED0
int __stdcall STATLIST_GetMinDamageFromUnit(D2UnitStrc* pUnit, BOOL b2Handed)
{
	if (b2Handed)
	{
		return D2Common_11248(NULL, pUnit, STAT_SECONDARY_MINDAMAGE);
	}
	else
	{
		return D2Common_11248(NULL, pUnit, STAT_MINDAMAGE);
	}
}

//D2Common.0x6FDB9810 (#10570)
int __stdcall STATLIST_GetMaxThrowDamageFromUnit(D2UnitStrc* pUnit)
{
	return D2Common_11248(NULL, pUnit, STAT_ITEM_THROW_MAXDAMAGE);
}

//D2Common.0x6FDB9900 (#10571)
int __stdcall STATLIST_GetMinThrowDamageFromUnit(D2UnitStrc* pUnit)
{
	return D2Common_11248(NULL, pUnit, STAT_ITEM_THROW_MINDAMAGE);
}

//D2Common.0x6FDB99F0 (#10572)
int __stdcall STATLIST_GetDefenseFromUnit(D2UnitStrc* pUnit)
{
	return D2Common_11248(NULL, pUnit, STAT_ARMORCLASS);
}

// 1.13c D2Common+0x3A5D0
//D2Common.0x6FDB9AE0 (#10524)
void __stdcall STATLIST_ExpireUnitStatlist(D2UnitStrc* pUnused, D2UnitStrc* pUnit)
{
	D2_MAYBE_UNUSED(pUnused);
	if (pUnit && pUnit->pStatListEx)
	{
		D2Common_ExpireStatList_6FDB6E30(pUnit->pStatListEx);
	}
}

//D2Common.0x6FDB9B00 (#10531)
// 1.13c D2Common+0x3A5C0
int __stdcall D2Common_10531_SetStatInStatListLayer0(D2StatListStrc* pStatList, int nStatId, int nValue, int nUnused)
{
	D2_MAYBE_UNUSED(nUnused);
	return STATLIST_SetStat(pStatList, nStatId, nValue, 0);
}

//D2Common.0x6FDB9B10 (#11248)
int __stdcall D2Common_11248(D2UnitStrc* pUnused, D2UnitStrc* pUnit, int nStatId)
{
	D2_MAYBE_UNUSED(pUnused);
	if (pUnit)
	{
		return STATLIST_GetTotalStatValue_Layer0(pUnit->pStatListEx, nStatId);
	}

	return 0;
}

//D2Common.0x6FDA9E60 (#11264)
void __stdcall STATLIST_SetSkillId(D2StatListStrc* pStatList, int nSkillId)
{
	pStatList->dwSkillNo = nSkillId;
}

//D2Common.0x6FDB9C10 (#11265)
int __stdcall STATLIST_GetSkillId(D2StatListStrc* pStatList)
{
	if (pStatList)
	{
		return pStatList->dwSkillNo;
	}

	REMOVE_LATER_Trace("STATLIST_GetSkillId: NULL pointer");
	return 0;
}

//D2Common.0x6FDB9C20 (#11266)
void __stdcall STATLIST_SetSkillLevel(D2StatListStrc* pStatList, int nSkillLevel)
{
	if (pStatList)
	{
		pStatList->dwSLvl = nSkillLevel;
		return;
	}

	REMOVE_LATER_Trace("STATLIST_SetSkillLevel: NULL pointer");
}

//D2Common.0x6FDA9E70 (#11267)
int __stdcall STATLIST_GetSkillLevel(D2StatListStrc* pStatList)
{
	if (pStatList)
	{
		return pStatList->dwSLvl;
	}

	REMOVE_LATER_Trace("STATLIST_GetSkillLevel: NULL pointer");
	return 0;
}

//D2Common.0x6FDB9C30 (#11269)
int __stdcall D2COMMON_11269_CopyStats(D2StatListExStrc* pStatListEx, int nStatId, D2StatStrc* pBuffer, int nBufferSize)
{
	return D2Common_CopyStats_6FDB9C50(pStatListEx, nStatId, pBuffer, nBufferSize);
}

//D2Common.0x6FDB9C50
int __fastcall D2Common_CopyStats_6FDB9C50(D2StatListStrc* pStatList, int nStatId, D2StatStrc* pBuffer, int nBufferSize)
{
	if (!pStatList)
	{
		return 0;
	}

	D2StatListExStrc* pStatListEx = STATLIST_StatListExCast(pStatList);
	D2StatsArrayStrc* pStatArray = pStatListEx ? &pStatListEx->FullStats : &pStatList->Stats;
	int nStatIndex = StatArray_FindInsertionIndex(pStatArray, D2SLayerStatIdStrc::MakeFromStatId(nStatId).nPackedValue, nullptr);
	if (nStatIndex < 0)
	{
		return 0;
	}

	int nCopiedStats = 0;
	while (nStatIndex < pStatArray->nStatCount
		&& pStatArray->pStat[nStatIndex].nStat == nStatId
		&& nCopiedStats < nBufferSize
		)
	{
		pBuffer[nCopiedStats].nPackedValue = pStatArray->pStat[nStatIndex].nPackedValue;
		pBuffer[nCopiedStats].nValue = pStatArray->pStat[nStatIndex].nValue;

		++nCopiedStats;
		++nStatIndex;
	}

	return nCopiedStats;
}

//D2Common.0x6FDB9D20 (#11270)
int __stdcall STATLIST_CopyStats(D2UnitStrc* pUnit, int nStatId, D2StatStrc* pBuffer, int nBufferSize)
{
	if (D2StatListExStrc* pStatListEx = STATLIST_StatListExCast(pUnit->pStatListEx))
	{
		return D2Common_CopyStats_6FDB9C50(pStatListEx, nStatId, pBuffer, nBufferSize);
	}
	return 0;
}

//D2Common.0x6FDB9D60 (#11273)
int __stdcall D2Common_11273(D2UnitStrc* pUnit, int nStatId)
{
	if (D2StatListExStrc* pStatListEx = STATLIST_StatListExCast(pUnit->pStatListEx))
	{
		return sub_6FDB5830(pStatListEx, D2SLayerStatIdStrc::MakeFromStatId(nStatId).nPackedValue);
	}
	return 0;
}


// Helper function
BOOL __stdcall D2Common_11274_11275_Impl(D2UnitStrc* pTarget, D2UnitStrc* pUnit, bool addOrSubstract)
{

	if (!pTarget || !pUnit || !pUnit->pStatListEx)
	{
		return FALSE;
	}

	if (pUnit->pStatListEx->pUnit != pTarget)
	{
		STATLIST_MergeStatLists(pTarget, pUnit, addOrSubstract);
		return FALSE;
	}

	if (!(pTarget->pStatListEx && STATLIST_IsExtended(pTarget->pStatListEx)))
	{
		return FALSE;
	}

	bool bDynamicBitChanged = BITMANIP_SetBitsValueForMask(pUnit->pStatListEx->dwFlags, STATLIST_DYNAMIC, !addOrSubstract);
	if (!bDynamicBitChanged)
	{
		return FALSE;
	}

	D2StatListExStrc* pUnitStatListEx = STATLIST_StatListExCast(pUnit->pStatListEx);
	D2StatsArrayStrc* pStatsArray = pUnitStatListEx ? &pUnitStatListEx->FullStats : &pUnit->pStatListEx->Stats;

	for (D2StatStrc* i = pStatsArray->pStat; i < &pStatsArray->pStat[pStatsArray->nStatCount]; ++i)
	{
		D2ItemStatCostTxt* pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(i->nStat);
		if (pItemStatCostTxtRecord && (pItemStatCostTxtRecord->dwItemStatFlags & gdwBitMasks[ITEMSTATCOSTFLAGINDEX_DAMAGERELATED]))
		{
			const int nValue = addOrSubstract ? i->nValue : -i->nValue;
			sub_6FDB6C10(pTarget->pStatListEx, i->nPackedValue, nValue, pUnit);
		}
	}

	return TRUE;
}

//D2Common.0x6FDB9D90 (#11274)
// 1.13c D2Common+0x3A100
// 1.13c D2Common+0x3a030 (subtract direction, addOrSubstract=false, via D2Common_11275)
BOOL __stdcall D2Common_11274(D2UnitStrc* pTarget, D2UnitStrc* pUnit)
{
	return D2Common_11274_11275_Impl(pTarget, pUnit, true);
}

//D2Common.0x6FDB9E60 (#11275)
BOOL __stdcall D2Common_11275(D2UnitStrc* pTarget, D2UnitStrc* pUnit)
{
	return D2Common_11274_11275_Impl(pTarget, pUnit, false);
}

// s3439 replay probe for 1.13c D2Common+0x38640. w = one D2VecStatArray38640 record (35 words);
// builds the array from ids0 by the function's own inserts, inserts the probe id, overwrites the output words.
#include <emscripten.h>
extern "C" EMSCRIPTEN_KEEPALIVE void d2_statarray_38640(uint32_t* w)
{
	D2StatsArrayStrc a = { nullptr, 0, 0 };
	auto ins = [&](uint32_t id) -> int32_t {
		bool found = false;
		const int idx = StatArray_FindInsertionIndex(&a, id, &found);
		if (found) return -1;
		return (int32_t)(StatArray_InsertStat(nullptr, &a, id, idx) - a.pStat);
	};
	for (uint32_t i = 0; i < w[0]; i++) ins(w[1 + i]);
	w[11] = a.nStatCount; w[12] = a.nCapacity;
	w[10] = (uint32_t)ins(w[9]);
	w[13] = a.nStatCount; w[14] = a.nCapacity;
	// The 4-byte mod-stat instantiation (same function, other element size): insert 2 then 1 (memmove path); a wrong order poisons the slot word.
	{
		D2ModStatsArrayStrc m = { nullptr, 0, 0 };
		StatArray_InsertStat(nullptr, &m, 2, 0);
		StatArray_InsertStat(nullptr, &m, 1, 0);
		if (m.nStatCount != 2 || m.pStat[0].nPackedValue != 1 || m.pStat[1].nPackedValue != 2) w[10] = 0xdeadbeefu;
	}
	for (int i = 0; i < 10; i++) { w[15 + i] = i < a.nStatCount ? a.pStat[i].nPackedValue : 0; w[25 + i] = i < a.nStatCount ? (uint32_t)a.pStat[i].nValue : 0; }
}
