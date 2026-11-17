// 1.13c D2Client+0x15AB0
// Do function 19 (Charged Strike): fastcall (ECX unit, EDX skill id, one stack word: level). The path's
// target unit, checked against the client's units (+0x2A0B0, then D2Common #10392), is required. The
// skill's client missile (+0x40550) must be a Missiles row. The count is the Skills row's calc +0x138
// (D2Common #10786 with the skill id and level). Each bolt starts on the target and aims at the point
// mirrored through it from the unit (2*target - unit), with flags 0x21, the skill and level, and the
// init step +0x73B60 with its loop index (block words 21 and 22). The unit's do flag 0x40 is not set
// here. Returns 1 once the target and missile are good, even when the count is 0.
#include <D2DataTbls.h>
#include <D2Skills.h>
#include <Units/Missile.h>
#include <Units/Units.h>
#include "realm_skill_do.h"

int d2client_realm_15ab0(D2UnitStrc* unit, int skillId, int level, const RealmSkillDoHost& host) {
    D2SkillsTxt* row = DATATBLS_GetSkillsTxtRecord(skillId);
    if (!row || !unit) return 0;
    D2UnitStrc* target = d2client_realm_a5f70(unit, host);
    if (!target) return 0;
    const int missile = d2client_realm_40550(unit, skillId);
    if (missile < 0 || missile >= static_cast<int>(sgptDataTables->nMissilesTxtRecordCount)) return 0;
    const int tx = UNITS_GetXPosition(target), ty = UNITS_GetYPosition(target);
    const int ux = UNITS_GetXPosition(unit), uy = UNITS_GetYPosition(unit);
    const int count = SKILLS_EvaluateSkillFormula(unit, row->dwCalc[0], skillId, level);
    D2MissileStrc params{};
    params.dwFlags = 0x21u;
    params.pOwner = unit;
    params.nMissile = missile;
    params.nX = tx; params.nY = ty;
    params.nTargetX = tx * 2 - ux; params.nTargetY = ty * 2 - uy;
    params.nSkill = skillId; params.nSkillLevel = level;
    params.pInitFunc = d2client_realm_73b60;
    for (int i = 0; i < count; ++i) {
        params.pInitArgs = i;
        if (host.createMissile) host.createMissile(&params);
    }
    return 1;
}
