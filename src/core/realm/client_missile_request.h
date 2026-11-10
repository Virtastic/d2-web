#pragma once

#include <cstdint>

namespace d2::realm {

// The client's missile maker reads a request (flags and numbers) and the missile's table row, and decides
// where the missile starts, where it heads, how fast it flies and how long it lives. This is that decision
// with no unit or room in it, so the same rule serves a cast, a shrine and a missile's own children.
//
// Request flags read here:
//   0x0001 the start is the request's x, y (else the origin unit's position)
//   0x0002 the target pair is added to the start (a step, not a cell)
//   0x0004 the speed is the request's (else the row's, grown by level)
//   0x0008 a row with a sub-loop lives (sub stop - sub start) * loops longer
//   0x0010 the request's speed is already in 8.8 (else it is shifted up)
//   0x0020 the target pair is a cell (ignored when 0x0002 is set; neither: the missile heads nowhere)
//   0x0200 the missile starts `frame` frames into its life
//   0x0400 the life is the time the flight to the target takes (see missileFlightFrames)
//   0x0800 the activation delay is the request's (else the row's)
//   0x8000 the life is the request's (else the row's, grown by level)
struct MissileRequestRow { int vel, velLev, range, levRange, activate, subLoop, subStart, subStop; };
struct MissileRequest {
    uint32_t flags;
    bool hasOrigin; int originX, originY;      // the origin unit's position, when the request names one
    int x, y, targetX, targetY, velocity, level, loops, frame, range, activateFrame;
};
struct MissileStart { int x, y, targetX, targetY, velocity, range, startFrame, activate; };

// False when the request cannot make a missile: no start, or a moving missile aimed 100 cells or more away.
// `targetX/Y` of the result is the aim before a target unit or the same-cell rule replaces it.
constexpr bool resolveMissileRequest(const MissileRequest& in, const MissileRequestRow& row, MissileStart& out) {
    if (in.flags & 0x1u) { out.x = in.x; out.y = in.y; }
    else if (in.hasOrigin) { out.x = in.originX; out.y = in.originY; }
    else return false;
    if (in.flags & 0x2u) { out.targetX = out.x + in.targetX; out.targetY = out.y + in.targetY; }
    else if (in.flags & 0x20u) { out.targetX = in.targetX; out.targetY = in.targetY; }
    else { out.targetX = out.x; out.targetY = out.y; }
    int velocity = 0;
    if (in.flags & 0x4u) velocity = (in.flags & 0x10u) ? in.velocity : in.velocity * 256;
    else velocity = (row.vel + (row.velLev * in.level) / 8) * 256;
    out.velocity = velocity * 75 / 100;
    int range = (in.flags & 0x8000u) ? in.range : row.range + row.levRange * in.level;
    if (!(in.flags & 0x8000u) && (in.flags & 0x8u) && row.subLoop) range += (row.subStop - row.subStart) * in.loops;
    out.startFrame = 0;
    if (in.flags & 0x200u) { range -= in.frame; out.startFrame = in.frame; }
    out.range = range;
    out.activate = (in.flags & 0x800u) ? in.activateFrame : row.activate;
    return true;
}

// A moving missile whose aim is 100 cells or more from its start on either axis is not made.
constexpr bool missileAimTooFar(int velocity, int x, int y, int aimX, int aimY) {
    const int dx = aimX > x ? aimX - x : x - aimX, dy = aimY > y ? aimY - y : y - aimY;
    return velocity != 0 && (dx >= 100 || dy >= 100);
}

// Flag 0x0400: the frames a thrown missile takes to reach its target, from the distance the engine measures
// between the new missile and the target (never less than 1). -1 when the missile does not move: the life the
// row gave stays.
constexpr int missileFlightFrames(uint32_t distance, int velocity) {
    if (!distance) distance = 1;
    const uint32_t step = static_cast<uint32_t>(velocity) << 4;
    return step ? static_cast<int>((distance << 16) / step) : -1;
}

// The walk-test mask a missile's collide type gives its path.
// GUESS: the client's table is taken to equal the server's (type 3, the only one checked on the client, agrees).
constexpr uint32_t missileCollideMask(int collideType) {
    constexpr uint32_t kMask[9] = { 0, 0x84, 0x104, 0x184, 0, 0x104, 0x4, 0x40, 0x185 };
    return collideType >= 0 && collideType < 9 ? kMask[collideType] : 0;
}

} // namespace d2::realm
