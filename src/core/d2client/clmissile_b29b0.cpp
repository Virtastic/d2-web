// 1.13c D2Client+0xB29B0 (Missiles.txt pCltDoFunc 20, the Frozen Orb nova): while its age (total - current frame) is below
// CltParam1 and a multiple of max(CltParam2, 1), the stored offset (TargetX, TargetY) turns 45 degrees, x' = (x - y) / 2,
// y' = (x + y) / 2; the path is re-aimed at position + offset (10170, 10142), and the offset is stored back. The caller runs the
// 0xB17C0 update after it. The server's twin is MISSMODE_SrvDo16_FrozenOrbNova.
#include <algorithm>
#include <D2DataTbls.h>
#include <DataTbls/MissilesTbls.h>
#include <Path/Path.h>
#include <Units/Missile.h>
#include <Units/Units.h>
#include "clmissile_orb.h"

namespace d2client_clmissile_orb {
void novaMove(D2UnitStrc* unit) {
    const D2MissilesTxt* txt = unit ? DATATBLS_GetMissilesTxtRecord(unit->dwClassId) : nullptr;
    if (!txt || !unit->pDynamicPath) return;
    const int32_t step = std::max(static_cast<int32_t>(txt->dwCltParam[1]), 1);
    const int32_t age = MISSILE_GetRemainingFrames(unit);
    if (age >= static_cast<int32_t>(txt->dwCltParam[0]) || age % step != 0) return;
    const int32_t a = MISSILE_GetTargetX(unit), b = MISSILE_GetTargetY(unit);
    const int32_t dx = (a - b) / 2, dy = (a + b) / 2;
    const int32_t y = UNITS_GetYPosition(unit) + dy, x = UNITS_GetXPosition(unit) + dx;
    D2COMMON_10170_PathSetTargetPos(unit->pDynamicPath, x, y);
    D2Common_10142(unit->pDynamicPath, unit, 0);
    MISSILE_SetTargetX(unit, dx);
    MISSILE_SetTargetY(unit, dy);
}
}
