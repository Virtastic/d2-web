// 1.13c D2Client+0xA2F60
// Do function 2 (Throw, Left Hand Throw): marks the unit's do as done and throws the active weapon.
// The weapon is the unit's #10641 item (none: nothing happens); its Items.txt missile type is the
// missile. A thrown weapon that is not a throwing potion goes through +0xA15C0: the path's aim point
// (+0xA1340), and when both coordinates are set +0xB0450 builds the missile: flags 0x20, the unit as
// origin, its owner (the unit itself for a player or monster, else +0xA60D0), the aim point as
// target, the skill id and skill level 1 (the constant +0xA15C0 is called with).
// NOT PORTED: the throwing-potion branch (item type 0x26), the lobbed request of +0xB9A80.
#include <D2Items.h>
#include <Units/Missile.h>
#include <Units/Units.h>
#include "realm_skill_do.h"

int d2client_realm_a2f60(D2UnitStrc* unit, int skillId, int level, const RealmSkillDoHost& host) {
    (void)level;
    if (unit) unit->dwFlags |= 0x40u;
    D2UnitStrc* weapon = unit ? D2Common_10434(unit, 0) : nullptr;
    if (!weapon) return 0;
    const int missile = ITEMS_GetMissileType(weapon);
    if (ITEMS_GetItemType(weapon) == 0x26) return 0;
    int x = 0, y = 0;   // +0xA15C0
    if (!d2client_realm_a1340(unit, host, &x, &y) || !x || !y) return 1;
    D2UnitStrc* owner = unit;  // +0xB0450
    if (unit->dwUnitType != UNIT_PLAYER && unit->dwUnitType != UNIT_MONSTER)
        owner = host.missileOwner ? host.missileOwner(unit) : nullptr;
    if (!owner) return 1;
    D2MissileStrc params{};
    params.dwFlags = 0x20;
    params.pOwner = owner; params.pOrigin = unit;
    params.nMissile = missile;
    params.nTargetX = x; params.nTargetY = y;
    params.nSkill = skillId; params.nSkillLevel = 1;
    if (host.createMissile) host.createMissile(&params);
    return 1;
}
