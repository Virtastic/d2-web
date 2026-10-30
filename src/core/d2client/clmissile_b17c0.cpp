// D2Client client missiles of a spread cast: cltdofunc 17 (Skills.txt Teeth, skill 67) 1.13c D2Client+0x15DA0, its create
// 0xB30F0, the per-frame update (Missiles.txt pCltDoFunc 1) 0xB17C0 and the terminal 0xB1420.
// The client flies its own missiles (75% of the Missiles.txt velocity) whatever the server did; their TravelSound plays at
// creation and their HitSound at a wall or unit collision (an expiry without AlwaysExplode is silent).
// Session mode: the missiles move through the server act (the units they hit are the server's) without entering its room
// lists. Not simulated: the pGfx terminal (no gfx), CltSrcTown (0 for the classes cast here), CltHitFunc (0), the
// ExplosionMissile child (teethexplode: no sound, no collision) and the stat 0x57 velocity term.
// Fire Wall (skill 51): cltdofunc 26 0x74770 makes two makers (cltmissilea, firewallmaker) walking out both ways from the
// target, perpendicular to the cast, and one segment (the maker's SubMissile1, firewall) on the target; each maker (pCltDoFunc 6
// 0xB92B0) drops a segment where it stands whenever its path entered a new subtile on the last step (D2Common #10203, path
// flags & 8). The client target is the hovered unit even when it is dead (the server clears it and refuses the cast). Not
// simulated: the 0x51410 caster gate and the town test (#10057), both pass in the recorded sessions.
#include <cstdint>
#include <vector>
#include <D2Collision.h>
#include <D2Monsters.h>
#include <D2DataTbls.h>
#include <D2Skills.h>
#include <D2StatList.h>
#include <D2States.h>
#include <DataTbls/MissilesTbls.h>
#include <DataTbls/SkillsTbls.h>
#include <Path/Path.h>
#include <Path/PathMisc.h>
#include <Path/Step.h>
#include <Units/Missile.h>
#include <Units/Units.h>

namespace d2client_ambient { void submit(int32_t row); }

namespace d2client_clmissile {
namespace {
constexpr int32_t kTeeth = 67;            // Skills.txt id, cltdofunc 17
constexpr int32_t kActionDelay = 9;       // the cast's action frame (the Poison Nova constant, nova_73cb0.cpp: retail TravelSound at cast+8)
constexpr int32_t kFireWall = 51;         // Skills.txt id, cltdofunc 26
constexpr int32_t kFireWallDelay = 8;     // retail's first segment TravelSound lands at cast+7
constexpr int32_t kStepRate = 1024;       // 1.13c D2Client+0x119824 (DAT_6fbc9824), the D2Common_10226 rate
struct Pending { int64_t due; int32_t skillId, level, targetType; uint32_t targetId; int32_t tx, ty; };
struct NextHit { int32_t type; uint32_t id; int64_t until; };
std::vector<Pending> pending;
std::vector<D2UnitStrc*> missiles;        // creation order (consecutive client ids: ascending hash buckets)
std::vector<D2UnitStrc*> born;            // created by a missile's own do function this frame, updated from the next
bool inUpdate = false;
std::vector<NextHit> nextHits;            // the client's state 0x56 (Missiles.txt NextHit), held per target
int64_t frame = 0;
uint32_t nextId = 1;

const D2MissilesTxt* row(D2UnitStrc* unit) { return DATATBLS_GetMissilesTxtRecord(unit->dwClassId); }

bool nextHitHeld(D2UnitStrc* target) {
    for (const auto& n : nextHits) if (n.type == static_cast<int32_t>(target->dwUnitType) && n.id == target->dwUnitId && frame < n.until) return true;
    return false;
}

void freeMissile(D2UnitStrc* unit) {
    if (unit->pDynamicPath) { PATH_FreeDynamicPath(nullptr, unit->pDynamicPath); unit->pDynamicPath = nullptr; }
    MISSILE_FreeMissileData(unit);
    UNITS_FreeUnit(unit);
}

// 0xB1000 behind the type-3 callback 0xB11D0 (move-test mask 0x184): a player or monster that is attackable, not
// carrying the NextHit state, not the owner, and hostile to the owner (or CollideFriend).
struct Query { D2UnitStrc* missile; D2UnitStrc* owner; const D2MissilesTxt* txt; };
int __fastcall relationHit(D2UnitStrc* target, void* context) {
    auto* q = static_cast<Query*>(context);
    if (!target || (target->dwUnitType != UNIT_PLAYER && target->dwUnitType != UNIT_MONSTER)) return 0;
    if ((target->dwFlags & 0xcu) != 0xcu) return 0;
    if (q->txt->nNextHit && nextHitHeld(target)) return 0;
    if (MISSILE_CheckUnitIfOwner(q->missile, target)) return 0;
    if (!q->owner) return 1;
    return !STATLIST_AreUnitsAligned(target, q->owner) || q->txt->nCollideFriend ? 1 : 0;
}

// 0x55440: the first query result is skipped when it is dying or dead.
bool targetDead(const D2UnitStrc* unit) {
    if (unit->dwFlags & UNITFLAG_ISDEAD) return true;
    if (unit->dwUnitType == UNIT_PLAYER) return unit->dwAnimMode == PLRMODE_DEATH || unit->dwAnimMode == PLRMODE_DEAD;
    if (unit->dwUnitType == UNIT_MONSTER) return unit->dwAnimMode == MONMODE_DEATH || unit->dwAnimMode == MONMODE_DEAD;
    return false;
}

// 1.13c D2Client+0xB1420. wall: the move test collided (param 1); target: the unit the sweep found. Returns true when the
// missile is freed.
bool terminal(D2UnitStrc* unit, D2UnitStrc* owner, bool wall, D2UnitStrc* target) {
    const D2MissilesTxt* txt = row(unit);
    if (!txt) return false;
    bool play = true;
    if (target) {
        if (txt->nNextHit && nextHitHeld(target)) return false;
        if (D2Common_10227(unit)) return false;   // at its own target point: no hit
        if (!(target->dwFlags & 0x4u)) return false;
        if (owner && STATLIST_AreUnitsAligned(target, owner) && !txt->nCollideFriend) return false;
    } else if (!wall && !txt->nAlwaysExplode) play = false;
    if (play) {
        if (txt->nNextHit && target) nextHits.push_back({static_cast<int32_t>(target->dwUnitType), target->dwUnitId, frame + txt->nNextDelay});
        if (static_cast<int16_t>(txt->wHitSound) >= 0) d2client_ambient::submit(static_cast<int16_t>(txt->wHitSound));
    }
    if (target && !txt->nCollideKill) return false;
    freeMissile(unit);
    return true;
}

// 1.13c D2Client+0xB30F0: origin (x, y) (the owner's position for flags 0x820, the request's point for flag 1), target
// point (tx, ty) (flag 0x20; the origin otherwise).
void create(D2UnitStrc* owner, int32_t missileId, int32_t skillId, int32_t level, int32_t x, int32_t y, int32_t tx, int32_t ty, int32_t activateBack) {
    const D2MissilesTxt* txt = DATATBLS_GetMissilesTxtRecord(missileId);
    D2ActiveRoomStrc* room = UNITS_GetRoom(owner);
    if (!txt || !room || !owner->pDynamicPath) return;
    if (D2ActiveRoomStrc* at = COLLISION_GetRoomBySubTileCoordinates(room, x, y)) room = at;
    const int32_t velocity = ((txt->nVelLev * level) / 8 + txt->nVel) * 256 * 75 / 100;
    if (velocity && (tx - x > 99 || x - tx > 99 || ty - y > 99 || y - ty > 99)) return;
    D2UnitStrc* unit = UNITS_AllocUnit(nullptr, UNIT_MISSILE);
    if (!unit) return;
    unit->dwClassId = missileId;
    unit->dwUnitId = nextId++;
    unit->dwAnimMode = txt->nCollideType;
    unit->nAct = owner->nAct;
    unit->pDrlgAct = owner->pDrlgAct;
    PATH_AllocDynamicPath(nullptr, nullptr, x, y, unit, FALSE);   // outside the server room lists
    MISSILE_AllocMissileData(unit);
    if (!unit->pDynamicPath || !unit->pMissileData) { freeMissile(unit); return; }
    unit->pDynamicPath->pRoom = room;
    unit->dwFlags = (unit->dwFlags & ~0xau) | 0x600020u;
    PATH_SetFootprintCollisionMask(unit->pDynamicPath, 0);
    PATH_SetMoveTestCollisionMask(unit->pDynamicPath, txt->nCollideType == 3 ? 0x184 : 0);
    UNITS_SetAnimStartFrame(unit);
    unit->dwFrameCountPrecise = static_cast<uint32_t>(txt->nAnimLen) << 8;
    MISSILE_SetAnimSpeed(unit, static_cast<int>(txt->nAnimSpeed) << 4);
    const int32_t total = static_cast<int16_t>(txt->wRange) + static_cast<int16_t>(txt->wLevRange) * level;
    MISSILE_SetTotalFrames(unit, total);
    MISSILE_SetCurrentFrame(unit, total);
    MISSILE_SetActivateFrame(unit, total - activateBack);
    PATH_SetVelocity(unit->pDynamicPath, 0, __FILE__, __LINE__);
    D2COMMON_10170_PathSetTargetPos(unit->pDynamicPath, tx, ty);
    PATH_SetVelocity(unit->pDynamicPath, velocity, __FILE__, __LINE__);
    if (velocity) D2Common_10142(unit->pDynamicPath, unit, 0);
    if (txt->nInitSteps) unit->dwFlagEx |= 0x40000u;
    MISSILE_SetOwner(unit, owner);
    PATH_SetAcceleration(unit->pDynamicPath, static_cast<int16_t>(txt->wAccel));
    PATH_SetMaxVelocity(unit->pDynamicPath, static_cast<int>(txt->nMaxVel) << 8);
    MISSILE_SetSkill(unit, skillId);
    MISSILE_SetLevel(unit, static_cast<uint16_t>(level));
    (inUpdate ? born : missiles).push_back(unit);
    if (static_cast<int16_t>(txt->wTravelSound) >= 0) d2client_ambient::submit(static_cast<int16_t>(txt->wTravelSound));
}

// 1.13c D2Client+0x15DA0: N = calc1 missiles on a line through the target, perpendicular to the cast.
void spread(D2UnitStrc* owner, const Pending& p, int32_t tx, int32_t ty) {
    const D2SkillsTxt* skill = DATATBLS_GetSkillsTxtRecord(p.skillId);
    if (!skill) return;
    int32_t missile = static_cast<int16_t>(skill->wCltMissileA);
    if (UNITS_GetWeaponClass(owner) != 1 && static_cast<int16_t>(skill->wCltMissileB) >= 0) missile = static_cast<int16_t>(skill->wCltMissileB);
    if (missile < 0 || !DATATBLS_GetMissilesTxtRecord(missile)) return;
    const int32_t n = SKILLS_EvaluateSkillFormula(owner, skill->dwCalc[0], p.skillId, p.level);
    int32_t dx = tx - UNITS_GetXPosition(owner), dy = ty - UNITS_GetYPosition(owner);
    if (dx * dx + dy * dy < 4) { dx *= 4; dy *= 4; }   // 0xA0800
    if (dx * dx + dy * dy < 16) { dx *= 2; dy *= 2; }
    int32_t c = -dx, s = dy;                            // 0xA07C0: the perpendicular, halved to length <= sqrt(3)
    while (c * c + s * s > 3) { s /= 2; c /= 2; }
    dx = s; dy = c;
    int32_t x = tx - (dx * n) / 2, y = ty - (dy * n) / 2;
    const int32_t activateBack = SKILLS_EvaluateSkillFormula(owner, skill->dwCalc[1], p.skillId, p.level);
    const int32_t ox = UNITS_GetXPosition(owner), oy = UNITS_GetYPosition(owner);
    for (int32_t i = 0; i < n; ++i) { create(owner, missile, p.skillId, p.level, ox, oy, x, y, activateBack); x += dx; y += dy; }
}

// 1.13c D2Client+0x74770 (cltdofunc 26): makers to both sides of the target (request flags 0x21), the segment on it (flag 1).
void fireWall(D2UnitStrc* owner, const Pending& p, int32_t tx, int32_t ty) {
    const D2SkillsTxt* skill = DATATBLS_GetSkillsTxtRecord(p.skillId);
    if (!skill) return;
    const int32_t maker = static_cast<int16_t>(skill->wCltMissileA);
    const D2MissilesTxt* makerTxt = maker >= 0 ? DATATBLS_GetMissilesTxtRecord(maker) : nullptr;
    if (!makerTxt) return;
    const int32_t segment = static_cast<int16_t>(makerTxt->wSubMissile[0]);
    const D2MissilesTxt* segmentTxt = segment >= 0 ? DATATBLS_GetMissilesTxtRecord(segment) : nullptr;
    if (!segmentTxt || !tx || !ty) return;
    const int32_t ox = UNITS_GetXPosition(owner), oy = UNITS_GetYPosition(owner);
    create(owner, maker, p.skillId, p.level, tx, ty, tx - (oy - ty), ty + (ox - tx), makerTxt->nActivate);
    create(owner, maker, p.skillId, p.level, tx, ty, tx + (oy - ty), ty - (ox - tx), makerTxt->nActivate);
    create(owner, segment, p.skillId, p.level, tx, ty, tx, ty, segmentTxt->nActivate);
}

// 1.13c D2Client+0xB92B0 (pCltDoFunc 6) ahead of the update: a segment (CltSubMissile1) where the maker stands when its
// last step entered a new subtile.
void makerDrop(D2UnitStrc* unit, D2UnitStrc* owner) {
    const D2MissilesTxt* txt = row(unit);
    if (!txt || !owner || !D2Common_10234(unit->pDynamicPath)) return;
    const int32_t sub = static_cast<int16_t>(txt->wCltSubMissile[0]);
    const D2MissilesTxt* subTxt = sub >= 0 ? DATATBLS_GetMissilesTxtRecord(sub) : nullptr;
    if (!subTxt) return;
    const int32_t x = UNITS_GetXPosition(unit), y = UNITS_GetYPosition(unit);
    create(owner, sub, MISSILE_GetSkill(unit), MISSILE_GetLevel(unit), x, y, x, y, subTxt->nActivate);
}

// 1.13c D2Client+0xB17C0, one missile; returns false once it is freed.
bool update(D2UnitStrc* unit, D2UnitStrc* owner) {
    const D2MissilesTxt* txt = row(unit);
    if (!txt) { freeMissile(unit); return false; }
    if (txt->wCltDoFunc == 6) makerDrop(unit, owner);
    for (int32_t pass = 0; pass < 2; ++pass) {
        D2DynamicPathStrc* path = unit->pDynamicPath;
        const bool waiting = MISSILE_GetActivateFrame(unit) < MISSILE_GetCurrentFrame(unit);
        if (!waiting && MISSILE_GetRemainingFrames(unit) > txt->nInitSteps) unit->dwFlagEx &= ~0x40000u;
        if (!waiting) {
            if (!txt->nLoopAnim) { if (!UNITS_IsAtEndOfFrameCycle(unit)) UNITS_UpdateFrame(unit); }
            else if (txt->nSubLoop && txt->nAnimLen - txt->nSubStop + 1 < MISSILE_GetCurrentFrame(unit)) D2COMMON_10375_UNITS_SetFrameNonRate(unit, txt->nSubStart, txt->nSubStop);
            else UNITS_UpdateFrame(unit);
        }
        if (PATH_GetVelocity(path)) D2Common_10226(unit, kStepRate);
        MISSILE_SetCurrentFrame(unit, MISSILE_GetCurrentFrame(unit) - 1);
        if (MISSILE_GetCurrentFrame(unit) < 1) return !terminal(unit, owner, false, nullptr);
        if (waiting || !unit->dwAnimMode) return true;
        const uint16_t mask = D2Common_10201(path);
        if (mask & 5) return !terminal(unit, owner, true, nullptr);
        if (unit->dwAnimMode == 6) return true;
        if (txt->nClientCol && mask && unit->dwAnimMode == 3) {
            Query q{unit, owner, txt};
            D2PathPointStrc* steps = nullptr;
            const int count = D2COMMON_10198_PathGetSaveStep(path, &steps);
            for (int i = 0; i < count; ++i) {
                D2UnitStrc* hit = D2Common_10407(UNITS_GetRoom(unit), steps[i].X, steps[i].Y, relationHit, &q, UNITS_GetUnitSizeX(unit));
                if (hit && !targetDead(hit)) return !terminal(unit, owner, false, hit);
            }
        }
        // The local owner's first frame catches up once (single player, the owner is the local player).
        if (pass || MISSILE_GetRemainingFrames(unit) != 1 || !PATH_GetVelocity(path)) return true;
    }
    return true;
}
}

void reset() {
    for (D2UnitStrc* unit : missiles) freeMissile(unit);
    for (D2UnitStrc* unit : born) freeMissile(unit);
    missiles.clear(); born.clear(); pending.clear(); nextHits.clear(); frame = 0; nextId = 1;
}

// The client's cast of skillId at a unit target (its path target) or a point; the action runs kActionDelay frames later.
void cast(int32_t skillId, int32_t level, D2UnitStrc* target, int32_t tx, int32_t ty) {
    if (skillId != kTeeth && skillId != kFireWall) return;
    pending.push_back({frame + (skillId == kFireWall ? kFireWallDelay : kActionDelay), skillId, level, target ? static_cast<int32_t>(target->dwUnitType) : -1,
        target ? target->dwUnitId : 0u, target ? UNITS_GetXPosition(target) : tx, target ? UNITS_GetYPosition(target) : ty});
}

// Once per session frame. find resolves the cast's target unit at the action frame.
void tick(D2UnitStrc* owner, D2UnitStrc* (*find)(int32_t type, uint32_t id)) {
    ++frame;
    inUpdate = true;
    for (size_t i = 0; i < missiles.size();) { if (update(missiles[i], owner)) ++i; else missiles.erase(missiles.begin() + i); }
    inUpdate = false;
    missiles.insert(missiles.end(), born.begin(), born.end()); born.clear();
    for (size_t i = 0; i < pending.size();) {
        if (pending[i].due != frame) { ++i; continue; }
        const Pending p = pending[i];
        pending.erase(pending.begin() + i);
        D2UnitStrc* target = p.targetType >= 0 && find ? find(p.targetType, p.targetId) : nullptr;
        if (owner && p.skillId == kFireWall) {
            // 1.13c D2Client+0x51340: the path's target unit (a dead one is not held), else the path target point, else
            // 0x50B30: two subtiles off the owner by its direction / 8. The client player's path holds no point here.
            static constexpr int8_t kDx[8] = {0, -1, -2, -1, 0, 1, 2, 1}, kDy[8] = {2, 1, 0, -1, -2, -1, 0, 1};
            if (target && target->dwUnitType == UNIT_MONSTER && (target->dwAnimMode == MONMODE_DEATH || target->dwAnimMode == MONMODE_DEAD)) target = nullptr;
            const int32_t d = owner->pDynamicPath ? PATH_GetDirection(owner->pDynamicPath) >> 3 : 0;
            fireWall(owner, p, target ? UNITS_GetXPosition(target) : UNITS_GetXPosition(owner) + kDx[d & 7],
                target ? UNITS_GetYPosition(target) : UNITS_GetYPosition(owner) + kDy[d & 7]);
        }
        else if (owner) spread(owner, p, target ? UNITS_GetXPosition(target) : p.tx, target ? UNITS_GetYPosition(target) : p.ty);
    }
}
}
