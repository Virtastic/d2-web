// The client's area scan (1.13c D2Client+0xA1860) and the searches built on it, for units the
// joined-game client keeps as engine units. The scan walks the rooms around a unit, tests each
// unit in them against a radius and the aura-filter flags, and calls a callback on the ones that
// pass. One file: realm_a1860.cpp.
#pragma once
#include <cstdint>
#include "realm_skill_do.h"
struct D2UnitStrc;

// A callback's context: the scanning unit, how many callbacks returned nonzero, the caller's value.
struct RealmAreaContext { D2UnitStrc* unit; int count; void* value; };
using RealmAreaCallback = int (*)(RealmAreaContext* ctx, D2UnitStrc* candidate);

// 1.13c D2Client+0xA1860. x or y 0: the unit's own position. flags 0: 0x583. noMultishot: a monster
// whose MonStats row carries the no-multishot flag is skipped.
void d2client_realm_a1860(D2UnitStrc* unit, int x, int y, int radius, uint32_t flags, RealmAreaCallback cb,
    void* value, int noMultishot, const RealmSkillDoHost& host);
// 1.13c D2Client+0xA1620: whether `candidate` passes the aura-filter `flags` seen from `origin`.
int d2client_realm_a1620(uint32_t flags, D2UnitStrc* candidate, D2UnitStrc* origin, const RealmSkillDoHost& host);

// The record of 1.13c D2Client+0xA09F0: the candidate with the lowest id above `floor`, else the one
// with the lowest id at or below it (ids compared unsigned; a missing unit counts as id 0xFFFFFFFF).
struct RealmNextTarget {
    D2UnitStrc* best = nullptr; uint32_t bestGuid = 0xFFFFFFFFu;
    D2UnitStrc* low = nullptr; uint32_t lowGuid;
    uint32_t floor; int count = 0;
    explicit RealmNextTarget(uint32_t after) : lowGuid(after), floor(after) {}
    D2UnitStrc* result() const { return best ? best : low; }
};
int d2client_realm_a09f0(RealmAreaContext* ctx, D2UnitStrc* candidate);
// 1.13c D2Client+0xA0740: counts.
int d2client_realm_a0740(RealmAreaContext* ctx, D2UnitStrc* candidate);
// 1.13c D2Client+0xA1A50: the next target after id `after` within `range` (flags | 0xA280).
D2UnitStrc* d2client_realm_a1a50(uint32_t after, uint32_t flags, int range, D2UnitStrc* unit, int* count,
    const RealmSkillDoHost& host);
// 1.13c D2Client+0xA1AD0: how many units within `range` pass flags | 0xA783.
int d2client_realm_a1ad0(uint32_t flags, int range, D2UnitStrc* unit, const RealmSkillDoHost& host);
// 1.13c D2Client+0x505F0: the unit's current frame drops by `percent` of itself (the repeat speeds up).
void d2client_realm_505f0(D2UnitStrc* unit, int percent);
