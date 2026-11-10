// The Skills row's client "do" function (its cltdofunc column) for a unit the joined-game client
// keeps as an engine unit. It runs at the action frame from the generic do (1.13c D2Client+0xA2C90).
// One file per function; the table is realm_skill_do_table.cpp.
#pragma once
#include <cstdint>
struct D2UnitStrc;
struct D2MissileStrc;

// What the do functions need from the client that holds the units.
struct RealmSkillDoHost {
    D2UnitStrc* (*createMissile)(const D2MissileStrc* params);    // 1.13c D2Client+0xB30F0
    void (*unitSound)(D2UnitStrc* unit, int soundRow, int skillId); // 1.13c D2Client+0x88A70
    D2UnitStrc* (*findUnit)(int unitType, uint32_t guid);           // 1.13c D2Client+0xA4E20
};

// 1.13c D2Client+0xA1340: the point the unit's path aims at (its target unit's position, else the
// path's target point, else two subtiles ahead by its facing). Always 1.
int d2client_realm_a1340(D2UnitStrc* unit, const RealmSkillDoHost& host, int* x, int* y);
// 1.13c D2Client+0xA5F70: the path's target unit once it is checked against the client's units.
D2UnitStrc* d2client_realm_a5f70(D2UnitStrc* unit, const RealmSkillDoHost& host);
// 1.13c D2Client+0x40550: the client missile of a skill (cltmissilea; a progressive skill by its charges).
int d2client_realm_40550(D2UnitStrc* unit, int skillId);

int d2client_realm_14f40(D2UnitStrc* unit, int skillId, int level, const RealmSkillDoHost& host); // do 16
int d2client_realm_15da0(D2UnitStrc* unit, int skillId, int level, const RealmSkillDoHost& host); // do 17
int d2client_realm_15c40(D2UnitStrc* unit, int skillId, int level, const RealmSkillDoHost& host); // do 18

// Runs do function `fn`; 0 when the function did nothing (or `fn` is not ported).
int d2client_realm_skill_do(int fn, D2UnitStrc* unit, int skillId, int level, const RealmSkillDoHost& host);
// Whether the table's entry `fn` is ported here.
int d2client_realm_skill_do_ported(int fn);
