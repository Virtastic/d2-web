// 1.13c D2Client+0x14FD0
// Do function 21 (Fend): one strike per action frame. The target is the unit the used skill's
// second and third parameters name while it is still in melee reach; otherwise the next unit in
// reach after that id (+0xA09F0, flags 0x2A283), and with none the do does nothing. One strike is
// taken from the first parameter; with strikes left the next unit in reach after the target is
// aimed at and named, and the current frame is cut by the row's param2 percent.
// NOT PORTED: the mode restart and its sound after the cut (#10862, +0x22280, +0x88A70).
#include <D2DataTbls.h>
#include <D2Skills.h>
#include <Path/Path.h>
#include <Units/Units.h>
#include "realm_area.h"

static D2UnitStrc* next_in_reach(D2UnitStrc* unit, uint32_t after, int range, const RealmSkillDoHost& host) {
    RealmNextTarget t(after);
    d2client_realm_a1860(unit, 0, 0, range, 0x2A283u, d2client_realm_a09f0, &t, 0, host);
    return t.result();
}

int d2client_realm_14fd0(D2UnitStrc* unit, int skillId, int level, const RealmSkillDoHost& host) {
    (void)level;
    const D2SkillsTxt* row = DATATBLS_GetSkillsTxtRecord(skillId);
    if (!row || !unit) return 0;
    D2SkillStrc* skill = UNITS_GetUsedSkill(unit);
    if (!skill) return 0;
    const uint32_t guid = static_cast<uint32_t>(SKILLS_GetParam3(skill));
    const int type = SKILLS_GetParam2(skill);
    D2UnitStrc* target = host.findUnit ? host.findUnit(type, guid) : nullptr;
    const int range = UNITS_GetMeleeRange(unit) + 4;
    if (!(target && UNITS_IsInMeleeRange(unit, target, 0))) {
        target = next_in_reach(unit, guid, range, host);
        if (!target) return 0;
    }
    const int left = SKILLS_GetParam1(skill) - 1;
    SKILLS_SetParam1(skill, left);
    if (left <= 0) return 1;
    D2UnitStrc* next = next_in_reach(unit, target->dwUnitId, range, host);
    if (!next) return 0;
    // GUESS: 1.13c #10739 takes a fourth argument (0 here) that #10170 does not.
    if (unit->pDynamicPath)
        D2COMMON_10170_PathSetTargetPos(unit->pDynamicPath, UNITS_GetXPosition(next), UNITS_GetYPosition(next));
    SKILLS_SetParam2(skill, static_cast<int>(next->dwUnitType));
    SKILLS_SetParam3(skill, static_cast<int>(next->dwUnitId));
    d2client_realm_505f0(unit, row->dwParam[1]);
    return 1;
}
