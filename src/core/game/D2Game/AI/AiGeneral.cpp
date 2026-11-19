#include "AI/AiGeneral.h"

#include <test_seam.h>

#include <Drlg/D2DrlgPreset.h>
#include <D2Monsters.h>

#include "AI/AiUtil.h"
#include "MONSTER/Monster.h"
#include "UNIT/SUnit.h"
#include "UNIT/SUnitInactive.h"


//D2Game.0x6FCCED00
// not-1.13c: 1.13c inlines the pool allocation into the monster initializer (+0xB0320); no stand-alone function.
D2AiControlStrc* __fastcall AIGENERAL_AllocAiControl(D2GameStrc* pGame)
{
	D2AiControlStrc* pAiControl = D2_CALLOC_STRC_POOL(pGame->pMemoryPool, D2AiControlStrc);
	pAiControl->dwOwnerTypeEx = UNIT_MONSTER;
	pAiControl->dwOwnerType = UNIT_MONSTER;
	pAiControl->dwOwnerGUIDEx = -1;
	pAiControl->dwOwnerGUID = -1;

	return pAiControl;
}

// not-1.13c: retail inlines the index 1/2/3 -> control+0x14/+0x18/+0x1C store at each caller
void __fastcall AIGENERAL_SetAiControlParam(D2UnitStrc* pMonster, int32_t nIndex, int32_t nParamValue)
{
	D2AiControlStrc* pAiControl = AIGENERAL_GetAiControlFromUnit(pMonster);
	if (!pAiControl)
	{
		return;
	}

	switch (nIndex)
	{
	case 1:
		pAiControl->dwAiParam[0] = nParamValue;
		break;

	case 2:
		pAiControl->dwAiParam[1] = nParamValue;
		break;

	case 3:
		pAiControl->dwAiParam[2] = nParamValue;
		break;
	default:
		break;
	}
}

// not-1.13c: retail inlines the index 1/2/3 -> control+0x14/+0x18/+0x1C load at each caller
int32_t __fastcall AIGENERAL_GetAiControlParam(D2UnitStrc* pUnit, int32_t nIndex)
{
	D2AiControlStrc* pAiControl = AIGENERAL_GetAiControlFromUnit(pUnit);
	if (!pAiControl)
	{
		return 0;
	}

	switch (nIndex)
	{
	case 1:
		return pAiControl->dwAiParam[0];

	case 2:
		return pAiControl->dwAiParam[1];

	case 3:
		return pAiControl->dwAiParam[2];
	default:
		break;
	}

	return 0;
}

//D2Game.0x6FCCEDC0
// 1.13c D2Game+0x20F30
D2AiCmdStrc* __fastcall AIGENERAL_AllocAiCommand(D2GameStrc* pGame, D2UnitStrc* pUnit)
{
	D2AiControlStrc* pAiControl = AIGENERAL_GetAiControlFromUnit(pUnit);
	if (!pAiControl)
	{
		return nullptr;
	}

	D2AiCmdStrc* pAiCmd = D2_CALLOC_STRC_POOL(pGame->pMemoryPool, D2AiCmdStrc);
	D2AiCmdStrc* pLastCmd = pAiControl->pLastCmd;
	if (pLastCmd)
	{
		D2AiCmdStrc* pCurrentCmd = pAiControl->pCurrentCmd;
		if (pLastCmd == pCurrentCmd)
		{
			pAiControl->pLastCmd = pAiCmd;
		}

		pAiCmd->pNextCmd = pCurrentCmd;
		pAiCmd->pPrevCmd = pAiControl->pCurrentCmd->pPrevCmd;
		pAiControl->pCurrentCmd->pPrevCmd = pAiCmd;
		pAiCmd->pPrevCmd->pNextCmd = pAiCmd;
	}
	else
	{
		pAiControl->pLastCmd = pAiCmd;
		pAiControl->pCurrentCmd = pAiCmd;
		pAiCmd->pNextCmd = pAiCmd;
		pAiCmd->pPrevCmd = pAiCmd;
	}

	return pAiCmd;
}

//D2Game.0x6FCCEE40
// 1.13c D2Game+0x20EC0
void __fastcall AIGENERAL_FreeCurrentAiCommand(D2GameStrc* pGame, D2UnitStrc* pUnit)
{
	D2AiControlStrc* pAiControl = AIGENERAL_GetAiControlFromUnit(pUnit);
	if (!pAiControl)
	{
		return;
	}

	D2AiCmdStrc* pCurrentCmd = pAiControl->pCurrentCmd;
	if (!pCurrentCmd)
	{
		return;
	}

	if (pCurrentCmd == pCurrentCmd->pNextCmd)
	{
		pAiControl->pLastCmd = nullptr;
		pAiControl->pCurrentCmd = nullptr;
	}
	else
	{
		if (pAiControl->pLastCmd == pCurrentCmd)
		{
			pAiControl->pLastCmd = pCurrentCmd->pNextCmd;
		}

		pAiControl->pCurrentCmd = pCurrentCmd->pNextCmd;
		pCurrentCmd->pPrevCmd->pNextCmd = pCurrentCmd->pNextCmd;
		pCurrentCmd->pNextCmd->pPrevCmd = pCurrentCmd->pPrevCmd;
	}

	D2_FREE_POOL(pGame->pMemoryPool, pCurrentCmd);
}

//D2Game.0x6FCCEEB0
// 1.13c D2Game+0x21420
void __fastcall AIGENERAL_FreeAllAiCommands(D2GameStrc* pGame, D2UnitStrc* pUnit)
{
	D2AiControlStrc* pAiControl = AIGENERAL_GetAiControlFromUnit(pUnit);
	if (!pAiControl)
	{
		return;
	}

	while (pAiControl->pLastCmd)
	{
		AIGENERAL_FreeCurrentAiCommand(pGame, pUnit);
	}
}

// not-1.13c: retail inlines the control->pCurrentCmd (+0x20) read at each caller
D2AiCmdStrc* __fastcall AIGENERAL_GetCurrentAiCommandFromUnit(D2UnitStrc* pUnit)
{
	D2AiControlStrc* pAiControl = AIGENERAL_GetAiControlFromUnit(pUnit);
	if (!pAiControl)
	{
		return nullptr;
	}

	return pAiControl->pCurrentCmd;
}

//D2Game.0x6FCCEF10
// 1.13c D2Game+0x20E00
D2AiCmdStrc* __fastcall AIGENERAL_GetAiCommandFromParam(D2UnitStrc* pUnit, int32_t nCmdParam, int32_t bSet)
{
	if (D2Seam<&AIGENERAL_GetAiCommandFromParam>::hook) return D2Seam<&AIGENERAL_GetAiCommandFromParam>::hook(pUnit, nCmdParam, bSet);
	D2AiControlStrc* pAiControl = AIGENERAL_GetAiControlFromUnit(pUnit);
	if (!pAiControl)
	{
		return nullptr;
	}

	D2AiCmdStrc* pCurrentAiCmd = pAiControl->pCurrentCmd;
	if (!pCurrentAiCmd)
	{
		return nullptr;
	}

	D2AiCmdStrc* pAiCmd = pCurrentAiCmd->pNextCmd;
	while (pAiCmd->nCmdParam[0] != nCmdParam)
	{
		if (pAiCmd == pCurrentAiCmd)
		{
			break;
		}

		pAiCmd = pAiCmd->pNextCmd;
	}

	if (pAiCmd->nCmdParam[0] != nCmdParam)
	{
		return nullptr;
	}

	if (bSet)
	{
		pAiControl->pCurrentCmd = pAiCmd;
	}

	return pAiCmd;
}

//D2Game.0x6FCCEF70
// 1.13c D2Game+0x212F0
void __fastcall AIGENERAL_AllocCommandsForMinions(D2GameStrc* pGame, D2UnitStrc* pUnit, D2AiCmdStrc* pAiCmd)
{
	if (D2Seam<&AIGENERAL_AllocCommandsForMinions>::hook) { D2Seam<&AIGENERAL_AllocCommandsForMinions>::hook(pGame, pUnit, pAiCmd); return; }
	D2AiControlStrc* pAiControl = AIGENERAL_GetAiControlFromUnit(pUnit);
	if (!pAiControl || !pAiControl->pGame || pUnit->dwUnitType != UNIT_MONSTER)
	{
		return;
	}

	D2UnitStrc* pOwner = SUNIT_GetServerUnit(pAiControl->pGame, pAiControl->dwOwnerTypeEx, pAiControl->dwOwnerGUIDEx);
	D2AiControlStrc* pOwnerAiControl = AIGENERAL_GetAiControlFromUnit(pOwner);
	if (!pOwnerAiControl || pOwner->dwUnitType != UNIT_MONSTER)
	{
		return;
	}

	for (D2MinionListStrc* pMinionList = pOwnerAiControl->pMinionList; pMinionList; pMinionList = pMinionList->pNext)
	{
		D2_ASSERT(pOwnerAiControl->pGame);
		D2UnitStrc* pMinion = SUNIT_GetServerUnit(pOwnerAiControl->pGame, UNIT_MONSTER, pMinionList->dwMinionGUID);
		if (!pMinion)
		{
			continue;
		}

		// A minion with no command slot is skipped; retail does not dereference the allocation.
		D2AiCmdStrc* pCopy = AIGENERAL_AllocAiCommand(pGame, pMinion);
		if (!pCopy)
		{
			continue;
		}
		memcpy(pCopy->nCmdParam, pAiCmd->nCmdParam, sizeof(pCopy->nCmdParam));

		// Retail stores the new command pointer in the minion's game block (+0x20), not in its control block.
		D2AiControlStrc* pMinionAiControl = AIGENERAL_GetAiControlFromUnit(pMinion);
		if (pMinion->dwUnitType == UNIT_MONSTER && pMinionAiControl && pMinionAiControl->pGame)
		{
			pMinionAiControl->pGame->nGameData = (uint32_t)(uintptr_t)pCopy;
		}
	}
}

//D2Game.0x6FCCF050
// 1.13c D2Game+0x21290
D2AiCmdStrc* __fastcall AIGENERAL_CopyAiCommand(D2GameStrc* pGame, D2UnitStrc* pUnit, D2AiCmdStrc* pAiCmd)
{
	if (D2Seam<&AIGENERAL_CopyAiCommand>::hook) return D2Seam<&AIGENERAL_CopyAiCommand>::hook(pGame, pUnit, pAiCmd);
	D2AiCmdStrc* pCopy = AIGENERAL_AllocAiCommand(pGame, pUnit);
	memcpy(pCopy->nCmdParam, pAiCmd->nCmdParam, sizeof(pCopy->nCmdParam));

	D2AiControlStrc* pAiControl = AIGENERAL_GetAiControlFromUnit(pUnit);
	if (pAiControl)
	{
		pAiControl->pCurrentCmd = pCopy;
	}

	return pCopy;
}

//D2Game.0x6FCCF090
// 1.13c D2Game+0x217D0
D2AiCmdStrc* __fastcall AIGENERAL_SetCurrentAiCommand(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nCmdParam, int32_t bSet)
{
	if (D2Seam<&AIGENERAL_SetCurrentAiCommand>::hook) return D2Seam<&AIGENERAL_SetCurrentAiCommand>::hook(pGame, pUnit, nCmdParam, bSet);
	// 1.13c: no AiControl check up front; a missing command is created through the copy routine (+0x21290)
	// from a block whose params are { nCmdParam, 0, 0, 0, 0 }.
	if (!D2_CALL(AIGENERAL_GetAiCommandFromParam, pUnit, nCmdParam, bSet))
	{
		D2AiCmdStrc tBlock = {};
		tBlock.nCmdParam[0] = nCmdParam;
		D2_CALL(AIGENERAL_CopyAiCommand, pGame, pUnit, &tBlock);
	}

	return D2_CALL(AIGENERAL_GetAiCommandFromParam, pUnit, nCmdParam, bSet);
}

// 1.13c D2Game+0x20B60: free every command while both the last and the current pointer are set
void __fastcall AIGENERAL_FreeAiCommandChain(D2GameStrc* pGame, D2AiControlStrc* pAiControl)
{
	for (D2AiCmdStrc* i = pAiControl->pLastCmd; i; i = pAiControl->pLastCmd)
	{
		D2AiCmdStrc* pCurrentCmd = pAiControl->pCurrentCmd;
		if (!pCurrentCmd)
		{
			break;
		}

		if (pCurrentCmd == pCurrentCmd->pNextCmd)
		{
			pAiControl->pLastCmd = nullptr;
			pAiControl->pCurrentCmd = nullptr;
		}
		else
		{
			if (i == pCurrentCmd)
			{
				pAiControl->pLastCmd = pCurrentCmd->pNextCmd;
			}

			pAiControl->pCurrentCmd = pCurrentCmd->pNextCmd;

			pCurrentCmd->pPrevCmd->pNextCmd = pCurrentCmd->pNextCmd;
			pCurrentCmd->pNextCmd->pPrevCmd = pCurrentCmd->pPrevCmd;
		}

		D2_FREE_POOL(pGame->pMemoryPool, pCurrentCmd);
	}
}

// 1.13c D2Game+0x20C00 (the command-chain free loop is 0x20B60, the minion-node loop is 0x20B30 inlined)
void __fastcall AIGENERAL_FreeAiControl(D2GameStrc* pGame, D2AiControlStrc* pAiControl)
{
	if (!pAiControl)
	{
		return;
	}

	if (pAiControl->pMapAi)
	{
		D2_CALL(DRLGPRESET_FreeMapAI, pGame->pMemoryPool, pAiControl->pMapAi);
	}

	D2MinionListStrc* pNext = nullptr;
	for (D2MinionListStrc* i = pAiControl->pMinionList; i; i = pNext)
	{
		pNext = i->pNext;
		D2_FREE_POOL(pGame->pMemoryPool, i);
	}

	D2_CALL(AIGENERAL_FreeAiCommandChain, pGame, pAiControl);

	D2_FREE_POOL(pGame->pMemoryPool, pAiControl);
}

// 1.13c D2Game+0x20DE0
// 1.13c does not null-check the control: with none it returns &((D2AiControlStrc*)nullptr)->pMapAi (0x38).
D2MapAIStrc** __stdcall AIGENERAL_GetMapAiFromUnit(D2UnitStrc* pUnit)
{
	D2AiControlStrc* pAiControl = AIGENERAL_GetAiControlFromUnit(pUnit);
	return &pAiControl->pMapAi;
}

// 1.13c D2Game+0x20D90
void __fastcall AIGENERAL_SetOwnerData(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nOwnerGUID, int32_t nOwnerType, int32_t bSetFlag1, int32_t bSetFlag2)
{
	// retail only acts on a monster unit (type 1) and writes straight through its control
	D2AiControlStrc* pAiControl = AIGENERAL_GetAiControlFromUnit(pUnit);
	if (!pAiControl)
	{
		return;
	}

	if (bSetFlag2)
	{
		pAiControl->nAiFlags |= 2;
	}

	if (bSetFlag1)
	{
		pAiControl->nAiFlags |= 1;
	}

	pAiControl->dwOwnerGUIDEx = nOwnerGUID;
	pAiControl->dwOwnerTypeEx = nOwnerType;
	pAiControl->pGame = pGame;
}

//D2Game.0x6FCCF2D0
// 1.13c D2Game+0x20d50
void __fastcall AIGENERAL_GetOwnerData(D2UnitStrc* pUnit, int32_t* pUnitGUID, int32_t* pUnitType)
{
	D2AiControlStrc* pAiControl = AIGENERAL_GetAiControlFromUnit(pUnit);
	if (pAiControl && pAiControl->pGame)
	{
		*pUnitGUID = pAiControl->dwOwnerGUIDEx;
		*pUnitType = pAiControl->dwOwnerTypeEx;
	}
	else
	{
		*pUnitGUID = -1;
	}
}

//D2Game.0x6FCCF320
// 1.13c D2Game+0x21260
D2UnitStrc* __fastcall AIGENERAL_GetMinionOwner(D2UnitStrc* pUnit)
{
	if (D2Seam<&AIGENERAL_GetMinionOwner>::hook) return D2Seam<&AIGENERAL_GetMinionOwner>::hook(pUnit);
	D2AiControlStrc* pAiControl = AIGENERAL_GetAiControlFromUnit(pUnit);
	if (pAiControl && pAiControl->pGame)
	{
		return D2_CALL(SUNIT_GetServerUnit, pAiControl->pGame, pAiControl->dwOwnerTypeEx, pAiControl->dwOwnerGUIDEx);
	}

	return nullptr;
}

// 1.13c D2Game+0x20CF0
void __fastcall AIGENERAL_AllocMinionList(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pMinion)
{
	// retail returns without allocating unless pUnit is a monster (type 1)
	D2AiControlStrc* pAiControl = AIGENERAL_GetAiControlFromUnit(pUnit);
	if (!pAiControl)
	{
		return;
	}

	D2MinionListStrc* pMinionList = D2_CALLOC_STRC_POOL(pGame->pMemoryPool, D2MinionListStrc);

	if (pMinion)
	{
		pMinionList->dwMinionGUID = pMinion->dwUnitId;
	}
	else
	{
		pMinionList->dwMinionGUID = -1;
	}

	pMinionList->pNext = pAiControl->pMinionList;
	pAiControl->pMinionList = pMinionList;
}

// 1.13c D2Game+0x216A0
void __fastcall AIGENERAL_FreeMinionList(D2UnitStrc* pUnit)
{
	if (D2Seam<&AIGENERAL_FreeMinionList>::hook) { D2Seam<&AIGENERAL_FreeMinionList>::hook(pUnit); return; }
	D2AiControlStrc* pAiControl = AIGENERAL_GetAiControlFromUnit(pUnit);
	if (!pAiControl || !pAiControl->pGame)
	{
		return;
	}

	// retail returns unless the owner resolves to a monster unit (type 1) with a control
	D2AiControlStrc* pOwnerAiControl = AIGENERAL_GetAiControlFromUnit(SUNIT_GetServerUnit(pAiControl->pGame, pAiControl->dwOwnerTypeEx, pAiControl->dwOwnerGUIDEx));
	if (!pOwnerAiControl)
	{
		return;
	}

	D2MinionListStrc* pPreviousMinionList = nullptr;
	for (D2MinionListStrc* pMinionList = pOwnerAiControl->pMinionList; pMinionList; pMinionList = pMinionList->pNext)
	{
		if (pMinionList->dwMinionGUID == pUnit->dwUnitId)
		{
			D2UnitStrc* pMinion = SUNIT_GetServerUnit(pOwnerAiControl->pGame, UNIT_MONSTER, pMinionList->dwMinionGUID);
			if (pMinion && pMinion->dwUnitType == UNIT_MONSTER && pMinion->pMonsterData)
			{
				pMinion->pMonsterData->pAiControl->dwOwnerGUIDEx = -1;
				pMinion->pMonsterData->pAiControl->dwOwnerTypeEx = 1;
				pMinion->pMonsterData->pAiControl->pGame = pOwnerAiControl->pGame;
			}

			if (pPreviousMinionList)
			{
				pPreviousMinionList->pNext = pMinionList->pNext;
			}
			else
			{
				pOwnerAiControl->pMinionList = pMinionList->pNext;
			}

			D2_FREE_POOL(pOwnerAiControl->pGame->pMemoryPool, pMinionList);
			return;
		}

		pPreviousMinionList = pMinionList;
	}
}

// 1.13c D2Game+0x21590
void __fastcall sub_6FCCF4B0(D2UnitStrc* pUnit)
{
	if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER || !pUnit->pMonsterData)
	{
		return;
	}

	D2AiControlStrc* pAiControl = pUnit->pMonsterData->pAiControl;
	if (!pAiControl || !pAiControl->pGame)
	{
		return;
	}

	D2UnitStrc* pOwner = D2_CALL(SUNIT_GetServerUnit, pAiControl->pGame, pAiControl->dwOwnerTypeEx, pAiControl->dwOwnerGUIDEx);
	if (!pOwner || pOwner != pUnit)
	{
		return;
	}

	D2MinionListStrc* pMinionList = pAiControl->pMinionList;
	while (pMinionList)
	{
		D2MinionListStrc* pNext = pMinionList->pNext;
		D2UnitStrc* pMinion = SUNIT_GetServerUnit(pAiControl->pGame, UNIT_MONSTER, pMinionList->dwMinionGUID);
		if (pMinion && pMinion->dwUnitType == UNIT_MONSTER && pMinion->pMonsterData)
		{
			pMinion->pMonsterData->pAiControl->dwOwnerGUIDEx = -1;
			pMinion->pMonsterData->pAiControl->dwOwnerTypeEx = UNIT_MONSTER;
			pMinion->pMonsterData->pAiControl->pGame = pAiControl->pGame;
		}

		D2_FREE_POOL(pAiControl->pGame->pMemoryPool, pMinionList);
		pMinionList = pNext;
	}

	pAiControl->pMinionList = nullptr;
}

// 1.13c D2Game+0x20B30
void __fastcall AIGENERAL_FreeAllMinionLists(D2GameStrc* pGame, D2MinionListStrc* pMinionList)
{
	while (pMinionList)
	{
		D2MinionListStrc* pNext = pMinionList->pNext;
		D2_FREE_POOL(pGame->pMemoryPool, pMinionList);
		pMinionList = pNext;
	}
}

// 1.13c D2Game+0x21490
void __fastcall AIGENERAL_ExecuteCallbackOnMinions(D2UnitStrc* pUnit, void* a2, void* a3, void(__fastcall* pfnParty)(D2UnitStrc*, void*, void*))
{
	D2_ASSERT(pfnParty);

	D2AiControlStrc* pAiControl = AIGENERAL_GetAiControlFromUnit(pUnit);
	if (!pAiControl || !pAiControl->pGame)
	{
		return;
	}

	// retail stops here unless the owner resolves to a monster unit (type 1)
	D2AiControlStrc* pOwnerAiControl = AIGENERAL_GetAiControlFromUnit(D2_CALL(SUNIT_GetServerUnit,pAiControl->pGame, pAiControl->dwOwnerTypeEx, pAiControl->dwOwnerGUIDEx));
	if (!pOwnerAiControl)
	{
		return;
	}

	D2MinionListStrc* pNext = nullptr;
	for (D2MinionListStrc* pMinionList = pOwnerAiControl->pMinionList; pMinionList; pMinionList = pNext)
	{
		pNext = pMinionList->pNext; // the callback may free this node (AIGENERAL_FreeMinionList); upstream read pNext after it
		D2UnitStrc* pMinion = D2_CALL(SUNIT_GetServerUnit, pOwnerAiControl->pGame, UNIT_MONSTER, pMinionList->dwMinionGUID);
		if (pMinion)
		{
			pfnParty(pMinion, a2, a3);
		}
	}
}

// not-1.13c: retail has no stand-alone function; the inactive-monster compress (D2Game+0x1F600) inlines it: when control+0x30 == 1 it reads +0x2C, the two flag bits and +0x34, then writes +0x2C = -1 and +0x34 = 0; the node's owner field starts at -1
void __fastcall AIGENERAL_GetAiControlInfo(D2UnitStrc* pUnit, int32_t* pOwnerGUID, int32_t* pOwnerType, int32_t* pAiControlFlag1, int32_t* pAiControlFlag2, D2MinionListStrc** ppMinionList)
{
	D2AiControlStrc* pAiControl = AIGENERAL_GetAiControlFromUnit(pUnit);
	if (pAiControl && pAiControl->dwOwnerTypeEx == UNIT_MONSTER)
	{
		*pOwnerGUID = pAiControl->dwOwnerGUIDEx;
		*pOwnerType = pAiControl->dwOwnerTypeEx;
		*pAiControlFlag1 = AIUTIL_CheckAiControlFlag(pAiControl, 1u);
		*pAiControlFlag2 = AIUTIL_CheckAiControlFlag(pAiControl, 2u);
		*ppMinionList = pAiControl->pMinionList;
		pAiControl->dwOwnerGUIDEx = -1;
		pAiControl->pMinionList = nullptr;
	}
	else
	{
		*pOwnerGUID = -1;
	}
}

// 1.13c D2Game+0x21210
void __fastcall AIGENERAL_SetAiControlInfo(D2GameStrc* pGame, D2UnitStrc* pUnit, DWORD nOwnerGUID, DWORD nOwnerType, int32_t bSetFlag1, int32_t bSetFlag2, D2MinionListStrc* pMinionList)
{
	D2AiControlStrc* pAiControl = AIGENERAL_GetAiControlFromUnit(pUnit);
	if (!pAiControl)
	{
		return;
	}

	AIGENERAL_FreeAllMinionLists(pGame, pAiControl->pMinionList);
	AIGENERAL_SetOwnerData(pGame, pUnit, nOwnerGUID, nOwnerType, bSetFlag1, bSetFlag2);
	pAiControl->pMinionList = pMinionList;
}

// 1.13c D2Game+0x21040: owner flag clear -> every listed minion that still exists drops its owner (guid -1, type 1, game kept)
void AIGENERAL_ClearMinionOwners(D2AiControlStrc* pAiControl)
{
	D2GameStrc* pGame = pAiControl->pGame;
	for (D2MinionListStrc* pMinionList = pAiControl->pMinionList; pMinionList; pMinionList = pMinionList->pNext)
	{
		D2_ASSERT(pGame);

		D2UnitStrc* pMinion = SUNIT_GetServerUnit(pGame, UNIT_MONSTER, pMinionList->dwMinionGUID);
		if (pMinion)
		{
			AIGENERAL_SetOwnerData(pGame, pMinion, -1, UNIT_MONSTER, 0, 0);
		}
	}
}

// 1.13c D2Game+0x210D0: owner flag set -> the first listed minion that still exists becomes the owner of the whole list
void AIGENERAL_LinkMinionsToFirst(D2AiControlStrc* pAiControl, D2UnitStrc* pUnit)
{
	D2MinionListStrc* pHead = pAiControl->pMinionList;
	if (!pHead)
	{
		return;
	}

	D2GameStrc* pGame = pAiControl->pGame;
	D2MinionListStrc* pMinionList = pHead;
	D2UnitStrc* pFirst = nullptr;
	while (pMinionList)
	{
		D2_ASSERT(pGame);

		pFirst = SUNIT_GetServerUnit(pGame, UNIT_MONSTER, pMinionList->dwMinionGUID);
		if (pFirst)
		{
			break;
		}

		pMinionList = pMinionList->pNext;
	}

	if (!pFirst)
	{
		return;
	}

	const int32_t nFirstGUID = pMinionList->dwMinionGUID;
	AIGENERAL_SetOwnerData(pGame, pFirst, nFirstGUID, UNIT_MONSTER, 1, 1);
	AIGENERAL_SetOwnerData(pGame, pUnit, nFirstGUID, UNIT_MONSTER, 0, 0);
	D2_CALL(AIGENERAL_AllocMinionList, pGame, pFirst, pUnit);

	for (D2MinionListStrc* i = pHead; i; i = i->pNext)
	{
		D2UnitStrc* pOther = SUNIT_GetServerUnit(pGame, UNIT_MONSTER, i->dwMinionGUID);
		if (pOther && pOther != pFirst)
		{
			AIGENERAL_SetOwnerData(pGame, pOther, nFirstGUID, UNIT_MONSTER, 0, 0);
			D2_CALL(AIGENERAL_AllocMinionList, pGame, pFirst, pOther);
		}
	}
}

// not-1.13c: retail has no stand-alone function; the monster-death handler (D2Game+0x75350) tests control flag 1, then flag 2, and calls 0x21040 or 0x210D0
void __fastcall AIGENERAL_UpdateMinionList(D2UnitStrc* pUnit)
{
	D2AiControlStrc* pAiControl = AIGENERAL_GetAiControlFromUnit(pUnit);
	if (!AIUTIL_CheckAiControlFlag(pAiControl, 1u))
	{
		return;
	}

	if (!AIUTIL_CheckAiControlFlag(pAiControl, 2u))
	{
		AIGENERAL_ClearMinionOwners(pAiControl);
	}
	else
	{
		AIGENERAL_LinkMinionsToFirst(pAiControl, pUnit);
	}
}

// 1.13c D2Game+0x20C60
int32_t __fastcall AIGENERAL_GetMinionSpawnClassId(D2UnitStrc* pUnit)
{
	D2AiControlStrc* pAiControl = AIGENERAL_GetAiControlFromUnit(pUnit);
	if (!pAiControl)
	{
		return 0;
	}

	return pAiControl->nMinionSpawnClassId;
}
