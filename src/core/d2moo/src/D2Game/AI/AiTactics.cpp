#include "AI/AiTactics.h"
#include <test_seam.h>

#include <algorithm>

#include <D2BitManip.h>
#include <D2Math.h>
#include <D2PacketDef.h>

#include <D2Chat.h>
#include <D2Collision.h>
#include <D2DataTbls.h>
#include <D2Monsters.h>
#include <D2States.h>
#include <D2Skills.h>
#include <Units/UnitRoom.h>

#include "AI/AiUtil.h"
#include "AI/AiGeneral.h"
#include "GAME/Clients.h"
#include "GAME/Event.h"
#include "GAME/SCmd.h"
#include "ITEMS/Items.h"
#include "MONSTER/Monster.h"
#include "MONSTER/MonsterMode.h"
#include "MONSTER/MonsterUnique.h"
#include "MONSTER/MonsterRegion.h"
#include "QUESTS/Quests.h"
#include "UNIT/SUnit.h"
#include "UNIT/SUnitInactive.h"

//D2Game.0x6FCCF9D0
// 1.13c D2Game+0xF3220 (the walk-capable AI-state test and the walk-mode flag test are inline, F1B10 gets AL = 5)
D2UnitStrc* __fastcall sub_6FCCF9D0(D2GameStrc* pGame, D2UnitStrc* pUnit, D2AiControlStrc* pAiControl, int32_t* pDistance, int32_t* pCombat)
{
	D2UnitStrc* pResult = sub_6FCF2110(pGame, pUnit, pAiControl, pDistance, pCombat);
	if (pResult)
	{
		return pResult;
	}

	if (sub_6FCF2E70(pUnit) && MONSTERUNIQUE_CheckMonModeFlag(pUnit->dwClassId, MONMODE_WALK))
	{
		AITACTICS_WalkCloseToUnit(pGame, pUnit, 5u);
		return nullptr;
	}

	if (COLLISION_CheckAnyCollisionWithPattern(UNITS_GetRoom(pUnit), CLIENTS_GetUnitX(pUnit), CLIENTS_GetUnitY(pUnit), PATH_GetUnitCollisionPattern(pUnit), COLLIDE_MISSILE)
		&& MONSTERUNIQUE_CheckMonModeFlag(pUnit->dwClassId, MONMODE_WALK))
	{
		AITACTICS_WalkCloseToUnit(pGame, pUnit, 5u);
		return nullptr;
	}

	const int32_t nDistance = *pDistance;
	if (nDistance > 34)
	{
		AITACTICS_IdleInNeutralMode(pGame, pUnit, 25);
	}
	else if (nDistance < 25)
	{
		AITACTICS_IdleInNeutralMode(pGame, pUnit, 10);
	}
	else
	{
		AITACTICS_IdleInNeutralMode(pGame, pUnit, nDistance - 10);
	}

	return nullptr;
}

//D2Game.0x6FCCFC00
// 1.13c D2Game+0xF3150
D2UnitStrc* __fastcall sub_6FCCFC00(D2GameStrc* pGame, D2UnitStrc* pUnit, D2AiControlStrc* pAiControl, int32_t* pDistance, int32_t* pCombat)
{
	D2UnitStrc* pResult = sub_6FCF2110(pGame, pUnit, pAiControl, pDistance, pCombat);
	if (pResult)
	{
		return pResult;
	}

	if (COLLISION_CheckAnyCollisionWithPattern(UNITS_GetRoom(pUnit), CLIENTS_GetUnitX(pUnit), CLIENTS_GetUnitY(pUnit), PATH_GetUnitCollisionPattern(pUnit), COLLIDE_MISSILE))
	{
		D2MonStats2Txt* pMonStats2TxtRecord = MONSTERREGION_GetMonStats2TxtRecord(pUnit->dwClassId);
		if (pMonStats2TxtRecord && pMonStats2TxtRecord->dwModeFlags & gdwBitMasks[MONMODE_WALK])
		{
			AITACTICS_WalkCloseToUnit(pGame, pUnit, 5u);
			return nullptr;
		}
	}

	AITACTICS_Idle(pGame, pUnit, 20);
	return nullptr;
}

//D2Game.0x6FCCFD40
// not-1.13c: retail callers call 0xF1C90 and compare its distance against the limit inline, no standalone function
D2UnitStrc* __fastcall sub_6FCCFD40(D2GameStrc* pGame, D2UnitStrc* pUnit, D2AiControlStrc* pAiControl, int32_t* pDistance, int32_t* pCombat, int32_t nMaxDistance)
{
	D2UnitStrc* pResult = sub_6FCF2110(pGame, pUnit, pAiControl, pDistance, pCombat);

	if (pResult && *pDistance <= nMaxDistance)
	{
		return pResult;
	}

	return nullptr;
}

//D2Game.0x6FCCFD70
// 1.13c D2Game+0xF30C0
D2UnitStrc* __fastcall sub_6FCCFD70(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t* bCloseToTarget)
{
	if (!UNITS_GetRoom(pUnit))
	{
		return nullptr;
	}

	UnkAiStrc1 pCallbackArg = {};
	pCallbackArg.pTarget = nullptr;
	pCallbackArg.nDistance = INT_MAX;
	sub_6FCF1E80(pGame, pUnit, &pCallbackArg, sub_6FCCFDE0, 2);

	if (!pCallbackArg.pTarget)
	{
		return pUnit;
	}

	if (pCallbackArg.nDistance < 4)
	{
		*bCloseToTarget = 1;
	}

	return pCallbackArg.pTarget;
}

// 1.13c D2Game+0x15D0: whether the monster class has the interact flag
int32_t __fastcall AITACTICS_IsInteractClass(int32_t nClassId)
{
	D2MonStatsTxt* pMonStatsTxtRecord = MONSTERMODE_GetMonStatsTxtRecord(nClassId);
	return pMonStatsTxtRecord && (gdwBitMasks[MONSTATSFLAGINDEX_INTERACT] & pMonStatsTxtRecord->dwMonStatsFlags);
}

//D2Game.0x6FCCFDE0
// 1.13c D2Game+0xF1A90 (the near-player callback 0xF30C0 hands the unit scan)
D2UnitStrc* __fastcall sub_6FCCFDE0(D2GameStrc* pGame, D2UnitStrc* pNPC, D2UnitStrc* pPlayer, void* pCallbackArg)
{
	UnkAiStrc1* pArg = (UnkAiStrc1*)pCallbackArg;

	if (!pPlayer || pPlayer->dwUnitType != UNIT_PLAYER)
	{
		return nullptr;
	}

	const int32_t nDistance = D2_CALL(AIUTIL_GetDistanceToCoordinates_FullUnitSize, pNPC, pPlayer);
	if (nDistance > 15)
	{
		return nullptr;
	}

	// the class check runs for a missing NPC too (class -1 has no record)
	if (!D2_CALL(AITACTICS_IsInteractClass, pNPC ? (int32_t)pNPC->dwClassId : -1))
	{
		pArg->nDistance = nDistance;
		return pPlayer;
	}

	if (D2_CALL(QUESTS_ActiveCycler, pGame, pPlayer, pNPC) && nDistance < pArg->nDistance)
	{
		pArg->pTarget = pPlayer;
		pArg->nDistance = nDistance;
		return pPlayer;
	}

	return nullptr;
}

//D2Game.0x6FCCFEA0
// 1.13c D2Game+0xF2220 (unit from EAX: mode-change info 0x75070, target unit at +0x10, then 0x75E10)
int32_t __fastcall AITACTICS_ChangeModeAndTargetUnit(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nMode, D2UnitStrc* pTargetUnit)
{
	D2ModeChangeStrc modeChange = {};

	MONSTERMODE_GetModeChangeInfo(pUnit, nMode, &modeChange);
	modeChange.pTargetUnit = pTargetUnit;
	return D2GAME_ModeChange_6FC65220(pGame, &modeChange, 1);
}

//D2Game.0x6FCCFEE0
// not-1.13c: retail callers build the coordinate mode change inline (0x75070 mode-change info, x/y words, 0x75E10), no standalone function
int32_t __fastcall AITACTICS_ChangeModeAndTargetCoordinates(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nMode, int32_t nX, int32_t nY)
{
	D2ModeChangeStrc modeChange = {};

	MONSTERMODE_GetModeChangeInfo(pUnit, nMode, &modeChange);
	modeChange.nX = nX;
	modeChange.nY = nY;
	return D2GAME_ModeChange_6FC65220(pGame, &modeChange, 1);
}

//D2Game.0x6FCCFF20
// 1.13c D2Game+0xF2120
int32_t __fastcall AITACTICS_UseSequenceSkill(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nSkillId, D2UnitStrc* pTargetUnit, int32_t nX, int32_t nY)
{
	if (nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount)
	{
		return 0;
	}

	D2ModeChangeStrc modeChange = {};
	MONSTERMODE_GetModeChangeInfo(pUnit, MONMODE_SEQUENCE, &modeChange);
	UNITS_SetUsedSkill(pUnit, SKILLS_GetSkillById(pUnit, nSkillId, -1));

	PATH_SetStepNum(pUnit->pDynamicPath, 1);

	modeChange.pTargetUnit = pTargetUnit;
	modeChange.nX = nX;
	modeChange.nY = nY;

	return D2GAME_ModeChange_6FC65220(pGame, &modeChange, 0);
}

// 1.13c D2Game+0xF2380
//D2Game.0x6FCCFFB0
int32_t __fastcall AITACTICS_UseSkill(D2GameStrc* pGame, D2UnitStrc* pUnit, uint8_t nMode, int32_t nSkillId, D2UnitStrc* pTarget, int32_t nX, int32_t nY)
{
	if (!pUnit || nMode >= 16u)
	{
		return 0;
	}

	D2ModeChangeStrc modeChange = {};
	MONSTERMODE_GetModeChangeInfo(pUnit, nMode, &modeChange);
	UNITS_SetUsedSkill(pUnit, SKILLS_GetSkillById(pUnit, nSkillId, -1));

	pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

	PATH_SetStepNum(pUnit->pDynamicPath, 1);

	modeChange.pTargetUnit = pTarget;
	modeChange.nX = nX;
	modeChange.nY = nY;

	if (D2GAME_ModeChange_6FC65220(pGame, &modeChange, 0))
	{
		return 1;
	}

	AITACTICS_IdleInNeutralMode(pGame, pUnit, 10);
	return 0;
}

//D2Game.0x6FCD00A0
// 1.13c D2Game+0xF22C0 (the mode-change record is built inline, not through MONSTERMODE_GetModeChangeInfo:
// the used skill is cleared, the byte at +0x15 is always 100 and no per-mode 101 lookup runs)
void __fastcall AITACTICS_IdleInNeutralMode(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nFrames)
{
	if (!pUnit || pUnit->dwAnimMode != MONMODE_NEUTRAL)
	{
		UNITS_SetUsedSkill(pUnit, nullptr);
		D2ModeChangeStrc modeChange = {};
		modeChange.unk0x14[1] = 100;
		modeChange.nMode = MONMODE_NEUTRAL;
		modeChange.pUnit = pUnit;
		modeChange.pTargetUnit = pUnit;
		D2GAME_ModeChange_6FC65220(pGame, &modeChange, 1);
	}

	AITACTICS_Idle(pGame, pUnit, nFrames);
}

//D2Game.0x6FCD0110
// 1.13c D2Game+0xF20C0
void __fastcall AITACTICS_Idle(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nFrames)
{
	if (!nFrames)
	{
		nFrames = 1;
	}

	D2GAME_EVENTS_Delete_6FC34840(pGame, pUnit, EVENTTYPE_AITHINK, 0);
	EVENT_SetEvent(pGame, pUnit, EVENTTYPE_AITHINK, nFrames + pGame->dwGameFrame, 0, 0);
}

//D2Game.0x6FCD0150
// 1.13c D2Game+0xF2040
void __fastcall sub_6FCD0150(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nFrames)
{
	if (!nFrames)
	{
		nFrames = 1;
	}

	const uint32_t dwEventFrame = D2_CALL(EVENT_GetEventFrame, pGame, pUnit, EVENTTYPE_AITHINK);
	if (!dwEventFrame || dwEventFrame == pGame->dwGameFrame || dwEventFrame >= nFrames + pGame->dwGameFrame)
	{
		D2GAME_EVENTS_Delete_6FC34840(pGame, pUnit, EVENTTYPE_AITHINK, 0);
		D2_CALL(EVENT_SetEvent, pGame, pUnit, EVENTTYPE_AITHINK, nFrames + pGame->dwGameFrame, 0, 0);
	}
}

//D2Game.0x6FCD01B0
// 1.13c D2Game+0xF1A20
void __fastcall AITACTICS_SetVelocity(D2UnitStrc* pUnit, int32_t a2, int32_t nVel, uint8_t a4)
{
	D2_ASSERT((nVel >= -126) && (nVel <= 126));

	if (a2 == 1)
	{
		a2 = 7;
	}

	if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && pUnit->pMonsterData)
	{
		D2GAME_MONSTERMODE_SetVelocityParams_6FC62FC0(pUnit->pMonsterData->pAiParam, a2, nVel, a4);
	}
	else
	{
		D2GAME_MONSTERMODE_SetVelocityParams_6FC62FC0(nullptr, a2, nVel, a4);
	}
}

//D2Game.0x6FCD0220
// not-1.13c: retail callers call 0xF2540 directly with mode 2, step 1 and the flag byte; no standalone function
int32_t __fastcall AITACTICS_WalkToTargetUnitWithFlags(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTargetUnit, uint8_t nFlag)
{
	return AITACTICS_MoveToTarget(pGame, pUnit, pTargetUnit, MONMODE_WALK, 0, 0, 1u, nFlag);
}

//D2Game.0x6FCD0240
// 1.13c D2Game+0xF2540 (decrepify reset of the velocity fields and the AI-think event sweep are inline)
int32_t __fastcall AITACTICS_MoveToTarget(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTarget, int32_t nMode, int32_t nX, int32_t nY, uint8_t bStep, uint8_t nFlags)
{
	int32_t nNewMode = nMode;

	if (nMode == MONMODE_RUN && STATES_CheckState(pUnit, STATE_DECREPIFY))
	{
		D2GAME_MONSTERMODE_ResetVelocityParams_6FC62F90(pUnit);
		nNewMode = MONMODE_WALK;
	}

	PATH_SetStepNum(pUnit->pDynamicPath, bStep);

	D2ModeChangeStrc modeChange = {};
	MONSTERMODE_GetModeChangeInfo(pUnit, nNewMode, &modeChange);

	if (pTarget)
	{
		modeChange.pTargetUnit = pTarget;
	}
	else
	{
		modeChange.nX = nX;
		modeChange.nY = nY;
	}

	if (D2GAME_ModeChange_6FC65220(pGame, &modeChange, 1))
	{
		return 1;
	}

	if (nFlags & 4)
	{
		D2GAME_EVENTS_Delete_6FC34840(pGame, pUnit, EVENTTYPE_AITHINK, 0);
	}

	if (nFlags & 1)
	{
		D2AiControlStrc* pAiControl = AIGENERAL_GetAiControlFromUnit(pUnit);
		if (pAiControl)
		{
			AIUTIL_ToggleAiControlFlag(pAiControl, 0x40u, 1);
		}
	}

	if (nFlags & 2)
	{
		if ((ITEMS_RollRandomNumber(&pUnit->pSeed) % 100) >= 70)
		{
			AITACTICS_IdleInNeutralMode(pGame, pUnit, 10);
		}
		else
		{
			AITACTICS_WalkCloseToUnit(pGame, pUnit, 4u);
		}

		return 1;
	}

	return 0;
}

//D2Game.0x6FCD03B0
// not-1.13c: retail callers call 0xF2540 directly with mode 2, step 1, flags 0; no standalone function
int32_t __fastcall AITACTICS_WalkToTargetUnit(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTarget)
{
	return AITACTICS_MoveToTarget(pGame, pUnit, pTarget, MONMODE_WALK, 0, 0, 1u, 0);
}

//D2Game.0x6FCD03D0
// not-1.13c: retail callers call 0xF2540 directly with mode 0xF, step 1 and the flag byte; no standalone function
int32_t __fastcall AITACTICS_RunToTargetUnitWithFlags(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTarget, uint8_t nFlags)
{
	return AITACTICS_MoveToTarget(pGame, pUnit, pTarget, MONMODE_RUN, 0, 0, 1u, nFlags);
}

//D2Game.0x6FCD03F0
// not-1.13c: retail callers call 0xF2540 directly with mode 0xF, step 1, flags 0; no standalone function
int32_t __fastcall AITACTICS_RunToTargetUnit(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTarget)
{
	return AITACTICS_MoveToTarget(pGame, pUnit, pTarget, MONMODE_RUN, 0, 0, 1u, 0);
}

//D2Game.0x6FCD0410
// 1.13c D2Game+0xF3000 (AI param +0x18 = 0xD written directly, then F2540 walk with one step)
int32_t __fastcall sub_6FCD0410(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTarget, uint8_t nFlags)
{
	if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && pUnit->pMonsterData)
	{
		pUnit->pMonsterData->pAiParam->unk0x18 = 13;
	}

	return AITACTICS_MoveToTarget(pGame, pUnit, pTarget, MONMODE_WALK, 0, 0, 1u, nFlags);
}

//D2Game.0x6FCD0460
// not-1.13c: retail callers call 0xF2540 directly with mode 2 and coordinates; no standalone function
int32_t __fastcall AITACTICS_WalkToTargetCoordinates(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nX, int32_t nY)
{
	return AITACTICS_MoveToTarget(pGame, pUnit, nullptr, MONMODE_WALK, nX, nY, 1u, 0);
}

//D2Game.0x6FCD0480
// not-1.13c: retail callers call 0xF2540 directly with mode 0xF and coordinates; no standalone function
int32_t __fastcall AITACTICS_RunToTargetCoordinates(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nX, int32_t nY)
{
	return AITACTICS_MoveToTarget(pGame, pUnit, nullptr, MONMODE_RUN, nX, nY, 1u, 0);
}

//D2Game.0x6FCD04A0
// not-1.13c: retail callers call 0xF2540 directly with mode 2, coordinates and flag 4; no standalone function
int32_t __fastcall AITACTICS_WalkToTargetCoordinatesDeleteAiEvent(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nX, int32_t nY)
{
	return AITACTICS_MoveToTarget(pGame, pUnit, nullptr, MONMODE_WALK, nX, nY, 1u, 4);
}

//D2Game.0x6FCD04C0
// not-1.13c: retail callers call 0xF2540 directly with mode 0xF, coordinates and flag 4; no standalone function
int32_t __fastcall AITACTICS_RunToTargetCoordinatesDeleteAiEvent(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nX, int32_t nY)
{
	return AITACTICS_MoveToTarget(pGame, pUnit, nullptr, MONMODE_RUN, nX, nY, 1u, 4);
}

//D2Game.0x6FCD04E0
// not-1.13c: 1.13c inlines this wrapper; its callers (e.g. +0x16970) call MoveToTarget (+0xF2540) in walk mode directly.
int32_t __fastcall AITACTICS_WalkToTargetCoordinatesNoSteps(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nX, int32_t nY)
{
	return AITACTICS_MoveToTarget(pGame, pUnit, nullptr, MONMODE_WALK, nX, nY, 0, 0);
}

//D2Game.0x6FCD0500
// 1.13c D2Game+0xF2F30 (step byte 0 becomes 1, then F2540 with walk mode 2)
int32_t __fastcall AITACTICS_WalkToTargetUnitWithSteps(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTarget, uint8_t bStep)
{
	if (!bStep)
	{
		bStep = 1;
	}

	return AITACTICS_MoveToTarget(pGame, pUnit, pTarget, MONMODE_WALK, 0, 0, bStep, 0);
}

//D2Game.0x6FCD0530
// 1.13c D2Game+0xF2F00 (step byte 0 becomes 1, then F2540 with run mode 0xF)
int32_t __fastcall AITACTICS_RunToTargetUnitWithSteps(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTarget, uint8_t bStep)
{
	if (!bStep)
	{
		bStep = 1;
	}

	return AITACTICS_MoveToTarget(pGame, pUnit, pTarget, MONMODE_RUN, 0, 0, bStep, 0);
}

//D2Game.0x6FCD0560
// 1.13c D2Game+0xF2D80 (checked against decompile; byte distance fixed)
int32_t __fastcall D2GAME_AICORE_Escape_6FCD0560(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTarget, int32_t nMaxDistance, int32_t bDeleteAiEventCallback)
{
	if (!pUnit || !pTarget)
	{
		return 0;
	}

	const int32_t nX = CLIENTS_GetUnitX(pUnit);
	const int32_t nY = CLIENTS_GetUnitY(pUnit);
	// 1.13c takes the distance as a byte: compare, clamp and multiply all use the low 8 bits.
	nMaxDistance = (uint8_t)nMaxDistance;
	const int32_t nXSign = D2signum(nX - CLIENTS_GetUnitX(pTarget));
	const int32_t nYSign = D2signum(nY - CLIENTS_GetUnitY(pTarget));

	if (nMaxDistance > 5u)
	{
		D2AiParamStrc* pAiParam = nullptr;
		if (pUnit->dwUnitType == UNIT_MONSTER && pUnit->pMonsterData)
		{
			pAiParam = pUnit->pMonsterData->pAiParam;
		}

		D2GAME_MONSTERMODE_SetVelocityParams_6FC62FC0(pAiParam, 0, 0, nMaxDistance);
	}

	return AITACTICS_MoveToTarget(pGame, pUnit, 0, MONMODE_WALK, nX + nXSign * nMaxDistance, nY + nYSign * nMaxDistance, 1u, bDeleteAiEventCallback != 0 ? 4 : 0);
}

//D2Game.0x6FCD06D0
// 1.13c D2Game+0xF2C10 (no null checks; the distance is a byte, speed byte +0x20 = min(distance, 0x4D) when above 5)
int32_t __fastcall sub_6FCD06D0(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTarget, int32_t nMaxDistance, int32_t bDeleteAiEventCallback)
{
	const int32_t nX = CLIENTS_GetUnitX(pUnit);
	const int32_t nY = CLIENTS_GetUnitY(pUnit);
	const int32_t nXSign = D2signum(nX - CLIENTS_GetUnitX(pTarget));
	const int32_t nYSign = D2signum(nY - CLIENTS_GetUnitY(pTarget));
	const int32_t nDistance = (uint8_t)nMaxDistance;

	if (nDistance > 5)
	{
		D2AiParamStrc* pAiParam = nullptr;
		if (pUnit->dwUnitType == UNIT_MONSTER && pUnit->pMonsterData)
		{
			pAiParam = pUnit->pMonsterData->pAiParam;
		}

		D2GAME_MONSTERMODE_SetVelocityParams_6FC62FC0(pAiParam, 0, 0, (uint8_t)nDistance);
	}

	return AITACTICS_MoveToTarget(pGame, pUnit, 0, MONMODE_RUN, nX + nXSign * nDistance, nY + nYSign * nDistance, 1u, bDeleteAiEventCallback != 0 ? 4 : 0);
}

// 1.13c D2Game+0xF1B10
//D2Game.0x6FCD0840
int32_t __fastcall AITACTICS_WalkCloseToUnit(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nMaxDistance)
{
	int32_t nOffsetX = 0;
	int32_t nOffsetY = 0;

	const int32_t nTargetX = CLIENTS_GetUnitX(pUnit);
	const int32_t nTargetY = CLIENTS_GetUnitY(pUnit);

	if (ITEMS_RollRandomNumber(&pUnit->pSeed) & 1)
	{
		nOffsetX = nMaxDistance;
		nOffsetY = ITEMS_RollLimitedRandomNumber(&pUnit->pSeed, nMaxDistance);
	}
	else
	{
		nOffsetX = ITEMS_RollLimitedRandomNumber(&pUnit->pSeed, nMaxDistance);
		nOffsetY = nMaxDistance;
	}

	if (ITEMS_RollRandomNumber(&pUnit->pSeed) & 1)
	{
		nOffsetX = -nOffsetX;
	}

	if (ITEMS_RollRandomNumber(&pUnit->pSeed) & 1)
	{
		nOffsetY = -nOffsetY;
	}

	return AITACTICS_MoveToTarget(pGame, pUnit, nullptr, MONMODE_WALK, nTargetX + nOffsetX, nTargetY + nOffsetY, 1u, 0);
}

// 1.13c D2Game+0xF2870: null owner passes the caller's nMaxDistance (AL) through, not the 1.10f constant 2;
// the 1.10f AITACTICS_WanderToTarget (0x6FCD09D0) is inlined here in retail, so it is folded in
//D2Game.0x6FCD0B60
int32_t __fastcall D2GAME_AICORE_WalkToOwner_6FCD0B60(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pOwner, int32_t nMaxDistance)
{
	if (!pOwner)
	{
		return AITACTICS_WalkCloseToUnit(pGame, pUnit, nMaxDistance);
	}

	int32_t nOffsetX = 0;
	int32_t nOffsetY = 0;

	const int32_t nTargetX = CLIENTS_GetUnitX(pOwner);
	const int32_t nTargetY = CLIENTS_GetUnitY(pOwner);

	if (ITEMS_RollRandomNumber(&pUnit->pSeed) & 1)
	{
		nOffsetX = nMaxDistance;
		nOffsetY = ITEMS_RollLimitedRandomNumber(&pUnit->pSeed, nMaxDistance);
	}
	else
	{
		nOffsetX = ITEMS_RollLimitedRandomNumber(&pUnit->pSeed, nMaxDistance);
		nOffsetY = nMaxDistance;
	}

	if (ITEMS_RollRandomNumber(&pUnit->pSeed) & 1)
	{
		nOffsetX = -nOffsetX;
	}

	if (ITEMS_RollRandomNumber(&pUnit->pSeed) & 1)
	{
		nOffsetY = -nOffsetY;
	}

	return AITACTICS_MoveToTarget(pGame, pUnit, nullptr, MONMODE_WALK, nTargetX + nOffsetX, nTargetY + nOffsetY, 1u, 0);
}

//D2Game.0x6FCD0D00
// 1.13c D2Game+0xF2750
int32_t __fastcall AITACTICS_RunCloseToTargetUnit(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTarget, int32_t nMaxDistance)
{
	int32_t nOffsetX = 0;
	int32_t nOffsetY = 0;

	const int32_t nTargetX = CLIENTS_GetUnitX(pTarget);
	const int32_t nTargetY = CLIENTS_GetUnitY(pTarget);

	if (ITEMS_RollRandomNumber(&pUnit->pSeed) & 1)
	{
		nOffsetX = nMaxDistance;
		nOffsetY = D2_CALL(ITEMS_RollLimitedRandomNumber, &pUnit->pSeed, nMaxDistance);
	}
	else
	{
		nOffsetX = D2_CALL(ITEMS_RollLimitedRandomNumber, &pUnit->pSeed, nMaxDistance);
		nOffsetY = nMaxDistance;
	}

	if (ITEMS_RollRandomNumber(&pUnit->pSeed) & 1)
	{
		nOffsetX = -nOffsetX;
	}

	if (ITEMS_RollRandomNumber(&pUnit->pSeed) & 1)
	{
		nOffsetY = -nOffsetY;
	}

	return D2_CALL(AITACTICS_MoveToTarget, pGame, pUnit, nullptr, MONMODE_RUN, nTargetX + nOffsetX, nTargetY + nOffsetY, 1u, 0);
}

//D2Game.0x6FCD0E80
// 1.13c D2Game+0xF26B0
int32_t __fastcall sub_6FCD0E80(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTarget, int32_t a4, int32_t bDeleteAiEventCallback)
{
	//TODO: v8
	int32_t v8 = 0;
	if ((uint8_t)ITEMS_RollRandomNumber(&pUnit->pSeed) < 128u)
	{
		v8 = 5;
	}
	else
	{
		v8 = 6;
	}

	D2AiParamStrc* pAiParam = nullptr;
	if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && pUnit->pMonsterData)
	{
		pAiParam = pUnit->pMonsterData->pAiParam;
	}

	D2GAME_MONSTERMODE_SetVelocityParams_6FC62FC0(pAiParam, v8, 0, a4);

	return AITACTICS_MoveToTarget(pGame, pUnit, pTarget, MONMODE_WALK, 0, 0, 1u, bDeleteAiEventCallback != 0 ? 4 : 0);
}

//D2Game.0x6FCD0F10
// 1.13c D2Game+0xF18B0 (only the zero-message test remains; the word cannot exceed 0xFFFF)
void __fastcall AITACTICS_AddMessage(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pScrollTarget, uint16_t wMessage, int32_t bScrollMessage)
{
	if (!wMessage)
	{
		return;
	}

	if (bScrollMessage)
	{
		D2ClientStrc* pClient = SUNIT_GetClientFromPlayer(pScrollTarget, __FILE__, __LINE__);

		D2GSPacketSrv27 packet27 = {};
		packet27.nHeader = 0x27;

		if (pUnit)
		{
			packet27.dwUnitGUID = pUnit->dwUnitId;
		}
		else
		{
			packet27.dwUnitGUID = -1;
		}

		packet27.nUnitType = UNIT_MONSTER;
		packet27.pMessageList.nCount = 1;
		packet27.pMessageList.pMessages[0].nMenu = 3;
		packet27.pMessageList.pMessages[0].nStringId = wMessage;
		D2GAME_PACKETS_SendPacket_6FC3C710(pClient, &packet27, sizeof(packet27));
	}
	else
	{
		char szText[12] = {};
		sprintf_s(szText, "%d", wMessage);

		if (pUnit && pUnit->pHoverText)
		{
			CHAT_FreeHoverMsg(pGame->pMemoryPool, pUnit->pHoverText);
		}

		D2HoverTextStrc* pHoverMsg = CHAT_AllocHoverMsg(pGame->pMemoryPool, szText, pGame->dwGameFrame);
		if (pHoverMsg)
		{
			if (pUnit)
			{
				pUnit->pHoverText = pHoverMsg;
			}

			UNITROOM_RefreshUnit(pUnit);
			if (pUnit)
			{
				pUnit->dwFlags |= UNITFLAG_HASTXTMSG;
			}

			EVENT_SetEvent(pGame, pUnit, EVENTTYPE_FREEHOVER, CHAT_GetTimeoutFromHoverMsg(pHoverMsg), 0, 0);
		}
	}
}

//D2Game.0x6FCD1020
// not-1.13c: retail callers set path step 1 and the mode change inline (0x75070, 0x75E10), no standalone function
int32_t __fastcall AITACTICS_ChangeModeAndTargetCoordinatesOneStep(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nX, int32_t nY, int32_t nMode)
{
	D2ModeChangeStrc modeChange = {};

	MONSTERMODE_GetModeChangeInfo(pUnit, nMode, &modeChange);
	PATH_SetStepNum(pUnit->pDynamicPath, 1u);
	modeChange.nX = nX;
	modeChange.nY = nY;
	return D2GAME_ModeChange_6FC65220(pGame, &modeChange, 1);
}

//D2Game.0x6FCD1070
// not-1.13c: retail callers set path step 0 and the mode change inline (0x75070, 0x75E10), no standalone function
int32_t __fastcall AITACTICS_ChangeModeAndTargetCoordinatesNoStep(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nX, int32_t nY, int32_t nMode)
{
	D2ModeChangeStrc modeChange = {};

	MONSTERMODE_GetModeChangeInfo(pUnit, nMode, &modeChange);
	PATH_SetStepNum(pUnit->pDynamicPath, 0);
	modeChange.nX = nX;
	modeChange.nY = nY;
	return D2GAME_ModeChange_6FC65220(pGame, &modeChange, 1);
}

//D2Game.0x6FCD10C0
// TODO: Check name
// 1.13c D2Game+0xF1D90 (checked against decompile)
int32_t __fastcall AITACTICS_MoveInRadiusToTarget(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTarget, int32_t nMode, int32_t a5, int32_t a6)
{
	const int32_t nDistance = AIUTIL_GetDistanceToCoordinates_FullUnitSize(pUnit, pTarget);
	const int32_t nSign = D2signum(nDistance - a6);
	const int32_t nDiff = std::min(std::abs(nDistance - a6), a5);

	const int32_t nTargetX = CLIENTS_GetUnitX(pTarget);
	const int32_t nTargetY = CLIENTS_GetUnitY(pTarget);
	const int32_t nX = CLIENTS_GetUnitX(pUnit);
	const int32_t nY = CLIENTS_GetUnitY(pUnit);

	const int32_t nXDiff = std::abs(nTargetX - nX);
	const int32_t nYDiff = std::abs(nTargetY - nY);

	const int32_t nSum = std::max(nYDiff + nXDiff, nDiff);

	int32_t nXOffset = 0;
	int32_t nYOffset = 0;
	if (nSum > 0)
	{
		nXOffset = nDiff * nXDiff / nSum;
		nYOffset = nDiff * nYDiff / nSum;
		while (nXOffset + nYOffset < nDiff)
		{
			++nXOffset;
			++nYOffset;
		}
	}

	const int32_t nSignX = D2signum(nTargetX - nX);
	const int32_t nSignY = D2signum(nTargetY - nY);

	D2ModeChangeStrc modeChange = {};
	MONSTERMODE_GetModeChangeInfo(pUnit, nMode, &modeChange);
	PATH_SetStepNum(pUnit->pDynamicPath, 1u);

	modeChange.nX = nX + nSign * nXOffset * nSignX;
	modeChange.nY = nY + nSign * nYOffset * nSignY;

	return D2GAME_ModeChange_6FC65220(pGame, &modeChange, 1);
}

//D2Game.0x6FCD12C0
// not-1.13c: retail callers call 0xF1D90 directly with walk mode 2, no standalone function
// TODO: Check name
int32_t __fastcall AITACTICS_WalkInRadiusToTarget(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTarget, int32_t a4, int32_t a5)
{
	return AITACTICS_MoveInRadiusToTarget(pGame, pUnit, pTarget, MONMODE_WALK, a4, a5);
}

//D2Game.0x6FCD12E0
// 1.13c D2Game+0xF1C30 (standalone in retail, called only from the Raven AI at 0xAA1D0; EAX = target, EDI = unit)
// Retail does not use the mode-change info call: it fills a zeroed block directly (mode 2, path type 0x65) and skips the pAiControl argument.
int32_t __fastcall AITACTICS_WalkAroundTargetWithScaledDistance(D2GameStrc* pGame, D2UnitStrc* pUnit, D2AiControlStrc* pAiControl, D2UnitStrc* pTarget, int32_t nScale)
{
	const int32_t nDistance = UNITS_GetDistanceToOtherUnit(pUnit, pTarget);
	const int32_t nTargetX = CLIENTS_GetUnitX(pTarget);
	const int32_t nTargetY = CLIENTS_GetUnitY(pTarget);
	if (!nDistance)
	{
		return 0;
	}

	const int32_t nUnitX = CLIENTS_GetUnitX(pUnit);
	const int32_t nUnitY = CLIENTS_GetUnitY(pUnit);

	D2ModeChangeStrc modeChange = {};
	modeChange.nMode = MONMODE_WALK;
	modeChange.pUnit = pUnit;
	modeChange.nX = nTargetX + (nUnitX - nTargetX) * nScale / nDistance;
	modeChange.nY = nTargetY + (nUnitY - nTargetY) * nScale / nDistance;
	modeChange.unk0x14[1] = 101;
	return D2GAME_ModeChange_6FC65220(pGame, &modeChange, 1);
}

//D2Game.0x6FCD1430
// not-1.13c: retail inlines the last-attacker minion test in the Overseer AI 0x17CB0 (seen: it calls 0xE09D0 and 0xE0A60 itself), no standalone function
D2UnitStrc* __fastcall AITACTICS_GetTargetMinion(D2GameStrc* pGame, D2UnitStrc* pUnit)
{
	D2UnitStrc* pTarget = SUNIT_GetLastAttacker(pGame, pUnit);
	if (!pTarget)
	{
		return nullptr;
	}

	switch (pTarget->dwUnitType)
	{
	case UNIT_PLAYER:
		if (pTarget->dwAnimMode == PLRMODE_DEATH || pTarget->dwAnimMode == PLRMODE_DEAD)
		{
			return nullptr;
		}

		return pTarget;

	case UNIT_MONSTER:
		if (pTarget->dwAnimMode == MONMODE_DEATH || pTarget->dwAnimMode == MONMODE_DEAD || !sub_6FCBD900(pGame, pUnit, pTarget))
		{
			return nullptr;
		}

		return pTarget;
	default:
		return nullptr;
	}
}

// 1.13c D2Game+0xF2420 (checked against decompile)
//D2Game.0x6FCD1490
void __fastcall AITACTICS_UseSkillInRange(D2UnitStrc* pUnit, int32_t nRange, uint16_t wSkillId, uint8_t nMode)
{
	const int32_t nX = CLIENTS_GetUnitX(pUnit) + ITEMS_RollLimitedRandomNumber(&pUnit->pSeed, 2 * nRange) - nRange;
	const int32_t nY = CLIENTS_GetUnitY(pUnit) + ITEMS_RollLimitedRandomNumber(&pUnit->pSeed, 2 * nRange) - nRange;

	AITACTICS_UseSkill(pUnit->pGame, pUnit, nMode, wSkillId, nullptr, nX, nY);
}
