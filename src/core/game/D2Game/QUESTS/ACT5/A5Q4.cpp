#include "QUESTS/ACT5/A5Q4.h"
#include <test_seam.h>

#include <DataTbls/LevelsIds.h>
#include <DataTbls/MonsterIds.h>
#include <DataTbls/ObjectsIds.h>
#include <Drlg/D2DrlgDrlg.h>
#include <D2Dungeon.h>
#include <D2QuestRecord.h>
#include <D2Waypoints.h>

#include "GAME/Clients.h"
#include "GAME/Game.h"
#include "QUESTS/Quests.h"
#include "QUESTS/ACT5/A5Q3.h"
#include "SKILLS/Skills.h"
#include "UNIT/Party.h"
#include "UNIT/SUnit.h"


//D2Game.0x6FD3D828
D2NPCMessageTableStrc gpAct5Q4NpcMessages[] =
{
	{
		{
			{ MONSTER_DREHYA, 20137, 0, 0, },
		},
		1
	},
	{
		{
			{ MONSTER_LARZUK, 20141, 0, 2, },
			{ MONSTER_CAIN6, 20139, 0, 2, },
			{ MONSTER_MALAH, 20140, 0, 2, },
			{ MONSTER_QUAL_KEHK, 20142, 0, 2, },
		},
		4
	},
	{
		{
			{ MONSTER_LARZUK, 20145, 0, 2, },
			{ MONSTER_CAIN6, 20144, 0, 2, },
			{ MONSTER_DREHYA, 20143, 0, 2, }, // 1.13c: Anya's lines, not Nihlathak's (also the two below)
			{ MONSTER_MALAH, 20146, 0, 2, },
			{ MONSTER_QUAL_KEHK, 20147, 0, 2, },
		},
		5
	},
	{
		{
			{ MONSTER_LARZUK, 20150, 0, 2, },
			{ MONSTER_CAIN6, 20149, 0, 2, },
			{ MONSTER_DREHYA, 20148, 0, 0, },
			{ MONSTER_MALAH, 20151, 0, 2, },
			{ MONSTER_QUAL_KEHK, 20152, 0, 2, },
		},
		5
	},
	{
		{
			{ MONSTER_LARZUK, 20150, 0, 2, },
			{ MONSTER_CAIN6, 20149, 0, 2, },
			{ MONSTER_DREHYA, 20148, 0, 2, },
			{ MONSTER_MALAH, 20151, 0, 2, },
			{ MONSTER_QUAL_KEHK, 20152, 0, 2, },
		},
		5
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


//D2Game.0x6FCB59B0
// 1.13c D2Game+0x57830 (checked line by line against the decompile; no delta)
void __fastcall ACT5Q4_UnitIterate_SetPrimaryGoalDone(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pUnit)->pQuestData[pGame->nDifficulty];
	if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_REWARDGRANTED) == 1 || QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_COMPLETEDNOW) == 1)
	{
		return;
	}

	if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_REWARDPENDING) == 1 || QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_PRIMARYGOALDONE) == 1)
	{
		return;
	}

	D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pUnit);
	if (!pRoom)
	{
		return;
	}

	const int32_t nLevelId = DUNGEON_GetLevelIdFromRoom(pRoom);
	if (!nLevelId || DRLG_GetActNoFromLevelId(nLevelId) != ACT_V || (!QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q3, QFLAG_REWARDGRANTED) && !QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q3, QFLAG_REWARDPENDING)))
	{
		return;
	}

	if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_REWARDGRANTED) || QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_REWARDPENDING))
	{
		return;
	}

	QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_REWARDPENDING);
	QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_PRIMARYGOALDONE);
}

//D2Game.0x6FCB5A80
// 1.13c D2Game+0x56840 (no delta)
bool __fastcall ACT5Q4_ActiveFilterCallback(D2QuestDataStrc* pQuest, int32_t nNpcId, D2UnitStrc* pPlayer, D2BitBufferStrc* pQuestFlags, D2UnitStrc* pNPC)
{
	if (nNpcId != MONSTER_DREHYA)
	{
		return false;
	}

	if (pQuest->fState == 1)
	{
		if (!QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q3, QFLAG_REWARDGRANTED) && !QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q3, QFLAG_REWARDPENDING)
			|| QUESTRECORD_GetQuestState(pQuestFlags, pQuest->nQuestFilter, QFLAG_REWARDGRANTED))
		{
			return false;
		}

		if (!QUESTRECORD_GetQuestState(pQuestFlags, pQuest->nQuestFilter, QFLAG_REWARDPENDING))
		{
			return true;
		}
	}
	else
	{
		if (!QUESTRECORD_GetQuestState(pQuestFlags, pQuest->nQuestFilter, QFLAG_PRIMARYGOALDONE))
		{
			return false;
		}

		if (!QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_ENTERAREA))
		{
			return true;
		}
	}

	return false;
}

//D2Game.0x6FCB5C00
// 1.13c D2Game+0x5D5C0
void __fastcall ACT5Q4_InitQuestData(D2QuestDataStrc* pQuestData)
{
	memset(pQuestData->pfCallback, 0x00, sizeof(pQuestData->pfCallback));
	pQuestData->fState = 0;
	pQuestData->pfCallback[QUESTEVENT_PLAYERLEAVESGAME] = ACT5Q4_Callback10_PlayerLeavesGame;
	pQuestData->pfCallback[QUESTEVENT_CHANGEDLEVEL] = ACT5Q4_Callback03_ChangedLevel;
	pQuestData->pfCallback[QUESTEVENT_MONSTERKILLED] = ACT5Q4_Callback08_MonsterKilled;
	pQuestData->pfCallback[QUESTEVENT_NPCACTIVATE] = ACT5Q4_Callback00_NpcActivate;
	pQuestData->pfCallback[QUESTEVENT_NPCDEACTIVATE] = ACT5Q4_Callback02_NpcDeactivate;
	pQuestData->pfCallback[QUESTEVENT_SCROLLMESSAGE] = ACT5Q4_Callback11_ScrollMessage;
	pQuestData->pfCallback[QUESTEVENT_PLAYERSTARTEDGAME] = ACT5Q4_Callback13_PlayerStartedGame;
	pQuestData->pNPCMessages = gpAct5Q4NpcMessages;
	pQuestData->bActive = 1;
	
	D2Act5Quest4Strc* pQuestDataEx = D2_ALLOC_STRC_POOL(pQuestData->pGame->pMemoryPool, D2Act5Quest4Strc);
	pQuestData->pQuestDataEx = pQuestDataEx;
	pQuestData->nQuestFilter = QUESTSTATEFLAG_A5Q4;
	pQuestData->pfStatusFilter = ACT5Q4_StatusFilterCallback;
	pQuestData->nInitNo = 4;
	pQuestData->pfActiveFilter = ACT5Q4_ActiveFilterCallback;
	pQuestData->pfSeqFilter = ACT5Q4_SeqCallback;
	pQuestData->nSeqId = 35;
	memset(pQuestDataEx, 0x00, sizeof(D2Act5Quest4Strc));
	QUESTS_ResetPlayerGUIDCount(&pQuestDataEx->tPlayerGUIDs);
}

//D2Game.0x6FCB5CD0
// 1.13c D2Game+0x568C0 (no delta)
bool __fastcall ACT5Q4_StatusFilterCallback(D2QuestDataStrc* pQuest, D2UnitStrc* pPlayer, D2BitBufferStrc* pGlobalFlags, D2BitBufferStrc* pFlags, uint8_t* pStatus)
{
	*pStatus = 0;

	if (QUESTRECORD_GetQuestState(pFlags, QUESTSTATEFLAG_A5Q4, QFLAG_REWARDGRANTED))
	{
		return true;
	}

	if (QUESTRECORD_GetQuestState(pFlags, QUESTSTATEFLAG_A5Q4, QFLAG_REWARDPENDING) || QUESTRECORD_GetQuestState(pFlags, QUESTSTATEFLAG_A5Q4, QFLAG_PRIMARYGOALDONE))
	{
		*pStatus = (QUESTRECORD_GetQuestState(pFlags, QUESTSTATEFLAG_A5Q4, QFLAG_ENTERAREA) != 0) + 4;
	}
	else if (pQuest->bNotIntro)
	{
		if (QUESTRECORD_GetQuestState(pFlags, QUESTSTATEFLAG_A5Q4, QFLAG_COMPLETEDNOW))
		{
			*pStatus = 12;
		}
		else if (pQuest->fState < 4)
		{
			*pStatus = pQuest->fLastState;
		}
	}

	return true;
}

//D2Game.0x6FCB5D60
// 1.13c D2Game+0x5D520. Retail's portal spawn is the helper at 0x5D380, which inlines
// D2GAME_CreatePortalObject_6FD13DF0 (same x+10 / y+5 offsets, permanent portal object 60, destination 0x79) and sets
// +0x88 / clears +0x87 on success; the call below is that helper.
void __fastcall ACT5Q4_Callback02_NpcDeactivate(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	if (!pQuestArg->pTarget || pQuestArg->pTarget->dwClassId != MONSTER_DREHYA)
	{
		return;
	}

	D2Act5Quest4Strc* pQuestDataEx = (D2Act5Quest4Strc*)pQuestData->pQuestDataEx;

	if (pQuestDataEx->bDrehyaActivated == 1)
	{
		pQuestData->dwFlags &= 0xFFFFFF00;
		QUESTS_UnitIterate(pQuestData, 1, 0, ACT5Q4_UnitIterate_StatusCyclerEx, 1);
		pQuestDataEx->bDrehyaActivated = 0;
	}

	if (!pQuestDataEx->bNeedsToOpenPortal || pQuestDataEx->bPortalCreated)
	{
		return;
	}

	const int32_t nX = CLIENTS_GetUnitX(pQuestArg->pTarget);
	const int32_t nY = CLIENTS_GetUnitY(pQuestArg->pTarget);

	D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pQuestArg->pTarget);
	if (D2GAME_CreatePortalObject_6FD13DF0(pQuestData->pGame, nullptr, pRoom, nX + 10, nY + 5, LEVEL_NIHLATHAKSTEMPLE, nullptr, OBJECT_PERMANENT_TOWN_PORTAL, 1))
	{
		pQuestDataEx->bPortalCreated = 1;
		pQuestDataEx->bNeedsToOpenPortal = 0;
	}
}

//D2Game.0x6FCB5E70
// 1.13c D2Game+0x590C0 (no delta)
int32_t __fastcall ACT5Q4_UnitIterate_StatusCyclerEx(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pUnit)->pQuestData[pGame->nDifficulty];

	if (!QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_REWARDGRANTED) && !QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_COMPLETEDBEFORE)
		|| QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_PRIMARYGOALDONE)
		|| QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_COMPLETEDNOW))
	{
		QUESTS_StatusCyclerEx(pGame, pUnit, QUEST_A5Q4_NIHLATHAK);
	}

	return 0;
}

//D2Game.0x6FCB5EE0
// 1.13c D2Game+0x5AFB0 (no delta)
bool __fastcall ACT5Q4_SeqCallback(D2QuestDataStrc* pQuestData)
{
	if (pQuestData->fState < 4 && pQuestData->bNotIntro)
	{
		if (!pQuestData->fState)
		{
			QUESTS_StateDebug(pQuestData, 1, __FILE__, __LINE__);
		}
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

//D2Game.0x6FCB5F70
// 1.13c D2Game+0x5B7A0: after the state-5 debug line retail calls its own sequence callback (0x5AFB0) directly
void __fastcall ACT5Q4_Callback11_ScrollMessage(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	D2Act5Quest4Strc* pQuestDataEx = (D2Act5Quest4Strc*)pQuestData->pQuestDataEx;

	if (pQuestArg->nNPCNo != MONSTER_DREHYA)
	{
		return;
	}

	if (pQuestArg->nMessageIndex == 20137)
	{
		if (!pQuestData->bNotIntro)
		{
			return;
		}

		QUESTS_StateDebug(pQuestData, 2, __FILE__, __LINE__);
		pQuestDataEx->bDrehyaActivated = 1;
		pQuestDataEx->bNeedsToOpenPortal = 1;
		SUNIT_IterateUnitsOfType(pQuestData->pGame, 0, 0, ACT5Q4_UnitIterate_UpdateQuestStateFlags);
	}
	else if (pQuestArg->nMessageIndex == 20148)
	{
		pQuestData->dwFlags &= 0xFFFFFF00;
		QUESTS_UnitIterate(pQuestData, pQuestData->fLastState, 0, ACT5Q4_UnitIterate_StatusCyclerEx, 1);

		D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pQuestArg->pPlayer)->pQuestData[pQuestData->pGame->nDifficulty];

		QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_ENTERAREA);

		if (!QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_PRIMARYGOALDONE) && (pQuestData->bNotIntro || !QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_REWARDPENDING)))
		{
			return;
		}

		QUESTS_StateDebug(pQuestData, 5, __FILE__, __LINE__);
		ACT5Q4_SeqCallback(pQuestData);
	}
}

//D2Game.0x6FCB6100
// 1.13c D2Game+0x57C00 (no delta)
int32_t __fastcall ACT5Q4_UnitIterate_UpdateQuestStateFlags(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pUnit)->pQuestData[pGame->nDifficulty];
	if (!QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q3, QFLAG_REWARDGRANTED) && !QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q3, QFLAG_REWARDPENDING))
	{
		return 0;
	}

	if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_REWARDGRANTED) == 1 || QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_REWARDPENDING) == 1)
	{
		return 0;
	}

	D2QuestDataStrc* pQuestData = QUESTS_GetQuestData(pGame, QUEST_A5Q4_NIHLATHAK);
	if (!pQuestData)
	{
		return 0;
	}

	if (pQuestData->fState == 2)
	{
		QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_STARTED);
	}
	else if (pQuestData->fState == 3)
	{
		QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_LEAVETOWN);
	}

	return 0;
}

//D2Game.0x6FCB6190
// 1.13c D2Game+0x57A60 (no delta)
void __fastcall ACT5Q4_Callback00_NpcActivate(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	static const int32_t nIndices[] =
	{
		-1, 0, 1, 2, 3, 4
	};

	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pQuestArg->pPlayer)->pQuestData[pQuestData->pGame->nDifficulty];

	int32_t nNpcId = -1;
	if (pQuestArg->pTarget)
	{
		nNpcId = pQuestArg->pTarget->dwClassId;
	}

	if (!QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q3, QFLAG_REWARDGRANTED) && !QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q3, QFLAG_REWARDPENDING))
	{
		return;
	}

	if (nNpcId != MONSTER_DREHYA || pQuestData->fState != 1)
	{
		if (QUESTRECORD_GetQuestState(pQuestFlags, pQuestData->nQuestFilter, QFLAG_REWARDPENDING))
		{
			if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_ENTERAREA))
			{
				QUESTS_InitScrollTextChain(pQuestData, pQuestArg->pTextControl, nNpcId, 4);
			}
			else
			{
				QUESTS_InitScrollTextChain(pQuestData, pQuestArg->pTextControl, nNpcId, 3);
			}
		}
		else
		{
			if (QUESTS_CheckPlayerGUID(pQuestData, (pQuestArg->pPlayer ? pQuestArg->pPlayer->dwUnitId : -1)) == 1)
			{
				QUESTS_InitScrollTextChain(pQuestData, pQuestArg->pTextControl, nNpcId, 4);
			}
			else if (QUESTRECORD_GetQuestState(pQuestFlags, pQuestData->nQuestFilter, QFLAG_REWARDGRANTED) != 1 && (pQuestData->fState < 4 || QUESTRECORD_GetQuestState(pQuestFlags, pQuestData->nQuestFilter, QFLAG_PRIMARYGOALDONE)) && pQuestData->bNotIntro)
			{
				const int32_t nIndex = nIndices[pQuestData->fState];
				if (nIndex != -1 && nIndex < 8)
				{
					QUESTS_InitScrollTextChain(pQuestData, pQuestArg->pTextControl, nNpcId, nIndex);
				}
			}
		}
	}
	else
	{
		if (!QUESTRECORD_GetQuestState(pQuestFlags, pQuestData->nQuestFilter, QFLAG_REWARDPENDING) && !QUESTRECORD_GetQuestState(pQuestFlags, pQuestData->nQuestFilter, QFLAG_REWARDGRANTED))
		{
			QUESTS_InitScrollTextChain(pQuestData, pQuestArg->pTextControl, MONSTER_DREHYA, 0);
		}
	}
}

//D2Game.0x6FCB6320
// 1.13c D2Game+0x5B690: the party step is this quest's own iterator (0x5AF50), not Act 5 quest 3's
void __fastcall ACT5Q4_Callback08_MonsterKilled(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	if (!pQuestData->bNotIntro)
	{
		return;
	}

	D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pQuestArg->pTarget);
	if (!pRoom)
	{
		return;
	}

	D2Act5Quest4Strc* pQuestDataEx = (D2Act5Quest4Strc*)pQuestData->pQuestDataEx;
	pQuestDataEx->pRoom = pRoom;

	SUNIT_IterateUnitsOfType(pQuestData->pGame, 0, 0, ACT5Q4_UnitIterate_SetRewardPending);
	SUNIT_IterateUnitsOfType(pQuestArg->pGame, 0, pQuestArg->pTarget, ACT5Q4_UnitIterate_SetPrimaryGoalDoneForPartyMembers);
	SUNIT_IterateUnitsOfType(pQuestArg->pGame, 0, pQuestArg->pTarget, ACT5Q4_UnitIterate_SetCompletionFlag);
	SUNIT_IterateUnitsOfType(pQuestArg->pGame, 0, pQuestArg->pTarget, ACT5Q4_UnitIterate_AttachCompletionSound);

	QUESTS_SetGlobalState(pQuestData->pGame, QUESTSTATEFLAG_A5Q4, QFLAG_PRIMARYGOALDONE);
	QUESTS_TriggerFX(pQuestData->pGame, 17);
	QUESTS_StateDebug(pQuestData, 4, __FILE__, __LINE__);

	if (!pQuestDataEx->bTimerActive)
	{
		pQuestDataEx->bTimerActive = 1;
		QUESTS_CreateTimer(pQuestData, ACT5Q4_Timer_StatusCycler, 8);
	}
}

//D2Game.0x6FCB63F0
// 1.13c D2Game+0x57900 (no delta)
int32_t __fastcall ACT5Q4_UnitIterate_SetRewardPending(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pUnit)->pQuestData[pGame->nDifficulty];
	D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pUnit);
	if (!pRoom || QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_COMPLETEDNOW) || QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_REWARDGRANTED))
	{
		return 0;
	}

	D2Act5Quest4Strc* pQuestDataEx = (D2Act5Quest4Strc*)QUESTS_GetQuestData(pGame, QUEST_A5Q4_NIHLATHAK)->pQuestDataEx;

	if (pRoom != pQuestDataEx->pRoom)
	{
		D2ActiveRoomStrc** ppRoomList = nullptr;
		int32_t nNumRooms = 0;
		DUNGEON_GetAdjacentRoomsListFromRoom(pRoom, &ppRoomList, &nNumRooms);

		int32_t nCounter = 0;
		while (nCounter < nNumRooms)
		{
			if (ppRoomList[nCounter] == pQuestDataEx->pRoom)
			{
				break;
			}

			++nCounter;
		}

		if (nCounter >= nNumRooms)
		{
			return 0;
		}
	}

	if (!QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q3, QFLAG_REWARDGRANTED) && !QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q3, QFLAG_REWARDPENDING))
	{
		return 0;
	}

	if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_REWARDGRANTED) || QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_REWARDPENDING))
	{
		return 0;
	}

	QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_REWARDPENDING);
	QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_PRIMARYGOALDONE);
	return 0;
}

//D2Game.0x6FCB6500
// 1.13c D2Game+0x5AF50: the party walker runs this quest's SetPrimaryGoalDone (0x57830), not Act 5 quest 3's
int32_t __fastcall ACT5Q4_UnitIterate_SetPrimaryGoalDoneForPartyMembers(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	if (!D2_CALL(QUESTRECORD_GetQuestState, D2_CALL(UNITS_GetPlayerData, pUnit)->pQuestData[pGame->nDifficulty], QUESTSTATEFLAG_A5Q4, QFLAG_PRIMARYGOALDONE))
	{
		return 0;
	}

	const int16_t nPartyId = SUNIT_GetPartyId(pUnit);
	if (nPartyId != -1)
	{
		D2_CALL(PARTY_IteratePartyMembers, pGame, nPartyId, ACT5Q4_UnitIterate_SetPrimaryGoalDone, 0);
	}

	return 0;
}

//D2Game.0x6FCB6550
// 1.13c D2Game+0x59060 (no delta)
int32_t __fastcall ACT5Q4_UnitIterate_SetCompletionFlag(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pUnit)->pQuestData[pGame->nDifficulty];

	if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_REWARDGRANTED) != 1
		&& QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_REWARDPENDING) != 1
		&& QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_PRIMARYGOALDONE) != 1)
	{
		QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_COMPLETEDNOW);
		QUESTS_SendLogUpdateEx(pUnit, QUEST_A5Q4_NIHLATHAK, 0);
	}

	return 0;
}

//D2Game.0x6FCB65C0
// 1.13c D2Game+0x58FF0 (no delta)
int32_t __fastcall ACT5Q4_UnitIterate_AttachCompletionSound(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pData)
{
	if (D2_CALL(QUESTRECORD_GetQuestState, D2_CALL(UNITS_GetPlayerData, pUnit)->pQuestData[pGame->nDifficulty], QUESTSTATEFLAG_A5Q4, QFLAG_PRIMARYGOALDONE))
	{
		SUNIT_AttachSound(pUnit, 82, pUnit);
	}

	return 0;
}

//D2Game.0x6FCB6600
// 1.13c D2Game+0x59CD0 (no delta)
bool __fastcall ACT5Q4_Timer_StatusCycler(D2GameStrc* pGame, D2QuestDataStrc* pQuestData)
{
	D2Act5Quest4Strc* pQuestDataEx = (D2Act5Quest4Strc*)pQuestData->pQuestDataEx;

	if (pQuestData->fLastState != 4)
	{
		pQuestData->dwFlags &= 0xFFFFFF00;
		QUESTS_UnitIterate(pQuestData, 4, 0, ACT5Q4_UnitIterate_StatusCyclerEx, 1);
	}

	pQuestDataEx->bTimerActive = 0;
	return true;
}

//D2Game.0x6FCB6630
// 1.13c D2Game+0x5AE20 (no delta)
void __fastcall ACT5Q4_Callback13_PlayerStartedGame(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	D2Act5Quest4Strc* pQuestDataEx = (D2Act5Quest4Strc*)pQuestData->pQuestDataEx;
	if (pQuestData->bNotIntro)
	{
		D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pQuestArg->pPlayer)->pQuestData[pQuestArg->pGame->nDifficulty];
		if (!QUESTRECORD_GetQuestState(pQuestFlags, pQuestData->nQuestFilter, QFLAG_REWARDPENDING))
		{
			if (QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_LEAVETOWN))
			{
				pQuestDataEx->bNeedsToOpenPortal = 1;
				QUESTS_UnitIterate(pQuestData, 2, 0, ACT5Q4_UnitIterate_StatusCyclerEx, 1);
				QUESTS_StateDebug(pQuestData, 3, __FILE__, __LINE__);
			}
			else if (QUESTRECORD_GetQuestState(pQuestFlags, pQuestData->nQuestFilter, QFLAG_STARTED))
			{
				pQuestDataEx->bNeedsToOpenPortal = 1;
				QUESTS_UnitIterate(pQuestData, 1, 0, ACT5Q4_UnitIterate_StatusCyclerEx, 1);
				QUESTS_StateDebug(pQuestData, 2, __FILE__, __LINE__);
			}
		}
	}
	else
	{
		pQuestData->fState = 5;

		D2WaypointDataStrc* pWaypointFlags = UNITS_GetPlayerData(pQuestArg->pPlayer)->pWaypointData[pQuestArg->pGame->nDifficulty];
		int16_t nWpNo = 0;
		if (WAYPOINTS_GetWaypointNoFromLevelId(LEVEL_HALLSOFDEATHSCALLING, &nWpNo) && !WAYPOINTS_IsActivated(pWaypointFlags, nWpNo))
		{
			pQuestDataEx->bWaypointNotActivated = 1;
		}
	}
}

//D2Game.0x6FCB6750
// 1.13c D2Game+0x5AD50 (no delta)
void __fastcall ACT5Q4_Callback03_ChangedLevel(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	D2Act5Quest4Strc* pQuestDataEx = (D2Act5Quest4Strc*)pQuestData->pQuestDataEx;

	if (pQuestArg->nOldLevel == LEVEL_HARROGATH && pQuestData->fState == 2)
	{
		QUESTS_StateDebug(pQuestData, 3, __FILE__, __LINE__);
		SUNIT_IterateUnitsOfType(pQuestData->pGame, 0, 0, ACT5Q4_UnitIterate_UpdateQuestStateFlags);
	}

	if (pQuestArg->nNewLevel == LEVEL_NIHLATHAKSTEMPLE && pQuestData->fLastState == 1)
	{
		pQuestData->dwFlags &= 0xFFFFFF00;
		QUESTS_UnitIterate(pQuestData, 2, 0, ACT5Q4_UnitIterate_StatusCyclerEx, 1);
		pQuestDataEx->bDrehyaActivated = 0;
	}
}

//D2Game.0x6FCB67C0
// 1.13c D2Game+0x577F0 (set reward-granted 0, clear reward-pending 1, read completed-before 0xF of quest 0x26; only caller D2Game+0xC00C0)
void __fastcall ACT5Q4_SetRewardGranted(D2GameStrc* pGame, D2UnitStrc* pUnit)
{
	D2BitBufferStrc* pQuestFlags = UNITS_GetPlayerData(pUnit)->pQuestData[pGame->nDifficulty];

	QUESTRECORD_SetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_REWARDGRANTED);
	QUESTRECORD_ClearQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_REWARDPENDING);

	QUESTRECORD_GetQuestState(pQuestFlags, QUESTSTATEFLAG_A5Q4, QFLAG_COMPLETEDBEFORE);//TODO: Needed? Set?
}

//D2Game.0x6FCB6800
// 1.13c D2Game+0x59C50 (no delta)
void __fastcall ACT5Q4_OnNihlathakActivated(D2GameStrc* pGame)
{
	D2QuestDataStrc* pQuestData = QUESTS_GetQuestData(pGame, QUEST_A5Q4_NIHLATHAK);

	if (!pQuestData || !pQuestData->bNotIntro || pQuestData->fLastState >= 3)
	{
		return;
	}

	pQuestData->dwFlags &= 0xFFFFFF00;
	QUESTS_UnitIterate(pQuestData, 3, 0, ACT5Q4_UnitIterate_StatusCyclerEx, 1);
}

//D2Game.0x6FCB6840
// 1.13c D2Game+0x5D1D0. Retail inlines the portal creation here (D2GAME_CreatePortalObject_6FD13DF0: x+10, y+5,
// permanent portal object, destination 0x79, source level from the unit's room) and then clears +0x89.
void __fastcall ACT5Q4_AnyaOpenPortal(D2GameStrc* pGame, D2UnitStrc* pUnit)
{
	D2Act5Quest4Strc* pQuestDataEx = (D2Act5Quest4Strc*)QUESTS_GetQuestData(pGame, QUEST_A5Q4_NIHLATHAK)->pQuestDataEx;
	if (!pQuestDataEx->bWaypointNotActivated)
	{
		return;
	}

	D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pUnit);
	const int32_t nLevelId = DUNGEON_GetLevelIdFromRoom(pRoom);
	if (DRLG_GetActNoFromLevelId(nLevelId) != ACT_V || !D2GAME_CreatePortalObject_6FD13DF0(pGame, nullptr, pRoom, CLIENTS_GetUnitX(pUnit) + 10, CLIENTS_GetUnitY(pUnit) + 5, LEVEL_NIHLATHAKSTEMPLE, nullptr, OBJECT_PERMANENT_TOWN_PORTAL, 1))
	{
		return;
	}

	pQuestDataEx->bWaypointNotActivated = 0;
}

//D2Game.0x6FCB6890
// 1.13c D2Game+0x57A20 (no delta)
void __fastcall ACT5Q4_Callback10_PlayerLeavesGame(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg)
{
	D2Act5Quest4Strc* pQuestDataEx = (D2Act5Quest4Strc*)pQuestData->pQuestDataEx;

	if (pQuestArg->pPlayer)
	{
		QUESTS_FastRemovePlayerGUID(&pQuestData->tPlayerGUIDs, pQuestArg->pPlayer->dwUnitId);
		QUESTS_FastRemovePlayerGUID(&pQuestDataEx->tPlayerGUIDs, pQuestArg->pPlayer->dwUnitId);
	}
	else
	{
		QUESTS_FastRemovePlayerGUID(&pQuestData->tPlayerGUIDs, -1);
		QUESTS_FastRemovePlayerGUID(&pQuestDataEx->tPlayerGUIDs, -1);
	}
}
