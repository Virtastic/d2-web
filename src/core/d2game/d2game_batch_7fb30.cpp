// Prior art: D2MOO 1.10f AiThink (Baboon, BloodHawk, CorruptLancer, Zombie and
// friends); contracts and deltas checked against the cached 1.13c functions.
// All entries are AI think functions: (pGame, pUnit, pAiTickParam).
#include <cstdint>
#include <algorithm>
#include <D2Seed.h>
#include <D2Monsters.h>
#include <D2DataTbls.h>
#include <D2StatList.h>
#include <D2Dungeon.h>
#include <DataTbls/LevelsIds.h>
#include <DataTbls/MonsterTbls.h>
#include <Units/Units.h>
#include <Units/UnitRoom.h>
#include "AI/AiGeneral.h"
#include "AI/AiTactics.h"
#include "AI/AiUtil.h"
#include "ITEMS/Items.h"
#include "MONSTER/Monster.h"
#include "MONSTER/MonsterMode.h"
#include "MONSTER/MonsterRegion.h"
#include <Path/Path.h>

namespace {
inline int32_t P(D2GameStrc* g, D2AiTickParamStrc* t, int32_t k)
{
    return t->pMonstatsTxt->wAiParam[k][g->nDifficulty];
}
inline uint32_t Pct(D2UnitStrc* u) { return SEED_RollPercentage(&u->pSeed); }
inline uint32_t Lim100(D2UnitStrc* u) { return ITEMS_RollLimitedRandomNumber(&u->pSeed, 100); }
inline void Attack(D2GameStrc* g, D2UnitStrc* u, D2AiTickParamStrc* t, int32_t mode)
{
    AITACTICS_ChangeModeAndTargetUnit(g, u, mode, t->pTarget);
}
inline void Attack1Or2(D2GameStrc* g, D2UnitStrc* u, D2AiTickParamStrc* t, uint32_t roll)
{
    Attack(g, u, t, roll < (uint32_t)P(g, t, 3) ? MONMODE_ATTACK1 : MONMODE_ATTACK2);
}

// Shared body of 0x82430 and 0x82570 (identical code, different jump targets).
void WalkOrAttack1(D2GameStrc* g, D2UnitStrc* u, D2AiTickParamStrc* t)
{
    if (!t->bCombat) {
        if (Pct(u) < (uint32_t)P(g, t, 0)) { AITACTICS_WalkToTargetUnitWithFlags(g, u, t->pTarget, 7); return; }
    } else if (Pct(u) < (uint32_t)P(g, t, 2)) {
        Attack(g, u, t, MONMODE_ATTACK1);
        return;
    }
    AITACTICS_IdleInNeutralMode(g, u, P(g, t, 1));
}
}

// 1.13c D2Game+0x82430
void D2GAME_82430_AiThink(D2GameStrc* g, D2UnitStrc* u, D2AiTickParamStrc* t) { WalkOrAttack1(g, u, t); }

// 1.13c D2Game+0x82570
void D2GAME_82570_AiThink(D2GameStrc* g, D2UnitStrc* u, D2AiTickParamStrc* t) { WalkOrAttack1(g, u, t); }

// 1.13c D2Game+0x83AC0
void D2GAME_83AC0_AiThink(D2GameStrc* g, D2UnitStrc* u, D2AiTickParamStrc* t)
{
    if (!t->bCombat) {
        if (Pct(u) < (uint32_t)P(g, t, 0)) { AITACTICS_WalkToTargetUnitWithFlags(g, u, t->pTarget, 7); return; }
    } else if (Pct(u) < (uint32_t)P(g, t, 2)) {
        Attack1Or2(g, u, t, Pct(u));
        return;
    }
    AITACTICS_IdleInNeutralMode(g, u, P(g, t, 1));
}

// 1.13c D2Game+0x81F50
void D2GAME_81F50_AiThink(D2GameStrc* g, D2UnitStrc* u, D2AiTickParamStrc* t)
{
    const bool bAlerted = u && u->dwUnitType == UNIT_MONSTER && sub_6FCF2E70(u);
    if (!(bAlerted && !t->bCombat)) {
        if (t->nTargetDistance > P(g, t, 0)) {
            if (Pct(u) < (uint32_t)P(g, t, 1)) AITACTICS_WalkCloseToUnit(g, u, 3);
            else AITACTICS_IdleInNeutralMode(g, u, P(g, t, 4));
            return;
        }
        if (t->bCombat) {
            if (Pct(u) < (uint32_t)P(g, t, 2)) Attack1Or2(g, u, t, Lim100(u));
            else AITACTICS_IdleInNeutralMode(g, u, P(g, t, 4));
            return;
        }
    }
    AITACTICS_WalkToTargetUnitWithFlags(g, u, t->pTarget, 7);
}

// 1.13c D2Game+0x83550  (BloodHawk-shaped)
void D2GAME_83550_AiThink(D2GameStrc* g, D2UnitStrc* u, D2AiTickParamStrc* t)
{
    D2AiControlStrc* ai = t->pAiControl;
    if (ai->dwAiParam[0] == 1 && t->bCombat) {
        ai->dwAiParam[0] = 0;
        Attack(g, u, t, MONMODE_ATTACK1);
        return;
    }
    ai->dwAiParam[0] = 0;
    if (t->bCombat) {
        if (Pct(u) < (uint32_t)P(g, t, 2)) { Attack(g, u, t, MONMODE_ATTACK1); return; }
    } else {
        if (Pct(u) < (uint32_t)P(g, t, 0)) {
            AITACTICS_SetVelocity(u, 0, P(g, t, 4), (uint8_t)t->nTargetDistance);
            ai->dwAiParam[0] = 1;
            AITACTICS_WalkToTargetUnitWithFlags(g, u, t->pTarget, 0);
            return;
        }
        if (t->nTargetDistance > 3) {
            if (Lim100(u) < (uint32_t)P(g, t, 1)) AITACTICS_SetVelocity(u, 0, -50, 0);
            AITACTICS_WalkCloseToUnit(g, u, 4);
            return;
        }
    }
    AITACTICS_SetVelocity(u, 0, P(g, t, 3), 0);
    if (!D2GAME_AICORE_Escape_6FCD0560(g, u, t->pTarget, 4, 1)) Attack(g, u, t, MONMODE_ATTACK1);
}

// 1.13c D2Game+0x7FB30  (CorruptLancer-shaped)
void D2GAME_7FB30_AiThink(D2GameStrc* g, D2UnitStrc* u, D2AiTickParamStrc* t)
{
    D2AiControlStrc* ai = t->pAiControl;
    const uint8_t nSteps = t->pMonstats2Txt->nMeleeRng;
    if (t->nTargetDistance > P(g, t, 4)) {
        PATH_SetStepNum(u->pDynamicPath, nSteps);
        AITACTICS_SetVelocity(u, 13, 100, 0);
        AITACTICS_RunToTargetUnitWithSteps(g, u, t->pTarget, nSteps ? nSteps : 1);
        ai->dwAiParam[0] = 1;
        return;
    }
    if (!t->bCombat) {
        if (Pct(u) >= (uint32_t)P(g, t, 0)) { AITACTICS_IdleInNeutralMode(g, u, P(g, t, 2)); return; }
        PATH_SetStepNum(u->pDynamicPath, nSteps);
        if (Pct(u) >= (uint32_t)P(g, t, 3)) { AITACTICS_WalkToTargetUnitWithSteps(g, u, t->pTarget, 3); return; }
        AITACTICS_SetVelocity(u, 13, 100, 0);
        AITACTICS_RunToTargetUnitWithSteps(g, u, t->pTarget, nSteps);
        return;
    }
    if (ai->dwAiParam[0] == 0 && Pct(u) >= (uint32_t)P(g, t, 1)) {
        AITACTICS_IdleInNeutralMode(g, u, P(g, t, 2));
        return;
    }
    ai->dwAiParam[0] = 0;
    D2MonStatsTxt* m = t->pMonstatsTxt;
    for (int k = 0; k < 3; ++k) {
        if (m->nSkill[k] >= 0 && Lim100(u) < (uint32_t)P(g, t, 5 + k)) {
            AITACTICS_UseSkill(g, u, m->nSkillMode[k], m->nSkill[k], t->pTarget, 0, 0);
            return;
        }
    }
    Attack(g, u, t, MONMODE_ATTACK1);
}

// 1.13c D2Game+0x82990  (Baboon-shaped)
void D2GAME_82990_AiThink(D2GameStrc* g, D2UnitStrc* u, D2AiTickParamStrc* t)
{
    D2AiControlStrc* ai = t->pAiControl;
    D2MonStatsTxt* m = t->pMonstatsTxt;
    int32_t nVel = 0;
    const int32_t nLife = UNITS_GetCurrentLifePercentage(u);
    if (m->nVelocity > 0) {
        const int32_t r = 100 * m->nRun / m->nVelocity;
        if (r >= 100) nVel = std::min(r - 100, 120);
    }
    if (ai->dwAiParam[0]) {
        ai->dwAiParam[1] = 0;
        --ai->dwAiParam[0];
        if (!ai->dwAiParam[0] || nLife > 75)
            STATLIST_SetUnitStat(u, STAT_HPREGEN, STATLIST_UnitGetStatValue(u, STAT_HPREGEN, 0) - ai->dwAiParam[2], 0);
        if (t->bCombat) {
            if (Pct(u) < 33) { Attack1Or2(g, u, t, Pct(u)); return; }
        } else if (nLife > 75) {
            ai->dwAiParam[0] = 0;
            AITACTICS_SetVelocity(u, 13, nVel, 0);
            sub_6FCD0410(g, u, t->pTarget, 7);
            return;
        }
        if (t->nTargetDistance >= 24 && !sub_6FCF2E70(u)) {
            if (Lim100(u) < 33) sub_6FCD0E80(g, u, t->pTarget, 4, 0);
            AITACTICS_IdleInNeutralMode(g, u, 20);
            return;
        }
        AITACTICS_SetVelocity(u, 2, nVel, 0);
        if (!D2GAME_AICORE_Escape_6FCD0560(g, u, t->pTarget, 15, 1)) {
            if (!t->bCombat) AITACTICS_WalkCloseToUnit(g, u, 5);
            else Attack1Or2(g, u, t, Pct(u));
        }
        return;
    }
    if (!t->bCombat) { sub_6FCD0410(g, u, t->pTarget, 7); return; }
    if (sub_6FCF2E70(u)) {
        if (nLife < P(g, t, 0) && Lim100(u) < 50) {
            ai->dwAiParam[0] = ITEMS_RollLimitedRandomNumber(&u->pSeed, 5) + 2;
            const int32_t reg = STATLIST_UnitGetStatValue(u, STAT_HPREGEN, 0);
            if (reg) {
                ai->dwAiParam[2] = (reg * P(g, t, 4)) / 8;
                STATLIST_SetUnitStat(u, STAT_HPREGEN, reg + ai->dwAiParam[2], 0);
            } else ai->dwAiParam[2] = 0;
            AITACTICS_SetVelocity(u, 2, nVel, 0);
            D2GAME_AICORE_Escape_6FCD0560(g, u, t->pTarget, 15, 0);
            return;
        }
        if (ai->dwAiParam[1]) {
            if (Lim100(u) < 20) {
                sub_6FCD0E80(g, u, t->pTarget, 3, 0);
                ai->dwAiParam[1] = 0;
                return;
            }
        }
    }
    if (!ai->dwAiParam[1] || Pct(u) < (uint32_t)P(g, t, 2)) {
        ai->dwAiParam[1] = 1;
        Attack1Or2(g, u, t, Pct(u));
        return;
    }
    if (Lim100(u) < (uint32_t)P(g, t, 1)) {
        sub_6FCD0E80(g, u, t->pTarget, 3, 0);
        ai->dwAiParam[1] = 0;
    }
    AITACTICS_IdleInNeutralMode(g, u, 15);
}
