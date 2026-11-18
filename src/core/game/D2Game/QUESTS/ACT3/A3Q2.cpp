#include "QUESTS/ACT3/A3Q2.h"
#include <test_seam.h>

#include <DataTbls/LevelsIds.h>
#include <DataTbls/MonsterIds.h>
#include <DataTbls/ObjectsIds.h>
#include <DataTbls/ObjectsTbls.h>
#include <D2Items.h>
#include <D2QuestRecord.h>

#include "GAME/Game.h"
#include "GAME/SCmd.h"
#include "ITEMS/Items.h"
#include "OBJECTS/Objects.h"
#include "OBJECTS/ObjMode.h"
#include "QUESTS/Quests.h"
#include "QUESTS/ACT2/A2Q6.h"
#include "QUESTS/ACT3/A3Q5.h"
#include "UNIT/SUnit.h"


//D2Game.0x6FD382E8
D2NPCMessageTableStrc gpAct3Q2NpcMessages[] =
{
	{
		{
			{ MONSTER_CAIN3, 543, 0, 1 },
		},
		1
	},
	{
		{
			{ MONSTER_CAIN3, 545, 0, 1 },
		},
		1
	},
	{
		{
			{ MONSTER_CAIN3, 544, 0, 1 },
		},
		1
	},
	{
		{
			{ MONSTER_CAIN3, 546, 0, 1 },
		},
		1
	},
	{
		{
			{ MONSTER_CAIN3, 547, 0, 1 },
		},
		1
	},
	{
		{
			{ MONSTER_CAIN3, 548, 0, 1 },
		},
		1
	},
	{
		{
			{ MONSTER_CAIN3, 543, 0, 2 },
		},
		1
	},
	{
		{
			{ MONSTER_CAIN3, 545, 0, 2 },
		},
		1
	},
	{
		{
			{ MONSTER_CAIN3, 544, 0, 2 },
		},
		1
	},
	{
		{
			{ MONSTER_CAIN3, 546, 0, 2 },
		},
		1
	},
	{
		{
			{ MONSTER_CAIN3, 547, 0, 2 },
		},
		1
	},
	{
		{
			{ MONSTER_CAIN3, 548, 0, 2 },
		},
		1
	},
	{
		{
			{ -1, 0, 0, 2 },
		},
		0
	}
};


//D2Game.0x6FCA7B70
// 1.13c D2Game+0xE4F60 (checked line by line against the decompile: same item/state order and results)
bool __fastcall ACT3Q2_ActiveFilterCallback(D2QuestDataStrc* pQuest, int32_t nNpcId, D2UnitStrc* pPlayer, D2BitBufferStrc* pQuestFlags, D2UnitStrc* pNPC)
{
	if (nNpcId != MONSTER_CAIN3)
	{
		return false;
	}

	if (pQuest->fState >= 1u && !QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_STARTED))
	{
		return true;
	}

	int32_t nFlailPartsAvailable = 0;
	if (ITEMS_FindQuestItem(pQuest->pGame, pPlayer, ' 1fq'))
	{
		++nFlailPartsAvailable;
		if (!QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_CUSTOM1))
		{
			return true;
		}
	}

	if (ITEMS_FindQuestItem(pQuest->pGame, pPlayer, ' yeq'))
	{
		++nFlailPartsAvailable;
		if (!QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_LEAVETOWN))
		{
			return true;
		}
	}

	if (ITEMS_FindQuestItem(pQuest->pGame, pPlayer, ' rhq'))
	{
		++nFlailPartsAvailable;
		if (!QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_CUSTOM2))
		{
			return true;
		}
	}

	if (ITEMS_FindQuestItem(pQuest->pGame, pPlayer, ' rbq'))
	{
		++nFlailPartsAvailable;
		if (!QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_ENTERAREA))
		{
			return true;
		}
	}

	if (ITEMS_FindQuestItem(pQuest->pGame, pPlayer, ' 2fq') && !QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_CUSTOM3))
	{
		D2QuestDataStrc* pQuestData19 = QUESTS_GetQuestData(pQuest->pGame, QUEST_A3Q5_TRAVINCAL);
		if (!pQuestData19 || !((D2Act3Quest5Strc*)pQuestData19->pQuestDataEx)->bOrbSmashed)
		{
			return true;
		}
	}

	if (nFlailPartsAvailable == 4 && !QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_CUSTOM1))
	{
		return true;
	}

	return false;
}

// 1.13c D2Game+0xE2EF0 (retail installs a bare ret as the player-leaves callback)
static void __fastcall ACT3Q2_Callback10_PlayerLeavesGame(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
}

//D2Game.0x6FCA7CD0
// 1.13c D2Game+0xE7CE0
void __fastcall ACT3Q2_InitQuestData(D2QuestDataStrc* pQuestData)
{
	memset(pQuestData->pfCallback, 0x00, sizeof(pQuestData->pfCallback));
	pQuestData->pNPCMessages = gpAct3Q2NpcMessages;
	pQuestData->bActive = 1;
	pQuestData->fLastState = 1;
	pQuestData->fState = 0;
	pQuestData->nQuestFilter = QUESTSTATEFLAG_A3Q2;
	pQuestData->nInitNo = 0;
	pQuestData->pfStatusFilter = ACT3Q2_StatusFilterCallback;
	pQuestData->pfActiveFilter = ACT3Q2_ActiveFilterCallback;
	pQuestData->pfSeqFilter = ACT3Q2_SeqCallback;
	pQuestData->nSeqId = 15;
	pQuestData->pfCallback[QUESTEVENT_NPCACTIVATE] = ACT3Q2_Callback00_NpcActivate;
	pQuestData->pfCallback[QUESTEVENT_SCROLLMESSAGE] = ACT3Q2_Callback11_ScrollMessage;
	pQuestData->pfCallback[QUESTEVENT_CHANGEDLEVEL] = ACT3Q2_Callback03_ChangedLevel;
	pQuestData->pfCallback[QUESTEVENT_PLAYERSTARTEDGAME] = ACT3Q2_Callback13_PlayerStartedGame;
	pQuestData->pfCallback[QUESTEVENT_PLAYERLEAVESGAME] = ACT3Q2_Callback10_PlayerLeavesGame;
	pQuestData->pfCallback[QUESTEVENT_ITEMPICKEDUP] = ACT3Q2_Callback04_ItemPickedUp;

	D2Act3Quest2Strc* pQuestDataEx = D2_ALLOC_STRC_POOL(pQuestData->pGame->pMemoryPool, D2Act3Quest2Strc);
	memset(pQuestDataEx, 0x00, sizeof(D2Act3Quest2Strc));
	pQuestData->pQuestDataEx = pQuestDataEx;
	pQuestDataEx->nSewerStairsObjectMode = 0;
}

//D2Game.0x6FCA7DA0
// 1.13c D2Game+0xE4C90 (chain indices and item/state order checked against the disassembly)
void __fastcall ACT3Q2_Callback00_NpcActivate(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pQuestArg->pPlayer)->pQuestData[pQuestArg->pGame->nDifficulty];

	if (!pQuestArg->pTarget || pQuestArg->pTarget->dwClassId != MONSTER_CAIN3)
	{
		return;
	}

	if (pQuestData->fState >= 1 && !QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_STARTED))
	{
		QUESTS_InitScrollTextChain(pQuestData, pQuestArg->pTextControl, MONSTER_CAIN3, 0);
		return;
	}

	bool bCubedFlailAvailable = false;
	if (ITEMS_FindQuestItem(pQuestData->pGame, pQuestArg->pPlayer, ' 2fq'))
	{
		bCubedFlailAvailable = true;
		if (!QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_CUSTOM3))
		{
			D2QuestDataStrc* pQuestData19 = QUESTS_GetQuestData(pQuestData->pGame, QUEST_A3Q5_TRAVINCAL);
			if (!pQuestData19 || !((D2Act3Quest5Strc*)pQuestData19->pQuestDataEx)->bOrbSmashed)
			{
				QUESTS_InitScrollTextChain(pQuestData, pQuestArg->pTextControl, MONSTER_CAIN3, 5);
				return;
			}
		}
	}

	int32_t nFlailPartsAvailable = 0;
	bool bBaseFlailAvailable = false;
	if (ITEMS_FindQuestItem(pQuestData->pGame, pQuestArg->pPlayer, ' 1fq'))
	{
		++nFlailPartsAvailable;

		bBaseFlailAvailable = true;
		if (!QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_CUSTOM1))
		{
			QUESTS_InitScrollTextChain(pQuestData, pQuestArg->pTextControl, MONSTER_CAIN3, 4);
			return;
		}
	}

	bool bEyeAvailable = false;
	if (ITEMS_FindQuestItem(pQuestData->pGame, pQuestArg->pPlayer, ' yeq'))
	{
		++nFlailPartsAvailable;

		bEyeAvailable = true;
		if (!QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_LEAVETOWN))
		{
			QUESTS_InitScrollTextChain(pQuestData, pQuestArg->pTextControl, MONSTER_CAIN3, 1);
			return;
		}
	}

	bool bHeartAvailable = false;
	if (ITEMS_FindQuestItem(pQuestData->pGame, pQuestArg->pPlayer, ' rhq'))
	{
		++nFlailPartsAvailable;

		bHeartAvailable = true;
		if (!QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_CUSTOM2))
		{
			QUESTS_InitScrollTextChain(pQuestData, pQuestArg->pTextControl, MONSTER_CAIN3, 2);
			return;
		}
	}

	bool bBrainAvailable = false;
	if (ITEMS_FindQuestItem(pQuestData->pGame, pQuestArg->pPlayer, ' rbq'))
	{
		++nFlailPartsAvailable;

		bBrainAvailable = true;
		if (!QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_ENTERAREA))
		{
			QUESTS_InitScrollTextChain(pQuestData, pQuestArg->pTextControl, MONSTER_CAIN3, 3);
			return;
		}
	}

	if (nFlailPartsAvailable <= 0)
	{
		if (bCubedFlailAvailable)
		{
			QUESTS_InitScrollTextChain(pQuestData, pQuestArg->pTextControl, MONSTER_CAIN3, 11);
		}
		else if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_STARTED))
		{
			QUESTS_InitScrollTextChain(pQuestData, pQuestArg->pTextControl, MONSTER_CAIN3, 6);
		}
		return;
	}

	if (bBaseFlailAvailable)
	{
		QUESTS_InitScrollTextChain(pQuestData, pQuestArg->pTextControl, MONSTER_CAIN3, 10);
		return;
	}

	if (bHeartAvailable)
	{
		QUESTS_InitScrollTextChain(pQuestData, pQuestArg->pTextControl, MONSTER_CAIN3, 8);
		return;
	}

	if (bEyeAvailable)
	{
		QUESTS_InitScrollTextChain(pQuestData, pQuestArg->pTextControl, MONSTER_CAIN3, 7);
		return;
	}

	if (bBrainAvailable)
	{
		QUESTS_InitScrollTextChain(pQuestData, pQuestArg->pTextControl, MONSTER_CAIN3, 9);
	}
	else
	{
		QUESTS_InitScrollTextChain(pQuestData, pQuestArg->pTextControl, MONSTER_CAIN3, 6);
	}
}

//D2Game.0x6FCA7FB0
// 1.13c D2Game+0xE7A10
void __fastcall ACT3Q2_Callback03_ChangedLevel(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	if (pQuestArg->nNewLevel == LEVEL_GREATMARSH && pQuestData->bNotIntro && pQuestData->fState < 3)
	{
		D2_CALL(QUESTS_StateDebug, pQuestData, 1, __FILE__, __LINE__);
	}
}

//D2Game.0x6FCA7FE0
// 1.13c D2Game+0xE4AB0 (status results checked against the decompile: 5/7 for four parts, 1/2/4/3+4b otherwise, 12/6 for the cubed flail)
bool __fastcall ACT3Q2_StatusFilterCallback(D2QuestDataStrc* pQuest, D2UnitStrc* pPlayer, D2BitBufferStrc* pGlobalFlags, D2BitBufferStrc* pFlags, uint8_t* pStatus)
{
	*pStatus = 0;

	if (!QUESTRECORD_GetQuestState(pFlags, QUESTSTATEFLAG_A2COMPLETED, QFLAG_REWARDGRANTED))
	{
		return true;
	}

	if (QUESTRECORD_GetQuestState(pFlags, QUESTSTATEFLAG_A3Q2, QFLAG_STARTED) || pQuest->bNotIntro && pQuest->fState >= 2)
	{
		*pStatus = 1;
	}

	if (ITEMS_FindQuestItem(pQuest->pGame, pPlayer, ' 2fq'))
	{
		D2QuestDataStrc* pQuestData19 = QUESTS_GetQuestData(pQuest->pGame, QUEST_A3Q5_TRAVINCAL);
		if (pQuestData19 && ((D2Act3Quest5Strc*)pQuestData19->pQuestDataEx)->bOrbSmashed)
		{
			*pStatus = 12;
		}
		else
		{
			*pStatus = 6;
		}

		return true;
	}

	int32_t nFlailPartsAvailable = 0;
	bool bBaseFlailAvailable = false;
	if (ITEMS_FindQuestItem(pQuest->pGame, pPlayer, ' 1fq'))
	{
		++nFlailPartsAvailable;
		bBaseFlailAvailable = true;
	}

	bool bEyeAvailable = false;
	if (ITEMS_FindQuestItem(pQuest->pGame, pPlayer, ' yeq'))
	{
		++nFlailPartsAvailable;
		bEyeAvailable = true;
	}

	bool bHeartAvailable = false;
	if (ITEMS_FindQuestItem(pQuest->pGame, pPlayer, ' rhq'))
	{
		++nFlailPartsAvailable;
		bHeartAvailable = true;
	}

	bool bBrainAvailable = false;
	if (ITEMS_FindQuestItem(pQuest->pGame, pPlayer, ' rbq'))
	{
		++nFlailPartsAvailable;
		bBrainAvailable = true;
	}

	if (nFlailPartsAvailable == 4)
	{
		*pStatus = 2 * (QUESTRECORD_GetQuestState(pFlags, QUESTSTATEFLAG_A3Q2, QFLAG_CUSTOM1) == 0) + 5;
		return true;
	}

	if (!nFlailPartsAvailable)
	{
		if (QUESTRECORD_GetQuestState(pFlags, QUESTSTATEFLAG_A3Q2, QFLAG_STARTED))
		{
			*pStatus = 1;
			return true;
		}

		*pStatus = pQuest->fState > 1;
		return true;
	}

	if (!bEyeAvailable)
	{
		*pStatus = 1;
		return true;
	}

	if (!bBrainAvailable)
	{
		*pStatus = 2;
		return true;
	}

	if (!bHeartAvailable)
	{
		*pStatus = 4;
		return true;
	}

	*pStatus = 4 * (bBaseFlailAvailable != 0) + 3;
	return true;

}

//D2Game.0x6FCA81E0
// 1.13c D2Game+0xE7940
void __fastcall ACT3Q2_Callback11_ScrollMessage(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pQuestArg->pPlayer)->pQuestData[pQuestArg->pGame->nDifficulty];

	if (pQuestArg->nNPCNo == MONSTER_CAIN3)
	{
		switch (pQuestArg->nMessageIndex)
		{
		case 543:
			QUESTS_StateDebug(pQuestData, 2, __FILE__, __LINE__);
			QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_STARTED);
			((D2Act3Quest2Strc*)pQuestData->pQuestDataEx)->bInitiallyTalkedToCain = 1;
			pQuestData->pfCallback[QUESTEVENT_NPCDEACTIVATE] = ACT3Q2_Callback02_NpcDeactivate;
			break;

		case 545:
			QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_LEAVETOWN);
			break;

		case 544:
			QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_CUSTOM2);
			break;

		case 546:
			QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_ENTERAREA);
			break;

		case 547:
			QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_CUSTOM1);
			break;

		case 548:
			QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_CUSTOM3);
			break;

		default:
			return;
		}
	}
}

//D2Game.0x6FCA82D0
// 1.13c D2Game+0xE6D90 (retail inlines the quest iterate: flags low byte cleared, last state 1, then the unit walk)
void __fastcall ACT3Q2_Callback02_NpcDeactivate(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	if (!pQuestArg->pTarget || pQuestArg->pTarget->dwClassId != MONSTER_CAIN3)
	{
		return;
	}

	if (((D2Act3Quest2Strc*)pQuestData->pQuestDataEx)->bInitiallyTalkedToCain == 1)
	{
		pQuestData->dwFlags &= 0xFFFFFF00;
		QUESTS_UnitIterate(pQuestData, 1, 0, ACT3Q2_UnitIterate_StatusCyclerEx, 1);
		pQuestData->pfCallback[QUESTEVENT_NPCDEACTIVATE] = nullptr;
	}
}

//D2Game.0x6FCA8310
// 1.13c D2Game+0xE6420
int32_t __fastcall ACT3Q2_UnitIterate_StatusCyclerEx(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	D2BitBufferStrc* pQuestFlags = D2_CALL(UNITS_GetPlayerData, pUnit)->pQuestData[pGame->nDifficulty];

	if (!D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_REWARDGRANTED) && !D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_COMPLETEDBEFORE))
	{
		D2_CALL(QUESTS_StatusCyclerEx, pGame, pUnit, QUEST_A3Q2_KHALIMFLAIL);
	}

	return 0;
}

//D2Game.0x6FCA8360
// 1.13c D2Game+0xE2E80
bool __fastcall ACT3Q2_SeqCallback(D2QuestDataStrc* pQuestData)
{
	if (pQuestData->fState != 5 && pQuestData->bNotIntro)
	{
		if (!pQuestData->fState)
		{
			pQuestData->fState = 1;
		}
		return true;
	}

	D2QuestDataStrc* pQuest = QUESTS_GetQuestData(pQuestData->pGame, pQuestData->nSeqId);
	if (!pQuest)
	{
		return false;
	}

	// d2-web: the next quest's own filter on its own data (retail 1.13c D2Game: every call through
	// quest data +0xF0 is "mov ecx, R; call [R+0xF0]"; upstream ran this quest's filter on the next's data).
	if (IsBadCodePtr((FARPROC)pQuest->pfSeqFilter))
	{
		FOG_DisplayAssert("pQuestInfo->pSequence", __FILE__, __LINE__);
		exit(-1);
	}
	return pQuest->pfSeqFilter(pQuest);
}

// 1.13c D2Game+0xE4A50 (the 0x5D quest-state packet; the client is fetched here, after the state is chosen)
static void ACT3Q2_SendKhalimFlailState(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg, uint8_t nState)
{
	D2ClientStrc* pClient = SUNIT_GetClientFromPlayer(pQuestArg->pPlayer, __FILE__, __LINE__);

	D2GSPacketSrv5D packet5D = {};
	packet5D.nHeader = 0x5D;
	packet5D.nQuestId = QUEST_A3Q2_KHALIMFLAIL;
	packet5D.nFlags = (uint8_t)pQuestData->dwFlags;
	packet5D.nState = nState;
	packet5D.field_4 = 0;

	D2GAME_PACKETS_SendPacket_6FC3C710(pClient, &packet5D, sizeof(packet5D));
}

//D2Game.0x6FCA83D0
// 1.13c D2Game+0xE6190
void __fastcall ACT3Q2_Callback04_ItemPickedUp(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pQuestArg->pPlayer)->pQuestData[pQuestArg->pGame->nDifficulty];
	if (!QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2COMPLETED, QFLAG_REWARDGRANTED) || QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_REWARDGRANTED))
	{
		return;
	}

	const uint32_t dwItemCode = ITEMS_GetBaseCode(pQuestArg->pTarget);

	// retail reads these states up front; only CUSTOM1 and CUSTOM3 are used below
	QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_STARTED);
	QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_LEAVETOWN);
	QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_ENTERAREA);
	const bool bNotCustom1 = !QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_CUSTOM1);
	QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_CUSTOM2);
	const bool bNotCustom3 = !QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_CUSTOM3);

	bool bBaseFlailAvailable = false;
	if (ITEMS_FindQuestItem(pQuestArg->pGame, pQuestArg->pPlayer, ' 1fq'))
	{
		bBaseFlailAvailable = true;
	}

	ITEMS_FindQuestItem(pQuestArg->pGame, pQuestArg->pPlayer, ' 2fq');

	bool bEyeAvailable = false;
	if (ITEMS_FindQuestItem(pQuestArg->pGame, pQuestArg->pPlayer, ' yeq'))
	{
		bEyeAvailable = true;
	}

	bool bBrainAvailable = false;
	if (ITEMS_FindQuestItem(pQuestArg->pGame, pQuestArg->pPlayer, ' rbq'))
	{
		bBrainAvailable = true;
	}

	bool bHeartAvailable = false;
	if (ITEMS_FindQuestItem(pQuestArg->pGame, pQuestArg->pPlayer, ' rhq'))
	{
		bHeartAvailable = true;
	}

	uint8_t nState = 0;
	if (dwItemCode == ' 2fq')
	{
		if (pQuestData->fLastState)
		{
			pQuestData->fLastState = 7;
		}

		if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2COMPLETED, QFLAG_REWARDGRANTED))
		{
			return;
		}

		nState = bNotCustom3 ? 5 : 6;
	}
	else if (dwItemCode == ' 1fq')
	{
		if (!bEyeAvailable)
		{
			nState = 1;
		}
		else if (!bBrainAvailable)
		{
			nState = 2;
		}
		else if (!bHeartAvailable)
		{
			nState = 4;
		}
		else
		{
			nState = bNotCustom1 ? 5 : 7;
		}
	}
	else if (dwItemCode == ' yeq')
	{
		if (!bBrainAvailable)
		{
			nState = 2;
		}
		else if (!bBaseFlailAvailable)
		{
			nState = 3;
		}
		else if (!bHeartAvailable)
		{
			nState = 4;
		}
		else
		{
			nState = 7;
		}
	}
	else if (dwItemCode == ' rhq')
	{
		if (!bBrainAvailable)
		{
			nState = 2;
		}
		else if (!bBaseFlailAvailable)
		{
			nState = 3;
		}
		else if (!bEyeAvailable)
		{
			nState = 1;
		}
		else
		{
			nState = 7;
		}
	}
	else if (dwItemCode == ' rbq')
	{
		if (!bHeartAvailable)
		{
			nState = 4;
		}
		else if (!bBaseFlailAvailable)
		{
			nState = 3;
		}
		else if (!bEyeAvailable)
		{
			nState = 1;
		}
		else
		{
			nState = 7;
		}
	}
	else
	{
		return;
	}

	ACT3Q2_SendKhalimFlailState(pQuestData, pQuestArg, nState);
}

// 1.13c D2Game+0xE49E0 (retail: reward granted, custom3, or any drop count set the finished state; started alone sets it too)
void __fastcall ACT3Q2_Callback13_PlayerStartedGame(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	D2BitBufferStrc* pQuestFlags = D2_CALL(UNITS_GetPlayerData, pQuestArg->pPlayer)->pQuestData[pQuestArg->pGame->nDifficulty];

	if (!D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_REWARDGRANTED))
	{
		if (!D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_CUSTOM3))
		{
			if (D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_STARTED))
			{
				pQuestData->fState = 2;
				pQuestData->fLastState = 7;
			}

			D2Act3Quest2Strc* pQuestDataEx = (D2Act3Quest2Strc*)pQuestData->pQuestDataEx;

			if (pQuestDataEx->nEyesDropped + pQuestDataEx->nBrainsDropped + pQuestDataEx->nHeartsDropped + pQuestDataEx->nFlailsDropped == 0)
			{
				return;
			}
		}

		pQuestData->fState = 2;
		pQuestData->fLastState = 7;
		return;
	}

	pQuestData->fState = 2;
	pQuestData->fLastState = 7;
}

// 1.13c D2Game+0xE95E0
int32_t __fastcall OBJECTS_OperateFunction44_SewerStairs(D2ObjOperateFnStrc* pOp, int32_t nOperate)
{
	if (pOp->pObject && pOp->pObject->dwAnimMode == OBJMODE_OPENED)
	{
		D2GAME_WarpPlayerToTile_6FCA5B20(pOp);
	}

	return 0;
}

// 1.13c D2Game+0xE9F60 (retail runs the body when the object is missing or neutral, and returns 0 on every path)
int32_t __fastcall OBJECTS_OperateFunction45_SewerLever(D2ObjOperateFnStrc* pOp, int32_t nOperate)
{
	D2UnitStrc* pObject = pOp->pObject;

	if (pObject && pObject->dwAnimMode != OBJMODE_NEUTRAL)
	{
		return 0;
	}

	D2QuestDataStrc* pQuestData = QUESTS_GetQuestData(pOp->pGame, QUEST_A3Q2_KHALIMFLAIL);
	if (!pQuestData)
	{
		return 0;
	}

	D2Act3Quest2Strc* pQuestDataEx = (D2Act3Quest2Strc*)pQuestData->pQuestDataEx;
	if (pQuestDataEx->bSewerStairsInitialized != 1 || !SUNIT_GetServerUnit(pOp->pGame, UNIT_OBJECT, pQuestDataEx->nSewerStairsObjectGUID))
	{
		return 0;
	}

	UNITS_ChangeAnimMode(pObject, OBJMODE_OPERATING);

	D2ObjectsTxt* pObjectsTxtRecord = DATATBLS_GetObjectsTxtRecord(pOp->nObjectIdx);
	EVENT_SetEvent(pOp->pGame, pObject, EVENTTYPE_ENDANIM, pOp->pGame->dwGameFrame + OBJECTS_GetOperatingTicks(pObjectsTxtRecord), 0, 0);

	pQuestDataEx->nSewerStairsObjectMode = OBJMODE_OPENED;
	EVENT_SetEvent(pOp->pGame, pObject, EVENTTYPE_QUESTFN, pOp->pGame->dwGameFrame + 30, 0, 0);

	QUESTS_TriggerFX(pQuestData->pGame, 9);
	return 0;
}

// 1.13c D2Game+0xE4980
void __fastcall OBJECTS_InitFunction41_SewerStairs(D2ObjInitFnStrc* pOp)
{
	D2QuestDataStrc* pQuestData = QUESTS_GetQuestData(pOp->pGame, QUEST_A3Q2_KHALIMFLAIL);
	if (!pQuestData)
	{
		return;
	}

	D2Act3Quest2Strc* pQuestDataEx = (D2Act3Quest2Strc*)pQuestData->pQuestDataEx;
	pQuestDataEx->bSewerStairsInitialized = 1;

	int32_t nUnitId = -1;
	if (pOp->pObject)
	{
		nUnitId = pOp->pObject->dwUnitId;
	}
	pQuestDataEx->nSewerStairsObjectGUID = nUnitId;

	if (pQuestData->bNotIntro)
	{
		UNITS_ChangeAnimMode(pOp->pObject, pQuestDataEx->nSewerStairsObjectMode);
	}
	else
	{
		UNITS_ChangeAnimMode(pOp->pObject, OBJMODE_OPENED);
	}
}

// 1.13c D2Game+0xE2DB0 (EAX = game; flags the flail as dropped on the Khalim's Will quest record and adds to its flail count)
void __fastcall ACT3Q2_AddFlailsDropped(D2GameStrc* pGame, int32_t nFlailsDropped)
{
	D2QuestDataStrc* pQuestData = QUESTS_GetQuestData(pGame, QUEST_A3Q2_KHALIMFLAIL);
	if (pQuestData)
	{
		D2Act3Quest2Strc* pQuestDataEx = (D2Act3Quest2Strc*)pQuestData->pQuestDataEx;
		pQuestDataEx->bFlailDropped = 1;
		pQuestDataEx->nFlailsDropped += nFlailsDropped;
	}
}

// 1.13c D2Game+0xE2E00
void __fastcall OBJECTS_InitFunction42_SewerLever(D2ObjInitFnStrc* pOp)
{
	D2QuestDataStrc* pQuestData = QUESTS_GetQuestData(pOp->pGame, QUEST_A3Q2_KHALIMFLAIL);
	if (pQuestData && !pQuestData->bNotIntro)
	{
		UNITS_ChangeAnimMode(pOp->pObject, OBJMODE_OPENED);
	}
}

// 1.13c D2Game+0xE4900 (retail reads the reward-granted state, then the next state, before the counts change)
void __fastcall ACT3Q2_UpdateKhalimItemCounts(D2GameStrc* pGame, D2UnitStrc* pUnit)
{
	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pUnit)->pQuestData[pGame->nDifficulty];
	if (!QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_REWARDGRANTED))
	{
		QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A3Q2, QFLAG_REWARDPENDING);
	}

	D2QuestDataStrc* pQuestData = QUESTS_GetQuestData(pGame, QUEST_A3Q2_KHALIMFLAIL);
	if (!pQuestData)
	{
		return;
	}

	D2Act3Quest2Strc* pQuestDataEx = (D2Act3Quest2Strc*)pQuestData->pQuestDataEx;

	--pQuestDataEx->nEyesDropped;
	--pQuestDataEx->nHeartsDropped;
	--pQuestDataEx->nBrainsDropped;
	--pQuestDataEx->nFlailsDropped;
	++pQuestDataEx->nCubedFlails;
}

// 1.13c D2Game+0xEA5F0
// In retail 1.13c OperateFn 57-59 return zero on every exit, including successful drops;
// callers must observe the chest's state/reward effects instead of interpreting this result.
int32_t __fastcall OBJECTS_OperateFunction57_KhalimChest(D2ObjOperateFnStrc* pOp, int32_t nOperate)
{
	D2UnitStrc* pObject = pOp->pObject;

	if (!QUESTS_SetObjectSelection(pOp))
	{
		return 0;
	}

	D2SeedStrc* pSeed = QUESTS_GetGlobalSeed(pOp->pGame);

	const int32_t nCounter = ITEMS_RollRandomNumber(pSeed) % 5 + 5;
	for (int32_t i = 0; i < nCounter; ++i)
	{
		OBJMODE_DropItemWithCodeAndQuality(pOp->pGame, pObject, ' dlg', 2);
	}
	pObject->dwDropItemCode = ' rhq';

	D2QuestDataStrc* pQuestData = QUESTS_GetQuestData(pOp->pGame, QUEST_A3Q2_KHALIMFLAIL);
	if (!pQuestData)
	{
		OBJMODE_DropFromChestTCWithQuality(pOp, ITEMQUAL_MAGIC);
		return 0;
	}

	D2Act3Quest2Strc* pQuestDataEx = (D2Act3Quest2Strc*)pQuestData->pQuestDataEx;
	pQuestDataEx->nItemDropCount = 0;
	SUNIT_IterateUnitsOfType(pOp->pGame, 0, pOp->pPlayer, ACT3Q2_UnitIterate_DetermineKhalimHeartDropCount);

	for (int32_t i = 0; i < pQuestDataEx->nItemDropCount; ++i)
	{
		int32_t nItemLevel = 0;
		if (D2GAME_DropItemAtUnit_6FC4FEC0(pOp->pGame, pObject, ITEMQUAL_NORMAL, &nItemLevel, 0, -1, 1))
		{
			pQuestDataEx->bHeartDropped = 1;
			++pQuestDataEx->nHeartsDropped;
		}
	}

	OBJMODE_DropFromChestTCWithQuality(pOp, ITEMQUAL_MAGIC);
	return 0;
}

// 1.13c D2Game+0xE47D0
int32_t __fastcall ACT3Q2_UnitIterate_DetermineKhalimHeartDropCount(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	if (D2_CALL(ITEMS_FindQuestItem, pGame, pUnit, ' rhq') || D2_CALL(ITEMS_FindQuestItem, pGame, pUnit, ' 2fq'))
	{
		return 0;
	}

	D2QuestDataStrc* pQuestData = QUESTS_GetQuestData(pGame, QUEST_A3Q2_KHALIMFLAIL);
	if (pQuestData)
	{
		++((D2Act3Quest2Strc*)pQuestData->pQuestDataEx)->nItemDropCount;
	}
	return 0;
}

// 1.13c D2Game+0xEA4C0
int32_t __fastcall OBJECTS_OperateFunction58_KhalimChest(D2ObjOperateFnStrc* pOp, int32_t nOperate)
{
	D2UnitStrc* pObject = pOp->pObject;

	if (!QUESTS_SetObjectSelection(pOp))
	{
		return 0;
	}

	D2SeedStrc* pSeed = QUESTS_GetGlobalSeed(pOp->pGame);

	const int32_t nCounter = ITEMS_RollRandomNumber(pSeed) % 5 + 5;
	for (int32_t i = 0; i < nCounter; ++i)
	{
		OBJMODE_DropItemWithCodeAndQuality(pOp->pGame, pObject, ' dlg', 2);
	}
	pObject->dwDropItemCode = ' yeq';

	D2QuestDataStrc* pQuestData = QUESTS_GetQuestData(pOp->pGame, QUEST_A3Q2_KHALIMFLAIL);
	if (!pQuestData)
	{
		OBJMODE_DropFromChestTCWithQuality(pOp, ITEMQUAL_MAGIC);
		return 0;
	}

	D2Act3Quest2Strc* pQuestDataEx = (D2Act3Quest2Strc*)pQuestData->pQuestDataEx;
	pQuestDataEx->nItemDropCount = 0;
	SUNIT_IterateUnitsOfType(pOp->pGame, 0, pOp->pPlayer, ACT3Q2_UnitIterate_DetermineKhalimEyeDropCount);

	for (int32_t i = 0; i < pQuestDataEx->nItemDropCount; ++i)
	{
		int32_t nItemLevel = 0;
		if (D2GAME_DropItemAtUnit_6FC4FEC0(pOp->pGame, pObject, ITEMQUAL_NORMAL, &nItemLevel, 0, -1, 1))
		{
			pQuestDataEx->bEyeDropped = 1;
			++pQuestDataEx->nEyesDropped;
		}
	}

	OBJMODE_DropFromChestTCWithQuality(pOp, ITEMQUAL_MAGIC);
	return 0;
}

// 1.13c D2Game+0xE4890
int32_t __fastcall ACT3Q2_UnitIterate_DetermineKhalimEyeDropCount(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	if (D2_CALL(ITEMS_FindQuestItem, pGame, pUnit, ' yeq') || D2_CALL(ITEMS_FindQuestItem, pGame, pUnit, ' 2fq'))
	{
		return 0;
	}

	D2QuestDataStrc* pQuestData = QUESTS_GetQuestData(pGame, QUEST_A3Q2_KHALIMFLAIL);
	if (pQuestData)
	{
		++((D2Act3Quest2Strc*)pQuestData->pQuestDataEx)->nItemDropCount;
	}

	return 0;
}

// 1.13c D2Game+0xEA390
int32_t __fastcall OBJECTS_OperateFunction59_KhalimChest(D2ObjOperateFnStrc* pOp, int32_t nOperate)
{
	D2UnitStrc* pObject = pOp->pObject;

	if (!QUESTS_SetObjectSelection(pOp))
	{
		return 0;
	}

	D2SeedStrc* pSeed = QUESTS_GetGlobalSeed(pOp->pGame);

	const int32_t nCounter = ITEMS_RollRandomNumber(pSeed) % 5 + 5;
	for (int32_t i = 0; i < nCounter; ++i)
	{
		OBJMODE_DropItemWithCodeAndQuality(pOp->pGame, pObject, ' dlg', 2);
	}
	pObject->dwDropItemCode = ' rbq';

	D2QuestDataStrc* pQuestData = QUESTS_GetQuestData(pOp->pGame, QUEST_A3Q2_KHALIMFLAIL);
	if (!pQuestData)
	{
		OBJMODE_DropFromChestTCWithQuality(pOp, ITEMQUAL_MAGIC);
		return 0;
	}

	D2Act3Quest2Strc* pQuestDataEx = (D2Act3Quest2Strc*)pQuestData->pQuestDataEx;
	pQuestDataEx->nItemDropCount = 0;
	SUNIT_IterateUnitsOfType(pOp->pGame, 0, pOp->pPlayer, ACT3Q2_UnitIterate_DetermineKhalimBrainDropCount);

	for (int32_t i = 0; i < pQuestDataEx->nItemDropCount; ++i)
	{
		int32_t nItemLevel = 0;
		if (D2GAME_DropItemAtUnit_6FC4FEC0(pOp->pGame, pObject, ITEMQUAL_NORMAL, &nItemLevel, 0, -1, 1))
		{
			pQuestDataEx->bBrainDropped = 1;
			++pQuestDataEx->nBrainsDropped;
		}
	}

	OBJMODE_DropFromChestTCWithQuality(pOp, ITEMQUAL_MAGIC);
	return 0;
}

// 1.13c D2Game+0xE4830
int32_t __fastcall ACT3Q2_UnitIterate_DetermineKhalimBrainDropCount(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	if (D2_CALL(ITEMS_FindQuestItem, pGame, pUnit, ' rbq') || D2_CALL(ITEMS_FindQuestItem, pGame, pUnit, ' 2fq'))
	{
		return 0;
	}

	D2QuestDataStrc* pQuestData = QUESTS_GetQuestData(pGame, QUEST_A3Q2_KHALIMFLAIL);
	if (pQuestData)
	{
		++((D2Act3Quest2Strc*)pQuestData->pQuestDataEx)->nItemDropCount;
	}

	return 0;
}
