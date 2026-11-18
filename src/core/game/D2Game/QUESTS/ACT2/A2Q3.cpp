#include "QUESTS/ACT2/A2Q3.h"
#include <test_seam.h>

#include <DataTbls/LevelsIds.h>
#include <DataTbls/MonsterIds.h>
#include <DataTbls/ObjectsIds.h>
#include <Drlg/D2DrlgDrlg.h>
#include <D2Dungeon.h>
#include <D2Environment.h>
#include <D2Items.h>
#include <D2QuestRecord.h>

#include "GAME/Clients.h"
#include "GAME/Game.h"
#include "GAME/SCmd.h"
#include "ITEMS/Items.h"
#include "OBJECTS/ObjMode.h"
#include "QUESTS/Quests.h"
#include "QUESTS/ACT2/A2Q2.h"
#include "UNIT/Party.h"
#include "UNIT/SUnit.h"


//D2Game.0x6FD35AA8
D2NPCMessageTableStrc gpAct2Q3NpcMessages[] =
{
	{
		{
			{ MONSTER_ATMA, 344, 0, 2, },
			{ MONSTER_WARRIV2, 343, 0, 2, },
			{ MONSTER_GREIZ, 341, 0, 2, },
			{ MONSTER_ELZIX, 342, 0, 2, },
			{ MONSTER_DROGNAN, 348, 0, 1, },
			{ MONSTER_LYSANDER, 347, 0, 2, },
			{ MONSTER_CAIN2, 350, 0, 2, },
			{ MONSTER_MESHIF1, 349, 0, 2, },
			{ MONSTER_GEGLASH, 345, 0, 2, },
			{ MONSTER_JERHYN, 340, 0, 2, },
			{ MONSTER_FARA, 346, 0, 2, },
		},
		11
	},
	{
		{
			{ MONSTER_ATMA, 344, 0, 2, },
			{ MONSTER_WARRIV2, 343, 0, 2, },
			{ MONSTER_GREIZ, 341, 0, 2, },
			{ MONSTER_ELZIX, 342, 0, 2, },
			{ MONSTER_DROGNAN, 348, 0, 2, },
			{ MONSTER_LYSANDER, 347, 0, 2, },
			{ MONSTER_CAIN2, 350, 0, 2, },
			{ MONSTER_MESHIF1, 349, 0, 2, },
			{ MONSTER_GEGLASH, 345, 0, 2, },
			{ MONSTER_JERHYN, 340, 0, 2, },
			{ MONSTER_FARA, 346, 0, 2, },
		},
		11
	},
	{
		{
			{ MONSTER_ATMA, 361, 0, 2, },
			{ MONSTER_WARRIV2, 353, 0, 2, },
			{ MONSTER_GREIZ, 352, 0, 2, },
			{ MONSTER_ELZIX, 359, 0, 2, },
			{ MONSTER_DROGNAN, 358, 0, 2, },
			{ MONSTER_LYSANDER, 357, 0, 2, },
			{ MONSTER_CAIN2, 360, 0, 2, },
			{ MONSTER_MESHIF1, 355, 0, 2, },
			{ MONSTER_JERHYN, 351, 0, 2, },
			{ MONSTER_GEGLASH, 354, 0, 2, },
			{ MONSTER_FARA, 356, 0, 2, },
		},
		11
	},
	{
		{
			{ MONSTER_ATMA, 368, 0, 1, },
			{ MONSTER_WARRIV2, 366, 0, 1, },
			{ MONSTER_GREIZ, 363, 0, 1, },
			{ MONSTER_ELZIX, 364, 0, 1, },
			{ MONSTER_DROGNAN, 371, 0, 1, },
			{ MONSTER_LYSANDER, 370, 0, 1, },
			{ MONSTER_CAIN2, 372, 0, 1, },
			{ MONSTER_MESHIF1, 367, 0, 1, },
			{ MONSTER_GEGLASH, 365, 0, 1, },
			{ MONSTER_JERHYN, 362, 0, 1, },
		},
		10
	},
	{
		{
			{ MONSTER_ATMA, 368, 0, 2, },
			{ MONSTER_WARRIV2, 366, 0, 2, },
			{ MONSTER_GREIZ, 363, 0, 2, },
			{ MONSTER_JERHYN, 362, 0, 2, },
			{ MONSTER_ELZIX, 364, 0, 2, },
			{ MONSTER_DROGNAN, 371, 0, 2, },
			{ MONSTER_LYSANDER, 370, 0, 2, },
			{ MONSTER_MESHIF1, 367, 0, 2, },
			{ MONSTER_GEGLASH, 365, 0, 2, },
			{ MONSTER_FARA, 369, 0, 2, },
		},
		10
	},
	{
		{
			{ -1, 0, 0, 2 },
		},
		0
	}
};


// 1.13c D2Game+0xC3900
void __fastcall ACT2Q3_UnitIterate_SetPrimaryGoalDone(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	D2BitBufferStrc* pQuestFlags = D2_CALL(UNITS_GetPlayerData, pUnit)->pQuestData[pGame->nDifficulty];
	if (D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_REWARDGRANTED) != 1)
	{
		if (D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_REWARDPENDING) != 1)
		{
			D2ActiveRoomStrc* pRoom = D2_CALL(UNITS_GetRoom, pUnit);
			if (pRoom)
			{
				const int32_t nLevelId = D2_CALL(DUNGEON_GetLevelIdFromRoom, pRoom);
				if (nLevelId)
				{
					if (D2_CALL(DRLG_GetActNoFromLevelId, nLevelId) == ACT_II)
					{
						D2_CALL(QUESTRECORD_SetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_PRIMARYGOALDONE);
						D2_CALL(QUESTRECORD_SetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_REWARDPENDING);
					}
				}
			}
		}
	}
}

// 1.13c D2Game+0xC2250
bool __fastcall ACT2Q3_ActiveFilterCallback(D2QuestDataStrc* pQuest, int32_t nNpcId, D2UnitStrc* pPlayer, D2BitBufferStrc* pQuestFlags, D2UnitStrc* pNPC)
{
	if (QUESTRECORD_GetQuestState(pQuestFlags, pQuest->nQuestFilter, QFLAG_REWARDGRANTED) != 1)
	{
		if (QUESTRECORD_GetQuestState(pQuestFlags, pQuest->nQuestFilter, QFLAG_REWARDPENDING) == 1
			&& (nNpcId == MONSTER_ATMA || nNpcId == MONSTER_WARRIV2 || nNpcId == MONSTER_GREIZ || nNpcId == MONSTER_ELZIX
				|| nNpcId == MONSTER_DROGNAN || nNpcId == MONSTER_LYSANDER || nNpcId == MONSTER_CAIN2 || nNpcId == MONSTER_MESHIF1
				|| nNpcId == MONSTER_GEGLASH || nNpcId == MONSTER_JERHYN)
			|| (nNpcId == MONSTER_DROGNAN && pQuest->fState == 1))
		{
			return true;
		}
	}

	return false;
}

// 1.13c D2Game+0xC52B0
int32_t __fastcall ACT2Q3_UnitIterate_StatusCyclerEx(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pUnit)->pQuestData[pGame->nDifficulty];

	if ((QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_REWARDGRANTED) == 0 && QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_COMPLETEDBEFORE) == 0)
		|| QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_PRIMARYGOALDONE) != 0
		|| QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_COMPLETEDNOW) != 0)
	{
		QUESTS_StatusCyclerEx(pGame, pUnit, QUEST_A2Q3_TAINTEDSUN);
	}

	return 0;
}

// 1.13c D2Game+0xC5190
int32_t __fastcall ACT2Q3_UnitIterate_UpdateClient(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	D2ClientStrc* pClient = SUNIT_GetClientFromPlayer(pUnit, __FILE__, __LINE__);
	D2_ASSERT(pClient);
	if (pClient->nAct == ACT_II)
	{
		D2GSPacketSrv53 packet53 = {};
		packet53.nHeader = 0x53;
		packet53.unk0x01 = 5;
		packet53.unk0x05 = 0;
		packet53.unk0x09 = 1;
		D2GAME_PACKETS_SendPacket_6FC3C710(pClient, &packet53, sizeof(packet53));

		D2GSPacketSrv5D packet5D = {};
		packet5D.field_4 = 0;
		packet5D.nHeader = 0x5D;
		packet5D.nQuestId = QUEST_A2Q3_TAINTEDSUN;
		packet5D.nFlags = 16;
		D2GAME_PACKETS_SendPacket_6FC3C710(pClient, &packet5D, sizeof(packet5D));
	}

	return 0;
}

// 1.13c D2Game+0xC7480
void __fastcall ACT2Q3_InitQuestData(D2QuestDataStrc* pQuestData)
{
	memset(pQuestData->pfCallback, 0x00, sizeof(pQuestData->pfCallback));
	pQuestData->pfCallback[QUESTEVENT_NPCACTIVATE] = ACT2Q3_Callback00_NpcActivate;
	pQuestData->pfCallback[QUESTEVENT_SCROLLMESSAGE] = ACT2Q3_Callback11_ScrollMessage;
	pQuestData->pfCallback[QUESTEVENT_CHANGEDLEVEL] = ACT2Q3_Callback03_ChangedLevel;
	pQuestData->pfCallback[QUESTEVENT_PLAYERLEAVESGAME] = ACT2Q3_Callback10_PlayerLeavesGame;
	pQuestData->pfCallback[QUESTEVENT_PLAYERSTARTEDGAME] = ACT2Q3_Callback13_PlayerStartedGame;
	pQuestData->pNPCMessages = gpAct2Q3NpcMessages;
	pQuestData->bActive = 1;
	pQuestData->fState = 0;

	D2Act2Quest3Strc* pQuestDataEx = D2_ALLOC_STRC_POOL(pQuestData->pGame->pMemoryPool, D2Act2Quest3Strc);
	pQuestData->nQuestFilter = QUESTSTATEFLAG_A2Q3;
	pQuestData->nSeqId = 11;
	pQuestData->pfStatusFilter = 0;
	pQuestData->pQuestDataEx = pQuestDataEx;
	pQuestData->nInitNo = 4;
	pQuestData->pfActiveFilter = ACT2Q3_ActiveFilterCallback;
	pQuestData->pfSeqFilter = ACT2Q3_SeqCallback;

	memset(pQuestDataEx, 0x00, sizeof(D2Act2Quest3Strc));
	pQuestDataEx->nAltarMode = 0;
	pQuestDataEx->tPlayerGUIDs.nPlayerCount = 0;
}

// 1.13c D2Game+0xC6B30
void __fastcall ACT2Q3_Callback11_ScrollMessage(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	const uint16_t nMessageIndex = (uint16_t)pQuestArg->nMessageIndex;
	if (nMessageIndex == 348)
	{
		if (pQuestData->bNotIntro && pQuestArg->nNPCNo == MONSTER_DROGNAN)
		{
			if (pQuestData->fLastState == 1)
			{
				pQuestData->dwFlags &= 0xFFFFFF00;
				pQuestData->fLastState = 2;
			}

			if (pQuestData->fState == 1)
			{
				QUESTS_StateDebug(pQuestData, 2, __FILE__, __LINE__);
				SUNIT_IterateUnitsOfType(pQuestData->pGame, 0, 0, ACT2Q3_UnitIterate_UpdateQuestStateFlags);
				return;
			}
		}
	}
	else if (nMessageIndex > 361 && nMessageIndex < 373)
	{
		D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pQuestArg->pPlayer)->pQuestData[pQuestArg->pGame->nDifficulty];
		if (pQuestData->fState != 5 && pQuestData->bNotIntro == 1 && QUESTRECORD_GetQuestState(pQuestFlags, pQuestData->nQuestFilter, QFLAG_PRIMARYGOALDONE) != 0)
		{
			pQuestData->dwFlags &= 0xFFFFFF00;
			pQuestData->fLastState = 13;
			QUESTS_StateDebug(pQuestData, 5, __FILE__, __LINE__);
			if (IsBadCodePtr((FARPROC)pQuestData->pfSeqFilter))
			{
				FOG_DisplayAssert("pQuestInfo->pSequence", __FILE__, __LINE__);
				exit(-1);
			}
			pQuestData->pfSeqFilter(pQuestData);
		}

		QUESTS_NPCActivateSpeeches(pQuestArg->pGame, pQuestArg->pPlayer, pQuestArg->pTarget);

		if (QUESTRECORD_GetQuestState(pQuestFlags, pQuestData->nQuestFilter, QFLAG_REWARDPENDING) != 0)
		{
			QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_REWARDGRANTED);
			QUESTRECORD_ClearQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_REWARDPENDING);
			QUESTS_AddPlayerGUID(&pQuestData->tPlayerGUIDs, (pQuestArg->pPlayer ? pQuestArg->pPlayer->dwUnitId : -1));
			if (!pQuestData->bNotIntro)
			{
				QUESTS_SetGlobalState(pQuestData->pGame, QUESTSTATEFLAG_A2Q3, QFLAG_PRIMARYGOALDONE);
			}
		}
	}
}

// 1.13c D2Game+0xC3970
void __fastcall ACT2Q3_Callback10_PlayerLeavesGame(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	D2_CALL(QUESTS_RemovePlayerGUID, pQuestData, pQuestArg);

	D2Act2Quest3Strc* pQuestDataEx = (D2Act2Quest3Strc*)pQuestData->pQuestDataEx;
	D2_CALL(QUESTS_FastRemovePlayerGUID, &pQuestDataEx->tPlayerGUIDs, pQuestArg->pPlayer ? pQuestArg->pPlayer->dwUnitId : -1);
}

// 1.13c D2Game+0xC37F0
void __fastcall ACT2Q3_Callback00_NpcActivate(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	D2BitBufferStrc* pQuestFlags = D2_CALL(UNITS_GetPlayerData, pQuestArg->pPlayer)->pQuestData[pQuestArg->pGame->nDifficulty];

	int32_t nNpcId = -1;
	if (pQuestArg->pTarget)
	{
		nNpcId = pQuestArg->pTarget->dwClassId;
	}

	if (D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_REWARDPENDING) != 0)
	{
		D2_CALL(QUESTS_InitScrollTextChain, pQuestData, pQuestArg->pTextControl, nNpcId, 3);
		return;
	}

	if (D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_COMPLETEDNOW) == 0)
	{
		const int32_t nPlayerGUID = pQuestArg->pPlayer ? pQuestArg->pPlayer->dwUnitId : -1;
		for (int32_t i = 0; i < pQuestData->tPlayerGUIDs.nPlayerCount; ++i)
		{
			if (pQuestData->tPlayerGUIDs.nPlayerGUIDs[i] == nPlayerGUID)
			{
				D2_CALL(QUESTS_InitScrollTextChain, pQuestData, pQuestArg->pTextControl, nNpcId, 4);
				return;
			}
		}

		static const int32_t nIndices[] = { -1, 0, 1, 2, 3, 0 };
		if (pQuestData->fState != 0 && pQuestData->bNotIntro != 0
			&& D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, pQuestData->nQuestFilter, QFLAG_REWARDGRANTED) != 1
			&& pQuestData->fState < 4 && nIndices[pQuestData->fState] != -1 && (uint32_t)nIndices[pQuestData->fState] < 6)
		{
			D2_CALL(QUESTS_InitScrollTextChain, pQuestData, pQuestArg->pTextControl, nNpcId, nIndices[pQuestData->fState]);
		}
	}
}

// 1.13c D2Game+0xC6A30
// The darken sequence shared by the darken timer, the Lut Gholein arrival and the late-join path; returns whether the sun was darkened now.
static bool ACT2Q3_StartDarken(D2QuestDataStrc* pQuestData)
{
	pQuestData->dwFlags &= 0xFFFFFF00;
	QUESTS_UnitIterate(pQuestData, 1, 0, ACT2Q3_UnitIterate_StatusCyclerEx, 1);
	if (!pQuestData->fState)
	{
		QUESTS_StateDebug(pQuestData, 1, __FILE__, __LINE__);
	}

	auto* pActII = pQuestData->pGame->pAct[ACT_II];
	if (!pActII)
	{
		((D2Act2Quest3Strc*)pQuestData->pQuestDataEx)->bBlackened = 1;
		return false;
	}

	ENVIRONMENT_TaintedSunBegin(pActII);
	SUNIT_IterateUnitsOfType(pQuestData->pGame, 0, 0, ACT2Q3_UnitIterate_UpdateClient);
	return true;
}

// 1.13c D2Game+0xC7250
static void ACT2Q3_CreateDarkenTimer(D2QuestDataStrc* pQuestData)
{
	D2Act2Quest3Strc* pQuestDataEx = (D2Act2Quest3Strc*)pQuestData->pQuestDataEx;
	if (!pQuestData->fState && !pQuestDataEx->bDarkenTimerCreated)
	{
		D2SeedStrc* pSeed = QUESTS_GetGlobalSeed(pQuestData->pGame);
		QUESTS_CreateTimer(pQuestData, ACT2Q3_DarkenEnvironment, (SEED_RollRandomNumber(pSeed) & 1) + 15);
		pQuestDataEx->bDarkenTimerCreated = 1;
	}
}

// 1.13c D2Game+0xC73A0
void __fastcall ACT2Q3_Callback03_ChangedLevel(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	D2Act2Quest3Strc* pQuestDataEx = (D2Act2Quest3Strc*)pQuestData->pQuestDataEx;
	const int32_t nOldLevel = pQuestArg->nOldLevel;
	const int32_t nNewLevel = pQuestArg->nNewLevel;

	if ((nNewLevel == LEVEL_LOSTCITY || nNewLevel == LEVEL_VALLEYOFSNAKES) && !pQuestData->fState && pQuestData->bNotIntro == 1)
	{
		ACT2Q3_CreateDarkenTimer(pQuestData);
	}

	if (nNewLevel == LEVEL_LUTGHOLEIN && pQuestDataEx->bBlackened == 1 && ACT2Q3_StartDarken(pQuestData))
	{
		SUNIT_IterateUnitsOfType(pQuestData->pGame, 0, 0, ACT2Q3_UnitIterate_UpdateQuestStateFlags);
		pQuestDataEx->bBlackened = 0;
	}

	if (nOldLevel == LEVEL_LUTGHOLEIN)
	{
		if (pQuestData->tPlayerGUIDs.nPlayerCount)
		{
			QUESTS_QuickRemovePlayerGUID(pQuestData, pQuestArg);
		}

		if (pQuestData->fState == 2)
		{
			QUESTS_StateDebug(pQuestData, 3, __FILE__, __LINE__);
		}
	}
}

// 1.13c D2Game+0xC6AF0
bool __fastcall ACT2Q3_DarkenEnvironment(D2GameStrc* pGame, D2QuestDataStrc* pQuestData)
{
	D2Act2Quest3Strc* pQuestDataEx = (D2Act2Quest3Strc*)pQuestData->pQuestDataEx;

	pQuestDataEx->bDarkenTimerCreated = 0;
	if (!pQuestDataEx->bRewarded)
	{
		if (!ACT2Q3_StartDarken(pQuestData))
		{
			return true;
		}

		pQuestDataEx->bRewarded = 1;
		SUNIT_IterateUnitsOfType(pQuestData->pGame, 0, 0, ACT2Q3_UnitIterate_UpdateQuestStateFlags);
	}

	return true;
}

// 1.13c D2Game+0xC21E0
bool __fastcall ACT2Q3_SeqCallback(D2QuestDataStrc* pQuestData)
{
	if (pQuestData->fState != 5 && pQuestData->bNotIntro != 0)
	{
		return true;
	}

	D2QuestDataStrc* pQuest = QUESTS_GetQuestData(pQuestData->pGame, pQuestData->nSeqId);
	if (!pQuest)
	{
		return false;
	}

	if (IsBadCodePtr((FARPROC)pQuest->pfSeqFilter))
	{
		FOG_DisplayAssert("pQuestInfo->pSequence", __FILE__, __LINE__);
		exit(-1);
	}
	return pQuest->pfSeqFilter(pQuest);
}

// 1.13c D2Game+0xC3750
int32_t __fastcall ACT2Q3_UnitIterate_UpdateQuestStateFlags(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	D2BitBufferStrc* pQuestFlags = D2_CALL(UNITS_GetPlayerData, pUnit)->pQuestData[pGame->nDifficulty];
	if (D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_REWARDGRANTED) != 1)
	{
		if (D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_REWARDPENDING) != 1)
		{
			D2QuestDataStrc* pQuestData = QUESTS_GetQuestData(pGame, QUEST_A2Q3_TAINTEDSUN);
			if (!pQuestData)
			{
				return 0;
			}

			if (pQuestData->fState != 0)
			{
				QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_STARTED);
			}

			if (pQuestData->fState == 2)
			{
				QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_LEAVETOWN);
				return 0;
			}

			if (pQuestData->fState == 3)
			{
				QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_ENTERAREA);
			}
		}
	}

	return 0;
}

// 1.13c D2Game+0xC72C0 (the started-game darken, retail D2Game+0xC7130, is inlined)
void __fastcall ACT2Q3_Callback13_PlayerStartedGame(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pQuestArg->pPlayer)->pQuestData[pQuestArg->pGame->nDifficulty];
	if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_REWARDGRANTED) == 1 || QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_REWARDPENDING) == 1)
	{
		QUESTS_SetGlobalState(pQuestData->pGame, QUESTSTATEFLAG_A2Q3, QFLAG_PRIMARYGOALDONE);
		return;
	}

	if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_STARTED) != 1)
	{
		return;
	}

	D2Act2Quest3Strc* pQuestDataEx = (D2Act2Quest3Strc*)pQuestData->pQuestDataEx;
	if (ACT2Q3_StartDarken(pQuestData))
	{
		pQuestDataEx->bRewarded = 1;
	}
	else
	{
		pQuestDataEx->bBlackened = 1;

		D2GSPacketSrv5D packet5D = {};
		packet5D.field_4 = 0;
		packet5D.nHeader = 0x5D;
		packet5D.nQuestId = QUEST_A2Q3_TAINTEDSUN;
		packet5D.nFlags = 1;
		D2GAME_PACKETS_SendPacket_6FC3C710(SUNIT_GetClientFromPlayer(pQuestArg->pPlayer, __FILE__, __LINE__), &packet5D, sizeof(packet5D));
	}

	if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_ENTERAREA) == 0)
	{
		if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_LEAVETOWN) == 0)
		{
			pQuestData->fLastState = 1;
			pQuestData->fState = 1;
			return;
		}

		pQuestData->fLastState = 2;
		pQuestData->fState = 2;
		return;
	}

	pQuestData->fLastState = 2;
	pQuestData->fState = 3;
}

// 1.13c D2Game+0xC76B0
int32_t __fastcall OBJECTS_OperateFunction24_TaintedSunAltar(D2ObjOperateFnStrc* pOp, int32_t nOperate)
{
	D2UnitStrc* pObject = pOp->pObject;

	D2QuestDataStrc* pQuestData = QUESTS_GetQuestData(pOp->pGame, QUEST_A2Q3_TAINTEDSUN);
	if (!pQuestData)
	{
		return 0;
	}

	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pOp->pPlayer)->pQuestData[pOp->pGame->nDifficulty];
	if ((QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_REWARDPENDING) == 1 || QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_REWARDGRANTED) == 1)
		&& (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q2, QFLAG_REWARDGRANTED) == 1 || ITEMS_FindQuestItem(pOp->pGame, pOp->pPlayer, ' piv') || ITEMS_FindQuestItem(pOp->pGame, pOp->pPlayer, ' tsh')))
	{
		SUNIT_AttachSound(pOp->pPlayer, 19, pOp->pPlayer);
		return 0;
	}

	if (pObject && pObject->dwAnimMode != OBJMODE_NEUTRAL)
	{
		return 0;
	}

	D2Act2Quest3Strc* pQuestDataEx = (D2Act2Quest3Strc*)pQuestData->pQuestDataEx;

	if (!pQuestData->bNotIntro)
	{
		UNITS_ChangeAnimMode(pObject, OBJMODE_OPERATING);
		pQuestDataEx->bAltarDestroyed = 1;
		pQuestDataEx->nAltarMode = OBJMODE_OPENED;
		pQuestDataEx->nViperAmuletDropCount = 0;
		SUNIT_IterateUnitsOfType(pOp->pGame, 0, pOp->pPlayer, ACT2Q3_UnitIterate_DetermineViperAmuletDropCount);

		D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pObject);
		int32_t nItemLevel = DUNGEON_GetLevelIdFromRoom(pRoom);

		int32_t nAmuletsDropped = 0;
		pObject->dwDropItemCode = ' piv';
		for (int32_t i = 0; i < pQuestDataEx->nViperAmuletDropCount; ++i)
		{
			D2UnitStrc* pItem = D2GAME_DropItemAtUnit_6FC4FEC0(pOp->pGame, pObject, ITEMQUAL_UNIQUE, &nItemLevel, 0, -1, 0);
			if (pItem)
			{
				ITEMS_SetItemFlag(pItem, IFLAG_IDENTIFIED, 1);
				++nAmuletsDropped;
			}
		}
		OBJMODE_DropFromChestTCWithQuality(pOp, ITEMQUAL_MAGIC);

		D2SeedStrc* pSeed = QUESTS_GetGlobalSeed(pOp->pGame);

		// retail rolls the gold-drop count once, before the loop
		for (uint32_t nGoldDrops = ITEMS_RollRandomNumber(pSeed) % 5 + 5; nGoldDrops; --nGoldDrops)
		{
			OBJMODE_DropItemWithCodeAndQuality(pOp->pGame, pOp->pObject, ' dlg', 2);
		}

		D2QuestDataStrc* pQuestDataStaff = QUESTS_GetQuestData(pOp->pGame, QUEST_A2Q2_HORADRICSTAFF);
		if (pQuestDataStaff)
		{
			((D2Act2Quest2Strc*)pQuestDataStaff->pQuestDataEx)->nAmulets += nAmuletsDropped;
		}
		return 0;
	}

	D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pOp->pObject);
	pQuestDataEx->pRoom = pRoom;
	pQuestDataEx->nAltarLevelId = DUNGEON_GetLevelIdFromRoom(pRoom);
	UNITS_ChangeAnimMode(pOp->pObject, OBJMODE_OPERATING);

	pQuestDataEx->nAltarMode = OBJMODE_OPENED;
	pQuestDataEx->bAltarInitialized = 1;
	pQuestDataEx->nAltarGUID = pObject ? pObject->dwUnitId : -1;
	pQuestDataEx->bAltarDestroyed = 1;

	if (pQuestData->bNotIntro)
	{
		// retail D2Game+0xC5E90 (the sun is restored and the clients told), inlined here
		if (pQuestData->pGame->pAct[ACT_II])
		{
			pQuestDataEx->bRewarded = 0;
			ENVIRONMENT_TaintedSunEnd(pQuestData->pGame->pAct[ACT_II]);
			SUNIT_IterateUnitsOfType(pQuestData->pGame, 0, 0, ACT2Q3_UnitIterate_SendPacket0x53ToClient);
		}

		QUESTS_DebugOutput(pQuestData->pGame, "Finished quest", "..\\D2Game/Quests/a2q3.cpp", 991);
		QUESTS_StateDebug(pQuestData, 4, "..\\D2Game/Quests/a2q3.cpp", 992);
		QUESTS_TriggerFX(pQuestData->pGame, 6);
	}

	pQuestDataEx->nViperAmuletDropCount = 0;
	SUNIT_IterateUnitsOfType(pOp->pGame, 0, pOp->pPlayer, ACT2Q3_UnitIterate_DetermineViperAmuletDropCount);

	if (pQuestData->bNotIntro)
	{
		if (!QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_REWARDGRANTED))
		{
			QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_REWARDPENDING);
			QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_PRIMARYGOALDONE);
			QUESTRECORD_ClearQuestState(pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_COMPLETEDNOW);
		}

		SUNIT_IterateUnitsOfType(pOp->pGame, 0, pOp->pObject, ACT2Q3_UnitIterate_SetRewardPending);
		SUNIT_IterateUnitsOfType(pOp->pGame, 0, pOp->pObject, ACT2Q3_UnitIterate_SetPrimaryGoalDoneForPartyMembers);
		SUNIT_IterateUnitsOfType(pOp->pGame, 0, pOp->pObject, ACT2Q3_UnitIterate_AttachCompletionSound);
		QUESTS_SetGlobalState(pOp->pGame, pQuestData->nQuestFilter, QFLAG_PRIMARYGOALDONE);
	}

	int32_t nItemLevel = DUNGEON_GetLevelIdFromRoom(pQuestDataEx->pRoom);

	int32_t nAmuletsDropped = 0;
	pObject->dwDropItemCode = ' piv';
	for (int32_t i = 0; i < pQuestDataEx->nViperAmuletDropCount; ++i)
	{
		D2UnitStrc* pItem = D2GAME_DropItemAtUnit_6FC4FEC0(pOp->pGame, pObject, ITEMQUAL_UNIQUE, &nItemLevel, 0, -1, 0);
		if (pItem)
		{
			ITEMS_SetItemFlag(pItem, IFLAG_IDENTIFIED, 1);
			++nAmuletsDropped;
		}
	}

	ACT2Q2_AddAmulets_C2380(pQuestData, nAmuletsDropped);

	OBJMODE_DropFromChestTCWithQuality(pOp, ITEMQUAL_MAGIC);

	D2SeedStrc* pSeed = QUESTS_GetGlobalSeed(pOp->pGame);

	// retail rolls the gold-drop count once, before the loop
	for (uint32_t nGoldDrops = ITEMS_RollRandomNumber(pSeed) % 5 + 5; nGoldDrops; --nGoldDrops)
	{
		OBJMODE_DropItemWithCodeAndQuality(pOp->pGame, pObject, ' dlg', 2);
	}

	SUNIT_IterateUnitsOfType(pQuestData->pGame, 0, 0, ACT2Q3_UnitIterate_SetCompletionFlag);
	QUESTS_CreateTimer(pQuestData, ACT2Q3_Timer_StatusCycler, 10);

	return 0;
}

// 1.13c D2Game+0xC5260
int32_t __fastcall ACT2Q3_UnitIterate_SetCompletionFlag(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	D2BitBufferStrc* pQuestFlags = D2_CALL(UNITS_GetPlayerData, pUnit)->pQuestData[pGame->nDifficulty];

	if (D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_REWARDGRANTED) != 1)
	{
		if (D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_REWARDPENDING) != 1)
		{
			D2_CALL(QUESTRECORD_SetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_COMPLETEDNOW);
			D2_CALL(QUESTS_SendLogUpdateEx, pUnit, QUEST_A2Q3_TAINTEDSUN, 0);
		}
	}

	return 0;
}

// 1.13c D2Game+0xC5ED0
int32_t __fastcall ACT2Q3_UnitIterate_SetPrimaryGoalDoneForPartyMembers(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	if (QUESTRECORD_GetQuestState(UNITS_GetPlayerData(pUnit)->pQuestData[pGame->nDifficulty], QUESTSTATEFLAG_A2Q3, QFLAG_PRIMARYGOALDONE) != 0)
	{
		const int16_t nPartyId = SUNIT_GetPartyId(pUnit);
		if (nPartyId != -1)
		{
			PARTY_IteratePartyMembers(pGame, nPartyId, ACT2Q3_UnitIterate_SetPrimaryGoalDone, 0);
		}
	}

	return 0;
}

// 1.13c D2Game+0xC5100
int32_t __fastcall ACT2Q3_UnitIterate_SendPacket0x53ToClient(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	D2ClientStrc* pClient = SUNIT_GetClientFromPlayer(pUnit, __FILE__, __LINE__);
	D2_ASSERT(pClient);

	if (pClient->nAct == ACT_II)
	{
		D2GSPacketSrv53 pPacket53 = {};
		pPacket53.nHeader = 0x53;
		pPacket53.unk0x01 = 2;
		pPacket53.unk0x05 = 0;
		pPacket53.unk0x09 = 0;
		D2GAME_PACKETS_SendPacket_6FC3C710(pClient, &pPacket53, sizeof(pPacket53));
	}

	return 0;
}

// 1.13c D2Game+0xC36B0
int32_t __fastcall ACT2Q3_UnitIterate_SetRewardPending(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	D2BitBufferStrc* pQuestFlags = D2_CALL(UNITS_GetPlayerData, pUnit)->pQuestData[pGame->nDifficulty];
	D2ActiveRoomStrc* pRoom = D2_CALL(UNITS_GetRoom, pUnit);
	D2QuestDataStrc* pQuestData = QUESTS_GetQuestData(pGame, QUEST_A2Q3_TAINTEDSUN);

	if (pRoom && pQuestData
		&& ((D2Act2Quest3Strc*)pQuestData->pQuestDataEx)->nAltarLevelId == D2_CALL(DUNGEON_GetLevelIdFromRoom, pRoom)
		&& D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_REWARDGRANTED) != 1
		&& D2_CALL(QUESTRECORD_GetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_COMPLETEDBEFORE) != 1)
	{
		D2_CALL(QUESTRECORD_SetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_PRIMARYGOALDONE);
		D2_CALL(QUESTRECORD_SetQuestState, pQuestFlags, QUESTSTATEFLAG_A2Q3, QFLAG_REWARDPENDING);
	}

	return 0;
}

// 1.13c D2Game+0xC5E30
bool __fastcall ACT2Q3_Timer_StatusCycler(D2GameStrc* pGame, D2QuestDataStrc* pQuestData)
{
	D2Act2Quest3Strc* pQuestDataEx = (D2Act2Quest3Strc*)pQuestData->pQuestDataEx;

	if (pQuestData->fState == 4)
	{
		pQuestData->dwFlags &= 0xFFFFFF00;
		pQuestData->fLastState = 3;
		SUNIT_IterateUnitsOfType(pGame, 0, 0, ACT2Q3_UnitIterate_StatusCyclerEx);
	}
	pQuestDataEx->unk0x06 = 0;

	return 1;
}

// 1.13c D2Game+0xC5090
int32_t __fastcall ACT2Q3_UnitIterate_AttachCompletionSound(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	if (QUESTRECORD_GetQuestState(UNITS_GetPlayerData(pUnit)->pQuestData[pGame->nDifficulty], QUESTSTATEFLAG_A2Q3, QFLAG_PRIMARYGOALDONE) != 0)
	{
		SUNIT_AttachSound(pUnit, 52, pUnit);
	}

	return 0;
}

// 1.13c D2Game+0xC3630
int32_t __fastcall ACT2Q3_UnitIterate_DetermineViperAmuletDropCount(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	if (!ITEMS_FindQuestItem(pGame, pUnit, ' piv') && !ITEMS_FindQuestItem(pGame, pUnit, ' tsh'))
	{
		if (QUESTRECORD_GetQuestState(UNITS_GetPlayerData(pUnit)->pQuestData[pGame->nDifficulty], QUESTSTATEFLAG_A2Q2, QFLAG_REWARDGRANTED) == 0)
		{
			D2QuestDataStrc* pQuestData = QUESTS_GetQuestData(pGame, QUEST_A2Q3_TAINTEDSUN);
			if (pQuestData)
			{
				++((D2Act2Quest3Strc*)pQuestData->pQuestDataEx)->nViperAmuletDropCount;
			}
		}
	}

	return 0;
}

// 1.13c D2Game+0xC2150
void __fastcall ACT2Q3_BrightenEnvironment(D2GameStrc* pGame, uint8_t nActNo)
{
	if (nActNo == ACT_II)
	{
		D2QuestDataStrc* pQuestData = QUESTS_GetQuestData(pGame, QUEST_A2Q3_TAINTEDSUN);
		if (pQuestData)
		{
			D2Act2Quest3Strc* pQuestDataEx = (D2Act2Quest3Strc*)pQuestData->pQuestDataEx;
			if (pQuestDataEx->bBlackened == 1)
			{
				ENVIRONMENT_TaintedSunBegin(pGame->pAct[ACT_II]);
				pQuestDataEx->bBlackened = 0;
				pQuestDataEx->bRewarded = 1;
			}
		}
	}
}
