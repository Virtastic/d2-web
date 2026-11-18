#include "oc20g17.h"

#include <test_seam.h>
#include "AI/AiThink.h"
#include "AI/AiGeneral.h"
#include "GAME/Event.h"

// 1.13c D2Game+0x1E900 (ECX = game, EDX = unit, stack = tick params; ret 4)
// Necro pet think entry: frees every AI think timer (event type 2) chained on the unit, then runs the no-command
// branch (D2Game+0x1E640) when the AI's first parameter is zero and the command branch (D2Game+0x1E3D0) otherwise.
void __fastcall sub_1E900(D2GameStrc* pGame, D2UnitStrc* pUnit, D2AiTickParamStrc* pAiTickParam)
{
	if (pUnit)
	{
		D2EventTimerStrc* pTimer = pUnit->pTimer;
		while (pTimer)
		{
			const D2C_EventTypes nType = pTimer->nEventType;
			D2EventTimerStrc* pNext = pTimer->pNext;
			if (nType == EVENTTYPE_AITHINK)
			{
				D2_CALL(sub_6FC34890, pGame, pTimer);
			}
			pTimer = pNext;
		}
	}

	if (pAiTickParam->pAiControl->dwAiParam[0] == 0)
	{
		D2_CALL(sub_6FCE3740, pGame, pUnit, pAiTickParam);
		return;
	}
	D2_CALL(AITHINK_Fn067_NecroPet, pGame, pUnit, pAiTickParam);
}
