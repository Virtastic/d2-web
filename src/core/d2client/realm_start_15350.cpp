// 1.13c D2Client+0x15350
// Client start function 14 (Fend). The range is the unit's melee range plus 4. The first target is
// the path's target unit once checked against the client's units; without one it is the next unit
// in melee reach (+0xA1A50, flags 0x20003), and with none the strike count is 0 and the start goes
// on. With a target the units in reach are counted (flags 0x2A783). The count, held to calc1 at
// most, goes to the used skill's first parameter; the path aims at the target and the second and
// third parameters name it.
#include <D2DataTbls.h>
#include <D2Skills.h>
#include <Path/Path.h>
#include <Units/Units.h>
#include "realm_area.h"
#include "realm_skill_start.h"

int d2client_realm_15350(D2UnitStrc* unit, int skillId, int level) {
    static const RealmSkillDoHost none{};
    const RealmSkillDoHost* h = d2client_realm_start_host();
    const RealmSkillDoHost& host = h ? *h : none;
    const D2SkillsTxt* row = DATATBLS_GetSkillsTxtRecord(skillId);
    if (!row || !unit) return 0;
    D2SkillStrc* skill = UNITS_GetUsedSkill(unit);
    if (!skill) return 0;
    const int range = UNITS_GetMeleeRange(unit) + 4;
    D2UnitStrc* target = d2client_realm_a5f70(unit, host);  // +0x2A0B0, then the path's target unit
    if (!unit->pDynamicPath) return 0;
    int count = 0;
    if (!target) {
        target = d2client_realm_a1a50(0xFFFFFFFFu, 0x20003u, range, unit, &count, host);
        if (!target) { SKILLS_SetParam1(skill, 0); return 1; }
    } else {
        d2client_realm_a1860(unit, 0, 0, range, 0x2A783u, d2client_realm_a0740, &count, 0, host);
    }
    const int most = SKILLS_EvaluateSkillFormula(unit, row->dwCalc[0], skillId, level);
    SKILLS_SetParam1(skill, count >= most ? most : count);
    // GUESS: 1.13c #10739 takes a fourth argument (0 here) that #10170 does not.
    D2COMMON_10170_PathSetTargetPos(unit->pDynamicPath, UNITS_GetXPosition(target), UNITS_GetYPosition(target));
    SKILLS_SetParam2(skill, static_cast<int>(target->dwUnitType));
    SKILLS_SetParam3(skill, static_cast<int>(target->dwUnitId));
    return 1;
}
