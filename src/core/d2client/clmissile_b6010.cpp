// 1.13c D2Client+0xB6010 (Missiles.txt pCltHitFunc 30, the Frozen Orb's end): a ring of up to 64 points (radius 30), every
// max(CltHitPar1, 1)-th, each a CltHitSubMissile1 (frozenorbnova) made at the orb's position and aimed at position + the ring
// point; the child keeps the point as its TargetX / TargetY (0xB29B0 turns it from there). Always returns 1 (the caller goes on).
#include <algorithm>
#include <D2DataTbls.h>
#include <DataTbls/MissilesTbls.h>
#include <Units/Missile.h>
#include <Units/Units.h>
#include "clmissile_orb.h"

namespace d2client_clmissile_orb {
void ring(D2UnitStrc* unit, D2UnitStrc* owner, Make make) {
    const D2MissilesTxt* txt = unit ? DATATBLS_GetMissilesTxtRecord(unit->dwClassId) : nullptr;
    if (!txt || !owner) return;
    const int32_t child = static_cast<int16_t>(txt->wCltHitSubMissile[0]);
    if (child < 0) return;
    const int32_t step = std::max(static_cast<int32_t>(txt->dwCltHitPar[0]), 1);
    const int32_t x = UNITS_GetXPosition(unit), y = UNITS_GetYPosition(unit);
    const int32_t skill = MISSILE_GetSkill(unit), level = MISSILE_GetLevel(unit);
    for (int32_t i = 0; i < 64; i += step) {
        const int32_t ox = kCos[i], oy = kSin[i];
        if (D2UnitStrc* m = make(owner, child, skill, level, x, y, x + ox, y + oy)) { MISSILE_SetTargetX(m, ox); MISSILE_SetTargetY(m, oy); }
    }
}
}
