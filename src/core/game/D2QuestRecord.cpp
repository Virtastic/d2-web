#include "D2QuestRecord.h"
#include <test_seam.h>

#include "D2BitManip.h"


// 1.13c D2Common+0x2D0F0
//D2Common.0x6FDAE800 (#11107)
BOOL __stdcall QUESTRECORD_GetQuestState(D2BitBufferStrc* pQuestRecord, int nQuest, int nState)
{
	if (D2Seam<&QUESTRECORD_GetQuestState>::hook) return D2Seam<&QUESTRECORD_GetQuestState>::hook(pQuestRecord, nQuest, nState);
	D2_ASSERT(pQuestRecord);

	return BITMANIP_GetBitState(pQuestRecord->pBuffer, nState + 8 * sizeof(uint16_t) * nQuest) != 0;
}

// 1.13c D2Common+0x2D0B0
//D2Common.0x6FDAE850 (#11108)
void __stdcall QUESTRECORD_SetQuestState(D2BitBufferStrc* pQuestRecord, int nQuest, int nState)
{
	if (D2Seam<&QUESTRECORD_SetQuestState>::hook) { D2Seam<&QUESTRECORD_SetQuestState>::hook(pQuestRecord, nQuest, nState); return; }
	D2_ASSERT(pQuestRecord);

	BITMANIP_SetBitState(pQuestRecord->pBuffer, nState + 8 * sizeof(uint16_t) * nQuest);
}

// 1.13c D2Common+0x2D070
//D2Common.0x6FDAE890 (#11109)
void __stdcall QUESTRECORD_ClearQuestState(D2BitBufferStrc* pQuestRecord, int nQuest, int nState)
{
	if (D2Seam<&QUESTRECORD_ClearQuestState>::hook) { D2Seam<&QUESTRECORD_ClearQuestState>::hook(pQuestRecord, nQuest, nState); return; }
	D2_ASSERT(pQuestRecord);

	BITMANIP_MaskBitstate(pQuestRecord->pBuffer, nState + 8 * sizeof(uint16_t) * nQuest);
}

//D2Common.0x6FDAE8D0 (#11110)
// 1.13c D2Common+0x2D020
void __stdcall QUESTRECORD_ResetIntermediateStateFlags(D2BitBufferStrc* pQuestRecord, int nQuest)
{
	if (D2Seam<&QUESTRECORD_ResetIntermediateStateFlags>::hook) { D2Seam<&QUESTRECORD_ResetIntermediateStateFlags>::hook(pQuestRecord, nQuest); return; }
	D2_ASSERT(pQuestRecord);

	for (int i = QFLAG_STARTED; i <= QFLAG_CUSTOM7; ++i)
	{
		BITMANIP_MaskBitstate(pQuestRecord->pBuffer, 8 * sizeof(uint16_t) * nQuest + i);
	}
}

//D2Common.0x6FDAE920 (#11111)
// 1.13c D2Common+0x2CFC0
D2BitBufferStrc* __stdcall QUESTRECORD_AllocRecord(void* pMemPool)
{
	D2BitBufferStrc* pBitBuffer = D2_ALLOC_STRC_POOL(pMemPool, D2BitBufferStrc);
	uint8_t* pBuffer = (uint8_t*)D2_CALLOC_POOL(pMemPool, sizeof(uint16_t) * NUM_QUEST_WORDS);

	BITMANIP_Initialize(pBitBuffer, pBuffer, sizeof(uint16_t) * NUM_QUEST_WORDS);

	return pBitBuffer;
}

//D2Common.0x6FDAE970 (#11112)
// 1.13c D2Common+0x2CF80
void __stdcall QUESTRECORD_FreeRecord(void* pMemPool, D2BitBufferStrc* pQuestRecord)
{
	if (pQuestRecord)
	{
		D2_FREE_POOL(pMemPool, pQuestRecord->pBuffer);
		D2_FREE_POOL(pMemPool, pQuestRecord);
	}
}

// 1.13c D2Common+0x2D140
//D2Common.0x6FDAE9B0 (#11113)
void __stdcall QUESTRECORD_CopyBufferToRecord(D2BitBufferStrc* pQuestRecord, uint8_t* pBuffer, uint16_t nBufferSize, BOOL bResetStates)
{
	D2_ASSERT(nBufferSize == (sizeof(uint16_t) * NUM_QUEST_WORDS));

	memcpy(pQuestRecord->pBuffer, pBuffer, sizeof(uint16_t) * NUM_QUEST_WORDS);

	if (bResetStates)
	{
		// 1.13c: the loop runs for bit bases 14, 30, ... while below 0x2AE, 42 quest rows (1.10f stopped at 41).
		for (int i = 0; i < 42; ++i)
		{
			BITMANIP_MaskBitstate(pQuestRecord->pBuffer, 8 * sizeof(uint16_t) * i + QFLAG_PRIMARYGOALDONE);
			BITMANIP_MaskBitstate(pQuestRecord->pBuffer, 8 * sizeof(uint16_t) * i + QFLAG_COMPLETEDNOW);

			if (BITMANIP_GetBitState(pQuestRecord->pBuffer, 8 * sizeof(uint16_t) * i + QFLAG_REWARDPENDING))
			{
				BITMANIP_SetBitState(pQuestRecord->pBuffer, 8 * sizeof(uint16_t) * i + QFLAG_COMPLETEDBEFORE);
			}
		}
	}
}

//D2Common.0x6FDAEA40 (#11114)
// 1.13c D2Common+0x2CF40
void __stdcall QUESTRECORD_CopyRecordToBuffer(D2BitBufferStrc* pQuestRecord, uint8_t* pBuffer, uint16_t nBufferSize, int nUnused)
{
	D2_MAYBE_UNUSED(nUnused);
	D2_ASSERT(nBufferSize == (sizeof(uint16_t) * NUM_QUEST_WORDS));

	memcpy(pBuffer, pQuestRecord->pBuffer, sizeof(uint16_t) * NUM_QUEST_WORDS);
}
