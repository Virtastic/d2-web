// Callee unblock batch c85a (retail 1.13c D2Game): the gold-only death penalty (0x38180) and the
// minion-release entry (0x23BF0). 0x75E10 and 0x6F520 are the vendored native bodies
// D2GAME_ModeChange_6FC65220 and sub_6FC79B50 (markers live on them). Native D2MOO structs; no
// env seams. Decompile-derived from the cached 1.13c D2Game decompiles and disassembly.
#include <cstdint>
#include <algorithm>
#include <D2StatList.h>
#include <Units/Units.h>
#include <GAME/Game.h>
#include <PLAYER/Player.h>
#include <UNIT/SUnit.h>
#include <UNIT/SUnitInactive.h>
#include <AI/AiGeneral.h>
#include <MONSTER/MonsterMode.h>

namespace d2game_c85a {

// 1.13c D2Game+0x38180
// Gold-only penalty: dropped gold, bank deduction and STAT_GOLDLOST. No experience part and no
// stash-cap branch (those belong to other callers / 0x37730).
void goldDeathPenalty(D2GameStrc* game, D2UnitStrc* defender, D2UnitStrc* attacker)
{
    const int32_t level = STATLIST_UnitGetStatValue(defender, STAT_LEVEL, 0);
    const int32_t gold = STATLIST_UnitGetStatValue(defender, STAT_GOLD, 0);
    const int32_t bank = STATLIST_UnitGetStatValue(defender, STAT_GOLDBANK, 0);
    const int32_t total = gold + bank;
    int32_t loss = std::min(level, 20) * total / 100;

    bool pvp = false;
    if (attacker && attacker != defender) {
        if (attacker->dwUnitType == UNIT_PLAYER) {
            pvp = true;
        } else if (attacker->dwUnitType == UNIT_MONSTER) {
            D2UnitStrc* owner = AIGENERAL_GetMinionOwner(attacker);
            pvp = owner && owner->dwUnitType == UNIT_PLAYER;
        }
    }

    if (game->nGameType == 3) {
        if (loss > total - 500 * level) loss = std::max(total - 500 * level, 0);
        loss = std::min(loss, gold);
    }

    const int32_t guid = defender ? int32_t(defender->dwUnitId) : -1;
    if (pvp) {
        if (loss > gold) {
            D2GAME_SetStatOrResetGold_6FC7CA70(defender, STAT_GOLDBANK, total - loss);
            D2GAME_SetStatOrResetGold_6FC7CA70(defender, STAT_GOLD, loss);
        }
        sub_6FC7C260(game, defender, guid, uint32_t(loss));
    } else {
        if (loss > gold) {
            if (game->nGameType != 3)
                D2GAME_SetStatOrResetGold_6FC7CA70(defender, STAT_GOLDBANK, total - loss);
        } else {
            sub_6FC7C260(game, defender, guid, uint32_t(gold - loss));
        }
        D2GAME_SetStatOrResetGold_6FC7CA70(defender, STAT_GOLD, 0);
    }
    D2GAME_SetStatOrResetGold_6FC7CA70(defender, STAT_GOLDLOST, std::max(loss, 0));
}

// Callback run per minion by 0x21490 (the retail callee is 0x23730).
static void releaseMinion(D2GameStrc* game, D2UnitStrc* minion)
{
    AIGENERAL_FreeMinionList(minion);
    if (!minion) return;
    if (minion->dwUnitType == UNIT_MONSTER) {
        D2AiControlStrc* ai = AIGENERAL_GetAiControlFromUnit(minion);
        ai->dwOwnerGUIDEx = D2UnitGUID(-1);
        ai->dwOwnerTypeEx = 1;
        ai->pGame = game;
    }
    if (minion->dwAnimMode) {
        D2ModeChangeStrc mc;
        MONSTERMODE_GetModeChangeInfo(minion, 0, &mc);
        D2GAME_ModeChange_6FC65220(game, &mc, 1);
    }
}

// 1.13c D2Game+0x23BF0
// ECX = game, EDX = unit. A neutral-dead (mode 0) monster with an owner record releases the
// minions of its linked owner, resets the owner's record and wakes the owner to neutral.
void releaseLinkedMinions(D2GameStrc* game, D2UnitStrc* unit)
{
    if (!unit || unit->dwAnimMode != 0 || unit->dwUnitType != UNIT_MONSTER) return;
    D2AiControlStrc* rec = AIGENERAL_GetAiControlFromUnit(unit);
    if (!rec || !rec->pGame) return;
    D2UnitStrc* linked = SUNIT_GetServerUnit(rec->pGame, rec->dwOwnerTypeEx, rec->dwOwnerGUIDEx);
    if (!linked) return;

    // 0x21490: every live minion in the linked monster's list.
    if (linked->dwUnitType == UNIT_MONSTER) {
        D2AiControlStrc* lai = AIGENERAL_GetAiControlFromUnit(linked);
        D2GameStrc* lgame = lai->pGame;
        for (D2MinionListStrc* n = lai->pMinionList; n;) {
            D2MinionListStrc* next = n->pNext;
            D2UnitStrc* m = SUNIT_GetServerUnit(lgame, UNIT_MONSTER, n->dwMinionGUID);
            if (m) releaseMinion(game, m);
            n = next;
        }
    }
    if (linked->dwUnitType == UNIT_MONSTER) {
        D2AiControlStrc* lai = AIGENERAL_GetAiControlFromUnit(linked);
        lai->dwOwnerGUIDEx = D2UnitGUID(-1);
        lai->dwOwnerTypeEx = 1;
        lai->pGame = game;
    }
    if (linked != unit && linked->dwUnitType == UNIT_MONSTER) {
        D2ModeChangeStrc mc;
        MONSTERMODE_GetModeChangeInfo(linked, 0, &mc);
        D2GAME_ModeChange_6FC65220(game, &mc, 1);
    }
}

} // namespace d2game_c85a
