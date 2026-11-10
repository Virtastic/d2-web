// 1.13c D2Client+0x14F40
// Do function 16 (Jab): only on the action frame 3, a player plays the row's "dosound a" and a
// monster its "dosound b" (each when above 0). Returns 1, also on the other frames.
#include <D2DataTbls.h>
#include <Units/Units.h>
#include "realm_skill_do.h"
int d2client_realm_14f40(D2UnitStrc* unit, int skillId, int, const RealmSkillDoHost& host) {
    if (!unit || unit->nActionFrame != 3) return 1;
    const D2SkillsTxt* row = DATATBLS_GetSkillsTxtRecord(skillId);
    if (!row) return 0;
    const int a = static_cast<int16_t>(row->wDoSoundA), b = static_cast<int16_t>(row->wDoSoundB);
    if (unit->dwUnitType == UNIT_PLAYER && a > 0 && host.unitSound) host.unitSound(unit, a, skillId);
    if (unit->dwUnitType == UNIT_MONSTER && b > 0 && host.unitSound) host.unitSound(unit, b, skillId);
    return 1;
}
