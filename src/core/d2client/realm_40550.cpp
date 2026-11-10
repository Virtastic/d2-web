// 1.13c D2Client+0x40550
// The client missile a skill fires: the row's cltmissilea. A progressive skill (Skills flag bit 2)
// with a progressive state and stat takes cltmissile[charges] (1 charge: cltmissilea, 3 at most).
// NOT PORTED: the progressive branch reads the state's stat list through D2Common ordinals 10871
// and 10680, whose 1.13c identities are not read here; a progressive skill answers -1.
#include <D2DataTbls.h>
#include <Units/Units.h>
#include "realm_skill_do.h"
int d2client_realm_40550(D2UnitStrc*, int skillId) {
    const D2SkillsTxt* row = DATATBLS_GetSkillsTxtRecord(skillId);
    if (!row) return -1;
    if (reinterpret_cast<const uint8_t*>(row)[4] & 4u) return -1;
    return static_cast<int16_t>(row->wCltMissileA);
}
