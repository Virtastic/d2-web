// 1.13c D2Client+0x15C40
// Do function 18 (Guided Arrow): marks the unit's do as done and sends one arrow (the skill's client
// missile, +0x40550) at the path's aim point. With a target unit the arrow homes on it (flags 0x20,
// state 1); without one it flies to the point and lives as long as that flight (0x420, state 2).
// The created arrow also keeps the aim's offset from the unit, x in the low word, y in the high.
#include <D2DataTbls.h>
#include <Units/Missile.h>
#include <Units/Units.h>
#include "realm_skill_do.h"

int d2client_realm_15c40(D2UnitStrc* unit, int skillId, int level, const RealmSkillDoHost& host) {
    if (!DATATBLS_GetSkillsTxtRecord(skillId)) return 0;
    if (unit) unit->dwFlags |= 0x40u;
    if (!unit) return 0;
    int X = 0, Y = 0;
    if (!d2client_realm_a1340(unit, host, &X, &Y)) return 0;
    const int missile = d2client_realm_40550(unit, skillId);
    if (missile < 0 || missile >= static_cast<int>(sgptDataTables->nMissilesTxtRecordCount)) return 0;
    D2UnitStrc* target = d2client_realm_a5f70(unit, host);
    D2MissileStrc params{};
    params.dwFlags = target ? 0x20u : 0x420u;
    params.pOwner = unit; params.pOrigin = unit; params.pTarget = target;
    params.nMissile = missile;
    params.nTargetX = X; params.nTargetY = Y;
    params.nSkill = skillId; params.nSkillLevel = level;
    D2UnitStrc* arrow = host.createMissile ? host.createMissile(&params) : nullptr;
    if (!arrow) return 1;
    if (arrow->dwUnitType == UNIT_MISSILE && arrow->pMissileData) {  // D2Common #10637, #10019
        arrow->pMissileData->nStatus = target ? 1 : 2;
        const int16_t ox = static_cast<int16_t>(X - UNITS_GetXPosition(unit));
        const int16_t oy = static_cast<int16_t>(Y - UNITS_GetYPosition(unit));
        const int32_t packed = static_cast<int32_t>(static_cast<uint32_t>(static_cast<int32_t>(oy)) << 16) + ox;
        *reinterpret_cast<int32_t*>(reinterpret_cast<uint8_t*>(arrow->pMissileData) + 0x2c) = packed;
    }
    return 1;
}
