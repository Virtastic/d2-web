// 1.13c D2Client+0x15DA0
// Do function 17 (Multiple Shot): marks the unit's do as done, then fans the row's calc1 arrows
// (cltmissilea; cltmissileb when the weapon is not a bow and the row has one) across the line to the
// path's aim point. The step between arrows is the aim direction turned a quarter, shrunk until its
// squared length is at most 3 (a short aim is first grown); the fan is centred on the aim point.
// Each arrow aims at its own point of the fan and takes calc2 as its activate frame.
#include <D2Composit.h>
#include <D2DataTbls.h>
#include <D2Skills.h>
#include <Units/Missile.h>
#include <Units/Units.h>
#include "realm_skill_do.h"

static int32_t mul(int32_t a, int32_t b) { return static_cast<int32_t>(static_cast<uint32_t>(a) * static_cast<uint32_t>(b)); }
static int32_t half(int32_t v) { return (v - (v >> 31)) >> 1; }  // halves toward zero

int d2client_realm_15da0(D2UnitStrc* unit, int skillId, int level, const RealmSkillDoHost& host) {
    if (unit) unit->dwFlags |= 0x40u;
    const D2SkillsTxt* row = DATATBLS_GetSkillsTxtRecord(skillId);
    if (!row || !unit) return 0;
    int missile = static_cast<int16_t>(row->wCltMissileA);
    if (UNITS_GetWeaponClass(unit) != WEAPONCLASS_BOW && static_cast<int16_t>(row->wCltMissileB) >= 0)
        missile = static_cast<int16_t>(row->wCltMissileB);
    if (missile < 0 || missile >= static_cast<int>(sgptDataTables->nMissilesTxtRecordCount)) return 0;
    const int count = SKILLS_EvaluateSkillFormula(unit, row->dwCalc[0], skillId, level);
    int X = 0, Y = 0;
    if (!d2client_realm_a1340(unit, host, &X, &Y)) return 0;
    int32_t dx = X - UNITS_GetXPosition(unit), dy = Y - UNITS_GetYPosition(unit);
    int32_t m = mul(dy, dy) + mul(dx, dx);  // +0xA0800
    if (m < 4) { dx = static_cast<int32_t>(static_cast<uint32_t>(dx) << 2); dy = static_cast<int32_t>(static_cast<uint32_t>(dy) << 2); m = mul(dy, dy) + mul(dx, dx); }
    if (m < 16) { dx = static_cast<int32_t>(static_cast<uint32_t>(dx) << 1); dy = static_cast<int32_t>(static_cast<uint32_t>(dy) << 1); }
    int32_t c = -dx, s = dy;                // +0xA07C0
    while (mul(c, c) + mul(s, s) > 3) { s = half(s); c = half(c); }
    dx = s; dy = c;
    int32_t x = X - half(mul(dx, count)), y = Y - half(mul(dy, count));
    D2MissileStrc params{};
    params.dwFlags = 0x820;
    params.pOwner = unit; params.pOrigin = unit;
    params.nMissile = missile;
    params.nSkill = skillId; params.nSkillLevel = level;
    params.nActivateFrame = SKILLS_EvaluateSkillFormula(unit, row->dwCalc[1], skillId, level);
    for (int i = count; i > 0; --i) {
        params.nTargetX = x; params.nTargetY = y;
        if (host.createMissile) host.createMissile(&params);
        x += dx; y += dy;
    }
    return 1;
}
