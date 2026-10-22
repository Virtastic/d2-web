// D2Game+0x7E070: AI think function (pGame ecx, pUnit edx, pAiTickParam stack). Decompile had hidden
// register inputs; resolved from the disassembly. Prior art: D2MOO AiThink (Baboon velocity block).
#include <cstdint>
#include <algorithm>
#include <D2Seed.h>
#include <D2Monsters.h>
#include <D2DataTbls.h>
#include <D2States.h>
#include <Units/Units.h>
#include "AI/AiGeneral.h"
#include "AI/AiTactics.h"
#include "ITEMS/Items.h"
#include "UNIT/SUnit.h"
#include "MONSTER/MonsterMode.h"
#include <DataTbls/MonsterTbls.h>

// 1.13c D2Game+0x7E070
void D2GAME_7E070_AiThink(D2GameStrc* g, D2UnitStrc* u, D2AiTickParamStrc* t)
{
    D2AiControlStrc* ai = t->pAiControl;
    D2MonStatsTxt* m = t->pMonstatsTxt;
    D2UnitStrc* tgt = t->pTarget;
    const int32_t nDist = t->nTargetDistance;
    const int32_t d = g->nDifficulty;
    auto P = [&](int k) -> int32_t { return m->wAiParam[k][d]; };
    auto Idle = [&](int n) { AITACTICS_IdleInNeutralMode(g, u, n); };
    auto Skill0 = [&] { AITACTICS_UseSkill(g, u, m->nSkillMode[0], m->nSkill[0], tgt, 0, 0); };
    auto Strike = [&] { AITACTICS_ChangeModeAndTargetUnit(g, u, MONMODE_ATTACK1, tgt); };

    const int32_t nLife = UNITS_GetCurrentLifePercentage(u);
    int32_t nVel = 0;
    if (m->nVelocity > 0) nVel = std::clamp(100 * m->nRun / m->nVelocity - 100, 0, 120);

    // 1.13c-only: while walking/running (ai state 3 or 0x13) restart combat or fire the ranged skill
    if (u && u->dwUnitType == UNIT_MONSTER && u->pMonsterData && (u->pMonsterData->dwAiState == 3 || u->pMonsterData->dwAiState == 0x13)) {
        ai->dwAiParam[0] = 0;
        if (t->bCombat) {
            UNITS_SetUsedSkill(u, nullptr);
            D2ModeChangeStrc mc = {};
            mc.nMode = MONMODE_ATTACK1;
            mc.pUnit = u;
            mc.pTargetUnit = tgt;
            D2MonStatsTxt* mi = MONSTERMODE_GetMonStatsTxtRecord(u->dwClassId);
            mc.unk0x14[1] = (mi && (mi->dwMonStatsFlags & MONSTATSFLAG_INTERACT)) ? 101 : 100;
            D2GAME_ModeChange_6FC65220(g, &mc, 1);
            return;
        }
        if (m->nSkill[0] >= 0 && nDist < P(4)) { Skill0(); return; }
    }

    if (ai->dwAiParam[0] == 1) {
        ++ai->dwAiParam[1];
        if (nLife > P(2) || SEED_RollPercentage(&u->pSeed) < 25 || ai->dwAiParam[1] > P(5)) {
            ai->dwAiParam[0] = 0;
        } else if (nDist < 14) {
            AITACTICS_SetVelocity(u, 0, nVel, 0);
            D2GAME_AICORE_Escape_6FCD0560(g, u, tgt, 14, 0);
            return;
        }
        Idle(15);
        return;
    }

    bool owned = false;
    if (u && u->dwUnitType == UNIT_MONSTER && u->pMonsterData && u->pMonsterData->pAiControl) {
        D2AiControlStrc* oc = u->pMonsterData->pAiControl;
        owned = oc->pGame && SUNIT_GetServerUnit(oc->pGame, oc->dwOwnerTypeEx, oc->dwOwnerGUIDEx);
    }
    if (!owned && nLife < P(3)) {
        ai->dwAiParam[0] = 1;
        ai->dwAiParam[1] = 0;
        D2GAME_AICORE_Escape_6FCD0560(g, u, tgt, 9, 0);
        return;
    }

    if (t->bCombat) {
        if ((int32_t)SEED_RollPercentage(&u->pSeed) < P(0)) { Strike(); return; }
        if (m->nSkill[0] >= 0 && (int32_t)ITEMS_RollLimitedRandomNumber(&u->pSeed, 100) < P(1)) { Skill0(); return; }
        sub_6FCD0E80(g, u, tgt, 5, 0);
        return;
    }
    if (nDist < P(4)) {
        if (m->nSkill[0] >= 0 && (int32_t)ITEMS_RollLimitedRandomNumber(&u->pSeed, 100) < P(1)) { Skill0(); return; }
        if (STATES_CheckState(tgt, 0x54)) AITACTICS_MoveToTarget(g, u, tgt, MONMODE_WALK, 0, 0, 1, 0);
        else Idle(P(7));
        return;
    }
    if (nDist < P(6)) { AITACTICS_MoveToTarget(g, u, tgt, MONMODE_WALK, 0, 0, 1, 7); return; }
    Idle(15);
}
