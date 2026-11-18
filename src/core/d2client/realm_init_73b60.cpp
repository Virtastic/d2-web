// 1.13c D2Client+0x73B60
// A client missile's init step (the D2MissileStrc +0x54 init function, called by the client missile
// maker +0xB30F0 with the +0x58 word): fastcall (ECX missile, EDX i). The life is capped at 0x4D frames
// (D2Common #10534 total frames; at 0x4E or more #10558 and #10970 set both total and current to 0x4D),
// the low seed (#10614) becomes the path's target x word (#10983, path +0x10) plus i, the path type
// (#10647) is 10, the new distance (#10168) is the life, and the path is stepped once (#10334, the 1.10f source's
// D2Common_10142 1.10f twin). Cltdofunc 19 (Charged Strike, +0x15AB0) gives it the bolt's loop index.
// Retail's unit +0x20 is pSeed and path +0x10 is tTargetCoord (the wasm64 layout differs; named fields here).
#include <D2Seed.h>
#include <Path/Path.h>
#include <Units/Missile.h>
#include <Units/Units.h>
#include "realm_skill_do.h"

void d2client_realm_73b60(D2UnitStrc* missile, int32_t i) {
    if (!missile) return;
    int32_t frames = MISSILE_GetTotalFrames(missile);
    if (frames >= 0x4e) {
        frames = 0x4d;
        MISSILE_SetTotalFrames(missile, frames);
        MISSILE_SetCurrentFrame(missile, frames);
    }
    D2DynamicPathStrc* path = missile->pDynamicPath;
    SEED_InitLowSeed(&missile->pSeed, static_cast<int>(path->tTargetCoord.X) + i);
    PATH_SetType(path, 10);
    PATH_SetNewDistance(path, static_cast<uint8_t>(frames));
    D2Common_10142(path, missile, 0);
}
