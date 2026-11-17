#include "AI/AiUtil.h"

#include <algorithm>

#include <D2Collision.h>
#include <D2Dungeon.h>
#include <D2Math.h>
#include <DataTbls/MonsterIds.h>
#include <DataTbls/MonsterTbls.h>
#include <DataTbls/ObjectsIds.h>
#include <DataTbls/SkillsIds.h>
#include <D2Monsters.h>
#include <D2States.h>
#include <D2StatList.h>

#include "AI/AiGeneral.h"
#include "AI/AiTactics.h"
#include "AI/AiThink.h"
#include "GAME/Clients.h"
#include "ITEMS/Items.h"
#include "MONSTER/Monster.h"
#include "MONSTER/MonsterAI.h"
#include "MONSTER/MonsterMode.h"
#include "MONSTER/MonsterUnique.h"
#include "UNIT/SUnit.h"
#include "UNIT/SUnitInactive.h"
#include "SKILLS/Skills.h"


#pragma pack(push, 1)
struct D2PartyCallbackArgStrc
{
	D2UnitStrc* (__fastcall* pfCallback)(D2GameStrc*, D2UnitStrc*, D2UnitStrc*, void*);
	D2GameStrc* pGame;
	D2UnitStrc* pUnit;
	void* unk0x0C;
};

struct D2UnkAiTableStrc
{
	int32_t unk0x00;
	D2UnitStrc* (__fastcall* unk0x04)(D2GameStrc*, D2UnitStrc*, D2UnitStrc*, void*);
};
#pragma pack(pop)


//Inlined in D2Game.0x6FCF1210 and D2Game.0x6FCF1310
// not-1.13c: inlined in D2Game+0xB0A20 (full unit size) and D2Game+0xB0980 (half unit size); no stand-alone function in retail.
int32_t __fastcall AIUTIL_GetDistanceToCoordinatesWithSize(D2UnitStrc* pUnit, int32_t nX, int32_t nY, int32_t nSize)
{
	// d2-web: retail 1.13c D2Game 0x6FCD0AED..0AFB (the full-size distance the target scan uses): each axis is
	// |d| - size, CLAMPED at 0 (`sub; test; jge; xor`), not |(|d| - size)|. Upstream's abs made a unit whose
	// footprint overlaps read farther than one exactly at arm's length (s233, act5barb1 #99 at frame 298).
	const int32_t nXFinal = std::max(std::abs(CLIENTS_GetUnitX(pUnit) - nX) - nSize, 0);
	const int32_t nYFinal = std::max(std::abs(CLIENTS_GetUnitY(pUnit) - nY) - nSize, 0);

	if (nXFinal <= nYFinal)
	{
		return (nXFinal + 2 * nYFinal) / 2;
	}
	else
	{
		return (nYFinal + 2 * nXFinal) / 2;
	}
}

//D2Game.0x6FCF1210
// 1.13c D2Game+0xB0A20 (checked against decompile)
int32_t __fastcall AIUTIL_GetDistanceToCoordinates_FullUnitSize(D2UnitStrc* pTarget, D2UnitStrc* pSource)
{
	return AIUTIL_GetDistanceToCoordinatesWithSize(pTarget, CLIENTS_GetUnitX(pSource), CLIENTS_GetUnitY(pSource), UNITS_GetUnitSizeX(pTarget));
}

//D2Game.0x6FCF1310
// 1.13c D2Game+0xB0980 (EBX = pUnit): both axis deltas first, then the half size, each axis clamped at 0
int32_t __fastcall AIUTIL_GetDistanceToCoordinates_HalfUnitSize(D2UnitStrc* pUnit, int32_t nX, int32_t nY)
{
	int32_t nXDiff = std::abs(CLIENTS_GetUnitX(pUnit) - nX);
	int32_t nYDiff = std::abs(CLIENTS_GetUnitY(pUnit) - nY);
	const int32_t nSize = (int32_t)((uint32_t)UNITS_GetUnitSizeX(pUnit) >> 1) + 1;
	nXDiff -= nSize;
	nYDiff -= nSize;
	if (nXDiff < 0)
	{
		nXDiff = 0;
	}
	if (nYDiff < 0)
	{
		nYDiff = 0;
	}

	if (nXDiff <= nYDiff)
	{
		return (nXDiff + 2 * nYDiff) / 2;
	}

	return (nYDiff + 2 * nXDiff) / 2;
}

//D2Game.0x6FCF13B0
// 1.13c D2Game+0xB08F0 (checked against decompile)
int32_t __fastcall AIUTIL_GetDistanceToCoordinates_NoUnitSize(D2UnitStrc* pUnit, int32_t nX, int32_t nY)
{
	const int32_t nXDiff = std::abs(CLIENTS_GetUnitX(pUnit) - nX);
	const int32_t nYDiff = std::abs(CLIENTS_GetUnitY(pUnit) - nY);

	if (nXDiff <= nYDiff)
	{
		return (nXDiff + 2 * nYDiff) / 2;
	}
	else
	{
		return (nYDiff + 2 * nXDiff) / 2;
	}
}

//D2Game.0x6FCF1440
// 1.13c D2Game+0xB07B0
uint32_t __fastcall AIUTIL_GetDistanceToCoordinates(D2UnitStrc* pUnit, int32_t nX, int32_t nY)
{
	D2_ASSERT(pUnit);
	D2_ASSERT(pUnit->pDynamicPath);

	const int32_t nXDiff = std::abs(PATH_GetXPosition(pUnit->pDynamicPath) - nX);
	const int32_t nYDiff = std::abs(PATH_GetYPosition(pUnit->pDynamicPath) - nY);

	if (nXDiff <= nYDiff)
	{
		return (nXDiff + 2 * nYDiff) / 2;
	}
	else
	{
		return (nYDiff + 2 * nXDiff) / 2;
	}
}

//D2Game.0x6FCF14D0
// 1.13c D2Game+0xB0E90
int32_t __fastcall sub_6FCF14D0(D2UnitStrc* pUnit1, D2UnitStrc* pUnit2)
{
	constexpr uint8_t nOffsets[25] = { 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

	if (!pUnit1 || !pUnit2)
	{
		return 0;
	}

	const int32_t nDistance = AIUTIL_GetDistanceToCoordinates_FullUnitSize(pUnit1, pUnit2);
	const int32_t nOffset = nDistance < std::size(nOffsets) ? nOffsets[nDistance] : 3;
	const int32_t nX2 = CLIENTS_GetUnitX(pUnit2);
	const int32_t nY2 = CLIENTS_GetUnitY(pUnit2);

	const int32_t nOffsetX = D2signum(CLIENTS_GetUnitX(pUnit1) - nX2) * nOffset;
	const int32_t nOffsetY = D2signum(CLIENTS_GetUnitY(pUnit1) - nY2) * nOffset;

	if (UNITS_TestCollisionByCoordinates(pUnit1, nX2, nY2, COLLIDE_WALL | COLLIDE_MISSILE_BARRIER | COLLIDE_DOOR)
		&& UNITS_TestCollisionByCoordinates(pUnit1, nX2 - nOffsetY, nOffsetX + nY2, COLLIDE_WALL | COLLIDE_MISSILE_BARRIER | COLLIDE_DOOR)
		&& UNITS_TestCollisionByCoordinates(pUnit1, nOffsetY + nX2, nY2 - nOffsetX, COLLIDE_WALL | COLLIDE_MISSILE_BARRIER | COLLIDE_DOOR))
	{
		return 0;
	}

	return 1;
}

//D2Game.0x6FCF16D0
// 1.13c D2Game+0xB14B0: one chain; the hostile test (0xE0A60) answering 0 counts the candidate, the nearer one is kept
D2UnitStrc* __fastcall sub_6FCF16D0(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTarget, void* pCallbackArg)
{
	D2BaalThroneAiCallbackArgStrc* pArg = (D2BaalThroneAiCallbackArgStrc*)pCallbackArg;

	if (pUnit != pTarget && pTarget && pTarget->dwUnitType == UNIT_MONSTER && !SUNIT_IsDead(pTarget))
	{
		const int32_t nDistance = AIUTIL_GetDistanceToCoordinates_FullUnitSize(pUnit, pTarget);
		if (nDistance < pArg->nMaxDistance && !sub_6FCBD900(pGame, pUnit, pTarget))
		{
			++pArg->unk0x08;
			if (nDistance < pArg->nDistance)
			{
				pArg->nDistance = nDistance;
				pArg->pTarget = pTarget;
			}
		}
	}

	return nullptr;
}

//D2Game.0x6FCF1740
// 1.13c D2Game+0xB0E30: the distance compare is unsigned (`sbb`)
D2UnitStrc* __fastcall sub_6FCF1740(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTarget, void* pCallbackArg)
{
	UnkAiStrc5* pArg = (UnkAiStrc5*)pCallbackArg;
	if (pTarget && pTarget->dwUnitType == UNIT_MONSTER && pTarget->dwAnimMode == MONMODE_DEATH)
	{
		const uint32_t nDistance = (uint32_t)AIUTIL_GetDistanceToCoordinates_FullUnitSize(pTarget, pUnit);
		return nDistance < (uint32_t)pArg->nMaxDistance ? pTarget : nullptr;
	}

	return nullptr;
}

// 1.13c D2Game+0xB08A0: the threat byte -- a player 14, a monster with a valid class its monstats threat, else 0
static int32_t AIUTIL_GetThreat(D2UnitStrc* pUnit)
{
	if (pUnit)
	{
		if (pUnit->dwUnitType == UNIT_PLAYER)
		{
			return 14;
		}

		if (pUnit->dwUnitType == UNIT_MONSTER)
		{
			D2MonStatsTxt* pMonStatsTxtRecord = MONSTERMODE_GetMonStatsTxtRecord(pUnit->dwClassId);
			if (pMonStatsTxtRecord)
			{
				return pMonStatsTxtRecord->nThreat;
			}
		}
	}

	return 0;
}

//D2Game.0x6FCF1780
// 1.13c D2Game+0xB1660
D2UnitStrc* __fastcall sub_6FCF1780(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTarget, void* pCallbackArg)
{
	D2UnkAiCallbackArgStrc* pArg = (D2UnkAiCallbackArgStrc*)pCallbackArg;

	if (!pTarget)
	{
		return nullptr;
	}

	if (pTarget->dwUnitType > UNIT_MONSTER || SUNIT_IsDead(pTarget) || !(pTarget->dwFlags & UNITFLAG_CANBEATTACKED) || !sub_6FCBD900(pGame, pUnit, pTarget))
	{
		if (pTarget->dwUnitType == UNIT_MONSTER && (!pUnit || pUnit->dwUnitType != UNIT_PLAYER) && STATLIST_GetUnitAlignment(pUnit) == UNIT_ALIGNMENT_EVIL
			&& pArg->unk0x08 && !MONSTERS_IsDead(pTarget))
		{
			D2AiControlStrc* pAiControl = AIGENERAL_GetAiControlFromUnit(pTarget);
			if (pAiControl->nAiFlags & 8)
			{
				const int32_t nY = CLIENTS_GetUnitY(pTarget);
				const int32_t nX = CLIENTS_GetUnitX(pTarget);
				if (pArg->unk0x10 == D2Common_10095(UNITS_GetRoom(pTarget), nX, nY))
				{
					pAiControl->nAiFlags |= 8u;
					pArg->unk0x08 = 0;
				}
			}
		}

		return nullptr;
	}

	const int32_t nDistance = AIUTIL_GetDistanceToCoordinates_FullUnitSize(pTarget, pUnit);
	if (nDistance > pArg->nMaxDistance)
	{
		return nullptr;
	}

	const int32_t nThreat = AIUTIL_GetThreat(pTarget);
	if (nThreat < 2)
	{
		if (nDistance >= pArg->nAlternativeDistance)
		{
			return nullptr;
		}
	}
	else if (nDistance >= pArg->nDistance)
	{
		return nullptr;
	}

	if (pArg->unk0x08 && UNITS_TestCollisionWithUnit(pTarget, pUnit, COLLIDE_MISSILE_BARRIER))
	{
		return nullptr;
	}

	if (nThreat > 1)
	{
		pArg->pTarget = pTarget;
		pArg->nDistance = nDistance;
		return nullptr;
	}

	pArg->pAlternativeTarget = pTarget;
	pArg->nAlternativeDistance = nDistance;
	return nullptr;
}

//D2Game.0x6FCF1980
// 1.13c D2Game+0xB15E0
D2UnitStrc* __fastcall sub_6FCF1980(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTarget, void* pCallbackArg)
{
	D2UnkAiCallbackArgStrc2* pArg = (D2UnkAiCallbackArgStrc2*)pCallbackArg;

	if (!sub_6FCF1A50(pGame, pUnit, pTarget))
	{
		return nullptr;
	}

	const int32_t nDistance = AIUTIL_GetDistanceToCoordinates_FullUnitSize(pTarget, pUnit);
	if (nDistance > 48)
	{
		return nullptr;
	}

	const int32_t nThreat = AIUTIL_GetThreat(pTarget);
	if (nThreat < 2)
	{
		if (nDistance >= pArg->nAlternativeDistance)
		{
			return nullptr;
		}
	}
	else if (nDistance >= pArg->nDistance)
	{
		return nullptr;
	}

	if (!UNITS_TestCollisionWithUnit(pTarget, pUnit, COLLIDE_MISSILE_BARRIER))
	{
		if (nThreat > 1)
		{
			pArg->pTarget = pTarget;
			pArg->nDistance = nDistance;
		}
		else
		{
			pArg->pAlternativeTarget = pTarget;
			pArg->nAlternativeDistance = nDistance;
		}
	}

	return nullptr;
}

//D2Game.0x6FCF1A50
// 1.13c D2Game+0xB1400 (EDI = pUnit, ESI = pTarget): the invisible-player roll draws from the TARGET's seed
int32_t __fastcall sub_6FCF1A50(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTarget)
{
	if (!pUnit || pUnit->dwUnitType > UNIT_MONSTER
		|| !pTarget || pTarget->dwUnitType > UNIT_MONSTER
		|| SUNIT_IsDead(pTarget) || SUNIT_IsDead(pUnit))
	{
		return 0;
	}

	if (DUNGEON_IsRoomInTown(UNITS_GetRoom(pTarget)) || !(pTarget->dwFlags & UNITFLAG_CANBEATTACKED))
	{
		return 0;
	}

	if (STATES_CheckState(pTarget, STATE_INVIS))
	{
		if (pTarget->dwUnitType == UNIT_MONSTER)
		{
			if (UNITS_IsInMeleeRange(pUnit, pTarget, 0))
			{
				return 0;
			}
		}
		else if (pTarget->dwUnitType == UNIT_PLAYER)
		{
			if (ITEMS_RollLimitedRandomNumber(&pTarget->pSeed, 100) < 80 && !UNITS_IsInMeleeRange(pUnit, pTarget, 0))
			{
				return 0;
			}
		}
	}

	return sub_6FCBD900(pGame, pUnit, pTarget);
}

//D2Game.0x6FCF1B30
// 1.13c D2Game+0xB1580
D2UnitStrc* __fastcall sub_6FCF1B30(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTarget, void* pCallbackArg)
{
	D2AiCallback7ArgStrc* pArg = (D2AiCallback7ArgStrc*)pCallbackArg;

	if (pTarget != pArg->unk0x08 && sub_6FCF1A50(pGame, pUnit, pTarget))
	{
		const int32_t nDistance = AIUTIL_GetDistanceToCoordinates_FullUnitSize(pTarget, pUnit);
		if (nDistance <= 48 && AIUTIL_GetThreat(pTarget) > 1 && nDistance < pArg->nDistance
			&& !UNITS_TestCollisionWithUnit(pTarget, pUnit, COLLIDE_MISSILE_BARRIER))
		{
			pArg->pTarget = pTarget;
			pArg->nDistance = nDistance;
		}
	}

	return nullptr;
}

//D2Game.0x6FCF1BD0
// 1.13c D2Game+0xB1520: the candidate filter is 0xB13B0 (type 0..1, alive, attackable, hostile)
D2UnitStrc* __fastcall sub_6FCF1BD0(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTarget, void* pCallbackArg)
{
	D2AiCallback11ArgStrc* pArg = (D2AiCallback11ArgStrc*)pCallbackArg;

	if (pTarget != pArg->unk0x08 && pTarget && pTarget->dwUnitType <= UNIT_MONSTER
		&& !SUNIT_IsDead(pTarget) && (pTarget->dwFlags & UNITFLAG_CANBEATTACKED) && sub_6FCBD900(pGame, pUnit, pTarget))
	{
		const int32_t nDistance = AIUTIL_GetDistanceToCoordinates_FullUnitSize(pTarget, pUnit);
		if (nDistance <= 48 && AIUTIL_GetThreat(pTarget) > 1 && nDistance < pArg->nDistance && sub_6FCF14D0(pUnit, pTarget))
		{
			pArg->pTarget = pTarget;
			pArg->nDistance = nDistance;
		}
	}

	return nullptr;
}

//D2Game.0x6FCF1CB0
// 1.13c D2Game+0xB0DF0
D2UnitStrc* __fastcall sub_6FCF1CB0(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTarget, void* pCallbackArg)
{
	D2DoorObjectAiCallbackArgStrc* pArg = (D2DoorObjectAiCallbackArgStrc*)pCallbackArg;

	if (!pTarget || pTarget->dwUnitType != UNIT_OBJECT || !UNITS_IsDoor(pTarget) || pTarget->dwAnimMode != OBJMODE_NEUTRAL)
	{
		return nullptr;
	}

	const int32_t nDistance = AITHINK_GetSquaredDistance(pTarget, pUnit);
	if (nDistance >= pArg->nDistance)
	{
		return nullptr;
	}

	pArg->pDoor = pTarget;
	pArg->nDistance = nDistance;
	return nullptr;
}

//D2Game.0x6FCF1DC0
// 1.13c D2Game+0xB12B0: the distance comes first, the owner test is 0x21260 (diffed: same calls, same order)
D2UnitStrc* __fastcall sub_6FCF1DC0(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTarget, void* pCallbackArg)
{
	D2FallenShamanAiCallbackArgStrc* pArg = (D2FallenShamanAiCallbackArgStrc*)pCallbackArg;
	const int32_t nDistance = AIUTIL_GetDistanceToCoordinates_FullUnitSize(pUnit, pTarget);
	if (nDistance <= pArg->nMaxDistance && pTarget && pTarget->dwUnitType == UNIT_MONSTER && pTarget->dwAnimMode == MONMODE_DEAD && pTarget->dwFlags & UNITFLAG_TARGETABLE && !STATES_CheckStateMaskUdeadOnUnit(pTarget) && AIGENERAL_GetMinionOwner(pTarget) == pUnit)
	{
		++pArg->nCounter;
		pArg->pTarget = pTarget;
		pArg->nDistance = nDistance;
	}

	return nullptr;
}

//D2Game.0x6FCF1E30
// 1.13c D2Game+0xB0DB0 (diffed: same calls, same order)
D2UnitStrc* __fastcall sub_6FCF1E30(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTarget, void* pCallbackArg)
{
	D2VileMotherAiCallbackArgStrc* pArg = (D2VileMotherAiCallbackArgStrc*)pCallbackArg;
	if (pTarget && pTarget->dwUnitType == UNIT_MONSTER && pTarget->dwClassId == pArg->nLastInClass && !SUNIT_IsDead(pTarget) && AIUTIL_GetDistanceToCoordinates_FullUnitSize(pUnit, pTarget) <= pArg->nMaxDistance)
	{
		++pArg->nCounter;
	}

	return nullptr;
}

//D2Game.0x6FCF1E80
// 1.13c D2Game+0xB17B0
D2UnitStrc* __fastcall sub_6FCF1E80(D2GameStrc* pGame, D2UnitStrc* pUnit, void* a3, D2UnitStrc*(__fastcall* a4)(D2GameStrc*, D2UnitStrc*, D2UnitStrc*, void*), int32_t nCallbackId)
{
	D2UnkAiTableStrc stru_6FD29600[13] =
	{
		{ 0, nullptr },
		{ 0, nullptr },
		{ 1, nullptr },
		{ 0, sub_6FCF16D0 },
		{ 0, sub_6FCF1740 },
		{ 2, sub_6FCF1780 },
		{ 0, sub_6FCF1980 },
		{ 0, sub_6FCF1B30 },
		{ 0, sub_6FCF1CB0 },
		{ 3, sub_6FCF1DC0 },
		{ 3, nullptr },
		{ 2, sub_6FCF1BD0 },
		{ 0, sub_6FCF1E30 },
	};

	D2UnitStrc*(__fastcall* pfCallback)(D2GameStrc*, D2UnitStrc*, D2UnitStrc*, void*) = stru_6FD29600[nCallbackId].unk0x04;

	if (!pfCallback)
	{
		pfCallback = a4;
		if (!a4)
		{
			return nullptr;
		}
	}

	switch (stru_6FD29600[nCallbackId].unk0x00)
	{
	case 0:
	{
		return AIUTIL_FindTargetInAdjacentRooms(pGame, pUnit, a3, pfCallback);
	}
	case 1:
	{
		D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pUnit);
		if (pRoom)
		{
			D2ActiveRoomStrc** ppRoomList = nullptr;
			int32_t nNumRooms = 0;
			DUNGEON_GetAdjacentRoomsListFromRoom(pRoom, &ppRoomList, &nNumRooms);

			for (int32_t i = 0; i < nNumRooms; ++i)
			{
				for (int32_t j = 0; j < ppRoomList[i]->nNumClients; ++j)
				{
					D2UnitStrc* pPlayer = CLIENTS_GetPlayerFromClient(ppRoomList[i]->ppClients[j], 0);
					if (pPlayer)
					{
						D2UnitStrc* pTarget = pfCallback(pGame, pUnit, pPlayer, a3);
						if (pTarget)
						{
							return pTarget;
						}
					}
				}
			}
		}
		return nullptr;
	}
	case 2:
	{
		return AIUTIL_FindTargetInAdjacentActiveRooms(pGame, pUnit, a3, pfCallback);
	}
	case 3:
	{
		D2UnitStrc* pOwner = AIGENERAL_GetMinionOwner(pUnit);
		if (pOwner && pOwner->dwUnitType == UNIT_MONSTER)
		{
			D2PartyCallbackArgStrc partyCallbackArg = {};
			partyCallbackArg.pfCallback = pfCallback;
			partyCallbackArg.pGame = pGame;
			partyCallbackArg.pUnit = pUnit;
			partyCallbackArg.unk0x0C = a3;

			D2UnitStrc* pTarget = nullptr;
			AIGENERAL_ExecuteCallbackOnMinions(pOwner, &partyCallbackArg, &pTarget, sub_6FCF20E0);
			return pTarget;
		}

		return nullptr;
	}
	default:
		return nullptr;
	}
}

//D2Game.0x6FCF20E0
// 1.13c D2Game+0xB06F0: only while no result is held, the callback's non-null answer is stored
void __fastcall sub_6FCF20E0(D2UnitStrc* pUnit, void* pArg, void* ppUnit)
{
	D2PartyCallbackArgStrc* pPartyCallbackArg = (D2PartyCallbackArgStrc*)pArg;

	if (!*(D2UnitStrc**)ppUnit)
	{
		D2UnitStrc* pResult = pPartyCallbackArg->pfCallback(pPartyCallbackArg->pGame, pPartyCallbackArg->pUnit, pUnit, pPartyCallbackArg->unk0x0C);
		if (pResult)
		{
			*(D2UnitStrc**)ppUnit = pResult;
		}
	}
}

//D2Game.0x6FCF2110
// 1.13c D2Game+0xB1C90
D2UnitStrc* __fastcall sub_6FCF2110(D2GameStrc* pGame, D2UnitStrc* pUnit, D2AiControlStrc* pAiControl, int32_t* pDistance, int32_t* pCombat)
{
	D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pUnit);
	if (!pRoom)
	{
		return nullptr;
	}

	// TODO: v66, v74
	int32_t v66 = 0;
	int32_t v74 = 0;
	void* pVision = nullptr;
	if (pAiControl->nAiFlags & 0x40)
	{
		v66 = 1;
		pAiControl->nAiFlags &= 0xFFBF;
	}
	else if (!DUNGEON_CheckLOSDraw(pRoom))
	{
		v66 = (pAiControl->nAiFlags & 8) == 0;

		if (pUnit && pUnit->dwUnitType == UNIT_MONSTER)
		{
			if (pUnit->pMonsterData)
			{
				pVision = (void*)pUnit->pMonsterData->pVision;
			}

			if (v66 && pVision)
			{
				v74 = *((uint32_t*)pVision + 9);
				v66 = v74 == 0;
			}
		}
	}

	D2UnitStrc* pTargetUnit = nullptr;
	int32_t nDistanceToTarget = 0;
	int32_t nCurrentDistance = INT_MAX;

	if (!sub_6FCF2920(pGame, pUnit, v66, 0, &pTargetUnit, &nDistanceToTarget))
	{
		if (STATLIST_GetUnitAlignment(pUnit) != UNIT_ALIGNMENT_EVIL)
		{
			D2UnkAiCallbackArgStrc arg = {};
			arg.pTarget = nullptr;
			arg.nDistance = INT_MAX;
			arg.unk0x08 = v66;
			arg.nMaxDistance = 35;
			arg.unk0x10 = D2Common_10095(pRoom, CLIENTS_GetUnitX(pUnit), CLIENTS_GetUnitY(pUnit));
			arg.pAlternativeTarget = nullptr;
			arg.nAlternativeDistance = INT_MAX;

			// d2-web: retail 1.13c D2Game 0x6FCD2075 calls the scan, THEN tests the alternative (0x6FCD207A:
			// mov eax,[esp+4C]; test eax,eax; je; ... call 0x6FCD1AB0): the scan's result is not a condition
			// (its callback always returns null, so upstream's `scan && ...` never took the alternative).
			sub_6FCF1E80(pGame, pUnit, &arg, nullptr, 5);
			if (arg.pAlternativeTarget && sub_6FCF27B0(pUnit, &arg.pTarget, &arg.nDistance, arg.pAlternativeTarget, arg.nAlternativeDistance))
			{
				pTargetUnit = arg.pAlternativeTarget;
				nDistanceToTarget = arg.nAlternativeDistance;
			}
			else
			{
				pTargetUnit = arg.pTarget;
				nDistanceToTarget = arg.nDistance;
			}
		}
		else
		{
			nDistanceToTarget = 35;

			if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && pUnit->pMonsterData && pUnit->pMonsterData->pMonstatsTxt)
			{
				const int32_t nAiDistance = pUnit->pMonsterData->pMonstatsTxt->nAiDist[pGame->nDifficulty];
				if (nAiDistance)
				{
					nDistanceToTarget = nAiDistance;
				}
			}

			const int32_t nUnitX = CLIENTS_GetUnitX(pUnit);
			const int32_t nUnitY = CLIENTS_GetUnitY(pUnit);

			D2TargetNodeStrc* pTargetNode = nullptr;
			for (int32_t i = 0; i < 8; ++i)
			{
				pTargetNode = pGame->pTargetNodes[i];
				if (pTargetNode)
				{
					D2_ASSERT(pTargetNode->pUnit && pTargetNode->pUnit->dwUnitType == UNIT_PLAYER);

					if (pUnit->nAct == pTargetNode->pUnit->nAct)
					{
						D2ActiveRoomStrc* pTargetRoom = UNITS_GetRoom(pTargetNode->pUnit);
						if (pTargetRoom && !DUNGEON_IsRoomInTown(pTargetRoom))
						{
							int32_t nDistance = AIUTIL_GetDistanceToCoordinates_NoUnitSize(pTargetNode->pUnit, nUnitX, nUnitY);
							if (nDistance < nCurrentDistance)
							{
								nCurrentDistance = nDistance;
							}

							if (nDistance < 55)
							{
								if (SUNIT_IsDead(pTargetNode->pUnit))
								{
									nDistance = INT_MAX;
								}

								while (1)
								{
									if (nDistance < nDistanceToTarget && (!v66 || !UNITS_TestCollisionWithUnit(pUnit, pTargetNode->pUnit, COLLIDE_MISSILE_BARRIER)))
									{
										nDistanceToTarget = nDistance;
										pTargetUnit = pTargetNode->pUnit;
									}

									pTargetNode = pTargetNode->pNext;
									if (!pTargetNode)
									{
										break;
									}

									nDistance = AIUTIL_GetDistanceToCoordinates_NoUnitSize(pTargetNode->pUnit, nUnitX, nUnitY);
								}
							}
						}
					}
				}
			}

			// d2-web: the good-aligned monsters' list (node 8: NpcBarb and the monster-region spawns register there,
			// sub_6FC40280(.., 8)). The loop above leaves pTargetNode at player slot 7, so this walk never reached it:
			// Harrogath's minions never targeted act5barb1 (s232 frame 15; retail's minion #13 targets barb #11).
			pTargetNode = pGame->pTargetNodes[8];
			while (pTargetNode)
			{
				if (pUnit->nAct == pTargetNode->pUnit->nAct)
				{
					const int32_t nTemp = AIUTIL_GetDistanceToCoordinates_NoUnitSize(pTargetNode->pUnit, nUnitX, nUnitY);
					if (nTemp < nDistanceToTarget && (!v66 || !UNITS_TestCollisionWithUnit(pUnit, pTargetNode->pUnit, COLLIDE_MISSILE_BARRIER)))
					{
						nDistanceToTarget = nTemp;
						pTargetUnit = pTargetNode->pUnit;
					}
				}

				pTargetNode = pTargetNode->pNext;
			}

			int32_t nMinDistance = INT_MAX;

			pTargetNode = pGame->pTargetNodes[9];
			if (pTargetNode)
			{
				D2UnitStrc* pPotentialTarget = nullptr;
				do
				{
					if (pUnit->nAct == pTargetNode->pUnit->nAct)
					{
						const int32_t nTemp = AIUTIL_GetDistanceToCoordinates_NoUnitSize(pTargetNode->pUnit, nUnitX, nUnitY);
						if (nTemp < nMinDistance && (!v66 || !UNITS_TestCollisionWithUnit(pUnit, pTargetNode->pUnit, COLLIDE_MISSILE_BARRIER)))
						{
							nMinDistance = nTemp;
							pPotentialTarget = pTargetNode->pUnit;
						}
					}

					pTargetNode = pTargetNode->pNext;
				}
				while (pTargetNode);

				if (pPotentialTarget && sub_6FCF27B0(pUnit, &pTargetUnit, &nDistanceToTarget, pPotentialTarget, nMinDistance))
				{
					pTargetUnit = pPotentialTarget;
					nDistanceToTarget = nMinDistance;
				}
			}
		}
	}

	if (!pTargetUnit)
	{
		*pCombat = 0;
		*pDistance = nCurrentDistance;
		return nullptr;
	}

	if (STATLIST_GetUnitAlignment(pUnit) != UNIT_ALIGNMENT_GOOD)
	{
		pAiControl->nAiFlags |= 8u;

		if (pVision)
		{
			*((DWORD*)pVision + 9) = v74 == 0;
		}
	}

	*pCombat = UNITS_IsInMeleeRange(pUnit, pTargetUnit, 0);
	*pDistance = nDistanceToTarget;
	return pTargetUnit;
}

//D2Game.0x6FCF27B0
// 1.13c D2Game+0xB1AB0
int32_t __fastcall sub_6FCF27B0(D2UnitStrc* pUnit, D2UnitStrc** ppTarget, int32_t* pTargetDistance, D2UnitStrc* a4, int32_t a5)
{
	if ((pUnit && pUnit->dwUnitType == UNIT_PLAYER) || !a4)
	{
		return 0;
	}

	if (!*ppTarget)
	{
		return 1;
	}

	if (a5 > 5)
	{
		return 0;
	}

	const int32_t nPathType = PATH_GetType(pUnit->pDynamicPath);
	D2UnitStrc* pTarget = PATH_GetTargetUnit(pUnit->pDynamicPath);
	const int32_t nX = D2COMMON_10175_PathGetFirstPointX(pUnit->pDynamicPath);
	const int32_t nY = D2COMMON_10176_PathGetFirstPointY(pUnit->pDynamicPath);

	PATH_SetTargetUnit(pUnit->pDynamicPath, *ppTarget);
	PATH_SetType(pUnit->pDynamicPath, PATHTYPE_TOWARD);
	D2Common_10142(pUnit->pDynamicPath, pUnit, 0);
	PATH_SetTargetUnit(pUnit->pDynamicPath, pTarget);
	D2COMMON_10170_PathSetTargetPos(pUnit->pDynamicPath, nX, nY);
	PATH_SetType(pUnit->pDynamicPath, nPathType);

	if (PATH_GetNumberOfPathPoints(pUnit->pDynamicPath))
	{
		return 0;
	}

	D2AiCallback7ArgStrc arg = {};
	arg.pTarget = nullptr;
	arg.nDistance = INT_MAX;
	arg.unk0x08 = *ppTarget;

	sub_6FCF1E80(pUnit->pGame, pUnit, &arg, nullptr, 7);

	if (!arg.pTarget || arg.nDistance >= 20)
	{
		return 1;
	}

	*ppTarget = arg.pTarget;
	*pTargetDistance = arg.nDistance;

	return 0;
}

//D2Game.0x6FCF2920
// 1.13c D2Game+0xB1820 (EAX = pUnit): the remembered target (type at monster data +0x38, GUID at +0x34)
// is looked up, or for type 3 the area is scanned with the alignment flipped by a 1-in-2 roll and restored.
int32_t __fastcall sub_6FCF2920(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t a3, int32_t a4, D2UnitStrc** ppUnit, int32_t* pDistance)
{
	if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER || !pUnit->pMonsterData)
	{
		return 0;
	}

	const int32_t nTargetType = sub_6FC61EC0(pUnit);
	if (nTargetType == 0)
	{
		return 0;
	}

	D2UnitStrc* pTarget = nullptr;
	int32_t nDistance = 0;

	switch (nTargetType)
	{
	case 1:
	case 2:
	case 4:
	{
		const int32_t nUnitType = nTargetType == 1 ? UNIT_PLAYER : (nTargetType == 2 ? UNIT_MONSTER : UNIT_MISSILE);
		pTarget = SUNIT_GetServerUnit(pGame, nUnitType, sub_6FC61EE0(pUnit));
		if (!pTarget)
		{
			sub_6FC61F00(pUnit);
			return 0;
		}

		nDistance = AIUTIL_GetDistanceToCoordinates_FullUnitSize(pTarget, pUnit);
		if (a4 && UNITS_TestCollisionWithUnit(pTarget, pUnit, COLLIDE_MISSILE_BARRIER))
		{
			sub_6FC61F00(pUnit);
			return 0;
		}
		break;
	}
	case 3:
	{
		const int32_t nAlignment = STATLIST_GetUnitAlignment(pUnit);
		const uint32_t nRand = ITEMS_RollLimitedRandomNumber(&pUnit->pSeed, 2);
		if (nAlignment == UNIT_ALIGNMENT_NEUTRAL)
		{
			sub_6FCBDD30(pUnit, nRand ? UNIT_ALIGNMENT_GOOD : UNIT_ALIGNMENT_EVIL, 1);
		}
		else if (nRand)
		{
			if (nAlignment == UNIT_ALIGNMENT_EVIL)
			{
				sub_6FCBDD30(pUnit, UNIT_ALIGNMENT_GOOD, 1);
			}
			else if (nAlignment == UNIT_ALIGNMENT_GOOD)
			{
				sub_6FCBDD30(pUnit, UNIT_ALIGNMENT_EVIL, 1);
			}
		}

		if (!a4)
		{
			D2UnkAiCallbackArgStrc arg = {};
			arg.pTarget = nullptr;
			arg.nDistance = INT_MAX;
			const int32_t nY = CLIENTS_GetUnitY(pUnit);
			const int32_t nX = CLIENTS_GetUnitX(pUnit);
			arg.unk0x10 = D2Common_10095(UNITS_GetRoom(pUnit), nX, nY);
			arg.unk0x08 = a3;
			arg.nMaxDistance = 35;
			arg.pAlternativeTarget = nullptr;
			arg.nAlternativeDistance = INT_MAX;

			sub_6FCF1E80(pGame, pUnit, &arg, nullptr, 5);
			pTarget = arg.pTarget;
			nDistance = arg.nDistance;
		}
		else
		{
			D2UnkAiCallbackArgStrc2 arg = {};
			arg.pTarget = nullptr;
			arg.nDistance = INT_MAX;
			arg.pAlternativeTarget = nullptr;
			arg.nAlternativeDistance = INT_MAX;

			sub_6FCF1E80(pGame, pUnit, &arg, nullptr, 6);
			pTarget = arg.pTarget;
			nDistance = arg.nDistance;
		}

		sub_6FCBDD30(pUnit, nAlignment, 1);
		break;
	}
	default:
		sub_6FC61F00(pUnit);
		return 0;
	}

	if (pTarget)
	{
		if (pTarget->dwUnitType <= UNIT_MONSTER)
		{
			if (!SUNIT_IsDead(pTarget))
			{
				*ppUnit = pTarget;
				*pDistance = nDistance;
				return 1;
			}
		}
		else if (pTarget->dwUnitType == UNIT_MISSILE)
		{
			*ppUnit = pTarget;
			*pDistance = nDistance;
			return 1;
		}
	}

	sub_6FC61F00(pUnit);
	return 0;
}

//D2Game.0x6FCF2B80
// 1.13c D2Game+0xB0720 (diffed: room, adjacent list, first non-null callback answer)
D2UnitStrc* __fastcall AIUTIL_FindTargetInAdjacentRooms(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pArg, D2UnitStrc* (__fastcall* pfCallback)(D2GameStrc*, D2UnitStrc*, D2UnitStrc*, void*))
{
	D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pUnit);
	if (!pRoom)
	{
		return nullptr;
	}

	D2ActiveRoomStrc** ppRoomList = nullptr;
	int32_t nNumRooms = 0;
	DUNGEON_GetAdjacentRoomsListFromRoom(pRoom, &ppRoomList, &nNumRooms);

	for (int32_t i = 0; i < nNumRooms; ++i)
	{
		for (D2UnitStrc* j = ppRoomList[i]->pUnitFirst; j; j = j->pRoomNext)
		{
			D2UnitStrc* pTarget = pfCallback(pGame, pUnit, j, pArg);
			if (pTarget)
			{
				return pTarget;
			}
		}
	}

	return nullptr;
}

//D2Game.0x6FCF2C00
// 1.13c D2Game+0xB0CE0
D2UnitStrc* __fastcall AIUTIL_FindTargetInAdjacentActiveRooms(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pArg, D2UnitStrc*(__fastcall* pfCallback)(D2GameStrc*, D2UnitStrc*, D2UnitStrc*, void*))
{
	if (!pUnit || (pUnit->dwUnitType != UNIT_PLAYER && pUnit->dwUnitType != UNIT_MONSTER) || SUNIT_IsDead(pUnit))
	{
		return nullptr;
	}

	D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pUnit);
	if (!pRoom)
	{
		return nullptr;
	}

	D2ActiveRoomStrc** ppRoomList = nullptr;
	int32_t nNumRooms = 0;
	DUNGEON_GetAdjacentRoomsListFromRoom(pRoom, &ppRoomList, &nNumRooms);

	for (int32_t i = 0; i < nNumRooms; ++i)
	{
		if (!DUNGEON_IsRoomInTown(ppRoomList[i]) && ppRoomList[i]->nNumClients)
		{
			for (D2UnitStrc* j = ppRoomList[i]->pUnitFirst; j; j = j->pRoomNext)
			{
				D2UnitStrc* pTarget = pfCallback(pGame, pUnit, j, pArg);
				if (pTarget)
				{
					return pTarget;
				}
			}
		}
	}

	return nullptr;
}

//D2Game.0x6FCF2CC0
// 1.13c D2Game+0xB1BC0
D2UnitStrc* __fastcall sub_6FCF2CC0(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t* pDistance, int32_t* pInMeleeRange)
{
	D2UnitStrc* pTarget = nullptr;
	int32_t nDistance = 0;

	if (!sub_6FCF2920(pGame, pUnit, 0, 1, &pTarget, &nDistance))
	{
		D2UnkAiCallbackArgStrc2 arg = {};
		arg.pTarget = nullptr;
		arg.nDistance = INT_MAX;
		arg.pAlternativeTarget = nullptr;
		arg.nAlternativeDistance = INT_MAX;

		sub_6FCF1E80(pGame, pUnit, &arg, nullptr, 6);

		if (sub_6FCF27B0(pUnit, &arg.pTarget, &arg.nDistance, arg.pAlternativeTarget, arg.nAlternativeDistance))
		{
			pTarget = arg.pAlternativeTarget;
			nDistance = arg.nAlternativeDistance;
		}
		else
		{
			pTarget = arg.pTarget;
			nDistance = arg.nDistance;
		}
	}

	if (pTarget)
	{
		if (pInMeleeRange)
		{
			*pInMeleeRange = UNITS_IsInMeleeRange(pUnit, pTarget, 0);
		}

		if (pDistance)
		{
			*pDistance = nDistance;
		}

		return pTarget;
	}

	if (pInMeleeRange)
	{
		*pInMeleeRange = 0;
	}

	if (pDistance)
	{
		*pDistance = INT_MAX;
	}

	return nullptr;
}

//D2Game.0x6FCF2DF0
// not-1.13c: retail inlines this flag test into its callers (e.g. D2Game+0x21260, +0x9CB30)
int32_t __fastcall AIUTIL_CheckAiControlFlag(D2AiControlStrc* pAiControl, uint16_t nFlag)
{
	return (pAiControl->nAiFlags & nFlag) != 0;
}

//D2Game.0x6FCF2E00
// not-1.13c: retail inlines this flag set/clear into its callers (e.g. D2Game+0x21BE0, +0xF2540)
void __fastcall AIUTIL_ToggleAiControlFlag(D2AiControlStrc* pAiControl, uint16_t nFlag, int32_t bSet)
{
	if (bSet)
	{
		pAiControl->nAiFlags |= nFlag;
	}
	else
	{
		pAiControl->nAiFlags &= ~nFlag;
	}
}

//D2Game.0x6FCF2E20
// not-1.13c: retail inlines the ai-control lookup and the 0x20 flag set/clear into its callers (e.g. D2Game+0xF2540)
void __fastcall AIUTIL_ToggleAiControlFlag0x20(D2UnitStrc* pUnit, int32_t bSet)
{
	D2AiControlStrc* pAiControl = AIGENERAL_GetAiControlFromUnit(pUnit);
	if (pAiControl)
	{
		AIUTIL_ToggleAiControlFlag(pAiControl, 0x20, bSet);
	}
}

//D2Game.0x6FCF2E70
// 1.13c D2Game+0xB0BE0
int32_t __fastcall sub_6FCF2E70(D2UnitStrc* pUnit)
{
	if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && pUnit->pMonsterData
		&& (pUnit->pMonsterData->dwAiState == 3 || pUnit->pMonsterData->dwAiState == 19))
	{
		return 1;
	}
	return 0;
}

//D2Game.0x6FCF2E90
// 1.13c D2Game+0xB0840
int32_t __fastcall AIUTIL_CheckIfMonsterUsesSkill(D2UnitStrc* pUnit, int32_t nSkillId)
{
	if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER)
	{
		return 0;
	}

	// Retail inlines the monstats lookup: class bounds-checked against the table count.
	const int32_t nClassId = pUnit->dwClassId;
	if (nClassId < 0 || nClassId >= sgptDataTables->nMonStatsTxtRecordCount || !sgptDataTables->pMonStatsTxt)
	{
		return 0;
	}

	const int16_t* pSkill = sgptDataTables->pMonStatsTxt[nClassId].nSkill;
	for (int32_t i = 0; i < 8; ++i)
	{
		if (pSkill[i] == nSkillId)
		{
			return 1;
		}
	}
	return 0;
}

//D2Game.0x6FCF2EF0
// 1.13c D2Game+0xB0BA0
void __fastcall AIUTIL_SetOwnerGUIDAndType(D2UnitStrc* pUnit, D2UnitStrc* pOwner)
{
	D2AiControlStrc* pAiControl = AIGENERAL_GetAiControlFromUnit(pUnit);
	if (!pAiControl)
	{
		return;
	}

	if (pOwner)
	{
		pAiControl->dwOwnerGUID = pOwner->dwUnitId;
		pAiControl->dwOwnerType = pOwner->dwUnitType;
	}
	else
	{
		pAiControl->dwOwnerGUID = -1;
		pAiControl->dwOwnerType = 1;
	}
}

//D2Game.0x6FCF2F30
// 1.13c D2Game+0xB2120 (nSkillId/nParam1 are not read by retail)
void __fastcall AIUTIL_ApplyTerrorCurseState(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTarget, int32_t nSkillId, int32_t nParam1, int32_t nDuration)
{
	// Retail inlines SUNIT_IsDead, then the switch-ai gates of D2Game+0xB0B20 without the
	// per-state AITHINK check, plus a superunique type-flag test and a second CanSwitchAI call.
	if (!pTarget || (pTarget->dwFlags & 0x10000))
	{
		return;
	}

	if (pTarget->dwUnitType == UNIT_PLAYER)
	{
		if (pTarget->dwAnimMode == PLRMODE_DEATH || pTarget->dwAnimMode == PLRMODE_DEAD)
		{
			return;
		}
	}
	else if (pTarget->dwUnitType == UNIT_MONSTER)
	{
		if (pTarget->dwAnimMode == MONMODE_DEATH || pTarget->dwAnimMode == MONMODE_DEAD)
		{
			return;
		}
	}
	else
	{
		return;
	}

	if (pTarget->dwUnitType != UNIT_MONSTER
		|| STATES_CheckState(pTarget, STATE_UNINTERRUPTABLE)
		|| !UNITS_CanSwitchAI(pTarget->dwClassId))
	{
		return;
	}

	if ((pTarget->dwFlags & UNITFLAG_CANBEATTACKED) == 0 && !SUNIT_IsDead(pTarget))
	{
		return;
	}

	if (MONSTERUNIQUE_CheckMonTypeFlag(pTarget, MONTYPEFLAG_UNIQUE | MONTYPEFLAG_SUPERUNIQUE))
	{
		return;
	}

	if (pTarget->dwUnitType == UNIT_MONSTER && pTarget->pMonsterData && (pTarget->pMonsterData->nTypeFlag & MONTYPEFLAG_SUPERUNIQUE))
	{
		return;
	}

	if (!UNITS_CanSwitchAI(pTarget->dwClassId))
	{
		return;
	}

	D2CurseStrc curse = {};
	curse.pUnit = pUnit;
	curse.pTarget = pTarget;
	curse.nSkill = SKILL_TERROR;
	curse.nSkillLevel = 1;
	curse.nDuration = nDuration;
	curse.nStat = -1;
	curse.nState = STATE_TERROR;
	sub_6FD10EC0(&curse);

	UNITS_SetTargetUnitForDynamicUnit(pTarget, pUnit);

	D2AiControlStrc* pAiControl = (pTarget->dwUnitType == UNIT_MONSTER && pTarget->pMonsterData) ? pTarget->pMonsterData->pAiControl : nullptr;
	AITHINK_ExecuteAiFn(pGame, pTarget, pAiControl, AISPECIALSTATE_TERROR);
	AITACTICS_Idle(pGame, pTarget, 1);
}

//D2Game.0x6FCF3000
// 1.13c D2Game+0xB0B20
BOOL __fastcall AIUTIL_CanUnitSwitchAi(D2UnitStrc* pUnit, D2C_AiSpecialState nAiSpecialState)
{
	if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER 
		|| nAiSpecialState > AISPECIALSTATE_INVALID
		|| STATES_CheckState(pUnit, STATE_UNINTERRUPTABLE) 
		|| !UNITS_CanSwitchAI(pUnit->dwClassId))
	{
		return FALSE;
	}

	if ((pUnit->dwFlags & UNITFLAG_CANBEATTACKED) == 0 && !SUNIT_IsDead(pUnit))
	{
		return FALSE;
	}

	if (MONSTERUNIQUE_CheckMonTypeFlag(pUnit, MONTYPEFLAG_UNIQUE | MONTYPEFLAG_SUPERUNIQUE))
	{
		return FALSE;
	}

	if (nAiSpecialState != AISPECIALSTATE_INVALID
		&& !AITHINK_CanUnitSwitchAi(pUnit, MONSTERMODE_GetMonStatsTxtRecord(pUnit->dwClassId), nAiSpecialState, 1))
	{
		return FALSE;
	}

	return TRUE;
}
