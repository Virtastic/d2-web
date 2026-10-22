// D2Common implementation batch: twelve rows translated from the pinned 1.13c decompiles.
// The vendored D2MOO (1.10f) helpers are used by field name where their layout is the engine's;
// retail images whose pointers are four bytes wide are modelled with raw 32-bit offsets.
#include <cstddef>
#include <cstdint>

#include "D2BitManip.h"
#include "D2Collision.h"
#include "D2DataTbls.h"
#include "D2Skills.h"
#include "D2StatList.h"
#include "DataTbls/ItemsTbls.h"
#include "DataTbls/MonsterTbls.h"
#include "DataTbls/SkillsTbls.h"
#include "Units/UnitRoom.h"
#include "Units/Units.h"

// 1.13c D2Common+0x33C20 (already defined in sub_33c20.cpp; same translation unit set).
uint32_t D2Common_33C20_BitmaskHit(bool hasOwner, bool hasRec, int32_t recFlag, const uint32_t* bits, const uint32_t* mask, int32_t nWords);

namespace
{
	// The compat clock (src/core/d2moo/compat/Server.h QueryPerformanceCounter) is the zero counter.
	struct PerfCounter
	{
		uint32_t nLow;
		int32_t nHigh;
	};

	inline void ReadPerfCounter(PerfCounter* pCounter)
	{
		pCounter->nLow = 0;
		pCounter->nHigh = 0;
	}

	// MonStats2 flag bit of the monster row nMonsterId, bit nBit of its dwFlags/nFlags (+0x04);
	// the 1.13c body at 0x1470 is implemented in d2common_batch_10b0.cpp.
	inline uint32_t MonStats2FlagBit(int32_t nMonsterId, int32_t nBit)
	{
		if (nMonsterId < 0 || nMonsterId >= sgptDataTables->nMonStatsTxtRecordCount) return 0;
		const int32_t nMonStats2 = sgptDataTables->pMonStatsTxt[nMonsterId].wMonStatsEx;
		if (nMonStats2 < 0 || nMonStats2 >= sgptDataTables->nMonStats2TxtRecordCount) return 0;
		return sgptDataTables->pMonStats2Txt[nMonStats2].nFlags[nBit >> 3] & gdwBitMasks[nBit & 7];
	}

	// 1.13c D2Common+0x37DB0: state bit nBit of the unit's stat list (sign bit = extended list).
	inline uint32_t UnitStateBit(D2UnitStrc* pUnit, int32_t nBit)
	{
		D2StatListExStrc* pStatList = pUnit ? pUnit->pStatListEx : nullptr;
		if (!pStatList || (int32_t)pStatList->dwFlags >= 0 || !pStatList->StatFlags) return 0;
		return pStatList->StatFlags[nBit >> 5] & gdwBitMasks[nBit & 31];
	}

	// Retail's timing slots at DAT_6fde9ec8, 0x1E uint32 per slot. Sixty-four slots cover the
	// caller-supplied indices the shipped callers use; out of range is ignored.
	struct PerfSlot
	{
		uint32_t nLow;
		int32_t nHigh;
		uint32_t nCount;
		uint32_t nPad[0x1E - 3];
	};
	static_assert(sizeof(PerfSlot) == 0x1E * 4, "perf slot stride");
	PerfSlot gPerfSlots[64];
}

// 1.13c D2Common+0x52660 (#10008) skills.txt row's passive state, stat-list bits, then add skill
// Docs/re: SKILLS_AddSkill. The passive state and the skill-list walk are implemented; the two
// stat-list calls (0x665C0 room refresh, 0x52480 skill refresh) are the vendored equivalents.
D2SkillStrc* D2Common_52660_AddSkill(D2UnitStrc* pUnit, int32_t nSkillId)
{
	if (!pUnit || nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount) return nullptr;

	D2SkillsTxt* pSkillsTxt = &sgptDataTables->pSkillsTxt[nSkillId];
	if (!pSkillsTxt) return nullptr;

	int32_t nPassiveState = 0;
	const int16_t nState = pSkillsTxt->nPassiveState;

	if (nState > 0 && nState < sgptDataTables->nStatesTxtRecordCount)
	{
		nPassiveState = nState;

		D2StatListExStrc* pStatList = pUnit->pStatListEx;
		if (pStatList && (int32_t)pStatList->dwFlags < 0 && pStatList->StatFlags)
		{
			const uint32_t nWord = (uint32_t)nState >> 5;
			const uint32_t nMask = gdwBitMasks[nState & 31];
			const uint32_t nOld = pStatList->StatFlags[nWord];

			pStatList->StatFlags[nWord] = nOld | nMask;

			if ((nOld & nMask) == 0)
			{
				const int32_t nWords = (sgptDataTables->nStatesTxtRecordCount + 31) / 32;
				pStatList->StatFlags[nWord + nWords] |= nMask;

				if (sgptDataTables->pStatesTxt[nState].nStateFlags[2] & 1)
				{
					pUnit->dwFlags |= 8;
				}
			}
		}

		UNITROOM_RefreshUnit(pUnit);
	}

	D2SkillStrc* pSkill = pUnit->pSkills ? pUnit->pSkills->pFirstSkill : nullptr;

	for (; pSkill; pSkill = pSkill->pNextSkill)
	{
		if (pSkill->pSkillsTxt == pSkillsTxt && pSkill->nOwnerGUID == (D2UnitGUID)-1)
		{
			int32_t nMaxLevel = pSkillsTxt->wMaxLvl;
			if (nMaxLevel < 1) nMaxLevel = 20;

			if (pSkill->nSkillLevel < nMaxLevel || pUnit->dwUnitType == UNIT_MONSTER)
			{
				++pSkill->nSkillLevel;
			}

			SKILLS_RefreshSkill(pUnit, nSkillId);
			return pSkill;
		}
	}

	if (!pUnit->pSkills) return nullptr;

	D2SkillStrc* pNewSkill = D2_CALLOC_STRC_POOL(pUnit->pSkills->pMemPool, D2SkillStrc);
	pNewSkill->pSkillsTxt = pSkillsTxt;

	if (pSkillsTxt->nSkillId == 1)
	{
		pNewSkill->dwSkillMode = pSkillsTxt->nMonAnim;
	}
	else if (pSkillsTxt->nSkillId == 0 && pSkillsTxt->dwFlags[0] == 6 && nPassiveState == 5)
	{
		pNewSkill->dwSkillMode = 16;
	}
	else
	{
		pNewSkill->dwSkillMode = pSkillsTxt->nAnim;
	}

	pNewSkill->nSkillLevel = 1;
	pNewSkill->nOwnerGUID = (D2UnitGUID)-1;
	pNewSkill->pNextSkill = nullptr;

	D2SkillStrc* pTail = pUnit->pSkills->pFirstSkill;
	if (pTail)
	{
		while (pTail->pNextSkill) pTail = pTail->pNextSkill;
		pTail->pNextSkill = pNewSkill;
	}
	else
	{
		pUnit->pSkills->pFirstSkill = pNewSkill;
	}

	SKILLS_RefreshSkill(pUnit, nSkillId);
	return pNewSkill;
}

// 1.13c D2Common+0x22700 (#10016) items.txt row bit 2 of dwBitField1 (+0xDC). Out of range is
// fatal in retail (_exit); we return 0.
uint32_t D2Common_22700_ItemBitField1Bit2(uint32_t nItemId)
{
	const D2ItemDataTbl* pTable = DATATBLS_GetItemDataTables();
	if (pTable && nItemId < (uint32_t)pTable->nItemsTxtRecordCount)
	{
		const D2ItemsTxt* pItem = DATATBLS_GetItemsTxtRecord((int32_t)nItemId);
		if (pItem) return pItem->dwBitField1 & 4;
	}
	return 0;
}

// 1.13c D2Common+0x22AD0 (#10017) items.txt dwMinStack (+0xE4). Out of range is fatal in retail
// (_exit); we return 0.
int32_t D2Common_22AD0_ItemMinStack(uint32_t nItemId)
{
	const D2ItemDataTbl* pTable = DATATBLS_GetItemDataTables();
	if (pTable && nItemId < (uint32_t)pTable->nItemsTxtRecordCount)
	{
		const D2ItemsTxt* pItem = DATATBLS_GetItemsTxtRecord((int32_t)nItemId);
		if (pItem) return (int32_t)pItem->dwMinStack;
	}
	return 0;
}

// 1.13c D2Common+0x1D7C0 (#10018) path/missile-stream rebuild. Implemented: the path field reset
// and the 0xB/8 fatal modes. Not implemented: the missile stream array build (unit type 3,
// pData+0x30), the per-point 0x5D180 direction call, and the param_5/param_6 callback loops;
// 0x36230 PathStep is not called (its Path32 type is private to sub_36230.cpp).
void D2Common_1D7C0_ResetPath(int32_t* pUnit32, int32_t nFrameCount, int32_t nFrameStep,
	int32_t (*pfnSegment)(uint32_t, uint32_t, int32_t, int32_t, int32_t), void (*pfnTail)())
{
	int32_t* pPath = (int32_t*)(uintptr_t)(uint32_t)pUnit32[0xB];

	pPath[0xD] &= (int32_t)0xFFF800FF; // +0x34
	pPath[0xF] = 0x10;                 // +0x3C
	pPath[0x26] = 0;                   // +0x98

	if (pPath[0x10] == 0xB) return;    // retail fatal 0x871
	if (pPath[0x10] == 8) return;      // retail fatal 0x872

	const int32_t nPathPoints = pPath[0xA]; // +0x28
	if (nPathPoints == 0 || nPathPoints == 1) return;

	(void)nFrameCount;
	(void)nFrameStep;
	(void)pfnSegment;
	(void)pfnTail;
	return;
}

// 1.13c D2Common+0x4A370 (#10021) performance accumulator: per-slot low/high clock delta and
// call count. this = slot index, param_1 = previous low, param_2 = previous high.
void D2Common_4A370_PerfAccumulate(int32_t nSlot, uint32_t nPreviousLow, int32_t nPreviousHigh)
{
	if (nSlot < 0 || nSlot >= 64) return;

	PerfCounter counter;
	ReadPerfCounter(&counter);

	PerfSlot& slot = gPerfSlots[nSlot];
	const uint32_t nDelta = counter.nLow - nPreviousLow;
	const uint32_t nOldLow = slot.nLow;

	slot.nLow = nOldLow + nDelta;
	slot.nHigh = slot.nHigh + (counter.nHigh - nPreviousHigh)
		- (counter.nLow < nPreviousLow ? 1 : 0)
		+ (nOldLow + nDelta < nOldLow ? 1 : 0);
	++slot.nCount;
}

// 1.13c D2Common+0x229D0 (#10026) items.txt nQuantityWarning (+0x136) of item units. Any other
// unit type or a bad class id is fatal in retail (_exit); we return 0.
uint8_t D2Common_229D0_ItemQuantityWarning(D2UnitStrc* pUnit)
{
	if (!pUnit || pUnit->dwUnitType != UNIT_ITEM) return 0;

	const D2ItemsTxt* pItem = DATATBLS_GetItemsTxtRecord(pUnit->dwClassId);
	return pItem ? pItem->nQuantityWarning : 0;
}

// 1.13c D2Common+0x50020 (#10028) 1 when the monster (anim mode 12) is in none of state-mask 33
// and has MonStats2 bits 7 and 12 set. The decompiler loses the class-id register input of the
// two 0x1470 calls; the only sensible source is the unit's own class id.
uint32_t D2Common_50020_MonStats2Flags(D2UnitStrc* pUnit)
{
	if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER || pUnit->dwAnimMode != 0xC) return 0;

	D2StatListExStrc* pStatList = pUnit->pStatListEx;
	const bool bHasRec = pStatList != nullptr;
	const int32_t nRecFlag = pStatList ? (int32_t)pStatList->dwFlags : 0;
	const uint32_t* pBits = pStatList ? pStatList->StatFlags : nullptr;
	const int32_t nWords = (sgptDataTables->nStatesTxtRecordCount + 31) / 32;

	if (D2Common_33C20_BitmaskHit(pUnit != nullptr, bHasRec, nRecFlag, pBits, sgptDataTables->fStateMasks[33], nWords) != 0)
	{
		return 0;
	}

	if (MonStats2FlagBit(pUnit->dwClassId, 7) == 0) return 0;
	if (MonStats2FlagBit(pUnit->dwClassId, 0xC) == 0) return 0;

	return 1;
}

// 1.13c D2Common+0x4F610 (#10029) skill setter wrapper. 0x4EE00 is the 1.13c equivalent of the
// vendored D2Common_10954(pUnit, nOwnerGUID, nSkillId, nSkillLevel, nCharges, bFreeMemory); the
// skill id travels in EAX (param_3), so the stack args map to owner/level/charges/free.
// partial: 0x4EE00 also reads EBX and its return-address slot, which the wrapper cannot recover;
// the vendored 1.10f D2Common_10954 stands in for it.
void D2Common_4F610_SetSkill(D2UnitStrc* pUnit, D2UnitGUID nOwnerGUID, uint32_t nSkillId,
	int32_t nSkillLevel, int32_t nCharges, BOOL bFreeMemory)
{
	D2Common_10954(pUnit, nOwnerGUID, (int32_t)nSkillId, nSkillLevel, nCharges, bFreeMemory);
}

// 1.13c D2Common+0x4EC50 (#10030) skill sequence number: a player's skills.txt nSeqNum (+0x13);
// otherwise MonStats nSequence[8] (+0x188) at the index whose nSkill[8] (+0x170) equals the skill
// id, with skill id 199 excluded. The class id register input is the unit's own class id.
uint32_t D2Common_4EC50_SkillSequence(D2UnitStrc* pUnit, D2SkillStrc* pSkill)
{
	if (!pUnit || !pSkill || !pSkill->pSkillsTxt) return 0;

	const D2SkillsTxt* pSkillsTxt = pSkill->pSkillsTxt;

	if (pUnit->dwUnitType == UNIT_PLAYER)
	{
		return pSkillsTxt->nSeqNum;
	}

	const int16_t nSkillId = pSkillsTxt->nSkillId;
	if (nSkillId == 199) return 0;

	const D2MonStatsTxt* pMonStats = DATATBLS_GetMonStatsTxtRecord(pUnit->dwClassId);
	if (!pMonStats) return 0;

	for (int32_t i = 0; i < 8; ++i)
	{
		if (pMonStats->nSkill[i] == nSkillId)
		{
			return (uint32_t)(int32_t)(int16_t)pMonStats->nSequence[i];
		}
	}

	return 0;
}

// 1.13c D2Common+0x31850 (#10032) set the collision mask with the unit's own size and mask; a
// null unit is fatal in retail (_exit), here a no-op.
void D2Common_31850_SetCollisionWithSize(D2UnitStrc* pUnit, D2ActiveRoomStrc* pRoom, int32_t nX, int32_t nY)
{
	if (!pUnit) return;

	const int nSizeX = UNITS_GetUnitSizeX(pUnit);
	const int nSizeY = UNITS_GetUnitSizeY(pUnit);

	if (nSizeX != 0 && nSizeY != 0)
	{
		const int nMask = UNITS_GetCollisionMask(pUnit);
		COLLISION_SetMaskWithSizeXY(pRoom, nX, nY, (unsigned int)nSizeX, (unsigned int)nSizeY, (uint16_t)nMask);
	}
}

static int32_t StatArray_Find25(const D2StatsArrayStrc* a)
{
	int32_t lo = 0, hi = (int16_t)a->nStatCount;
	while (lo < hi)
	{
		const int32_t mid = lo + (hi - lo) / 2;
		const int32_t id = *(const int32_t*)&a->pStat[mid];
		if (id < 25) lo = mid + 1;
		else if (id > 25) hi = mid;
		else return mid;
	}
	return -1;
}

// 1.13c D2Common+0x4FE80 (#10037) skill stat value: guards, state 0x2A membership, the
// itemstatcost record 25 value scaled by skills.txt dwParam[0] and divided by 8.
// 0x38B00 (stat 25 value with the FMIN floor, register args ebx=stat list, edi=25) is inlined.
int32_t D2Common_4FE80_GetSkillStatValue(D2UnitStrc* pUnit, int32_t nSkillId)
{
	if (!pUnit) return 0;

	const uint32_t nUnitType = pUnit->dwUnitType;
	if (nUnitType >= 2 && nUnitType != 3) return 0;

	if (sgptDataTables->nStatesTxtRecordCount <= 0x2A) return 0;
	if (UnitStateBit(pUnit, 0x2A) == 0) return 0;

	D2StatListExStrc* pStatList = pUnit->pStatListEx;
	if (!pStatList || (int32_t)pStatList->dwFlags >= 0) return 0;

	D2StatListStrc* pStateList = nullptr;

	for (D2StatListStrc* p = pStatList->pMyLastList; p; p = p->pPrevLink)
	{
		if (p->dwStateNo == 0x2A) { pStateList = p; break; }
	}

	if (!pStateList)
	{
		for (D2StatListStrc* p = pStatList->pMyStats; p; p = p->pPrevLink)
		{
			if (p->dwStateNo == 0x2A) { pStateList = p; break; }
		}
	}

	if (!pStateList) return 0;

	int32_t nValue = 0;
	D2ItemStatCostTxt* pRec = (sgptDataTables->nItemStatCostTxtRecordCount < 0x1A) ? nullptr : &sgptDataTables->pItemStatCostTxt[25];
	if (pRec)
	{
		const int nIdx = pStatList->Stats.nStatCount ? StatArray_Find25(&pStatList->Stats) : -1;
		if (nIdx >= 0)
		{
			nValue = pStatList->Stats.pStat[nIdx].nValue;
			D2UnitStrc* pOwner = pStatList->pOwner;
			if ((pRec->dwItemStatFlags & gdwBitMasks[8]) && (int32_t)pStatList->dwFlags < 0 && pOwner
				&& (pOwner->dwUnitType == 0 || pOwner->dwUnitType == 1) && nValue < pRec->dwMinAccr)
				nValue = pRec->dwMinAccr << (pRec->nValShift & 0x1F);
		}
	}

	if (nSkillId >= 0 && nSkillId < sgptDataTables->nSkillsTxtRecordCount)
	{
		nValue *= sgptDataTables->pSkillsTxt[nSkillId].dwParam[0];
		return (nValue + ((nValue >> 31) & 7)) >> 3;
	}

	return 0;
}

// 1.13c D2Common+0x3C810 (#10043) compare the stat-flag pointers reached from two retail 32-bit
// images and adjust a counter by -1/+1 when they differ.
extern "C" void d2_fn_d2common_2df80(uint32_t pContext, int32_t nDelta);
// pContext is retail's ESI, passed through untouched to 0x2DF80.
void D2Common_3C810_CompareStatFlags(const uint8_t* pFirst, const uint8_t* pSecond, uint32_t pContext)
{
	const uint32_t nRecFirst = pFirst ? *(const uint32_t*)(pFirst + 0x10) : 0;
	const uint32_t nRecSecond = pSecond ? *(const uint32_t*)(pSecond + 0x10) : 0;

	const uint32_t nFlagsFirst = nRecFirst ? *(const uint32_t*)(uintptr_t)(nRecFirst + 0x58) : 0;
	const uint32_t nFlagsSecond = nRecSecond ? *(const uint32_t*)(uintptr_t)(nRecSecond + 0x58) : 0;

	if (nFlagsFirst != nFlagsSecond)
	{
		if (nFlagsFirst) d2_fn_d2common_2df80(pContext, -1);
		if (nFlagsSecond) d2_fn_d2common_2df80(pContext, 1);
	}
}
