// Prior art used: D2MOO 1.10f MonsterUnique.cpp and SkillBar.cpp, checked against
// cached retail 1.13c decompiles. Typed entries expose arguments that retail passes
// in registers. Reuse the matching full implementations; split helpers below keep
// the narrower 1.13c contracts instead of invoking an enclosing skill handler.
#include <algorithm>
#include <cstdint>
#include <D2DataTbls.h>
#include <D2Skills.h>
#include <D2Dungeon.h>
#include <D2StatList.h>
#include <D2Items.h>
#include <D2Inventory.h>
#include <D2Collision.h>
#include <D2Composit.h>
#include <DataTbls/SkillsTbls.h>
#include <Path/Step.h>
#include "MONSTER/MonsterUnique.h"
#include "MONSTER/MonsterMode.h"
#include "SKILLS/SkillBar.h"
#include "SKILLS/Skills.h"
#include "SKILLS/SkillAss.h"
#include "ITEMS/Items.h"
#include "UNIT/SUnit.h"
#include "UNIT/SUnitDmg.h"
#include "PLAYER/Player.h"
#include "GAME/Clients.h"
#include "QUESTS/Quests.h"

void __fastcall MONSTERUNIQUE_SetUniqueFlag_6FC43260(D2GameStrc*, D2UnitStrc*);

static int32_t add32(int32_t a, int32_t b) { return uint32_t(a) + uint32_t(b); }
static int32_t sub32(int32_t a, int32_t b) { return uint32_t(a) - uint32_t(b); }

// 1.13c D2Game+0x241B0
extern "C" void d2game_241b0(D2UnitStrc* unit, int32_t unique)
{
    MONSTERUNIQUE_UMod26_Teleport(unit, 0, unique);
}

// EAX=unit, ECX=game. Retail marks the existing monster unique, applies the
// modifier initializer, refreshes its room, then creates quest chain zero.
// 1.13c D2Game+0x24B70
extern "C" void d2game_24b70(D2GameStrc* game, D2UnitStrc* unit)
{
    if (!unit || unit->dwUnitType != UNIT_MONSTER || !unit->pMonsterData) return;
    unit->pMonsterData->nTypeFlag |= 1;
    MONSTERUNIQUE_SetUniqueFlag_6FC43260(game, unit);
    sub_6FC6E940(game, unit, 1);
    UNITS_GetRoom(unit);
    QUESTS_CreateChainRecord(game, unit, 0);
    unit->dwFlags |= 0x800;
    if (unit->dwUnitType == UNIT_MONSTER && unit->pMonsterData)
        unit->pMonsterData->wSummonerFlag |= 1;
}

// 1.13c D2Game+0x25AD0
extern "C" void d2game_25ad0(D2GameStrc* game, D2UnitStrc* missile, int32_t mod, int32_t unique)
{
    sub_6FC6DA40(game, missile, mod, unique);
}

// 1.13c D2Game+0x260D0
extern "C" void d2game_260d0(D2GameStrc* game, D2UnitStrc* unit)
{
    MONSTERUNIQUE_CastBugLightningMissile(game, unit, 0, 0);
}

// 1.13c D2Game+0x26320
extern "C" void d2game_26320(D2GameStrc* game, D2UnitStrc* unit, int32_t mod, int32_t unique)
{
    if (unique) MONSTERUNIQUE_CastLightUniqueMissile(game, unit, mod, unique);
}

// 1.13c D2Game+0x265E0
extern "C" void d2game_265e0(D2GameStrc* game, D2UnitStrc* unit)
{
    MONSTERUNIQUE_CastCorpseExplode2(game, unit, 0, 0);
}

// EAX=unit; true when both hand composit items are type 0x2e.
// 1.13c D2Game+0x26A30
extern "C" int32_t d2game_26a30(D2UnitStrc* unit)
{
    if (!unit->pInventory) return 0;
    auto* left = INVENTORY_GetCompositItem(unit->pInventory, COMPOSIT_LEFTHAND);
    auto* right = INVENTORY_GetCompositItem(unit->pInventory, COMPOSIT_RIGHTHAND);
    return left && ITEMS_CheckItemTypeId(left, 0x2e) && right && ITEMS_CheckItemTypeId(right, 0x2e);
}

// EAX=skill id; coordinates are writable native pointers.
// 1.13c D2Game+0x26BB0
extern "C" int32_t d2game_26bb0(D2UnitStrc* unit, int32_t skillId, int32_t level, int32_t* x, int32_t* y)
{
    return SKILLS_FindLeapTargetPosition(unit, skillId, level, x, y);
}

// The extracted whirlwind scheduler returns 0, 1 or 2, not a C++ bool.
// 1.13c D2Game+0x26E40
extern "C" int32_t d2game_26e40(D2GameStrc* game, D2SkillStrc* skill, D2UnitStrc* unit)
{
    if (unit && unit->dwUnitType == UNIT_MONSTER)
        return (ITEMS_RollRandomNumber(&unit->pSeed) & 1) == 0;
    if (!game->bExpansion) return 1;
    const int32_t frame = SKILLS_GetParam4(skill);
    if (!frame) {
        SKILLS_SetParam4(skill, add32(game->dwGameFrame, 4));
        return 1;
    }
    if (int32_t(game->dwGameFrame) < frame) return 0;
    int32_t duration = 10;
    if (auto* weapon = sub_6FC7C7B0(unit)) {
        const int32_t speed = ITEMS_GetWeaponAttackSpeed(unit, weapon);
        duration = speed < 12 ? 4 : speed < 15 ? 6 : speed < 18 ? 8 :
            speed < 20 ? 10 : speed < 23 ? 12 : speed < 26 ? 14 : 16;
    }
    int32_t count = 1;
    if (unit->pInventory) {
        auto* left = INVENTORY_GetCompositItem(unit->pInventory, COMPOSIT_LEFTHAND);
        auto* right = INVENTORY_GetCompositItem(unit->pInventory, COMPOSIT_RIGHTHAND);
        if (left && ITEMS_CheckItemTypeId(left, 0x2e) && right && ITEMS_CheckItemTypeId(right, 0x2e))
            count = 2;
    }
    int32_t next = add32(frame, duration);
    if (next < int32_t(game->dwGameFrame)) next = add32(game->dwGameFrame, duration);
    SKILLS_SetParam4(skill, next);
    return count;
}

// EAX=unit; game and skill id are independent stack inputs.
// Table codes are explicit retail little-endian integers, independent of host literals.
// 1.13c D2Game+0x26F70
extern "C" uint32_t d2game_26f70(D2GameStrc* game, D2UnitStrc* unit, int32_t skillId)
{
    auto* room = UNITS_GetRoom(unit);
    if (!room) return 0;
    const uint32_t act = DRLG_GetActNoFromLevelId(DUNGEON_GetLevelIdFromRoom(room)) & 255;
    if (act >= 5) return 0;
    const uint32_t index = act + uint32_t(game->nDifficulty) * 5;
    const int32_t roll = ITEMS_RollRandomNumber(&unit->pSeed) % 100;
    // Retail helpers 0x1BF0/0x1C20 read skill parameters 3 and 4.
    auto* record = SKILLS_GetSkillsTxtRecord(skillId);
    const int32_t manaChance = record ? record->dwParam[2] : 0;
    const int32_t rejuvChance = record ? record->dwParam[3] : 0;
    const uint32_t column = roll < manaChance ? 1 : roll < add32(manaChance, rejuvChance) ? 2 : 0;
    static constexpr uint32_t table[15][3] = {
        {0x20327068,0x2032706d,0x20737672},{0x20337068,0x2033706d,0x20737672},
        {0x20337068,0x2033706d,0x20737672},{0x20347068,0x2034706d,0x206c7672},
        {0x20347068,0x2034706d,0x206c7672},{0x20347068,0x2034706d,0x206c7672},
        {0x20347068,0x2035706d,0x206c7672},{0x20357068,0x2035706d,0x206c7672},
        {0x20357068,0x2035706d,0x206c7672},{0x20357068,0x2035706d,0x206c7672},
        {0x20357068,0x2035706d,0x206c7672},{0x20357068,0x2035706d,0x206c7672},
        {0x20357068,0x2035706d,0x206c7672},{0x20357068,0x2035706d,0x206c7672},
        {0x20357068,0x2035706d,0x206c7672}
    };
    return index < 15 ? table[index][column] : 0;
}

// The landing helper resolves the saved target and sets the reflected path. Its
// town test occurs after animation/path bookkeeping, as in retail.
// 1.13c D2Game+0x27030
extern "C" int32_t d2game_27030(D2GameStrc* game, D2UnitStrc* unit, D2SkillStrc* skill, int32_t x, int32_t y)
{
    if (!skill || !unit->pDynamicPath) return 0;
    auto* target = SUNIT_GetServerUnit(game, SKILLS_GetParam3(skill), SKILLS_GetParam4(skill));
    if (!target) return 0;
    SKILLS_SetFlags(skill, 1);
    UNITS_SetAnimationFrame(unit, 12);
    unit->dwFrameCountPrecise = (unit->dwFrameCountPrecise & 0xffffff00) + 256;
    auto* path = unit->pDynamicPath;
    PATH_RecacheRoomIfNeeded(path);
    PATH_SetNewDistance(path, 5);
    PATH_SetStepNum(path, 1);
    PATH_SetTargetUnit(path, nullptr);
    const int32_t tx = sub32(uint32_t(x)*3, uint32_t(CLIENTS_GetUnitX(target))*2);
    const int32_t ty = sub32(uint32_t(y)*3, uint32_t(CLIENTS_GetUnitY(target))*2);
    auto* room = D2GAME_GetRoom_6FC52070(UNITS_GetRoom(unit), tx, ty);
    if (room && DUNGEON_IsRoomInTown(room)) return 0;
    D2COMMON_10170_PathSetTargetPos(path, tx, ty);
    PATH_SetType(path, PATHTYPE_KNOCKBACK_SERVER);
    D2COMMON_10190_PATH_SetDistance(path, 5);
    D2Common_10142(path, unit, 0);
    return 1;
}

// The attack-resolution half of LeapAttack, split from the stage dispatcher.
// 1.13c D2Game+0x27820
extern "C" int32_t d2game_27820(D2GameStrc* game, D2UnitStrc* unit, int32_t id, int32_t level)
{
    auto* record = SKILLS_GetSkillsTxtRecord(id);
    if (!record) return 0;
    auto* skill = UNITS_GetUsedSkill(unit);
    if (!skill) return 0;
    auto* target = SKILLS_FindLeapAttackTarget(game, unit, skill);
    if (!target) return 0;
    D2DamageStrc damage = {};
    damage.wResultFlags = SUNITDMG_GetResultFlags(game, unit, target, SKILLS_GetToHitFactor(unit,id,level), 0);
    if (damage.wResultFlags & 1) {
        damage.wResultFlags |= 8;
        damage.dwHitClass = UNITS_GetHitClass(unit);
        damage.dwHitFlags |= 0x20;
        damage.dwEnDmgPct = SKILLS_EvaluateSkillFormula(unit, record->dwCalc[0], id, level);
        if (record->nEType) {
            damage.dwConvPct = SKILLS_EvaluateSkillFormula(unit, record->dwCalc[3], id, level);
            if (int32_t(damage.dwConvPct) > 0) damage.nConvType = record->nEType;
        }
        D2GAME_RollElementalDamage_6FD14DD0(unit, &damage, id, level);
        SUNITDMG_AllocCombat(game, unit, target, &damage, record->nSrcDam ? record->nSrcDam : 128);
        SUNITDMG_DrainItemDurability(game, unit, target, 0);
        if (record->wSrvOverlay > 0 && record->wSrvOverlay <= sgptDataTables->nOverlayTxtRecordCount)
            UNITS_SetOverlay(target, record->wSrvOverlay, 0);
        if (auto* stats = STATLIST_GetStatListFromUnitAndState(target, 0x15)) {
            D2Common_10474(target, stats);
            STATLIST_FreeStatList(stats);
        }
    }
    return 1;
}

// EAX=target, EBX=skill. The monster's immediate melee strike preserves the
// selected target type and id on the skill for the following leap phase.
// 1.13c D2Game+0x279C0
extern "C" int32_t d2game_279c0(D2GameStrc* game, D2UnitStrc* unit, D2UnitStrc* target, D2SkillStrc* skill)
{
    if (!target || !skill) return 0;
    sub_6FC627B0(unit, MONMODE_ATTACK1);
    sub_6FC62D90(unit, game);
    SUNITDMG_DrainItemDurability(game, unit, target, 0);
    SKILLS_SetParam3(skill, target->dwUnitType);
    SKILLS_SetParam4(skill, target->dwUnitId);
    return 1;
}
