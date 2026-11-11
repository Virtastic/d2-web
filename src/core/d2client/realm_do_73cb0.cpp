// 1.13c D2Client+0x73CB0
// Do function 25 (the nova casts: Poison Nova, Frost Nova): the caster is marked done, and the skill's client
// missile (+0x40550) is launched 64 times around the caster (+0xA1020 then +0xA0DB0): a request with flags 3 | 4 (start
// at the given point, target as an offset from it, speed given), the speed being the missile row's base plus its
// per-level step (D2Common #11169) added to the skill's calc (#10786); the 64 offsets are the table at +0xD4390 (x) and
// +0xD4290 (y), a circle of radius 30 cut toward zero.
#include <cmath>
#include <D2DataTbls.h>
#include <D2Skills.h>
#include <Units/Missile.h>
#include <Units/Units.h>
#include "realm_skill_do.h"

int d2client_realm_73cb0(D2UnitStrc* unit, int skillId, int level, const RealmSkillDoHost& host) {
    const D2SkillsTxt* row = DATATBLS_GetSkillsTxtRecord(skillId);
    if (!row) return 0;
    const int missile = d2client_realm_40550(unit, skillId);
    if (missile < 0 || missile >= static_cast<int>(sgptDataTables->nMissilesTxtRecordCount)) return 0;
    if (!unit) return 0;   // retail with no unit only plays sound 2473 64 times
    unit->dwFlags |= 0x40u;
    const D2MissilesTxt* mrow = DATATBLS_GetMissilesTxtRecord(missile);
    if (!mrow) return 0;
    const int base = (static_cast<int>(mrow->nVelLev) * level) / 8 + mrow->nVel;   // #11169
    const int velocity = base + SKILLS_EvaluateSkillFormula(unit, row->dwCltCalc[0], skillId, level);
    for (int i = 0; i < 64; ++i) {
        const double a = 6.283185307179586 * i / 64.0;
        D2MissileStrc params{};
        params.dwFlags = 3u | 4u;
        params.pOwner = unit;
        params.nMissile = missile;
        params.nX = UNITS_GetXPosition(unit); params.nY = UNITS_GetYPosition(unit);
        params.nTargetX = static_cast<int>(std::trunc(30.0 * std::cos(a)));
        params.nTargetY = static_cast<int>(std::trunc(30.0 * std::sin(a)));
        params.nVelocity = velocity;
        params.nSkill = skillId; params.nSkillLevel = level;
        if (host.createMissile) host.createMissile(&params);
    }
    return 1;
}
