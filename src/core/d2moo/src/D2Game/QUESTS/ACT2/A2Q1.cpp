#include "QUESTS/ACT2/A2Q1.h"
#include <test_seam.h>

#include <DataTbls/LevelsIds.h>
#include <DataTbls/MonsterIds.h>
#include <Drlg/D2DrlgDrlg.h>
#include <D2Dungeon.h>
#include <D2Items.h>
#include <D2QuestRecord.h>

#include "GAME/Game.h"
#include "ITEMS/Items.h"
#include "QUESTS/Quests.h"
#include "UNIT/Party.h"
#include "UNIT/SUnit.h"


//D2Game.0x6FD34B38
D2NPCMessageTableStrc gpAct2Q1NpcMessages[] =
{
	{
		{
			{ MONSTER_ATMA, 304, 0, 0 },
		},
		1
	},
	{
		{
			{ MONSTER_ATMA, 310, 0, 2, },
			{ MONSTER_WARRIV2, 307, 0, 2, },
			{ MONSTER_GREIZ, 305, 0, 2, },
			{ MONSTER_ELZIX, 306, 0, 2, },
			{ MONSTER_DROGNAN, 312, 0, 2, },
			{ MONSTER_LYSANDER, 313, 0, 2, },
			{ MONSTER_CAIN2, 314, 0, 2, },
			{ MONSTER_MESHIF1, 311, 0, 2, },
			{ MONSTER_GEGLASH, 308, 0, 2, },
			{ MONSTER_FARA, 309, 0, 2, },
		},
		10
	},
	{
		{
			{ MONSTER_ATMA, 317, 0, 2, },
			{ MONSTER_WARRIV2, 315, 0, 2, },
			{ MONSTER_GREIZ, 318, 0, 2, },
			{ MONSTER_ELZIX, 320, 0, 2, },
			{ MONSTER_DROGNAN, 322, 0, 2, },
			{ MONSTER_LYSANDER, 321, 0, 2, },
			{ MONSTER_CAIN2, 324, 0, 2, },
			{ MONSTER_MESHIF1, 316, 0, 2, },
			{ MONSTER_GEGLASH, 319, 0, 2, },
			{ MONSTER_FARA, 323, 0, 2, },
		},
		10
	},
	{
		{
			{ MONSTER_ATMA, 334, 0, 1, },
			{ MONSTER_GREIZ, 325, 0, 2, },
			{ MONSTER_ELZIX, 330, 0, 2, },
			{ MONSTER_DROGNAN, 326, 0, 2, },
			{ MONSTER_LYSANDER, 327, 0, 2, },
			{ MONSTER_CAIN2, 333, 0, 2, },
			{ MONSTER_MESHIF1, 328, 0, 2, },
			{ MONSTER_GEGLASH, 329, 0, 2, },
			{ MONSTER_FARA, 332, 0, 2, },
		},
		9
	},
	{
		{
			{ MONSTER_WARRIV2, 331, 0, 2, }, // 1.13c: Warriv, not Jerhyn
			{ MONSTER_ATMA, 334, 0, 2, },
			{ MONSTER_GREIZ, 325, 0, 2, },
			{ MONSTER_ELZIX, 330, 0, 2, },
			{ MONSTER_DROGNAN, 326, 0, 2, },
			{ MONSTER_CAIN2, 333, 0, 2, },
			{ MONSTER_MESHIF1, 328, 0, 2, },
			{ MONSTER_GEGLASH, 329, 0, 2, },
			{ MONSTER_FARA, 332, 0, 2, },
		},
		9
	},
	{
		{
			{ -1, 0, 0, 2 },
		},
		0
	},
	{
		{
			{ -1, 0, 0, 2 },
		},
		0
	},
	{
		{
			{ -1, 0, 0, 2 },
		},
		0
	},
	{
		{
			{ -1, 0, 0, 2 },
		},
		0
	}
};


// 1.13c D2Game+0xC2520
bool __fastcall ACT2Q1_ActiveFilterCallback(D2QuestDataStrc* pQuest, int32_t nNpcId, D2UnitStrc* pPlayer, D2BitBufferStrc* pQuestFlags, D2UnitStrc* pNPC)
{
	if (nNpcId == MONSTER_ATMA)
	{
		if (D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, pQuest->nQuestFilter, QFLAG_REWARDGRANTED) != 1)
		{
			if (pQuest->bNotIntro && pQuest->fState == 1 && D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, pQuest->nQuestFilter, QFLAG_COMPLETEDBEFORE) == 0)
			{
				return true;
			}

			return D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, pQuest->nQuestFilter, QFLAG_REWARDPENDING) == 1;
		}
	}

	return false;
}

// 1.13c D2Game+0xC55B0
void __fastcall ACT2Q1_UnitIterate_SetPrimaryGoalDone(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pUnit)->pQuestData[pGame->nDifficulty];
	if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_REWARDGRANTED) != 1
		&& QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_REWARDPENDING) != 1)
	{
		D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pUnit);
		if (pRoom)
		{
			const int32_t nLevelId = DUNGEON_GetLevelIdFromRoom(pRoom);
			if (nLevelId && DRLG_GetActNoFromLevelId(nLevelId) == ACT_II)
			{
				QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_PRIMARYGOALDONE);
				QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_REWARDPENDING);
				QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_CUSTOM1);
				QUESTS_UpdatePlayerFlags(pGame, pUnit);
			}
		}
	}
}

//D2Game.0x6FC9F690
// 1.13c D2Game+0xC7B40 (checked against decompile)
void __fastcall ACT2Q1_InitQuestData(D2QuestDataStrc* pQuestData)
{
	memset(pQuestData->pfCallback, 0x00, sizeof(pQuestData->pfCallback));
	pQuestData->pfCallback[QUESTEVENT_CHANGEDLEVEL] = ACT2Q1_Callback03_ChangedLevel;
	pQuestData->bActive = 1;
	pQuestData->fState = 1;
	pQuestData->pfCallback[QUESTEVENT_MONSTERKILLED] = ACT2Q1_Callback08_MonsterKilled;
	pQuestData->pfCallback[QUESTEVENT_NPCACTIVATE] = ACT2Q1_Callback00_NpcActivate;
	pQuestData->pfCallback[QUESTEVENT_SCROLLMESSAGE] = ACT2Q1_Callback11_ScrollMessage;
	pQuestData->pfCallback[QUESTEVENT_PLAYERLEAVESGAME] = ACT2Q1_Callback10_PlayerLeavesGame;
	pQuestData->pfCallback[QUESTEVENT_NPCDEACTIVATE] = ACT2Q1_Callback02_NpcDeactivate;
	pQuestData->pfCallback[QUESTEVENT_PLAYERSTARTEDGAME] = ACT2Q1_Callback13_PlayerStartedGame;
	pQuestData->nSeqId = 13;
	pQuestData->pfSeqFilter = ACT2Q1_SeqCallback;
	pQuestData->pNPCMessages = gpAct2Q1NpcMessages;

	D2Act2Quest1Strc* pQuestDataEx = D2_ALLOC_STRC_POOL(pQuestData->pGame->pMemoryPool, D2Act2Quest1Strc);
	pQuestData->pQuestDataEx = pQuestDataEx;
	pQuestData->nQuestFilter = QUESTSTATEFLAG_A2Q1;
	pQuestData->pfStatusFilter = nullptr;
	pQuestData->nInitNo = 4;
	pQuestData->pfActiveFilter = ACT2Q1_ActiveFilterCallback;

	memset(pQuestDataEx, 0x00, sizeof(D2Act2Quest1Strc));
}

// 1.13c D2Game+0xC61C0
void __fastcall ACT2Q1_Callback02_NpcDeactivate(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	if (pQuestArg->pTarget && pQuestArg->pTarget->dwClassId == MONSTER_ATMA)
	{
		D2Act2Quest1Strc* pQuestDataEx = (D2Act2Quest1Strc*)pQuestData->pQuestDataEx;
		if (pQuestDataEx->bAtmaActivated == 1)
		{
			pQuestData->fLastState = 1;
			pQuestData->dwFlags &= 0xFFFFFF00;
			SUNIT_IterateUnitsOfType(pQuestData->pGame, 0, 0, ACT2Q1_UnitIterate_StatusCyclerEx);
			pQuestDataEx->bAtmaActivated = 0;
			pQuestData->pfCallback[QUESTEVENT_NPCDEACTIVATE] = nullptr;
		}
	}
}

// 1.13c D2Game+0xC5630
int32_t __fastcall ACT2Q1_UnitIterate_StatusCyclerEx(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pUnit)->pQuestData[pGame->nDifficulty];

	if ((QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_REWARDGRANTED)
			|| QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_COMPLETEDBEFORE))
		&& !QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_PRIMARYGOALDONE)
		&& !QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_COMPLETEDNOW))
	{
		return 0;
	}

	QUESTS_StatusCyclerEx(pGame, pUnit, QUEST_A2Q1_RADAMENT);
	return 0;
}

// 1.13c D2Game+0xC6E90
void __fastcall ACT2Q1_Callback11_ScrollMessage(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pQuestArg->pPlayer)->pQuestData[pQuestArg->pGame->nDifficulty];
	if (pQuestArg->nNPCNo != MONSTER_ATMA)
	{
		return;
	}

	D2Act2Quest1Strc* pQuestDataEx = (D2Act2Quest1Strc*)pQuestData->pQuestDataEx;
	if (pQuestArg->nMessageIndex == 304)
	{
		pQuestDataEx->bAtmaActivated = 1;
		QUESTS_StateDebug(pQuestData, 2, __FILE__, __LINE__);
		SUNIT_IterateUnitsOfType(pQuestData->pGame, 0, 0, ACT2Q1_UnitIterate_UpdateQuestStateFlags);
	}
	else if (pQuestArg->nMessageIndex == 334)
	{
		if (QUESTRECORD_GetQuestState(pQuestFlags, pQuestData->nQuestFilter, QFLAG_REWARDPENDING) == 0)
		{
			return;
		}

		if (QUESTRECORD_GetQuestState(pQuestFlags, pQuestData->nQuestFilter, QFLAG_PRIMARYGOALDONE) == 0)
		{
			if (pQuestDataEx->bRewardPending)
			{
				D2QuestDataStrc* pQuest = QUESTS_GetQuestData(pQuestData->pGame, pQuestData->nSeqId);
				if (pQuest)
				{
					if (IsBadCodePtr((FARPROC)pQuestData->pfSeqFilter))
					{
						FOG_DisplayAssert("pQuestInfo->pSequence", __FILE__, __LINE__);
						exit(-1);
					}
					pQuest->pfSeqFilter(pQuest);
				}
			}
		}
		else
		{
			if (pQuestData->fState != 5)
			{
				pQuestData->dwFlags &= 0xFFFFFF00;
				QUESTS_UnitIterate(pQuestData, 13, 0, ACT2Q1_UnitIterate_StatusCyclerEx, 1);
				pQuestData->pfCallback[QUESTEVENT_NPCDEACTIVATE] = nullptr;
				QUESTS_StateDebug(pQuestData, 5, __FILE__, __LINE__);

				if (IsBadCodePtr((FARPROC)pQuestData->pfSeqFilter))
				{
					FOG_DisplayAssert("pQuestInfo->pSequence", __FILE__, __LINE__);
					exit(-1);
				}
				pQuestData->pfSeqFilter(pQuestData);
			}

			if (!pQuestData->bNotIntro)
			{
				QUESTS_SetGlobalState(pQuestData->pGame, QUESTSTATEFLAG_A2Q1, QFLAG_PRIMARYGOALDONE);
			}
		}

		QUESTRECORD_SetQuestState(pQuestFlags, pQuestData->nQuestFilter, QFLAG_REWARDGRANTED);
		QUESTRECORD_ClearQuestState(pQuestFlags, pQuestData->nQuestFilter, QFLAG_REWARDPENDING);

		QUESTS_AddPlayerGUID(&pQuestData->tPlayerGUIDs, (pQuestArg->pPlayer ? pQuestArg->pPlayer->dwUnitId : -1));
	}

	QUESTS_NPCActivateSpeeches(pQuestArg->pGame, pQuestArg->pPlayer, pQuestArg->pTarget);
}

// 1.13c D2Game+0xC4430
int32_t __fastcall ACT2Q1_UnitIterate_UpdateQuestStateFlags(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	D2BitBufferStrc* pQuestFlags = D2_CALL(UNITS_GetPlayerData, pUnit)->pQuestData[pGame->nDifficulty];
	if (D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_REWARDGRANTED) != 1
		&& D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_REWARDPENDING) != 1)
	{
		D2QuestDataStrc* pQuestData = QUESTS_GetQuestData(pGame, QUEST_A2Q1_RADAMENT);
		if (pQuestData)
		{
			if (pQuestData->fState == 2)
			{
				D2_CALL(QUESTRECORD_SetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_STARTED);
			}
			else if (pQuestData->fState == 3)
			{
				if (pQuestData->fLastState == 1)
				{
					D2_CALL(QUESTRECORD_SetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_LEAVETOWN);
				}
				else
				{
					D2_CALL(QUESTRECORD_SetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_ENTERAREA);
				}
			}
		}
	}

	return 0;
}

// 1.13c D2Game+0xC4320
void __fastcall ACT2Q1_Callback00_NpcActivate(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	static const int32_t nIndices[] =
	{
		-1, 0, 1, 2, 3, 4, 0
	};

	int32_t nNpcId = -1;
	if (pQuestArg->pTarget)
	{
		nNpcId = pQuestArg->pTarget->dwClassId;
	}

	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pQuestArg->pPlayer)->pQuestData[pQuestArg->pGame->nDifficulty];

	const int32_t bPrimaryGoalDone = QUESTRECORD_GetQuestState(pQuestFlags, pQuestData->nQuestFilter, QFLAG_PRIMARYGOALDONE);
	const int32_t bRewardGranted = QUESTRECORD_GetQuestState(pQuestFlags, pQuestData->nQuestFilter, QFLAG_REWARDGRANTED);
	if (QUESTRECORD_GetQuestState(pQuestFlags, pQuestData->nQuestFilter, QFLAG_REWARDPENDING))
	{
		QUESTS_InitScrollTextChain(pQuestData, pQuestArg->pTextControl, nNpcId, 3);
		return;
	}

	const int32_t nPlayerGUID = pQuestArg->pPlayer ? pQuestArg->pPlayer->dwUnitId : -1;
	for (int32_t i = 0; i < pQuestData->tPlayerGUIDs.nPlayerCount; ++i)
	{
		if (pQuestData->tPlayerGUIDs.nPlayerGUIDs[i] == nPlayerGUID)
		{
			QUESTS_InitScrollTextChain(pQuestData, pQuestArg->pTextControl, nNpcId, 4);
			return;
		}
	}

	if (bRewardGranted != 1 && pQuestData->fState != 0 && (pQuestData->fState < 4 || bPrimaryGoalDone))
	{
		const int32_t nIndex = nIndices[pQuestData->fState];
		if (nIndex != -1 && nIndex < 9)
		{
			QUESTS_InitScrollTextChain(pQuestData, pQuestArg->pTextControl, nNpcId, nIndex);
		}
	}
}

// 1.13c D2Game+0xC7540
void __fastcall ACT2Q1_Callback08_MonsterKilled(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	if (pQuestData->bNotIntro)
	{
		QUESTS_DebugOutput(pQuestData->pGame, "killed radament for quest", __FILE__, __LINE__);
		QUESTS_StateDebug(pQuestData, 4, __FILE__, __LINE__);
		QUESTS_SetGlobalState(pQuestArg->pGame, pQuestData->nQuestFilter, QFLAG_PRIMARYGOALDONE);

		D2Act2Quest1Strc* pQuestDataEx = (D2Act2Quest1Strc*)pQuestData->pQuestDataEx;
		pQuestData->pfCallback[QUESTEVENT_NPCDEACTIVATE] = nullptr;
		pQuestDataEx->bRadamentKilled = 1;
		pQuestDataEx->pRoom = UNITS_GetRoom(pQuestArg->pTarget);
		SUNIT_IterateUnitsOfType(pQuestArg->pGame, 0, pQuestArg->pTarget, ACT2Q1_UnitIterate_SetPrimaryGoalDoneForPartyMembers);
		SUNIT_IterateUnitsOfType(pQuestArg->pGame, 0, pQuestArg->pTarget, ACT2Q1_UnitIterate_SetCompletionFlag);
		pQuestDataEx->nSkillBookDropCount = 0;
		SUNIT_IterateUnitsOfType(pQuestArg->pGame, 0, pQuestArg->pTarget, ACT2Q1_UnitIterate_DetermineSkillBookDropCount);

		if (!pQuestDataEx->bStatusTimerStarted)
		{
			pQuestDataEx->bStatusTimerStarted = 1;
			QUESTS_CreateTimer(pQuestData, ACT2Q1_Timer_StatusCycler, 12);
		}

		if (pQuestDataEx->nSkillBookDropCount)
		{
			pQuestArg->pTarget->dwDropItemCode = ' ssa';
			for (int32_t i = 0; i < pQuestDataEx->nSkillBookDropCount; ++i)
			{
				int32_t nItemLevel = 0;
				D2GAME_DropItemAtUnit_6FC4FEC0(pQuestData->pGame, pQuestArg->pTarget, ITEMQUAL_NORMAL, &nItemLevel, 0, -1, 0);
			}
		}
	}
}

// 1.13c D2Game+0xC5560
int32_t __fastcall ACT2Q1_UnitIterate_SetCompletionFlag(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	D2BitBufferStrc* pQuestFlags = D2_CALL(UNITS_GetPlayerData, pUnit)->pQuestData[pGame->nDifficulty];

	if (D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_REWARDGRANTED) != 1)
	{
		if (D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_REWARDPENDING) != 1)
		{
			D2_CALL(QUESTRECORD_SetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_COMPLETEDNOW);
			D2_CALL(QUESTS_SendLogUpdateEx, pUnit, QUEST_A2Q1_RADAMENT, 0);
		}
	}

	return 0;
}

//D2Game.0x6FC9FD10
// 1.13c D2Game+0xC6070
int32_t __fastcall ACT2Q1_UnitIterate_SetPrimaryGoalDoneForPartyMembers(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	D2ActiveRoomStrc* pRoom = D2_CALL(UNITS_GetRoom, pUnit);
	if (!pRoom)
	{
		return 0;
	}

	D2QuestDataStrc* pQuestData = QUESTS_GetQuestData(pGame, QUEST_A2Q1_RADAMENT);
	D2_ASSERT(pQuestData);

	D2Act2Quest1Strc* pQuestDataEx = (D2Act2Quest1Strc*)pQuestData->pQuestDataEx;

	// 1.13c: the quest-record checks run whether or not the unit is in the quest room; only the room result gates the writes
	bool bInQuestRoom = true;
	if (pRoom != pQuestDataEx->pRoom)
	{
		D2ActiveRoomStrc* pRoomAgain = D2_CALL(UNITS_GetRoom, pUnit);
		D2_ASSERT(pRoomAgain);

		D2ActiveRoomStrc** ppRoomList = nullptr;
		int32_t nNumRooms = 0;
		D2_CALL(DUNGEON_GetAdjacentRoomsListFromRoom, pRoomAgain, &ppRoomList, &nNumRooms);

		bInQuestRoom = false;
		for (int32_t nCounter = 0; nCounter < nNumRooms; ++nCounter)
		{
			if (ppRoomList[nCounter] == pQuestDataEx->pRoom)
			{
				bInQuestRoom = true;
				break;
			}
		}
	}

	D2BitBufferStrc* pQuestFlags = D2_CALL(UNITS_GetPlayerData, pUnit)->pQuestData[pGame->nDifficulty];
	if (D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, pQuestData->nQuestFilter, QFLAG_REWARDGRANTED) || D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, pQuestData->nQuestFilter, QFLAG_REWARDPENDING) || !bInQuestRoom)
	{
		return 0;
	}

	D2_CALL(QUESTRECORD_SetQuestState, pQuestFlags, pQuestData->nQuestFilter, QFLAG_PRIMARYGOALDONE);
	D2_CALL(QUESTRECORD_SetQuestState, pQuestFlags, pQuestData->nQuestFilter, QFLAG_REWARDPENDING);
	D2_CALL(QUESTRECORD_SetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_CUSTOM1);
	D2_CALL(QUESTS_UpdatePlayerFlags, pQuestData->pGame, pUnit);
	
	const int16_t nPartyId = D2_CALL(SUNIT_GetPartyId, pUnit);
	if (nPartyId != -1)
	{
		D2_CALL(PARTY_IteratePartyMembers, pGame, nPartyId, ACT2Q1_UnitIterate_SetPrimaryGoalDone, 0);
	}

	return 0;
}

// 1.13c D2Game+0xC6010
bool __fastcall ACT2Q1_Timer_StatusCycler(D2GameStrc* pGame, D2QuestDataStrc* pQuestData)
{
	D2Act2Quest1Strc* pQuestDataEx = (D2Act2Quest1Strc*)pQuestData->pQuestDataEx;

	if (pQuestData->fState == 4)
	{
		pQuestData->dwFlags &= 0xFFFFFF00;
		pQuestData->fLastState = 3;
		SUNIT_IterateUnitsOfType(pGame, 0, 0, ACT2Q1_UnitIterate_StatusCyclerEx);
	}
	pQuestDataEx->bStatusTimerStarted = 0;

	return 1;
}

// 1.13c D2Game+0xC54A0
int32_t __fastcall ACT2Q1_UnitIterate_DetermineSkillBookDropCount(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	D2BitBufferStrc* pQuestFlags = D2_CALL(UNITS_GetPlayerData, pUnit)->pQuestData[pGame->nDifficulty];
	if (D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_CUSTOM1) != 0)
	{
		D2QuestDataStrc* pQuestData = QUESTS_GetQuestData(pGame, QUEST_A2Q1_RADAMENT);
		if (pQuestData)
		{
			if (D2_CALL(ITEMS_FindQuestItem, pGame, pUnit, ' ssa') == nullptr)
			{
				++((D2Act2Quest1Strc*)pQuestData->pQuestDataEx)->nSkillBookDropCount;
			}
		}
	}

	pQuestFlags = D2_CALL(UNITS_GetPlayerData, pUnit)->pQuestData[pGame->nDifficulty];
	if (D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_PRIMARYGOALDONE) != 0)
	{
		SUNIT_AttachSound(pUnit, 50, pUnit);
	}

	return 0;
}

// 1.13c D2Game+0xC6E00
void __fastcall ACT2Q1_Callback03_ChangedLevel(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	if (pQuestArg->nOldLevel == LEVEL_LUTGHOLEIN)
	{
		if (pQuestData->tPlayerGUIDs.nPlayerCount != 0)
		{
			D2_CALL(QUESTS_FastRemovePlayerGUID, &pQuestData->tPlayerGUIDs, pQuestArg->pPlayer ? pQuestArg->pPlayer->dwUnitId : -1);
		}

		if (pQuestData->fState == 2)
		{
			D2BitBufferStrc* pQuestFlags = D2_CALL(UNITS_GetPlayerData, pQuestArg->pPlayer)->pQuestData[pQuestArg->pGame->nDifficulty];
			if (D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, pQuestData->nQuestFilter, QFLAG_REWARDGRANTED) != 1
				&& D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, pQuestData->nQuestFilter, QFLAG_REWARDPENDING) != 1)
			{
				D2_CALL(QUESTS_StateDebug, pQuestData, 3, __FILE__, __LINE__);
				SUNIT_IterateUnitsOfType(pQuestData->pGame, 0, 0, ACT2Q1_UnitIterate_UpdateQuestStateFlags);
			}
		}
	}
}

// 1.13c D2Game+0xC24A0
bool __fastcall ACT2Q1_SeqCallback(D2QuestDataStrc* pQuestData)
{
	if (pQuestData->fState != 5 && pQuestData->bNotIntro)
	{
		return true;
	}

	D2QuestDataStrc* pQuest = QUESTS_GetQuestData(pQuestData->pGame, pQuestData->nSeqId);
	if (!pQuest)
	{
		return false;
	}

	if (IsBadCodePtr((FARPROC)pQuestData->pfSeqFilter))
	{
		FOG_DisplayAssert("pQuestInfo->pSequence", __FILE__, __LINE__);
		exit(-1);
	}
	return pQuest->pfSeqFilter(pQuest);
}

//D2Game.0x6FCA0010
// 1.13c D2Game+0xC41D0
void __fastcall ACT2Q1_Callback13_PlayerStartedGame(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pQuestArg->pPlayer)->pQuestData[pQuestArg->pGame->nDifficulty];

	if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_REWARDGRANTED) == 1)
	{
		if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_CUSTOM1) == 1 && !ITEMS_FindQuestItem(pQuestArg->pGame, pQuestArg->pPlayer, ' ssa'))
		{
			QUESTRECORD_ClearQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_REWARDGRANTED);
			QUESTRECORD_ClearQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_REWARDPENDING);
			QUESTRECORD_ClearQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_COMPLETEDBEFORE);
			QUESTRECORD_ClearQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_CUSTOM1);
		}
		QUESTS_SetGlobalState(pQuestData->pGame, QUESTSTATEFLAG_A2Q1, QFLAG_PRIMARYGOALDONE);
		pQuestData->bNotIntro = 0;
		pQuestData->fState = 0;
	}
	else if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_COMPLETEDBEFORE) == QFLAG_REWARDPENDING)
	{
		if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_REWARDPENDING) == 1)
		{
			((D2Act2Quest1Strc*)pQuestData->pQuestDataEx)->bRewardPending = 1;
		}
		pQuestData->fState = 0;
		pQuestData->bNotIntro = 0;
	}
	else if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_ENTERAREA) == 1)
	{
		pQuestData->fLastState = 2;
		pQuestData->fState = 3;
	}
	else if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_LEAVETOWN) == 1)
	{
		pQuestData->fState = 3;
		pQuestData->fLastState = 1;
	}
	else if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q1, QFLAG_STARTED) == 1)
	{
		pQuestData->fState = 2;
		pQuestData->fLastState = 1;
	}
}

// 1.13c D2Game+0xC6D20
void __fastcall ACT2Q1_OnRadamentActivated(D2GameStrc* pGame, D2UnitStrc* pUnit)
{
	D2QuestDataStrc* pQuestData = QUESTS_GetQuestData(pGame, QUEST_A2Q1_RADAMENT);
	if (!pQuestData)
	{
		return;
	}

	if (pQuestData->bNotIntro == 1 && (pQuestData->fState < 3 || pQuestData->fLastState < 2))
	{
		if (DUNGEON_GetLevelIdFromRoom(UNITS_GetRoom(pUnit)) == LEVEL_SEWERSLEV3)
		{
			const uint8_t nOldState = pQuestData->fState;
			pQuestData->pfCallback[QUESTEVENT_NPCDEACTIVATE] = nullptr;
			if (nOldState < 3)
			{
				QUESTS_StateDebug(pQuestData, 3, __FILE__, __LINE__);
			}

			if (pQuestData->fLastState < 2)
			{
				D2Act2Quest1Strc* pQuestDataEx = (D2Act2Quest1Strc*)pQuestData->pQuestDataEx;
				pQuestData->dwFlags &= 0xFFFFFF00;

				const bool bIterate = pQuestDataEx->unk0x09 == 0;
				if (bIterate)
				{
					pQuestDataEx->unk0x09 = 1;
				}
				QUESTS_UnitIterate(pQuestData, 2, 0, ACT2Q1_UnitIterate_StatusCyclerEx, bIterate);
			}
			else if (nOldState >= 3)
			{
				return;
			}

			SUNIT_IterateUnitsOfType(pQuestData->pGame, 0, 0, ACT2Q1_UnitIterate_UpdateQuestStateFlags);
		}
	}
}

// 1.13c D2Game+0xC44D0
void __fastcall ACT2Q1_Callback10_PlayerLeavesGame(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	QUESTS_RemovePlayerGUID(pQuestData, pQuestArg);
}
