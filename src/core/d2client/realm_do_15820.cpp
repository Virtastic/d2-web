// 1.13c D2Client+0x15820
// Do function 20 (Strafe): one arrow per action frame. The arrows left are the used skill's first
// parameter (the start set it); none left does nothing. The target is the unit the second and third
// parameters name; when the client no longer holds it, the next unit in range after that id
// (+0xA09F0, flags 0xA283). The path aims at it and one cltmissilea (cltmissileb when the weapon is
// not a bow and the row has one) flies from the unit to the aim point (flags 0x21). With arrows still
// left the next target is the next unit in range after this one; the path aims there, the
// parameters name it, and the current frame is cut by the row's param6 percent so the next arrow
// comes sooner.
#include <D2Composit.h>
#include <D2DataTbls.h>
#include <D2Skills.h>
#include <Path/Path.h>
#include <Units/Missile.h>
#include <Units/Units.h>
#include "realm_area.h"

static D2UnitStrc* next_in_range(D2UnitStrc* unit, uint32_t after, int range, const RealmSkillDoHost& host) {
    RealmNextTarget t(after);
    d2client_realm_a1860(unit, 0, 0, range, 0xA283u, d2client_realm_a09f0, &t, 0, host);
    return t.result();
}

int d2client_realm_15820(D2UnitStrc* unit, int skillId, int level, const RealmSkillDoHost& host) {
    const D2SkillsTxt* row = DATATBLS_GetSkillsTxtRecord(skillId);
    if (!row || !unit) return 0;
    D2SkillStrc* skill = UNITS_GetUsedSkill(unit);
    if (!skill) return 0;
    const int qty = SKILLS_GetParam1(skill);
    if (qty <= 0) return 0;
    const int range = SKILLS_EvaluateSkillFormula(unit, row->dwAuraRangeCalc, skillId, level);
    const uint32_t guid = static_cast<uint32_t>(SKILLS_GetParam3(skill));
    const int type = SKILLS_GetParam2(skill);
    D2UnitStrc* target = host.findUnit ? host.findUnit(type, guid) : nullptr;
    if (!target) target = next_in_range(unit, guid, range, host);
    if (target) UNITS_SetTargetUnitForDynamicUnit(unit, target);
    int X = 0, Y = 0;
    if (!d2client_realm_a1340(unit, host, &X, &Y)) return 0;
    int missile = static_cast<int16_t>(row->wCltMissileA);
    if (UNITS_GetWeaponClass(unit) != WEAPONCLASS_BOW && static_cast<int16_t>(row->wCltMissileB) >= 0)
        missile = static_cast<int16_t>(row->wCltMissileB);
    if (missile < 0 || missile >= static_cast<int>(sgptDataTables->nMissilesTxtRecordCount)) return 0;
    D2MissileStrc params{};
    params.dwFlags = 0x21;
    params.pOwner = unit;
    params.nX = UNITS_GetXPosition(unit); params.nY = UNITS_GetYPosition(unit);
    params.nTargetX = X; params.nTargetY = Y;
    params.nMissile = missile;
    params.nSkill = skillId; params.nSkillLevel = level;
    if (host.createMissile) host.createMissile(&params);
    const int left = qty - 1;
    SKILLS_SetParam1(skill, left);
    if (left <= 0) return 1;
    D2UnitStrc* next = next_in_range(unit, target ? target->dwUnitId : 0xFFFFFFFFu, range, host);
    if (!next) return 0;
    // GUESS: 1.13c #10739 takes a fourth argument (0 here) that #10170 does not.
    if (unit->pDynamicPath)
        D2COMMON_10170_PathSetTargetPos(unit->pDynamicPath, UNITS_GetXPosition(next), UNITS_GetYPosition(next));
    SKILLS_SetParam2(skill, static_cast<int>(next->dwUnitType));
    SKILLS_SetParam3(skill, static_cast<int>(next->dwUnitId));
    d2client_realm_505f0(unit, row->dwParam[5]);
    return 1;
}
