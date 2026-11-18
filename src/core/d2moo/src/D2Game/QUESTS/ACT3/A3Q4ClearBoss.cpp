#include "QUESTS/ACT3/A3Q4.h"
#include "QUESTS/Quests.h"
#include <Units/Units.h>

// 1.13c D2Game+0xE3B00
// EAX = the Golden Bird quest data, ECX = unit (or null, the -1 GUID). When the boss-spawn latch (unk0x02) is 1 and the stored boss GUID is the unit's, the latch is dropped (unk0x02 = 0) and unk0x01 is set.
void __fastcall ACT3Q4_ClearBossSpawnLatch(D2QuestDataStrc* pQuestData, D2UnitStrc* pUnit)
{
	D2Act3Quest4Strc* pQuestDataEx = (D2Act3Quest4Strc*)pQuestData->pQuestDataEx;
	if (pQuestDataEx->unk0x02 != 1)
	{
		return;
	}

	int32_t nUnitGUID = -1;
	if (pUnit)
	{
		nUnitGUID = pUnit->dwUnitId;
	}

	if (pQuestDataEx->nBossGUID == nUnitGUID)
	{
		pQuestDataEx->unk0x02 = 0;
		pQuestDataEx->unk0x01 = 1;
	}
}
