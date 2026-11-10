// 1.13c D2Client+0xA1340 (with +0x2A0B0 and +0xA0B30)
// The point a unit's path aims at. First the path's target unit is checked against the client's
// own units (+0x2A0B0): when the type and id it records no longer find that same unit, the target
// is dropped. Then: the target unit's position, else the path's target point. When either
// coordinate is 0 the point is two subtiles off the unit by its facing (+0xA0B30). Always 1.
#include <Path/Path.h>
#include <Units/Units.h>
#include "realm_skill_do.h"

static void check_path_target(D2UnitStrc* unit, const RealmSkillDoHost& host) {  // +0x2A0B0
    D2DynamicPathStrc* path = unit->pDynamicPath;
    if (!PATH_GetTargetUnit(path)) return;
    int type = 0; D2UnitGUID guid = 0;
    PATH_GetTargetTypeAndGUID(path, &type, &guid);
    D2UnitStrc* found = host.findUnit ? host.findUnit(type, static_cast<uint32_t>(guid)) : nullptr;
    if (found != PATH_GetTargetUnit(path)) PATH_SetTargetUnit(path, nullptr);
}

int d2client_realm_a1340(D2UnitStrc* unit, const RealmSkillDoHost& host, int* x, int* y) {
    if (!unit || !unit->pDynamicPath) return 0;
    check_path_target(unit, host);
    D2DynamicPathStrc* path = unit->pDynamicPath;
    if (D2UnitStrc* target = PATH_GetTargetUnit(path)) {
        *x = UNITS_GetXPosition(target); *y = UNITS_GetYPosition(target);
    } else {
        *x = path->tTargetCoord.X; *y = path->tTargetCoord.Y;
    }
    if (!*x || !*y) {  // +0xA0B30, by the facing's eighth
        static constexpr int8_t kDx[8] = {0, -1, -2, -1, 0, 1, 2, 1}, kDy[8] = {2, 1, 0, -1, -2, -1, 0, 1};
        const int d = (PATH_GetDirection(path) >> 3) & 7;
        *x = UNITS_GetXPosition(unit) + kDx[d];
        *y = UNITS_GetYPosition(unit) + kDy[d];
    }
    return 1;
}

// 1.13c D2Client+0xA5F70
D2UnitStrc* d2client_realm_a5f70(D2UnitStrc* unit, const RealmSkillDoHost& host) {
    if (!unit || !unit->pDynamicPath) return nullptr;
    check_path_target(unit, host);
    return PATH_GetTargetUnit(unit->pDynamicPath);
}
