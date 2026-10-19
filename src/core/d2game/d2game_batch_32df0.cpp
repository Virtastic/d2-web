// Stage A, retail 1.13c: object placement, quest transitions and player helpers.
// Prior art used: D2MOO Objects.cpp, Quests.cpp, Player.cpp and PlayerList.cpp.
// Cached decompiles were checked against the x86 instructions, including register-only
// inputs. These entry points use native D2MOO pointers and explicit retail-width scalars;
// retail byte offsets are never applied to the widened game/unit/client structures.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <D2DataTbls.h>
#include <D2Inventory.h>
#include <D2StatList.h>
#include <D2QuestRecord.h>
#include <Units/Missile.h>
#include <GAME/Game.h>
#include <GAME/Clients.h>
#include <OBJECTS/Objects.h>
#include <OBJECTS/ObjMode.h>
#include <PLAYER/Player.h>
#include <PLAYER/PlayerList.h>
#include <PLAYER/PlrTrade.h>
#include <QUESTS/Quests.h>
#include <QUESTS/ACT5/A5Q5.h>
#include <UNIT/SUnit.h>
#include <UNIT/SUnitNpc.h>
#include <AI/AiGeneral.h>

namespace d2game_32df0 {
// 1.13c D2Game+0x32DF0
D2UnitStrc* populateRogues(D2GameStrc* game, D2ActiveRoomStrc* room,
                         uint8_t density, uint32_t objectId, uint16_t probability)
{
    // This prior-art function has the same four coordinate patterns, eight placement
    // attempts, inclusive probability gate and per-corpse fly callback as 1.13c.
    return OBJECTS_PopulateFn7_RogueOnStick(game, room, density, objectId, probability);
}

// 1.13c D2Game+0x35A20
void updateQuestGuids(D2GameStrc* game, D2QuestGUIDStrc* guids,
                     int32_t quest /* EBX */, uint16_t sound)
{
    QUESTS_GUIDUpdate(game, guids, quest, sound);
}

// 1.13c D2Game+0x35C90
void questScrollEvent(D2GameStrc* game /* EAX */, uint32_t targetGuid /* ECX */,
                      D2UnitStrc* player /* EDX */, uint16_t message)
{
    D2QuestArgStrc args{};
    args.pGame = game;
    args.nEvent = 11;
    args.pPlayer = player;
    if (targetGuid != UINT32_MAX) {
        // SUNIT_GetServerUnit preserves the retail fatal-null-game path and hash lookup.
        args.pTarget = SUNIT_GetServerUnit(game, UNIT_MONSTER, targetGuid);
        if (args.pTarget) args.nOldLevel = args.pTarget->dwClassId;
    }
    args.nNewLevel = message;
    QUESTS_EventCallback(&args, true, true);
}

// 1.13c D2Game+0x366C0
void hellPortalTransition(D2GameStrc* game /* EAX */, D2UnitStrc* player /* ESI */,
                          int32_t level, D2UnitStrc* portal)
{
    // D2MOO's 1.10f map agrees: level 102 grants A3-completed, consumes the five
    // Khalim components on the first grant, sets intro flags, then sends 61 04 once.
    // Other levels update the Mephisto portal/bridge quest state instead.
    QUESTS_HellPortalActChange(game, player, level, portal);
}

// 1.13c D2Game+0x37260
void mergeWeaponStats(D2UnitStrc* attacker /* EBX */, int32_t enabled /* EAX */)
{
    if (!enabled) return;
    D2UnitStrc* left = INVENTORY_GetLeftHandWeapon(attacker->pInventory);
    if (left && STATLIST_GetOwner(left, 0)) STATLIST_MergeStatLists(attacker, left, 1);
    D2UnitStrc* secondary = INVENTORY_GetSecondaryWeapon(attacker->pInventory);
    if (secondary && secondary != left && STATLIST_GetOwner(secondary, 0))
        STATLIST_MergeStatLists(attacker, secondary, 0);
}

// 1.13c D2Game+0x37730
void loseExperience(D2GameStrc* game /* EAX */, D2UnitStrc* player)
{
    auto* difficulty = DATATBLS_GetDifficultyLevelsTxtRecord(game->nDifficulty);
    int32_t classId = player ? player->dwClassId : -1;
    int32_t level = STATLIST_UnitGetStatValue(player, STAT_LEVEL, 0);
    if (level <= 1) return;
    uint32_t previous = DATATBLS_GetLevelThreshold(classId, level - 1);
    uint32_t current = DATATBLS_GetLevelThreshold(classId, level);
    uint32_t experience = STATLIST_UnitGetStatValue(player, STAT_EXPERIENCE, 0);
    // IMUL keeps its low 32 bits; the subsequent MUL/SHR is unsigned division by 100.
    uint32_t loss = (uint32_t(difficulty->dwDeathExpPenalty) * (current - previous)) / 100u;
    if (!loss) return;
    uint32_t remaining = experience - loss;
    if (remaining <= previous) {
        loss += remaining - previous;
        if (int32_t(loss) < 0) loss = 0;
        remaining = previous + 1u;
    }
    auto* client = player && player->dwUnitType == UNIT_PLAYER
        ? UNITS_GetPlayerData(player)->pClient : nullptr;
    if (int32_t(loss) < 0) std::_Exit(-1); // retail Fog assertion 0xC4A terminates with -1
    // Retail client+0x508 is the experience-loss field; use its native member,
    // rather than CLIENTS_SetExperienceLoss (currently a no-op compatibility seam).
    client->nExpLoss = int32_t(loss);
    STATLIST_SetUnitStat(player, STAT_EXPERIENCE, int32_t(remaining), 0);
}

// 1.13c D2Game+0x37890
int32_t setHotkey(D2UnitStrc* player /* EAX */, int32_t index /* ESI */,
                  int16_t skill, uint8_t hand, uint32_t itemGuid)
{
    if (!player || player->dwUnitType != UNIT_PLAYER) return 0;
    auto* client = UNITS_GetPlayerData(player)->pClient;
    if (!client) return 0;
    // No bounds check in retail. Only the low byte of the hand word is written.
    auto& key = client->HotkeySkills[index];
    key.nSkill = skill;
    std::memcpy(&key.nHand, &hand, sizeof hand);
    key.nItemGUID = itemGuid;
    return 1;
}

struct HostileIteration { int32_t bFlag; D2UnitStrc* pUnit; };

// 1.13c D2Game+0x37940
void findHostilePlayer(D2GameStrc*, D2UnitStrc* player, void* argument)
{
    auto* args = static_cast<HostileIteration*>(argument);
    if (PLAYERLIST_CheckFlag(args->pUnit, player, 8)) args->bFlag = 1;
}

// 1.13c D2Game+0x383A0
void deathPenalties(D2GameStrc* game /* EBX */, D2UnitStrc* defender /* ECX */,
                    D2UnitStrc* attacker /* EAX */,
                    void (*goldPenalty)(D2GameStrc*, D2UnitStrc*, D2UnitStrc*))
{
    // 0x38180 is the gold-only retail callee. The old D2MOO implementation combines
    // gold and experience, so invoking it here would charge experience twice.
    goldPenalty(game, defender, attacker);
    if (attacker) {
        if (attacker->dwUnitType == UNIT_PLAYER) return;
        if (attacker->dwUnitType == UNIT_MONSTER) {
            auto* owner = AIGENERAL_GetMinionOwner(attacker);
            if (owner && owner->dwUnitType == UNIT_PLAYER) return;
        }
    }
    loseExperience(game, defender);
}

// 1.13c D2Game+0x38AB0
void stopDeathInteractions(D2GameStrc* game /* ESI */, D2UnitStrc* player /* EAX */)
{
    uint32_t type = player->dwInteractType;
    if (uint8_t(player->nInteract)) {
        auto* target = SUNIT_GetServerUnit(game, type, player->dwInteractGUID);
        if (target) {
            switch (type) {
            case UNIT_PLAYER: PLRTRADE_StopAllPlayerInteractions(game, player); break;
            case UNIT_MONSTER: D2GAME_NPC_ResetInteract_6FCCB4D0(game, player, target); break;
            case UNIT_OBJECT: sub_6FC79B50(game, player, target); break;
            case UNIT_ITEM:
                if (DATATBLS_GetItemsTxtRecord(target->dwClassId)->dwCode == 0x20786f62u)
                    sub_6FC92EF0(game, player);
                break;
            }
        }
    }
    ACT5Q5_OnPlayerDied(game, player);
}

// 1.13c D2Game+0x38F40
int32_t missilePierce(D2UnitStrc* missile /* ESI */)
{
    int32_t id = missile ? missile->dwClassId : -1;
    D2MissilesTxt* record = id >= 0 && id < sgptDataTables->nMissilesTxtRecordCount
        ? &sgptDataTables->pMissilesTxt[id] : nullptr;
    // Retail dereferences a null record for invalid ids; no synthetic success path.
    if (!(record->dwMissileFlags & MISSILESFLAG_PIERCE)) return 3;
    uint32_t pierces = STATLIST_UnitGetStatValue(missile, 0x148, 0);
    if (!pierces) return 3;
    STATLIST_SetUnitStat(missile, 0x148, int32_t(pierces - 1u), 0);
    return 2;
}

// Damage is a retail scalar-only block: no host pointers are stored at these offsets.
struct DamageElementFields {
    uint8_t prefix[0x18];
    uint32_t at18;
    uint8_t pad1c[0x10];
    uint32_t at2c, at30, at34;
    uint8_t pad38[0xc];
    uint32_t at44;
};
static_assert(offsetof(DamageElementFields, at18) == 0x18);
static_assert(offsetof(DamageElementFields, at2c) == 0x2c);
static_assert(offsetof(DamageElementFields, at30) == 0x30);
static_assert(offsetof(DamageElementFields, at34) == 0x34);
static_assert(offsetof(DamageElementFields, at44) == 0x44);

// 1.13c D2Game+0x394B0
void assignElement(D2UnitStrc* missile /* EAX */, uint32_t value /* ESI */,
                    DamageElementFields* damage /* EDI */)
{
    if (!missile) return;
    switch (MISSILE_GetElemTypeFromMissileId(missile->dwClassId)) {
    case 4: damage->at30 = value; break;
    case 5: damage->at2c = value; break;
    case 9: damage->at44 = value; break;
    case 11: damage->at18 = value; break;
    case 12: damage->at34 = value; break;
    }
}
} // namespace d2game_32df0
