// 1.13c D2Client+0x401A0
// Client start function 7: starts when the skill's aura state is a real state the unit does not have.
#include <D2DataTbls.h>
#include <D2Skills.h>
#include <D2States.h>
#include <Path/Path.h>
#include <Units/Units.h>
#include "realm_skill_start.h"

int d2client_realm_401a0(D2UnitStrc* unit, int skillId, int) {
    D2SkillsTxt* row = DATATBLS_GetSkillsTxtRecord(skillId);
    if (!row) return 0;
    const int st = row->nAuraState;
    if (st < 0 || st >= sgptDataTables->nStatesTxtRecordCount) return 0;
    return STATES_CheckState(unit, st) == 0;
}
