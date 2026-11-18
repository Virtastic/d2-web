#include "oc20g17.h"

#include <test_seam.h>
#include <D2QuestRecord.h>
#include "GAME/Clients.h"

// 1.13c D2Game+0xE34D0 (EAX = quest record, ECX = unit, stack = game; ret 4)
// Act 3 boss quest completion: sets quest 0x16 states 0xD, 0 and 0xB, then raises the client's saved progression
// (bits 8-12 of the save flags word) to 3 + difficulty * (expansion ? 5 : 4) when it is lower.
void __fastcall sub_E34D0(D2BitBufferStrc* pQuestRecord, D2UnitStrc* pUnit, D2GameStrc* pGame)
{
	D2_CALL(QUESTRECORD_SetQuestState, pQuestRecord, 0x16, 0xD);
	D2_CALL(QUESTRECORD_SetQuestState, pQuestRecord, 0x16, 0);
	D2_CALL(QUESTRECORD_SetQuestState, pQuestRecord, 0x16, 0xB);

	D2ClientStrc* pClient = nullptr;
	if (pUnit && pUnit->dwUnitType == UNIT_PLAYER)
	{
		pClient = D2_CALL(UNITS_GetPlayerData, pUnit)->pClient;
	}

	const uint16_t nFlags = pClient->tSaveFlags.nPackedValue;
	const uint32_t nPerDifficulty = ((nFlags & 0x20) | 0x80) >> 5;
	const uint32_t nValue = nPerDifficulty * pGame->nDifficulty + 3;
	if (((nFlags & 0x1F00) >> 8) <= nValue)
	{
		pClient->tSaveFlags.nPackedValue = (uint16_t)((nValue * 0x100) | (nFlags & 0xE0FF));
	}
}
