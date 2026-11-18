// 1.13c D2Client+0xB8840 (Missiles.txt pCltDoFunc 19, the Frozen Orb): every CltParam1-th frame (age = total - current frame)
// the orb makes its CltSubMissile1 (frozenorbbolt) at its own position, aimed at position + (cos, sin)[v], v = |TargetX| % 64
// (the missile data's TargetX is the 64-step phase); TargetX then becomes (CltParam2 + v) % 64. Owner, skill and level are the
// orb's. The caller runs the 0xB17C0 update after it (the function's tail). The server's twin is MISSMODE_SrvDo15_FrozenOrb.
#include <algorithm>
#include <cstdlib>
#include <D2DataTbls.h>
#include <DataTbls/MissilesTbls.h>
#include <Units/Missile.h>
#include <Units/Units.h>
#include "clmissile_orb.h"

namespace d2client_clmissile_orb {
const int32_t kCos[64] = { 30, 29, 29, 28, 27, 26, 24, 23, 21, 19, 16, 14, 11, 8, 5, 2, 0, -2, -5, -8, -11, -14, -16, -19, -21, -23, -24, -26, -27, -28, -29, -29,
                           -30, -29, -29, -28, -27, -26, -24, -23, -21, -19, -16, -14, -11, -8, -5, -2, 0, 2, 5, 8, 11, 14, 16, 19, 21, 23, 24, 26, 27, 28, 29, 29 };   // 0xD3F10
const int32_t kSin[64] = { 0, 2, 5, 8, 11, 14, 16, 19, 21, 23, 24, 26, 27, 28, 29, 29, 30, 29, 29, 28, 27, 26, 24, 23, 21, 19, 16, 14, 11, 8, 5, 2,
                           0, -2, -5, -8, -11, -14, -16, -19, -21, -23, -24, -26, -27, -28, -29, -29, -30, -29, -29, -28, -27, -26, -24, -23, -21, -19, -16, -14, -11, -8, -5, -2 };   // 0xD3E10

void phaseSpawn(D2UnitStrc* unit, D2UnitStrc* owner, Make make) {
    const D2MissilesTxt* txt = unit ? DATATBLS_GetMissilesTxtRecord(unit->dwClassId) : nullptr;
    if (!txt) return;
    const int32_t child = static_cast<int16_t>(txt->wCltSubMissile[0]);
    if (child < 0 || !owner) return;
    const int32_t n = std::max(static_cast<int32_t>(txt->dwCltParam[0]), 1);
    if (MISSILE_GetRemainingFrames(unit) % n > 0) return;
    const int32_t v = std::abs(MISSILE_GetTargetX(unit) % 64);
    const int32_t x = UNITS_GetXPosition(unit), y = UNITS_GetYPosition(unit);
    MISSILE_SetTargetX(unit, (static_cast<int32_t>(txt->dwCltParam[1]) + v) % 64);
    make(owner, child, MISSILE_GetSkill(unit), MISSILE_GetLevel(unit), x, y, x + kCos[v], y + kSin[v]);
}
}
