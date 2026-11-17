#include "QUESTS/ACT2/A2Q5.h"

#include <DataTbls/LevelsIds.h>
#include <DataTbls/MonsterIds.h>
#include <Drlg/D2DrlgDrlg.h>
#include <D2Dungeon.h>
#include <D2QuestRecord.h>

#include "GAME/Game.h"
#include "QUESTS/Quests.h"
#include "QUESTS/ACT2/A2Q4.h"
#include "UNIT/Party.h"
#include "UNIT/SUnit.h"


//D2Game.0x6FD368A8
D2NPCMessageTableStrc gpAct2Q5NpcMessages[] =
{
	{
		{
			{ MONSTER_ATMA, 414, 0, 2, },
			{ MONSTER_WARRIV2, 413, 0, 2, },
			{ MONSTER_GREIZ, 408, 0, 2, },
			{ MONSTER_ELZIX, 417, 0, 2, },
			{ MONSTER_DROGNAN, 410, 0, 2, },
			{ MONSTER_LYSANDER, 411, 0, 2, },
			{ MONSTER_CAIN2, 418, 0, 2, },
			{ MONSTER_MESHIF1, 412, 0, 2, },
			{ MONSTER_JERHYN, 409, 0, 2, },
			{ MONSTER_GEGLASH, 415, 0, 2, },
			{ MONSTER_FARA, 416, 0, 2, },
		},
		11
	},
	{
		{
			{ MONSTER_ATMA, 427, 0, 1, },
			{ MONSTER_WARRIV2, 424, 0, 1, },
			{ MONSTER_GREIZ, 419, 0, 1, },
			{ MONSTER_ELZIX, 423, 0, 1, },
			{ MONSTER_DROGNAN, 422, 0, 1, },
			{ MONSTER_LYSANDER, 426, 0, 1, },
			{ MONSTER_CAIN2, 429, 0, 1, },
			{ MONSTER_MESHIF1, 425, 0, 1, },
			{ MONSTER_JERHYN, 421, 0, 1, },
			{ MONSTER_GEGLASH, 420, 0, 1, },
			{ MONSTER_FARA, 428, 0, 1, },
		},
		11
	},
	{
		{
			{ MONSTER_ATMA, 427, 0, 2, },
			{ MONSTER_WARRIV2, 424, 0, 2, },
			{ MONSTER_GREIZ, 419, 0, 2, },
			{ MONSTER_ELZIX, 423, 0, 2, },
			{ MONSTER_DROGNAN, 422, 0, 2, },
			{ MONSTER_LYSANDER, 426, 0, 2, },
			{ MONSTER_CAIN2, 429, 0, 2, },
			{ MONSTER_MESHIF1, 425, 0, 2, },
			{ MONSTER_JERHYN, 421, 0, 2, },
			{ MONSTER_GEGLASH, 420, 0, 2, },
			{ MONSTER_FARA, 428, 0, 2, },
		},
		11
	},
	{
		{
			{ -1, 0, 0, 2 },
		},
		0
	}
};


//D2Game.0x6FCA3DB0
// 1.13c D2Game+0xC1E10
bool __fastcall ACT2Q5_ActiveFilterCallback(D2QuestDataStrc* pQuest, int32_t nNpcId, D2UnitStrc* pPlayer, D2BitBufferStrc* pQuestFlags, D2UnitStrc* pNPC)
{
	if (nNpcId == MONSTER_ATMA || nNpcId == MONSTER_WARRIV2 || nNpcId == MONSTER_ELZIX || nNpcId == MONSTER_DROGNAN || nNpcId == MONSTER_LYSANDER
		|| nNpcId == MONSTER_CAIN2 || nNpcId == MONSTER_MESHIF1 || nNpcId == MONSTER_JERHYN || nNpcId == MONSTER_GEGLASH || nNpcId == MONSTER_FARA)
	{
		if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_REWARDPENDING) == 1)
		{
			return true;
		}
	}

	return false;
}

//D2Game.0x6FCA3E20
// 1.13c D2Game+0xC2E70
void __fastcall ACT2Q5_UnitIterate_SetPrimaryGoalDone(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	if (!QUESTS_GetQuestData(pGame, QUEST_A2Q5_SUMMONER))
	{
		return;
	}

	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pUnit)->pQuestData[pGame->nDifficulty];
	if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_REWARDGRANTED) != 1)
	{
		if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_REWARDPENDING) != 1)
		{
			D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pUnit);
			if (pRoom)
			{
				const int32_t nLevelId = DUNGEON_GetLevelIdFromRoom(pRoom);
				if (nLevelId && DRLG_GetActNoFromLevelId(nLevelId) == ACT_II)
				{
					QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_PRIMARYGOALDONE);
					QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_REWARDPENDING);
				}
			}
		}
	}
}

//D2Game.0x6FCA3EA0
// 1.13c D2Game+0xC7080 (checked against decompile)
void __fastcall ACT2Q5_InitQuestData(D2QuestDataStrc* pQuestData)
{
	memset(pQuestData->pfCallback, 0x00, sizeof(pQuestData->pfCallback));
	pQuestData->pNPCMessages = gpAct2Q5NpcMessages;
	pQuestData->bActive = 1;
	pQuestData->fState = 0;

	D2Act2Quest5Strc* pQuestDataEx = D2_ALLOC_STRC_POOL(pQuestData->pGame->pMemoryPool, D2Act2Quest5Strc);
	pQuestData->pQuestDataEx = pQuestDataEx;
	pQuestData->nQuestFilter = QUESTSTATEFLAG_A2Q5;
	pQuestData->pfStatusFilter = nullptr;
	pQuestData->nInitNo = 2;
	pQuestData->pfActiveFilter = ACT2Q5_ActiveFilterCallback;
	pQuestData->pfCallback[QUESTEVENT_CHANGEDLEVEL] = ACT2Q5_Callback03_ChangedLevel;
	pQuestData->pfCallback[QUESTEVENT_SCROLLMESSAGE] = ACT2Q5_Callback11_ScrollMessage;
	pQuestData->pfCallback[QUESTEVENT_PLAYERLEAVESGAME] = ACT2Q5_Callback10_PlayerLeavesGame;
	pQuestData->pfCallback[QUESTEVENT_PLAYERSTARTEDGAME] = ACT2Q5_Callback13_PlayerStartedGame;
	pQuestData->pfCallback[QUESTEVENT_MONSTERKILLED] = ACT2Q5_Callback08_MonsterKilled;
	pQuestData->pfCallback[QUESTEVENT_NPCACTIVATE] = ACT2Q5_Callback00_NpcActivate;

	pQuestDataEx->bSummonerKilled = 0;
	pQuestDataEx->unk0x01 = 0;
	pQuestDataEx->bStatusTimerStarted = 0;
	pQuestDataEx->pRoom = nullptr;
}

//D2Game.0x6FCA3F50
// 1.13c D2Game+0xC3080
void __fastcall ACT2Q5_Callback00_NpcActivate(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	static const int32_t nIndices[] =
	{
		-1, 0, 1, 2
	};

	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pQuestArg->pPlayer)->pQuestData[pQuestArg->pGame->nDifficulty];

	int32_t nNpcId = -1;
	if (pQuestArg->pTarget)
	{
		nNpcId = pQuestArg->pTarget->dwClassId;
	}

	if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_REWARDPENDING))
	{
		QUESTS_InitScrollTextChain(pQuestData, pQuestArg->pTextControl, nNpcId, 1);
		return;
	}

	const int32_t nUnitGUID = pQuestArg->pPlayer ? pQuestArg->pPlayer->dwUnitId : -1;
	for (int32_t i = 0; i < pQuestData->tPlayerGUIDs.nPlayerCount; ++i)
	{
		if (pQuestData->tPlayerGUIDs.nPlayerGUIDs[i] == nUnitGUID)
		{
			QUESTS_InitScrollTextChain(pQuestData, pQuestArg->pTextControl, nNpcId, 2);
			return;
		}
	}

	if (pQuestData->bNotIntro && pQuestData->fState)
	{
		const int32_t nRewardGranted = QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_REWARDGRANTED);
		const int32_t nPrimaryGoalDone = QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_PRIMARYGOALDONE);
		if ((nRewardGranted != 1 || nPrimaryGoalDone != 0) && (pQuestData->fState < 2 || nPrimaryGoalDone != 0))
		{
			const int32_t nIndex = nIndices[pQuestData->fState];
			if (nIndex != -1 && (uint32_t)nIndex < 4)
			{
				QUESTS_InitScrollTextChain(pQuestData, pQuestArg->pTextControl, nNpcId, nIndex);
			}
		}
	}
}

//D2Game.0x6FCA4040
// 1.13c D2Game+0xC67C0
void __fastcall ACT2Q5_Callback11_ScrollMessage(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	const int16_t nMessageIndex = (int16_t)pQuestArg->nMessageIndex;
	if (nMessageIndex == 427 || nMessageIndex == 424 || nMessageIndex == 419 || nMessageIndex == 423 || nMessageIndex == 422 || nMessageIndex == 426
		|| nMessageIndex == 429 || nMessageIndex == 425 || nMessageIndex == 421 || nMessageIndex == 420 || nMessageIndex == 428)
	{
		D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pQuestArg->pPlayer)->pQuestData[pQuestArg->pGame->nDifficulty];
		if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_REWARDPENDING))
		{
			if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_PRIMARYGOALDONE))
			{
				pQuestData->dwFlags &= 0xFFFFFF00;
				QUESTS_UnitIterate(pQuestData, 13, 0, ACT2Q5_UnitIterate_StatusCyclerEx, 1);
				QUESTS_SetGlobalState(pQuestData->pGame, QUESTSTATEFLAG_A2Q5, QFLAG_PRIMARYGOALDONE);
				QUESTS_StateDebug(pQuestData, 3, __FILE__, __LINE__);
			}

			QUESTS_AddPlayerGUID(&pQuestData->tPlayerGUIDs, (pQuestArg->pPlayer ? pQuestArg->pPlayer->dwUnitId : -1));
			QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_REWARDGRANTED);
			QUESTRECORD_ClearQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_REWARDPENDING);
		}
		QUESTS_NPCActivateSpeeches(pQuestArg->pGame, pQuestArg->pPlayer, pQuestArg->pTarget);
	}
}

//D2Game.0x6FCA4140
// 1.13c D2Game+0xC4E30
int32_t __fastcall ACT2Q5_UnitIterate_StatusCyclerEx(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pUnit)->pQuestData[pGame->nDifficulty];

	if ((QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_REWARDGRANTED) || QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_COMPLETEDBEFORE))
		&& !QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_PRIMARYGOALDONE)
		&& !QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_COMPLETEDNOW))
	{
		return 0;
	}

	QUESTS_StatusCyclerEx(pGame, pUnit, QUEST_A2Q5_SUMMONER);
	return 0;
}

//D2Game.0x6FCA41B0
// 1.13c D2Game+0xC66B0
void __fastcall ACT2Q5_Callback08_MonsterKilled(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	if (pQuestData->bNotIntro)
	{
		QUESTS_DebugOutput(pQuestData->pGame, "killed summoner for quest", __FILE__, __LINE__);
		QUESTS_StateDebug(pQuestData, 2, __FILE__, __LINE__);

		D2Act2Quest5Strc* pQuestDataEx = (D2Act2Quest5Strc*)pQuestData->pQuestDataEx;
		if (!pQuestDataEx->bStatusTimerStarted)
		{
			pQuestDataEx->bStatusTimerStarted = 1;
			QUESTS_CreateTimer(pQuestData, ACT2Q5_Timer_StatusCycler, 3);
		}

		pQuestDataEx->unk0x09 = 0;
		pQuestDataEx->bSummonerKilled = 1;
		pQuestDataEx->pRoom = UNITS_GetRoom(pQuestArg->pTarget);

		SUNIT_IterateUnitsOfType(pQuestArg->pGame, 0, pQuestArg->pTarget, ACT2Q5_UnitIterate_SetRewardPending);
		SUNIT_IterateUnitsOfType(pQuestArg->pGame, 0, pQuestArg->pTarget, ACT2Q5_UnitIterate_SetPrimaryGoalDoneForPartyMembers);
		SUNIT_IterateUnitsOfType(pQuestArg->pGame, 0, pQuestArg->pTarget, ACT2Q5_UnitIterate_SetCompletionFlag);
	}

	QUESTS_TriggerFX(pQuestData->pGame, 7);
}

//D2Game.0x6FCA4270
// 1.13c D2Game+0xC2F10
int32_t __fastcall ACT2Q5_UnitIterate_SetRewardPending(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pUnit)->pQuestData[pGame->nDifficulty];
	D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pUnit);
	if (!pRoom)
	{
		return 0;
	}

	D2QuestDataStrc* pQuestData = QUESTS_GetQuestData(pGame, QUEST_A2Q5_SUMMONER);
	D2_ASSERT(pQuestData);

	D2Act2Quest5Strc* pQuestDataEx = (D2Act2Quest5Strc*)pQuestData->pQuestDataEx;

	bool bNearSummoner = false;
	if (pRoom == pQuestDataEx->pRoom)
	{
		bNearSummoner = true;
	}
	else
	{
		pRoom = UNITS_GetRoom(pUnit);
		D2_ASSERT(pRoom);

		D2ActiveRoomStrc** ppRoomList = nullptr;
		int32_t nNumRooms = 0;
		DUNGEON_GetAdjacentRoomsListFromRoom(pRoom, &ppRoomList, &nNumRooms);

		for (int32_t i = 0; i < nNumRooms; ++i)
		{
			if (ppRoomList[i] == pQuestDataEx->pRoom)
			{
				bNearSummoner = true;
				break;
			}
		}
	}

	if (!QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_REWARDGRANTED) && !QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_REWARDPENDING) && bNearSummoner)
	{
		QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_PRIMARYGOALDONE);
		QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_REWARDPENDING);
	}

	return 0;
}

//D2Game.0x6FCA4380
// 1.13c D2Game+0xC5BE0
bool __fastcall ACT2Q5_Timer_StatusCycler(D2GameStrc* pGame, D2QuestDataStrc* pQuestData)
{
	D2Act2Quest5Strc* pQuestDataEx = (D2Act2Quest5Strc*)pQuestData->pQuestDataEx;

	if (pQuestDataEx->unk0x09)
	{
		pQuestData->dwFlags &= 0xFFFFFF00;
		QUESTS_UnitIterate(pQuestData, 4, 0, ACT2Q5_UnitIterate_StatusCyclerEx, 1);
		pQuestDataEx->bStatusTimerStarted = 0;
		return true;
	}

	pQuestDataEx->unk0x09 = 1;
	SUNIT_IterateUnitsOfType(pGame, 0, 0, ACT2Q5_UnitIterate_AttachCompletionSound);
	return false;
}

//D2Game.0x6FCA43D0
// 1.13c D2Game+0xC4DE0
int32_t __fastcall ACT2Q5_UnitIterate_AttachCompletionSound(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	if (QUESTRECORD_GetQuestState(UNITS_GetPlayerData(pUnit)->pQuestData[pGame->nDifficulty], QUESTSTATEFLAG_A2Q5, QFLAG_PRIMARYGOALDONE))
	{
		D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pUnit);
		if (pRoom && DUNGEON_GetLevelIdFromRoom(pRoom) == LEVEL_ARCANESANCTUARY)
		{
			SUNIT_AttachSound(pUnit, 51, pUnit);
		}
	}

	return 0;
}

//D2Game.0x6FCA4420
// 1.13c D2Game+0xC5B80
int32_t __fastcall ACT2Q5_UnitIterate_SetPrimaryGoalDoneForPartyMembers(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	if (QUESTRECORD_GetQuestState(UNITS_GetPlayerData(pUnit)->pQuestData[pGame->nDifficulty], QUESTSTATEFLAG_A2Q5, QFLAG_PRIMARYGOALDONE)
		&& pUnit && pUnit->pGame && pUnit->dwUnitType == UNIT_PLAYER)
	{
		const int16_t nPartyId = PARTY_GetPartyIdForUnitOwner(pUnit->pGame, pUnit);
		if (nPartyId != -1)
		{
			PARTY_IteratePartyMembers(pGame, nPartyId, ACT2Q5_UnitIterate_SetPrimaryGoalDone, 0);
		}
	}

	return 0;
}

//D2Game.0x6FCA4470
// 1.13c D2Game+0xC4D90
int32_t __fastcall ACT2Q5_UnitIterate_SetCompletionFlag(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pUnit)->pQuestData[pGame->nDifficulty];

	if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_REWARDGRANTED) != 1 && QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_REWARDPENDING) != 1)
	{
		QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_COMPLETEDNOW);
		QUESTS_SendLogUpdateEx(pUnit, QUEST_A2Q5_SUMMONER, 0);
	}

	return 0;
}

//D2Game.0x6FCA44D0
// 1.13c D2Game+0xC2E20
void __fastcall ACT2Q5_Callback03_ChangedLevel(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	if (pQuestData->bNotIntro && pQuestArg->nNewLevel >= LEVEL_LUTGHOLEIN && pQuestArg->nOldLevel == LEVEL_LUTGHOLEIN && pQuestData->tPlayerGUIDs.nPlayerCount)
	{
		QUESTS_QuickRemovePlayerGUID(pQuestData, pQuestArg);
	}
}

//D2Game.0x6FCA4500
// 1.13c D2Game+0xC2DC0
void __fastcall ACT2Q5_Callback13_PlayerStartedGame(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pQuestArg->pPlayer)->pQuestData[pQuestArg->pGame->nDifficulty];

	if (!QUESTRECORD_GetQuestState(pQuestFlags, pQuestData->nQuestFilter, QFLAG_REWARDGRANTED)
		&& !QUESTRECORD_GetQuestState(pQuestFlags, pQuestData->nQuestFilter, QFLAG_COMPLETEDBEFORE)
		&& QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_STARTED) == 1)
	{
		pQuestData->fLastState = 2;
		pQuestData->fState = 1;
	}
}

//D2Game.0x6FCA4560
// 1.13c D2Game+0xC6600
void __fastcall ACT2Q5_OnSummonerActivated(D2GameStrc* pGame)
{
	D2QuestDataStrc* pQuestData = QUESTS_GetQuestData(pGame, QUEST_A2Q5_SUMMONER);
	if (!pQuestData || pQuestData->bNotIntro != 1)
	{
		return;
	}

	D2Act2Quest5Strc* pQuestDataEx = (D2Act2Quest5Strc*)pQuestData->pQuestDataEx;
	if (!pQuestDataEx->unk0x01)
	{
		pQuestDataEx->unk0x01 = 1;
	}

	if (!pQuestData->fState)
	{
		QUESTS_StateDebug(pQuestData, 1, __FILE__, __LINE__);
	}

	if (pQuestData->fLastState < 2)
	{
		pQuestData->dwFlags &= 0xFFFFFF00;
		QUESTS_UnitIterate(pQuestData, 2, 0, ACT2Q5_UnitIterate_StatusCyclerEx, 1);
		SUNIT_IterateUnitsOfType(pQuestData->pGame, 0, 0, ACT2Q5_UnitIterate_UpdateQuestStateFlags);
	}
}

//D2Game.0x6FCA45E0
// 1.13c D2Game+0xC3010
int32_t __fastcall ACT2Q5_UnitIterate_UpdateQuestStateFlags(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pUnit)->pQuestData[pGame->nDifficulty];
	if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_REWARDGRANTED) != 1 && QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_REWARDPENDING) != 1)
	{
		D2QuestDataStrc* pQuestData = QUESTS_GetQuestData(pGame, QUEST_A2Q5_SUMMONER);
		if (pQuestData && pQuestData->fState == 1)
		{
			QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q5, QFLAG_STARTED);
		}
	}

	return 0;
}

//
// 1.13c D2Game+0xC2F00
void __fastcall ACT2Q5_Callback10_PlayerLeavesGame(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	QUESTS_RemovePlayerGUID(pQuestData, pQuestArg);
}
