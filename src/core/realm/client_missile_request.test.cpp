// Host test: tools/realm-tests.sh src/core/realm/client_missile_request.test.cpp
// The client missile maker's reading of a request, and the two hit rules that make a missile's children.
#include "client_missile_request.h"
#include "client_missile_hit_child.h"
#include "client_missile_hit_ring.h"
#include <cassert>
#include <cstdio>

using namespace d2::realm;

int main() {
    // Rows as the retail table holds them: {vel, velLev, range, levRange, activate, subLoop, subStart, subStop}.
    const MissileRequestRow fireball{ 20, 0, 50, 0, 0, 0, 0, 0 }, potion{ 16, 0, 30, 0, 0, 0, 0, 0 },
        cloud{ 0, 0, 60, 0, 0, 1, 10, 19 }, burst{ 0, 0, 16, 0, 0, 0, 0, 0 }, grown{ 8, 4, 10, 2, 3, 0, 0, 0 };
    MissileStart s{};

    // A fire shrine's missile: flags 3, the pair is a step added to the start.
    MissileRequest fire{}; fire.flags = 3; fire.x = 100; fire.y = 200; fire.targetX = 5; fire.targetY = -10; fire.level = 4;
    assert(resolveMissileRequest(fire, fireball, s));
    assert(s.x == 100 && s.y == 200 && s.targetX == 105 && s.targetY == 190);
    assert(s.velocity == 20 * 256 * 75 / 100 && s.range == 50 && s.startFrame == 0 && s.activate == 0);
    assert(!missileAimTooFar(s.velocity, s.x, s.y, s.targetX, s.targetY));

    // An exploding or poison shrine's missile: flags 0x520, start at the origin unit, the pair is a cell.
    MissileRequest thrown{}; thrown.flags = 0x520; thrown.hasOrigin = true; thrown.originX = 100; thrown.originY = 200;
    thrown.x = 1; thrown.y = 2; thrown.targetX = 94; thrown.targetY = 206; thrown.level = 8;
    assert(resolveMissileRequest(thrown, potion, s));
    assert(s.x == 100 && s.y == 200 && s.targetX == 94 && s.targetY == 206 && s.velocity == 3072 && s.range == 30);
    // Its life is its flight: the engine's distance for a (6, 6) step is 9 and for a (0, 6) step 6.
    assert(missileFlightFrames(9, 3072) == 12 && missileFlightFrames(6, 3072) == 8);
    assert(missileFlightFrames(0, 3072) == 1 && missileFlightFrames(9, 0) == -1);
    thrown.hasOrigin = false;
    assert(!resolveMissileRequest(thrown, potion, s));         // no origin unit and no flag 1: no missile

    // A cast (0x21): cell target, the row's speed grown by level. Fire Ball's child (0x201): starts late, stands still.
    MissileRequest cast{}; cast.flags = 0x21; cast.x = 10; cast.y = 10; cast.targetX = 40; cast.targetY = 12; cast.level = 6;
    assert(resolveMissileRequest(cast, grown, s));
    assert(s.targetX == 40 && s.targetY == 12 && s.velocity == (8 + 4 * 6 / 8) * 256 * 75 / 100 && s.range == 22 && s.activate == 3);
    MissileRequest child{}; child.flags = 0x201; child.x = 7; child.y = 9; child.frame = 5; child.level = 1;
    assert(resolveMissileRequest(child, burst, s));
    assert(s.targetX == 7 && s.targetY == 9 && s.velocity == 0 && s.range == 11 && s.startFrame == 5);
    assert(!missileAimTooFar(0, 0, 0, 500, 500) && missileAimTooFar(1, 0, 0, 100, 0) && missileAimTooFar(1, 0, 0, 0, -100) &&
        !missileAimTooFar(1, 0, 0, 99, -99));

    // Request-owned speed, life and activation; the sub-loop extension only with flag 8 on a looping row.
    MissileRequest own{}; own.flags = 0x1 | 0x4 | 0x800 | 0x8000; own.velocity = 4; own.range = 77; own.activateFrame = 9;
    assert(resolveMissileRequest(own, grown, s) && s.velocity == 4 * 256 * 75 / 100 && s.range == 77 && s.activate == 9);
    own.flags = 0x1F; own.velocity = 256; own.loops = 3; own.level = 0;
    assert(resolveMissileRequest(own, cloud, s) && s.velocity == 192 && s.range == 60 + 9 * 3);
    assert(resolveMissileRequest(own, potion, s) && s.range == 30);
    own.flags = 0x17;
    assert(resolveMissileRequest(own, cloud, s) && s.range == 60);

    assert(missileCollideMask(3) == 0x184 && missileCollideMask(6) == 0x4 && missileCollideMask(0) == 0 && missileCollideMask(9) == 0);

    // The gas potion's ring: hit numbers 2, 2, 3 and the cloud's speeds 2 and 4 make 8 + 8 clouds.
    MissileRingChild ring[31];
    assert(missileRingChildren(2, 2, 3, 2, 4, ring) == 16);
    assert(ring[0].flags == 0x1F && ring[0].stepX == 0 && ring[0].stepY == 2 && ring[0].velocity == 256 && ring[0].loops == 3);
    assert(ring[1].stepX == 2 && ring[1].stepY == 2 && ring[7].stepX == -2 && ring[7].stepY == 2);
    assert(ring[8].stepX == 1 && ring[8].stepY == 2 && ring[8].velocity == 512 && ring[15].stepX == -1 && ring[15].stepY == 2);
    // Each first-ring cloud, asked of the maker, flies one step from the missile and lives three sub-loops longer.
    MissileRequest gas{}; gas.flags = ring[1].flags; gas.x = 50; gas.y = 60; gas.targetX = ring[1].stepX; gas.targetY = ring[1].stepY;
    gas.velocity = ring[1].velocity; gas.loops = ring[1].loops;
    assert(resolveMissileRequest(gas, cloud, s) && s.targetX == 52 && s.targetY == 62 && s.velocity == 192 && s.range == 87);
    // No loops: flags 0x17. A stride below 1 walks every entry; no second ring without its stride.
    assert(missileRingChildren(0, 0, 0, 1, 1, ring) == 16 && ring[0].flags == 0x17 && ring[0].loops == 0 && ring[15].stepX == -1);
    assert(missileRingChildren(1, 1, 0, 1, 1, ring) == 31 && ring[30].stepX == -1 && ring[30].stepY == 2);
    assert(missileRingChildren(5, 16, 0, 1, 1, ring) == 4 && ring[1].stepX == 1 && ring[2].stepX == 2 && ring[2].stepY == -2);

    // The exploding potion's burst: one child, no draw. A second child, plain or drawn from a span.
    int rolls = 0;
    auto roll = [&rolls](uint32_t span) { ++rolls; return span - 1; };
    MissileHitChildren c = missileHitChildren(42, -1, -1, roll);
    assert(c.first == 42 && c.second == -1 && !c.rolled && rolls == 0);
    c = missileHitChildren(-1, 7, 9, roll);
    assert(c.first == -1 && c.second == -1 && rolls == 0);
    c = missileHitChildren(42, 7, -1, roll);
    assert(c.first == 42 && c.second == 7 && !c.rolled && rolls == 0);
    c = missileHitChildren(42, 7, 9, roll);
    assert(c.second == 9 && c.rolled && rolls == 1);
    std::puts("client_missile_request: ok");
    return 0;
}
