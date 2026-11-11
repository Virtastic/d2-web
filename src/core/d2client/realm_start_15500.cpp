// 1.13c D2Client+0x15500
// Client start function 13 (Strafe). The start refuses when the hand weapon has nothing to shoot.
// The search range is the row's aura range calc. The first target is the path's target unit once
// checked against the client's units; without one it is the next unit in range (+0xA1A50, flags 3)
// and the search's count is the count of units in range, otherwise the units in range are counted
// (+0xA1AD0, flags 3). The arrow count is that count held between calc3 (the least) and calc1 (the
// most). It goes to the used skill's first parameter, the target's type and id to the second and
// third (type 1 and id -1 with no target), and the path aims at the target.
#include <D2DataTbls.h>
#include <D2Skills.h>
#include <Path/Path.h>
#include <Units/Units.h>
#include "realm_area.h"
#include "realm_skill_start.h"

int d2client_realm_15500(D2UnitStrc* unit, int skillId, int level) {
    if (!d2client_realm_a0a30(unit)) return 0;
    static const RealmSkillDoHost none{};
    const RealmSkillDoHost* h = d2client_realm_start_host();
    const RealmSkillDoHost& host = h ? *h : none;
    const D2SkillsTxt* row = DATATBLS_GetSkillsTxtRecord(skillId);
    if (!row) return 0;
    D2SkillStrc* skill = UNITS_GetUsedSkill(unit);
    if (!skill) return 0;
    const int range = SKILLS_EvaluateSkillFormula(unit, row->dwAuraRangeCalc, skillId, level);
    D2UnitStrc* target = d2client_realm_a5f70(unit, host);
    int count = 0;
    if (!target) target = d2client_realm_a1a50(0xFFFFFFFFu, 3, range, unit, &count, host);
    else count = d2client_realm_a1ad0(3, range, unit, host);
    const int least = SKILLS_EvaluateSkillFormula(unit, row->dwCalc[2], skillId, level);
    const int most = SKILLS_EvaluateSkillFormula(unit, row->dwCalc[0], skillId, level);
    int qty = least < most ? least : most;
    if (count > qty) qty = count;
    if (qty > most) qty = most;
    SKILLS_SetParam1(skill, qty);
    if (target) {
        SKILLS_SetParam2(skill, static_cast<int>(target->dwUnitType));
        SKILLS_SetParam3(skill, static_cast<int>(target->dwUnitId));
        // GUESS: 1.13c #10739 takes a fourth argument (0 here) that #10170 does not.
        if (unit->pDynamicPath)
            D2COMMON_10170_PathSetTargetPos(unit->pDynamicPath, UNITS_GetXPosition(target), UNITS_GetYPosition(target));
    } else {
        SKILLS_SetParam2(skill, 1);
        SKILLS_SetParam3(skill, -1);
    }
    return 1;
}
