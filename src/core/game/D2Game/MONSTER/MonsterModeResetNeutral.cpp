#include "MONSTER/MonsterMode.h"
#include <D2Monsters.h>
#include <Units/Units.h>
#include "UNIT/SUnit.h"
#include <test_seam.h>

// 1.13c D2Game+0x750F0
// ESI = the unit, EDI = the mode, the game is one stack word. The unit's used skill is cleared; for a mode other than death or dead a dead unit answers 1 untouched, otherwise sub_6FC64310 runs with a neutral mode change on the unit and the answer is 0.
int32_t __fastcall MONSTERMODE_ResetToNeutral(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nMode)
{
	if (D2Seam<&MONSTERMODE_ResetToNeutral>::hook) return D2Seam<&MONSTERMODE_ResetToNeutral>::hook(pGame, pUnit, nMode);
    D2_CALL(UNITS_SetUsedSkill, pUnit, nullptr);

    if (nMode != MONMODE_DEATH && nMode != MONMODE_DEAD && D2_CALL(SUNIT_IsDead, pUnit))
    {
        return 1;
    }

    D2ModeChangeStrc modeChange = {};
    modeChange.pUnit = pUnit;
    modeChange.nMode = MONMODE_NEUTRAL;
    D2_CALL(sub_6FC64310, pGame, &modeChange);
    return 0;
}
